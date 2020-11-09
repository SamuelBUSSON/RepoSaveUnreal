// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterController.h"
#include "Camera/CameraComponent.h"
#include "SingingComponent.generated.h"

UENUM(BlueprintType)
enum ESingButton
{
	Earth,
	Wind,
	Fire,
	Water,
	Aether,
	None
};


class FOnEchoSinging;
class FOnSinging;
class FOnStartSingingDelegate;

class ACharacterController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEchoSinging, ESingButton, SingNote);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSinging, ESingButton, SingNote);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartSingingDelegate, bool, StartSinging);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API USingingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USingingComponent();

	/**
	* Delegate called when the player is in singign mode
	* @param SingNote The note he pressed
	*/
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnSinging OnSingingDelegate;

	/** Delegate called when the player start and stop singing
	* @param StartSinging True when the player enter the singing mode false when he leaves the singing mode
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FOnStartSingingDelegate StartSingingDelegate;

	/**
	* Delegate called when the player validate the echo and the echo loop
	* @param SingNote The note the echo is playing
	*/
	UPROPERTY(BlueprintCallable, Category = "Event")
	FOnEchoSinging OnEchoSinging;


	/** Delegate listener called when the player start and stop singing
	*@param isSinging True when the player enter the singing mode false when he leaves the singing mode
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ListenOnStartSinging(bool isSinging);

	/* Delegate listener called when the player is in singing mode
	*@param singingNote The note the player played
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ListenOnSinging(ESingButton singingNote);


	void PlayEcho();
	void PlaySong(ESingButton songPlayed);
	void ValidateEcho();
	void StartSinging();
	void SetController(ACharacterController* Controller);


	FTimerHandle EchoTimerHandle;
	int32 CurrentEchoPlayedSong;

	ACharacterController* pMyController;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Sing)
	int32 NotesNumber;


	int32 CurrentNumberOfNotesInEcho;

	bool bIsSinging;

	TSharedPtr<TArray<ESingButton>> p_CurrentEcho;



private:





public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	inline bool IsSinging() const { return bIsSinging; }


	UFUNCTION(BlueprintCallable)
	inline int32 GetCurrentNote() const { return CurrentNumberOfNotesInEcho; }


	UFUNCTION(BlueprintCallable)
	inline int32 GetNumberOfNotes() const { return NotesNumber; }
};
