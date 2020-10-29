// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterController.generated.h"


class UCurveFloat;

UCLASS()
class TEST_API ACharacterController : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	virtual void BeginPlay() override;

public:
	ACharacterController();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = Movements)
	float DashStrength;

	UPROPERTY(EditAnywhere, Category = Movements)
	float DashDelta;

	UFUNCTION()
	void TimelineProgress(FVector Value);

	UFUNCTION()
	void StopDashTimeline();

	virtual void Tick(float DeltaTime) override;


protected:

	FTimeline CurveFTimeline;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveDashFloat;

	UPROPERTY(EditAnywhere, Category = Movements)
	float DashFOVDelta;

	UPROPERTY(EditAnywhere, Category = Movements)
	float DashCameraLagDelta;

	UPROPERTY(EditAnywhere, Category = Movements)
	float DashChromaticAbberationDelta;

	UPROPERTY(EditAnywhere, Category = Movements)
	float HoldTimeAirControl;

	UPROPERTY(EditAnywhere, Category = Movements)
	float AirControlFallStrength;

	UPROPERTY(EditAnywhere, Category = Movements)
	float AirControlValue;

	float BaseFOV;
	float BaseCameraLag;
	float BaseChromaticAbberation;


	float BaseGravityScale;;
	float BaseAirControl;


	FPostProcessSettings* PostProcessSettings;
	FOnTimelineEvent CurveFTimelineFinish;



	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void Dash();

	void StopDashing();

	void CustomJump();

	void CustomStopJumping();

	void StartAirControl();

	void StopAirControl();

	void CheckHoldJump();
	FTimerHandle FuzeTimerHandle;
	bool bIsInputJump;


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
