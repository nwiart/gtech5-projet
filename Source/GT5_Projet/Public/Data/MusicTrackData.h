// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MusicTrackData.generated.h"

class USoundBase;

/**
 * Data structure representing a music track configuration.
 * Used by the Music Subsystem to play and manage background music.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FMusicTrackData
{
	GENERATED_BODY()

	FMusicTrackData()
		: Sound(nullptr)
		, TrackName(NAME_None)
		, BaseVolume(1.0f)
		, bLoop(true)
	{
	}

	/** The sound asset to play (can be MetaSound Source, Sound Cue, or Sound Wave) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> Sound;

	/** Unique identifier name for this track */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FName TrackName;

	/** Base volume level (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseVolume;

	/** Whether this track should loop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	bool bLoop;

	/** Check if this track data is valid (has a sound assigned) */
	bool IsValid() const
	{
		return Sound != nullptr;
	}

	bool operator==(const FMusicTrackData& Other) const
	{
		return TrackName == Other.TrackName && Sound == Other.Sound;
	}

	bool operator!=(const FMusicTrackData& Other) const
	{
		return !(*this == Other);
	}
};
