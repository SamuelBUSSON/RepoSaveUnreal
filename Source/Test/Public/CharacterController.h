// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovementController.h"
#include "SingingComponent.h"

UENUM(BlueprintType)
enum ECameraType
{
	NearCamera,
	MiddleCamera,
	FarCamera,
};



#include "CharacterController.generated.h"


class UCurveFloat;
enum ESingButton;

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

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraSinging;

	/** LerpCamera camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraLerp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movements, meta = (AllowPrivateAccess = "true"))
	class UMovementController* DashMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movements, meta = (AllowPrivateAccess = "true"))
	class UMovementController* WallJump;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movements, meta = (AllowPrivateAccess = "true"))
	class USingingComponent* SingingComponent;


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
	void TimelineProgressCameraTransition(FVector Value);

	UFUNCTION()
	void StopDashTimeline();

	UFUNCTION()
	void StopSingingTimeline();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Wall Run")
	bool IsWallRuningRight();

	UFUNCTION(BlueprintCallable, Category = "Wall Run")
	bool IsWallRuningLeft();


	UFUNCTION(BlueprintCallable, Category = "Wall Run")
	FVector GetWallPositionLeft();


	UFUNCTION(BlueprintCallable, Category = "Wall Run")
	FVector GetWallPositionRight();


	void SwitchCameras(bool isSinging);


protected:

	FTimeline CurveFTimeline;
	FTimeline CurveFCameraTimeline;



	UPROPERTY(EditAnywhere, Category = CameraType)
	TEnumAsByte<ECameraType> camera;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveDashVector;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveCameraVector;

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


	UPROPERTY(EditAnywhere, Category = WallRun)
	float WallRunForce;

	UPROPERTY(EditAnywhere, Category = WallRun)
	float WallRunCameraTiltTime;


	UPROPERTY(EditAnywhere, Category = WallRun)
	float WallRunCameraTiltAngle;




	float BaseFOV;
	float BaseCameraLag;
	float BaseChromaticAbberation;


	float BaseGravityScale;;
	float BaseAirControl;

	bool bIsThirdPersonCurrentCamera;

	bool bIsgoingForward;
	bool bIsgoingRight;


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

	void Dash();

	void StopDashing();

	void CustomJump();

	void CustomStopJumping();

	void StartAirControl();

	void StopAirControl();

	void StartSinging();
	void ValidateEcho();

	DECLARE_DELEGATE_OneParam(FPlaySongDelegate, ESingButton);
	void PlayNote(ESingButton note);

	void TurnCamera(float Rate);

	void LookUpCamera(float Rate);

	void CheckForWall(float Value);

	void StartWallRun();

	void StopWallRun();




	void CheckHoldJump();
	FTimerHandle FuzeTimerHandle;

	bool bIsInputJump;

	bool bIsWallRight;
	bool bIsWallLeft;
	bool bIsWallRunning;

	FVector WallPositionRight;
	FVector WallPositionLeft;



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	inline bool IsGoingForward() const { return bIsgoingForward || bIsgoingRight; }

};
