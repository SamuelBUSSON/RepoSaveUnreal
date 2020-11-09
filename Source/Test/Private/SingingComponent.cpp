// Fill out your copyright notice in the Description page of Project Settings.


#include "SingingComponent.h"

// Sets default values for this component's properties
USingingComponent::USingingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Init values that can be modifiy through the inspector
	NotesNumber = 4;

	//Init values that can't be modifiy through the inspector
	CurrentEchoPlayedSong = 0;
	CurrentNumberOfNotesInEcho = 0;
	bIsSinging = false;


	p_CurrentEcho = MakeShareable(new TArray<ESingButton>());
}


// Called when the game starts
void USingingComponent::BeginPlay()
{
	Super::BeginPlay();

	OnSingingDelegate.AddDynamic(this, &USingingComponent::ListenOnSinging);
	StartSingingDelegate.AddDynamic(this, &USingingComponent::ListenOnStartSinging);

	p_CurrentEcho->Init(ESingButton::None, NotesNumber);
}


// Called every frame
void USingingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USingingComponent::SetController(ACharacterController* Controller)
{
	pMyController = Controller;
}


/*
* ======================
*		 Singing
* ======================
*/

/*
* Called when player hit the singing input
* */
void USingingComponent::StartSinging()
{
	CurrentNumberOfNotesInEcho = 0;
	bIsSinging = !bIsSinging;

	StartSingingDelegate.Broadcast(bIsSinging);

	pMyController->SwitchCameras(bIsSinging);
}

/*
* Gameplay mechanic : Called when the player is in singing mode and press singing input
* @param songPlayed : The singing input pressed
* */
void USingingComponent::PlaySong(ESingButton songPlayed)
{
	if (bIsSinging)
	{
		if (p_CurrentEcho->Num() > 0)
		{
			//Add the sing to the echo list
			(*p_CurrentEcho)[CurrentNumberOfNotesInEcho] = songPlayed;
			//Then add it to UI or others
			OnSingingDelegate.Broadcast(songPlayed);

			//Increment
			++CurrentNumberOfNotesInEcho;
			if (CurrentNumberOfNotesInEcho == NotesNumber)
				CurrentNumberOfNotesInEcho = 0;

		}
	}
}

/*
* Gameplay mechanic : Called when the player hit the validate echo input
* */
void USingingComponent::ValidateEcho()
{
	if (bIsSinging)
	{
		//Switch cameras
		StartSinging();
		//Play the loop
		if (p_CurrentEcho->Num() > 0)
			GetOwner()->GetWorldTimerManager().SetTimer(EchoTimerHandle, this, &USingingComponent::PlayEcho, 0.1f, false, 0.5f);
	}
}

void USingingComponent::PlayEcho()
{
	if (p_CurrentEcho->Num() != 0)
	{
		switch (p_CurrentEcho->GetData()[CurrentEchoPlayedSong])
		{
			case Earth:
				print("Earth");
				OnEchoSinging.Broadcast(ESingButton::Earth);
				break;
			case Wind:
				print("Wind");
				OnEchoSinging.Broadcast(ESingButton::Wind);
				break;
			case Fire:
				print("Fire");
				OnEchoSinging.Broadcast(ESingButton::Fire);
				break;
			case Water:
				print("Water");
				OnEchoSinging.Broadcast(ESingButton::Water);
				break;
			case Aether:
				print("Aether");
				OnEchoSinging.Broadcast(ESingButton::Aether);
				break;
			default:
				OnEchoSinging.Broadcast(ESingButton::None);
				break;
		}

		++CurrentEchoPlayedSong;
		if (CurrentEchoPlayedSong == p_CurrentEcho->Num())
			CurrentEchoPlayedSong = 0;

		//Loop if there is sing in the list
		GetOwner()->GetWorldTimerManager().SetTimer(EchoTimerHandle, this, &USingingComponent::PlayEcho, .1f, false, 1.0f);
	}
}


void USingingComponent::ListenOnStartSinging_Implementation(bool isSinging)
{
	return;
}

void USingingComponent::ListenOnSinging_Implementation(ESingButton singingNote)
{
	return;
}
