// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "MasterVolumeSaveData.generated.h"

/**
 * Save data structure for master ("principal") volume persistence.
 * Stores the global master volume between sessions.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FMasterVolumeSaveState
{
	GENERATED_BODY()

	FMasterVolumeSaveState()
		: MasterVolume(1.0f)
	{
	}

	/** Master volume level (0.0 to 1.0) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "MasterVolume")
	float MasterVolume;

	void Reset()
	{
		MasterVolume = 1.0f;
	}
};

/**
 * SaveGame class for master volume persistence.
 * Saved separately from main game save for audio preferences.
 */
UCLASS()
class GT5_PROJET_API UMasterVolumeSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMasterVolumeSaveGame()
		: SlotName(TEXT("MasterVolumeSettings"))
		, UserIndex(0)
	{
	}

	/** Current master volume state */
	UPROPERTY()
	FMasterVolumeSaveState MasterVolumeState;

	/** Default slot name for saving */
	UPROPERTY()
	FString SlotName;

	/** User index for saving */
	UPROPERTY()
	int32 UserIndex;
};
