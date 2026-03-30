// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "SFXSaveData.generated.h"

/**
 * Save data structure for SFX state persistence.
 * Stores the master SFX volume between sessions.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FSFXSaveState
{
	GENERATED_BODY()

	FSFXSaveState()
		: MasterSFXVolume(1.0f)
	{
	}

	/** Master SFX volume level (0.0 to 1.0) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "SFX")
	float MasterSFXVolume;

	void Reset()
	{
		MasterSFXVolume = 1.0f;
	}
};

/**
 * SaveGame class for SFX system persistence.
 * Saved separately from main game save for audio preferences.
 */
UCLASS()
class GT5_PROJET_API USFXSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USFXSaveGame()
		: SlotName(TEXT("SFXSettings"))
		, UserIndex(0)
	{
	}

	/** Current SFX state */
	UPROPERTY()
	FSFXSaveState SFXState;

	/** Default slot name for saving */
	UPROPERTY()
	FString SlotName;

	/** User index for saving */
	UPROPERTY()
	int32 UserIndex;
};
