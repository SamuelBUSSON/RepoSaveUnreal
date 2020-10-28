// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementController.h"
#include "Containers/UnrealString.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime\Engine\Classes\Kismet\GameplayStatics.h>

// Sets default values for this component's properties
UMovementController::UMovementController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMovementController::BeginPlay()
{
	Super::BeginPlay();

	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Movement = (UCharacterMovementComponent*) Character->GetMovementComponent();

	//UInputComponent* myInputComp = InputComponent;

	//myInputComp->BindAction("Dash", IE_Pressed, this, &UMovementController::DashAction);
	
}


// Called every frame
void UMovementController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %f, y: %f"), Movement->JumpZVelocity, Movement->JumpZVelocity));


}

void UMovementController::DashAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Dash")));
	if (Movement->IsFalling())
	{

	}
	else
	{

	}


	//USceneComponent forwardActor = Movement->IsFalling();

}
