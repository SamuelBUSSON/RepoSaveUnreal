// Fill out your copyright notice in the Description page of Project Settings.


#include "DashMovement.h"
#include "MovementController.h"

UDashMovement::UDashMovement():UMovementController::UMovementController()
{
	DashStrength = 3000.0f;
	DashDelta = 300.0f;
	bIsTimelineSet = false;
}

void UDashMovement::OnMovementChoose()
{

}

void UDashMovement::OnMovementUpdate(float DeltaTime)
{
	CurveDashTimelineVector.TickTimeline(DeltaTime);
}

void UDashMovement::OnMovementExit()
{

}

void UDashMovement::MovementAction()
{
	if(!bIsTimelineSet)
		CheckTimeline();

	FVector dashDirectionActor = pMyController->GetCharacterMovement()->IsFalling() ? pMyController->GetFollowCamera()->GetForwardVector() : pMyController->GetActorForwardVector();
	dashDirectionActor *= DashStrength;
	dashDirectionActor.Z += DashDelta;

	pMyController->GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(dashDirectionActor, true, true);

	pMyController->GetCameraBoom()->bEnableCameraLag = true;
}

void UDashMovement::SetMovementActive()
{

}

void UDashMovement::SetController(ACharacterController* Controller)
{	
	Super::SetController(Controller);

}

void UDashMovement::DashTimelineProgress(FVector Value)
{

}

void UDashMovement::StopDashTimeline()
{
}

void UDashMovement::CheckTimeline()
{
	if (CurveDashVector)
	{
		FOnTimelineVector TimelineProgress;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineProgress.BindUFunction(this, FName("DashTimelineProgress"));
		TimelineFinishedCallback.BindUFunction(this, FName("StopDashTimeline"));

		CurveDashTimelineVector.AddInterpVector(CurveDashVector, TimelineProgress);
		CurveDashTimelineVector.SetTimelineFinishedFunc(TimelineFinishedCallback);

		bIsTimelineSet = true;
	}
}