// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "MovementController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API UMovementController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementController();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorCurve;

	UCharacterMovementComponent* Movement;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DashAction();

private:
	ACharacter* Character;

	UPROPERTY()
	class UInputComponent* InputComponent;
		
};
