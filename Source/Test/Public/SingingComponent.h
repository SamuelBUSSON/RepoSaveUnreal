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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSingingPercent, float, Position);

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
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnStartSingingDelegate StartSingingDelegate;

	/**
	* Delegate called when the player validate the echo and the echo loop
	* @param SingNote The note the echo is playing
	*/
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnEchoSinging OnEchoSinging;
	
	/**
	* Delegate called when the player validate the echo and the echo loop
	* @param SingNote The note the echo is playing
	*/
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnSingingPercent OnEchoSingingBarPercent;


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

	/* Add note to the echo if the player is in singing mode
	*@param songPlayed The input the player pressed => Note that will be added to the echo
	*/
	void PlaySong(ESingButton songPlayed);

	/* Called when the player pressed the validate input
	*/
	void ValidateEcho();

	/** Play Echo once the player validates his echo
	*/
	void PlayEcho();
	
	/**
	* Start or stop singing on input pressed
	*/
	void StartSinging();

	
	/**
	* Start holding the singing button in order to move the bar
	*/
	void StartSingingHold();

	
	/**
	*  Stop holding the singing button
	*/
	void StopSingingHold();

	/**
	* Set the character controller to call function from ACharacterController
	*/
	void SetController(ACharacterController* Controller);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//The number of notes the echo can store
	UPROPERTY(EditAnywhere, Category = Sing)
	int32 NotesNumber;
	
	//The number of notes the echo can store
	UPROPERTY(EditAnywhere, Category = Sing)
	float BarOffsetSpeed;
	
	//The offset of the bat in ui
	UPROPERTY(EditAnywhere, Category = UI)
	float BarOffset;
	
	//The offset of the bat in ui
	UPROPERTY(EditAnywhere, Category = UI)
	float UISize;

	//The offset of the bat in ui
	UPROPERTY(EditAnywhere, Category = UI)
	float UIImageSize;

	
	//The offset of the bat in ui
	UPROPERTY()
	int CurrentColumn;	
	
	float BarPosition;	
	float OldPercent;

	

	TSharedPtr<TArray<ESingButton>> p_CurrentEcho;
	int32 CurrentNumberOfNotesInEcho;
	int32 CurrentEchoPlayedSong;
	FTimerHandle EchoTimerHandle;

	ACharacterController* PMyController;

	bool bIsSinging;
	bool bIsHoldingSingingButton;
	bool bCanSwitchNote;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	inline bool IsSinging() const { return bIsSinging; }


	UFUNCTION(BlueprintCallable)
	inline int32 GetCurrentNote() const { return CurrentNumberOfNotesInEcho; }

	UFUNCTION(BlueprintCallable)
	inline int32 GetNumberOfNotes() const { return NotesNumber; }
	
	UFUNCTION(BlueprintCallable)
	inline float GetBarOffset() const { return BarOffset; }
	
	UFUNCTION(BlueprintCallable)
	void SetUISize(float NewSize);

	UFUNCTION(BlueprintCallable)
	void SetUIImageSize(float ImageSize);
	
	UFUNCTION(BlueprintCallable)
    void SetBarPosition(float NewPosition); 
};
