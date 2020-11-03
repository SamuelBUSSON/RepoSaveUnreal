// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "Components/TimelineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"
#include "DrawDebugHelpers.h" 
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


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

	HoldTimeAirControl = 0.2f;
	AirControlValue = 1.0f;
	AirControlFallStrength = 0.3f;

	WallRunForce = 1.5f; 
	WallRunCameraTiltTime = 0.3f;
	WallRunCameraTiltAngle = 20.0f;

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

	p_CurrentEcho = MakeShareable(new TArray<ESingButton>());


    //Take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

	bIsThirdPersonCurrentCamera = true;
	CurrentEchoPlayedSong = 0;
		
}

// Called when the game starts or when spawned
void ACharacterController::BeginPlay()
{
	Super::BeginPlay();


	BaseGravityScale = GetCharacterMovement()->GravityScale;
	BaseAirControl = GetCharacterMovement()->AirControl;

	CameraSinging->SetActive(false);

	if (CurveDashVector)
	{
		FOnTimelineVector TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;


		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		TimelineFinishedCallback.BindUFunction(this, FName("StopDashTimeline"));

		CurveFTimeline.AddInterpVector(CurveDashVector, TimelineProgress);
		CurveFTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);


		BaseFOV = FollowCamera->FieldOfView;
		BaseCameraLag = CameraBoom->CameraLagSpeed;
		BaseChromaticAbberation = FollowCamera->PostProcessSettings.SceneFringeIntensity;
	}

	if (CurveCameraVector)
	{
		FOnTimelineVector TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineProgress.BindUFunction(this, FName("TimelineProgressCameraTransition"));
		TimelineFinishedCallback.BindUFunction(this, FName("StopSingingTimeline"));

		CurveFCameraTimeline.AddInterpVector(CurveCameraVector, TimelineProgress);
		CurveFCameraTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
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
	CurveFCameraTimeline.TickTimeline(DeltaTime);
}

bool ACharacterController::IsWallRuningRight()
{
	return bIsWallRunning && bIsWallRight;
}

bool ACharacterController::IsWallRuningLeft()
{
	return bIsWallRunning && bIsWallLeft;
}

FVector ACharacterController::GetWallPositionLeft()
{
	return WallPositionLeft + FVector::UpVector * 50.0f;
}

FVector ACharacterController::GetWallPositionRight()
{
	return WallPositionRight + FVector::UpVector * 50.0f;
}




void ACharacterController::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterController::CustomJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterController::CustomStopJumping);


	PlayerInputComponent->BindAction("Sing", IE_Pressed, this, &ACharacterController::StartSinging);
	PlayerInputComponent->BindAction("ValidateEcho", IE_Pressed, this, &ACharacterController::ValidateEcho);

	

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACharacterController::Dash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &ACharacterController::StopDashing);

	PlayerInputComponent->BindAction<FPlaySongDelegate>("Do", IE_Pressed, this, &ACharacterController::PlaySong, ESingButton::Do);
	PlayerInputComponent->BindAction<FPlaySongDelegate>("Re", IE_Pressed, this, &ACharacterController::PlaySong, ESingButton::Re);
	PlayerInputComponent->BindAction<FPlaySongDelegate>("Mi", IE_Pressed, this, &ACharacterController::PlaySong, ESingButton::Mi);	


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

void ACharacterController::TurnCamera(float Rate)
{
	if(!bIsSinging)
		AddControllerYawInput(Rate);
}

void ACharacterController::LookUpCamera(float Rate)
{
	if (!bIsSinging)
		AddControllerPitchInput(Rate);
}



void ACharacterController::CustomJump()
{
	if (!bIsSinging)
	{
		Jump();
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ACharacterController::CheckHoldJump, HoldTimeAirControl, false);
		bIsInputJump = true;
	}
}

void ACharacterController::CustomStopJumping()
{
	StopJumping();
	StopAirControl();

	bIsInputJump = false;
}

void ACharacterController::CheckForWall(float Value)
{
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorRightVector() * 100.0f;
	FCollisionQueryParams CollisionParams;
	bIsWallRight = false;
	bIsWallLeft = false;

	// Check if there is a wall on the right side
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		bIsWallRight = true;
		WallPositionRight = OutHit.ImpactPoint;
	}

	// Check if there is a wall on the left side
	End = Start + GetActorRightVector() * -100.0f;	
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		bIsWallLeft = true;
		WallPositionLeft = OutHit.ImpactPoint;
	}

	//If there is no more walls stop wall running
	if ((!bIsWallRight && !bIsWallLeft) || FMath::Abs(Value) < 0.05f)
		StopWallRun();

}

void ACharacterController::StartWallRun()
{
	if (!bIsWallRunning)
	{
		GetCharacterMovement()->Velocity.Z = .0f;
		/* Rotation Lerp */
		//FollowCamera->SetWorldRotation(FollowCamera->GetComponentRotation() + FRotator(0, 0, (bIsWallRight ? -1 : 1) * 25));
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = FollowCamera;

		UKismetSystemLibrary::MoveComponentTo(
			FollowCamera,
			FollowCamera->GetRelativeLocation(), 
			FollowCamera->GetRelativeRotation() + FRotator(0, 0, (bIsWallRight ? -1 : 1) * WallRunCameraTiltAngle),
			true,
			true,
			WallRunCameraTiltTime,
			true,
			EMoveComponentAction::Move, 
			LatentInfo);

		FLatentActionInfo LatentInfoMesh;
		LatentInfoMesh.CallbackTarget = GetMesh();
		

		GetCharacterMovement()->GravityScale = 0.05f;
		bIsWallRunning = true;
	}


	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//AddMovementInput(Direction, 1.0f * WallRunForce);

	GetCharacterMovement()->AddForce(GetActorForwardVector() * WallRunForce);


	GetCharacterMovement()->AddForce(GetActorRightVector() * WallRunForce  * (bIsWallLeft ? -1 : 1));
}

void ACharacterController::StopWallRun()
{
	if (bIsWallRunning)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(
			FollowCamera,
			FollowCamera->GetRelativeLocation(),
			FRotator::ZeroRotator,
			true,
			true,
			WallRunCameraTiltTime,
			true,
			EMoveComponentAction::Move,
			LatentInfo);

		bIsWallRunning = false;
		GetCharacterMovement()->GravityScale = BaseGravityScale;
	}

}

void ACharacterController::PlaySong(ESingButton songPlayed)
{
	if (bIsSinging)
	{
		p_CurrentEcho->Add(songPlayed);
		OnSingingDelegate.Broadcast(songPlayed);
	}
}

void ACharacterController::ValidateEcho()
{
	if (bIsSinging)
	{
		StartSinging();	
		GetWorldTimerManager().SetTimer(EchoTimerHandle, this, &ACharacterController::PlayEcho, 0.1f, true, 0.5f);
	}
}

void ACharacterController::PlayEcho()
{
	switch (p_CurrentEcho->GetData()[CurrentEchoPlayedSong])
	{
		case Do:
			print("Do");
			break;
		case Re:
			print("Re");
			break;
		case Mi:
			print("Mi");
			break;
		case Fa:
			print("Fa");
			break;
		case Sol:
			print("Sol");
			break;
		case La:
			print("La");
			break;
		case Si:
			print("Si");
			break;
	}
	++CurrentEchoPlayedSong;
	if (CurrentEchoPlayedSong == p_CurrentEcho->Num() - 1)
		CurrentEchoPlayedSong = 0;

	//GetWorldTimerManager().SetTimer(EchoTimerHandle, this, &ACharacterController::PlayEcho, .0f, false, 1.0f);

}

void ACharacterController::CheckHoldJump()
{
	if (bIsInputJump)
	{
		StartAirControl();
	}

}

void ACharacterController::StartSinging()
{

	/*bIsThirdPersonCurrentCamera = !bIsThirdPersonCurrentCamera;

	CameraBoom->bUsePawnControlRotation = bIsThirdPersonCurrentCamera;

	FollowCamera->SetActive(bIsThirdPersonCurrentCamera);*/


	if (!bIsSinging)
	{
		CameraLerp->SetActive(true);
		CameraSinging->SetActive(false);
		FollowCamera->SetActive(false);
	}
	
	bIsSinging = !bIsSinging;
	CameraLerp->SetWorldLocationAndRotation(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());


	CurveFCameraTimeline.PlayFromStart();
	
}

void ACharacterController::TurnAtRate(float Rate)
{
	if (!bIsSinging)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterController::LookUpAtRate(float Rate)
{
	if (!bIsSinging)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterController::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !bIsSinging)
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
	if (!bIsSinging)
	{
		CheckForWall(Value);

		if (FMath::Abs(Value) > 0.05f)
		{
			if (GetCharacterMovement()->IsFalling() && ((Value < 0 && bIsWallLeft) || (Value > 0 && bIsWallRight)))
				StartWallRun();
			else
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
		}
	}
}

void ACharacterController::StartAirControl()
{
	//print("Start air control");
	GetCharacterMovement()->GravityScale = AirControlFallStrength;
	GetCharacterMovement()->AirControl = AirControlValue;
	//GetCharacterMovement()->Velocity.Z = 0;
}

void ACharacterController::StopAirControl()
{
	//print("Stop air control");
	GetCharacterMovement()->GravityScale = BaseGravityScale;
	GetCharacterMovement()->AirControl = BaseAirControl;

}



void ACharacterController::Dash() 
{
	if (!bIsSinging)
	{
		UE_LOG(LogTemp, Log, TEXT("Dash"));

		FVector dashDirectionActor = GetCharacterMovement()->IsFalling() ? FollowCamera->GetForwardVector() : GetActorForwardVector();
		dashDirectionActor *= DashStrength;
		dashDirectionActor.Z += DashDelta;

		GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(dashDirectionActor, true, true);

		CameraBoom->bEnableCameraLag = true;
		CurveFTimeline.PlayFromStart();
	}
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

void ACharacterController::TimelineProgressCameraTransition(FVector Value)
{

	if (bIsSinging)
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

void ACharacterController::StopSingingTimeline()
{
	if (!bIsSinging)
	{
		CameraLerp->SetActive(false);
		CameraSinging->SetActive(false);
		FollowCamera->SetActive(true);
	}
}

void ACharacterController::StopDashTimeline()
{
	CameraBoom->bEnableCameraLag = false;
}