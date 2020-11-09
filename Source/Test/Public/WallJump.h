// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementController.h"
#include "WallJump.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UWallJump : public UMovementController
{
	GENERATED_BODY()

protected:
	virtual void OnMovementChoose() override;
	virtual void OnMovementUpdate(float DeltaTime) override;
	virtual void OnMovementExit() override;
	virtual void MovementAction() override;
	virtual void SetMovementActive() override;
	virtual void SetController(ACharacterController* Controller) override;


public:

	UPROPERTY(EditAnywhere, Category = WallJump)
	float SlippingSpeed;
	UPROPERTY(EditAnywhere, Category = WallJump)
	float SlippingTime;
	UPROPERTY(EditAnywhere, Category = WallJump)
	float PropulsionSpeed;
	UPROPERTY(EditAnywhere, Category = WallJump, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	float PropulsionAngle;

private:
	bool bIsStick;

	float baseGravityScale;

	void SetPlayerFallStrength();

	FTimerHandle SlippingTimerHandle;
	FTimerHandle ResetTimerHandle;
	FVector WallNormal;
	
	void ResetWallStick();
};
