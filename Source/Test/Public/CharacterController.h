// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanded);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movements, meta = (AllowPrivateAccess = "true"))
	class UMovementController* DoubleJumpComponent;


	virtual void BeginPlay() override;

public:
	ACharacterController();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UFUNCTION()
	void TimelineProgressCameraTransition(FVector Value);

	UFUNCTION()
	void StopSingingTimeline();

	virtual void Tick(float DeltaTime) override;


	

	void SwitchCameras();

	UPROPERTY(BlueprintAssignable)
	FOnLanded OnLandEvent;
	
	virtual void Landed(const FHitResult& Hit) override;
	
protected:

	FTimeline CurveFTimeline;
	FTimeline CurveFCameraTimeline;

	UPROPERTY(EditAnywhere, Category = CameraType)
	TEnumAsByte<ECameraType> camera;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveDashVector;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveCameraVector;
	
	UPROPERTY()
	FVector ControllerInput;

	float BaseFOV;
	float BaseCameraLag;
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

	void CustomJump();

	void CustomStopJumping();

	void StartSinging();
	void ValidateEcho();
	void StartSingingHold();
	void StopSingingHold();

	DECLARE_DELEGATE_OneParam(FPlaySongDelegate, ESingButton);
	void PlayNote(ESingButton note);

	void TurnCamera(float Rate);

	void LookUpCamera(float Rate);


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
	
	FORCEINLINE FVector GetControllerInput() const { return ControllerInput; }

	inline bool IsGoingForward() const { return bIsgoingForward || bIsgoingRight; }

};
