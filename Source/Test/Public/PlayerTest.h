// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerTest.generated.h"

UCLASS()
class TEST_API APlayerTest : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* OurCameraSpringArm;
	UCameraComponent* OurCamera;
	UCharacterMovementComponent* CharacterMovement;

	FVector2D MovementInput;
	FVector2D CameraInput;

	void Dash();
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
