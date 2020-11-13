// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterController.h"
#include "MovementController.generated.h"

class ACharacterController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API UMovementController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementController();

	UMovementController(ACharacterController* Controller);

	virtual void OnMovementChoose();
	virtual void OnMovementUpdate(float DeltaTime);
	virtual void OnMovementExit();
	virtual void MovementAction();
	virtual void SetMovementActive();
	virtual void SetController(ACharacterController* Controller);

	UPROPERTY()
	ACharacterController* PMyController;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsComponentActive;
	bool bIsBlockingOthers;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDisplayValues;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	inline bool IsComponentActive() const { return bIsComponentActive; }
	inline bool IsBlockingOthers() const { return bIsBlockingOthers; }
	inline void SetIsComponentActive(bool IsActive) { bIsComponentActive = IsActive; }

private:


};
