// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementController.h"
#include "DashMovement.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UDashMovement : public UMovementController
{
	GENERATED_BODY()

protected : 
	virtual void OnMovementChoose() override;
	virtual void OnMovementUpdate(float DeltaTime) override;
	virtual void OnMovementExit() override;
	virtual void MovementAction() override;
	virtual void SetMovementActive() override;
	virtual void SetController(ACharacterController* Controller) override;

	UDashMovement();

	UPROPERTY(EditAnywhere, Category = DashProperty)
	float DashStrength;


	UPROPERTY(EditAnywhere, Category = DashProperty)
	float DashDelta;


	UFUNCTION()
	void DashTimelineProgress(FVector Value);


	UFUNCTION()
	void StopDashTimeline();

	void CheckTimeline();


protected:
	FTimeline CurveDashTimelineVector;

	UPROPERTY(EditAnywhere, Category = Movements)
	UCurveVector* CurveDashVector;

private:

	bool bIsTimelineSet;
	float BaseCameraFov;
	float BaseCameraLag;

};
