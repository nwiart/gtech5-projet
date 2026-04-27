// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "MusicSaveData.generated.h"

/**
 * Save data structure for music state persistence.
 * Stores current track, volume, and MetaSound parameters.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FMusicSaveState
{
	GENERATED_BODY()

	FMusicSaveState()
		: CurrentTrackName(NAME_None)
		, MasterVolume(1.0f)
		, PlaybackTime(0.0f)
		, bWasPlaying(false)
	{
	}

	/** Name of the currently playing track */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Music")
	FName CurrentTrackName;

	/** Master volume level (0.0 to 1.0) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Music")
	float MasterVolume;

	/** Current playback time in seconds (for resuming) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Music")
	float PlaybackTime;

	/** Whether music was playing when saved */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Music")
	bool bWasPlaying;

	/** MetaSound parameters state (parameter name -> value) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Music")
	TMap<FName, float> Parameters;

	/** Check if there is valid state to restore */
	bool HasValidState() const
	{
		return !CurrentTrackName.IsNone();
	}

	void Reset()
	{
		CurrentTrackName = NAME_None;
		MasterVolume = 1.0f;
		PlaybackTime = 0.0f;
		bWasPlaying = false;
		Parameters.Empty();
	}
};

/**
 * SaveGame class for music system persistence.
 * Saved separately from main game save for audio preferences.
 */
UCLASS()
class GT5_PROJET_API UMusicSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMusicSaveGame()
		: SlotName(TEXT("MusicSettings"))
		, UserIndex(0)
	{
	}

	/** Current music state */
	UPROPERTY()
	FMusicSaveState MusicState;

	/** Default slot name for saving */
	UPROPERTY()
	FString SlotName;

	/** User index for saving */
	UPROPERTY()
	int32 UserIndex;
};
