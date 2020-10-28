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

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT(text), fstring))

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

	DashStrength = 2000.f;
	DashDelta = 350.f;
	
	DashFOVDelta = 30.0f;
	DashCameraLagDelta = 60.0f;
	DashChromaticAbberationDelta = 5.0f;

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

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm



    //Take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (CurveDashFloat)
	{
		FOnTimelineVector TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;


		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		TimelineFinishedCallback.BindUFunction(this, FName("StopDashTimeline"));

		CurveFTimeline.AddInterpVector(CurveDashFloat, TimelineProgress);
		CurveFTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);


		BaseFOV = FollowCamera->FieldOfView;
		BaseCameraLag = CameraBoom->CameraLagSpeed;
		BaseChromaticAbberation = FollowCamera->PostProcessSettings.SceneFringeIntensity;
	}

	int32 count = GetWorld()->PostProcessVolumes.Num();

	for (int32 x = 0; x < count; ++x)
	{
		FPostProcessVolumeProperties volume = GetWorld()->PostProcessVolumes[x]->GetProperties();
		if (volume.bIsUnbound)
		{
			PostProcessSettings = (FPostProcessSettings*)volume.Settings;
		}

	}
	
}



// Called every frame
void ACharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurveFTimeline.TickTimeline(DeltaTime);


}
void ACharacterController::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACharacterController::Dash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &ACharacterController::StopDashing);


	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterController::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterController::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacterController::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacterController::TouchStopped);



}


void ACharacterController::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACharacterController::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACharacterController::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterController::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterController::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterController::MoveRight(float Value)
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


void ACharacterController::Dash() 
{
	UE_LOG(LogTemp, Log , TEXT("Dash"));

	FVector dashDirectionActor = GetCharacterMovement()->IsFalling() ? FollowCamera->GetForwardVector() : GetActorForwardVector();
	dashDirectionActor *= DashStrength;
	dashDirectionActor.Z += DashDelta;

	GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(dashDirectionActor, true, true);

	CameraBoom->bEnableCameraLag = true;
	CurveFTimeline.PlayFromStart();



}

void ACharacterController::StopDashing()
{

}

void ACharacterController::TimelineProgress(FVector Value)
{
	//printFString("My Variable Vector is: %s", *Value.ToString());
	FollowCamera->SetFieldOfView(FMath::Lerp(BaseFOV, BaseFOV + DashFOVDelta, Value.X));
	PostProcessSettings->SceneFringeIntensity = FMath::Lerp(BaseChromaticAbberation, BaseChromaticAbberation + DashChromaticAbberationDelta, Value.X);
	CameraBoom->CameraLagSpeed = FMath::Lerp(BaseCameraLag, BaseCameraLag + DashCameraLagDelta, Value.Y);
}

void ACharacterController::StopDashTimeline()
{
	CameraBoom->bEnableCameraLag = false;
}