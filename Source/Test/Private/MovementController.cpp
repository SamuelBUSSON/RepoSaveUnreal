// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementController.h"
#include "CharacterController.h"
#include "Containers/UnrealString.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime\Engine\Classes\Kismet\GameplayStatics.h>

// Sets default values for this component's properties
UMovementController::UMovementController()
{
	PrimaryComponentTick.bCanEverTick = true;
}


UMovementController::UMovementController(ACharacterController* Controller)
{
	pMyController = Controller;
	PrimaryComponentTick.bCanEverTick = true;
}


void UMovementController::OnMovementChoose()
{
}

void UMovementController::OnMovementUpdate(float DeltaTime)
{
}

void UMovementController::OnMovementExit()
{
}

void UMovementController::MovementAction()
{
}

void UMovementController::SetMovementActive()
{
}

void UMovementController::SetController(ACharacterController* Controller)
{
	pMyController = Controller;
}

// Called when the game starts
void UMovementController::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMovementController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	OnMovementUpdate(DeltaTime);
}
