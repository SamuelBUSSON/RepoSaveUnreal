// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "Components/TimelineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MovementController.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "DashMovement.h"
#include "DoubleJumpComponent.h"
#include "WallJump.h"
#include "SingingComponent.h"


// Sets default values
ACharacterController::ACharacterController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 0.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	CameraLerp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraLerp"));
	CameraLerp->SetupAttachment(RootComponent);

	CameraSinging = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraSinging"));
	CameraSinging->SetupAttachment(RootComponent); 

	DashMovement = CreateDefaultSubobject<UDashMovement>(TEXT("Dash Component"));
	DashMovement->SetController(this);

	WallJump = CreateDefaultSubobject<UWallJump>(TEXT("Wall Jump Component"));
	WallJump->SetController(this);

	SingingComponent = CreateDefaultSubobject<USingingComponent>(TEXT("Singing Component"));
	SingingComponent->SetController(this);

	DoubleJumpComponent = CreateDefaultSubobject<UDoubleJumpComponent>(TEXT("Double Jump Component"));
	DoubleJumpComponent->SetController(this);

    //Take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

	bIsThirdPersonCurrentCamera = true;

	ControllerInput = FVector::ZeroVector;
		
}



// Called when the game starts or when spawned
void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	BaseGravityScale = GetCharacterMovement()->GravityScale;
	BaseAirControl = GetCharacterMovement()->AirControl;

	CameraSinging->SetActive(false);	

	if (CurveCameraVector)
	{
		FOnTimelineVector TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineProgress.BindUFunction(this, FName("TimelineProgressCameraTransition"));
		TimelineFinishedCallback.BindUFunction(this, FName("StopSingingTimeline"));

		CurveFCameraTimeline.AddInterpVector(CurveCameraVector, TimelineProgress);
		CurveFCameraTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

}



// Called every frame
void ACharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurveFTimeline.TickTimeline(DeltaTime);
	CurveFCameraTimeline.TickTimeline(DeltaTime);
}


void ACharacterController::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterController::CustomJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterController::CustomStopJumping);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this->DoubleJumpComponent, &UMovementController::MovementAction);

	PlayerInputComponent->BindAction("Sing", IE_Pressed, this, &ACharacterController::StartSinging);
	PlayerInputComponent->BindAction("ValidateEcho", IE_Pressed, this, &ACharacterController::ValidateEcho);
	PlayerInputComponent->BindAction("SingHolding", IE_Pressed, this, &ACharacterController::StartSingingHold);
	PlayerInputComponent->BindAction("SingHolding", IE_Released, this, &ACharacterController::StopSingingHold);

	/*PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACharacterController::Dash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &ACharacterController::StopDashing);*/

	PlayerInputComponent->BindAction("Dash", IE_Pressed, DashMovement, &UMovementController::MovementAction);

	PlayerInputComponent->BindAction<FPlaySongDelegate>("Earth", IE_Pressed, this, &ACharacterController::PlayNote, ESingButton::Earth);
	PlayerInputComponent->BindAction<FPlaySongDelegate>("Wind", IE_Pressed, this, &ACharacterController::PlayNote, ESingButton::Wind);
	PlayerInputComponent->BindAction<FPlaySongDelegate>("Fire", IE_Pressed, this, &ACharacterController::PlayNote, ESingButton::Fire);
	PlayerInputComponent->BindAction<FPlaySongDelegate>("Water", IE_Pressed, this, &ACharacterController::PlayNote, ESingButton::Water);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ACharacterController::TurnCamera);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterController::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterController::LookUpCamera);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterController::LookUpAtRate);

}

#pragma region Basic movements



void ACharacterController::TurnCamera(float Rate)
{
	if(!SingingComponent->IsSinging())
		AddControllerYawInput(Rate);
}

void ACharacterController::LookUpCamera(float Rate)
{
	if (!SingingComponent->IsSinging())
		AddControllerPitchInput(Rate);
}

void ACharacterController::TurnAtRate(float Rate)
{
	if (!SingingComponent->IsSinging())
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterController::LookUpAtRate(float Rate)
{
	if (!SingingComponent->IsSinging())
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterController::CustomJump()
{
	GetCharacterMovement()->DoJump(false);
}

void ACharacterController::CustomStopJumping()
{
	
}

void ACharacterController::MoveForward(float Value)
{
	ControllerInput.Y = Value;
	
	if ((Controller != NULL) && (FMath::Abs(Value) >= KINDA_SMALL_NUMBER) && !SingingComponent->IsSinging() && !WallJump->IsBlockingOthers())
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	bIsgoingForward = FMath::Abs(Value) > KINDA_SMALL_NUMBER;
}

void ACharacterController::MoveRight(float Value)
{
	ControllerInput.X = Value;
	
	if (!SingingComponent->IsSinging() && !WallJump->IsBlockingOthers() && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{	
		if ((Controller != NULL) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}					
	}
	bIsgoingRight = FMath::Abs(Value) > KINDA_SMALL_NUMBER;
}
#pragma endregion


#pragma region Singing

/*
* ======================
*		 Singing
* ======================
*/

/*
* Called when player hit the singing input
* */
void ACharacterController::StartSinging()
{
	//If he was singing reset cameras
	SingingComponent->StartSinging();
}

void ACharacterController::ValidateEcho()
{
	SingingComponent->ValidateEcho();
}

void ACharacterController::StartSingingHold()
{
	SingingComponent->StartSingingHold();
}

void ACharacterController::StopSingingHold()
{
	SingingComponent->StopSingingHold();	
}

void ACharacterController::PlayNote(ESingButton note)
{
	SingingComponent->PlaySong(note);
}


void ACharacterController::SwitchCameras()
{
	if (SingingComponent->IsSinging())
	{
		CameraLerp->SetActive(true);
		CameraSinging->SetActive(false);
		FollowCamera->SetActive(false);
	}

	CameraLerp->SetWorldLocationAndRotation(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());
	CurveFCameraTimeline.PlayFromStart();
}

void ACharacterController::Landed(const FHitResult& Hit)
{
	ACharacter::Landed(Hit);
	OnLandEvent.Broadcast();
}

/*
* Camera Transition : Lerp between two cameras belong the timeline
* @param Value : The vector used by the curve
* */
void ACharacterController::TimelineProgressCameraTransition(const FVector Value)
{
	if (SingingComponent->IsSinging())
	{
		//printFString("Value : %f", Value.X);
		CameraLerp->SetWorldLocationAndRotation(
			FMath::Lerp(FollowCamera->GetComponentLocation(), CameraSinging->GetComponentLocation(), Value.X),
			FMath::Lerp(FollowCamera->GetComponentRotation(), CameraSinging->GetComponentRotation(), Value.X));
	}
	else
	{
		//printFString("Value : %f", Value.X);
		CameraLerp->SetWorldLocationAndRotation(
			FMath::Lerp(CameraSinging->GetComponentLocation(), FollowCamera->GetComponentLocation(), Value.X),
			FMath::Lerp(CameraSinging->GetComponentRotation(), FollowCamera->GetComponentRotation(), Value.X));
	}
}

/*
* Camera Transition : Called when the timeline finished
* */
void ACharacterController::StopSingingTimeline()
{
	if (!SingingComponent->IsSinging())
	{
		CameraLerp->SetActive(false);
		CameraSinging->SetActive(false);
		FollowCamera->SetActive(true);
	}
}


/*
* --=====================--
*		 End Singing
* --=====================--
*/

#pragma endregion

