// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTest.h"

// Sets default values
APlayerTest::APlayerTest()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //Create our components
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    OurCameraSpringArm->SetupAttachment(RootComponent);
    OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
    OurCameraSpringArm->TargetArmLength = 400.f;
    OurCameraSpringArm->bEnableCameraLag = true;
    OurCameraSpringArm->CameraLagSpeed = 3.0f;
    OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
    OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);
    CharacterMovement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("CharaMovement"));

    //Take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;


}

// Called when the game starts or when spawned
void APlayerTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


    //Hook up events for "ZoomIn"
    InputComponent->BindAction("Dash", IE_Pressed, this, &APlayerTest::Dash);

    //Hook up every-frame handling for our four axes
    InputComponent->BindAxis("MoveForward", this, &APlayerTest::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &APlayerTest::MoveRight);
    InputComponent->BindAxis("LookUpRate", this, &APlayerTest::PitchCamera);
    InputComponent->BindAxis("LookUp", this, &APlayerTest::YawCamera);
}

void APlayerTest::Dash()
{

}

void APlayerTest::MoveForward(float AxisValue)
{

}

void APlayerTest::MoveRight(float AxisValue)
{

}

void APlayerTest::PitchCamera(float AxisValue)
{

}

void APlayerTest::YawCamera(float AxisValue)
{

}
