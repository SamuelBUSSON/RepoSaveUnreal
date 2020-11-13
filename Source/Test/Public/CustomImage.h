// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SingingComponent.h"
#include "Components/Image.h"
#include "CustomImage.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UCustomImage : public UImage
{
	GENERATED_BODY()

public:
	//The number of notes the echo can store
	UPROPERTY(EditAnywhere, Category = Sing)
	TEnumAsByte<ESingButton> ImageNote;


	UFUNCTION(BlueprintCallable)
	inline TEnumAsByte<ESingButton> GetNote() const { return ImageNote; }

};
