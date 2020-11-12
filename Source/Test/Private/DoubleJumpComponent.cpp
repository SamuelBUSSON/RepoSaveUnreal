// Fill out your copyright notice in the Description page of Project Settings.


#include "DoubleJumpComponent.h"

uint8 UDoubleJumpComponent::GetCurExtraJump() const
{
    return CurExtraJump;
}

void UDoubleJumpComponent::OnLand()
{
    CurExtraJump = 0;
}

void UDoubleJumpComponent::SetController(ACharacterController* Controller)
{
    Super::SetController(Controller);
    PMyController->OnLandEvent.AddDynamic(this, &UDoubleJumpComponent::OnLand);
}

void UDoubleJumpComponent::MovementAction()
{

    if(bDisplayValues && PMyController->JumpCurrentCount < PMyController->JumpMaxCount)
    {
        const FVector VelocityDebug = PMyController->GetVelocity();        
        print("----------");
        printFString("Jump power: %f", PMyController->GetCharacterMovement()->JumpZVelocity);
        printFString("Speed: %f", FVector2D(VelocityDebug.X,VelocityDebug.Y).Size());
        print("Player performed a jump:");
    }
    
    if(PMyController->JumpCurrentCount >= PMyController->JumpMaxCount && CurExtraJump < MaxExtraJump)
    {
        if(bDisplayValues)
        {
            const FVector VelocityDebug = PMyController->GetVelocity();
            
            print("----------");
            printFString("Jump power: %f", JumpPower);
            printFString("Speed Multiplier: %f", HorizontalVelocityMultiplier);
            printFString("Speed before jump: %f", FVector2D(VelocityDebug.X,VelocityDebug.Y).Size());
            printFString("Speed after jump: %f", FVector2D(VelocityDebug.X,VelocityDebug.Y).Size() * HorizontalVelocityMultiplier);
            print("Player performed a double jump:");
        }
        
        ++CurExtraJump;
        const FVector Velocity = PMyController->GetVelocity();
        const float Multiplier = FVector2D(Velocity.X,Velocity.Y).Size() * HorizontalVelocityMultiplier;
        UCharacterMovementComponent* MovementComponent = PMyController->GetCharacterMovement();        
        MovementComponent->Velocity = PMyController->GetActorForwardVector() * Multiplier;		
        MovementComponent->AddImpulse(FVector::UpVector * JumpPower, true);
    }     
}

void UDoubleJumpComponent::BeginPlay()
{
    Super::BeginPlay();
    PMyController = Cast<ACharacterController>(GetOwner());
}



UDoubleJumpComponent::UDoubleJumpComponent() : CurExtraJump(0)
{
    PrimaryComponentTick.bCanEverTick = false;
}


