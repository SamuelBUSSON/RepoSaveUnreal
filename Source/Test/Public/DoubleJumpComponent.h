// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MovementController.h"
#include "DoubleJumpComponent.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UDoubleJumpComponent : public UMovementController
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 MaxExtraJump = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JumpPower = 600;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.f) )
    float HorizontalVelocityMultiplier = 1.5f;

    UDoubleJumpComponent();
    
    uint8 GetCurExtraJump() const;

    UFUNCTION()
    void OnLand();

    virtual void SetController(ACharacterController* Controller) override;
    
    virtual void MovementAction() override;
 
    virtual void BeginPlay() override;
    
protected:
    uint8 CurExtraJump;  
    
};
