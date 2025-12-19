// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "Save/TableEvents.h"

#include "VNSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UVNSaveGame() { }

	UPROPERTY()
	TMap<FGuid, FTableEvents> Events;

	// Add more tables if needed in the form "TMap<FGuid, FTable[YourName]> [YourName];
	//  with FTable[YourName] being a struct.
};
