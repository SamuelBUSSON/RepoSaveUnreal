// Fill out your copyright notice in the Description page of Project Settings.


#include "WallJump.h"
#include "MovementController.h"
#include "DrawDebugHelpers.h"

void UWallJump::OnMovementChoose()
{
	bIsStick = false;

	SlippingSpeed = 0.5f;
	SlippingTime = 0.2f;
	PropulsionSpeed = 500.0f;
	PropulsionAngle = 45.0f;
}


void UWallJump::OnMovementUpdate(float DeltaTime)
{

	FHitResult OutHit;
	FVector Start = pMyController->GetActorLocation();
	FVector End = Start + pMyController->GetActorForwardVector() * 50.0f;
	FCollisionQueryParams CollisionParams;

	if (pMyController->IsGoingForward() && 
		pMyController->GetCharacterMovement()->IsFalling() &&
		GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		if (!bIsStick)
		{
			bIsStick = true;
			isBlockingOthers = true;

			pMyController->GetCharacterMovement()->GravityScale = 0.0f;
			pMyController->GetCharacterMovement()->Velocity = FVector::ZeroVector;

			WallNormal = OutHit.ImpactNormal;
			WallNormal.Normalize();
			//print("Stick");

			if (bDisplayValues)
			{
				DrawDebugLine(
					GetWorld(),
					OutHit.ImpactPoint,
					OutHit.ImpactPoint + OutHit.ImpactNormal * 100.0f,
					FColor(255, 0, 0),
					false,
					3.5f
				);
			}
			


			GetWorld()->GetTimerManager().SetTimer(SlippingTimerHandle, this, &UWallJump::SetPlayerFallStrength, SlippingTime, false);
		}
	}
	else
	{
		if (bIsStick)
		{
			ResetWallStick();
		}
	}
}

void UWallJump::OnMovementExit()
{

}

void UWallJump::MovementAction()
{
	if (bIsStick)
	{
		//print("Jump");
		pMyController->GetCharacterMovement()->GravityScale = baseGravityScale;
		isBlockingOthers = false;

		FVector ImpulseForce = (-pMyController->GetActorForwardVector() * PropulsionSpeed).RotateAngleAxis(PropulsionAngle, pMyController->GetActorRightVector());

		if (bDisplayValues)
		{
			printFString("Impulse Angle : %f", PropulsionAngle);
			printFString("Impulse Force : %f", PropulsionSpeed);
			printFString("Result : %s", *(ImpulseForce.ToString()));
		}

		pMyController->GetCharacterMovement()->AddImpulse(ImpulseForce , true);

		
	}
}

void UWallJump::SetMovementActive()
{

}

void UWallJump::SetController(ACharacterController* Controller)
{
	Super::SetController(Controller);


	baseGravityScale = Controller->GetCharacterMovement()->GravityScale;
}

void UWallJump::SetPlayerFallStrength()
{

	if (bDisplayValues)
	{
		printFString("Slipping speed : %f", SlippingSpeed);
		printFString("Slipping timer : %f", SlippingTime);
	}

	if(bIsStick)
		pMyController->GetCharacterMovement()->GravityScale = SlippingSpeed;
}

void UWallJump::ResetWallStick()
{

	//print("Unstick");
	bIsStick = false;
	isBlockingOthers = false;
	pMyController->GetCharacterMovement()->GravityScale = baseGravityScale;
}
