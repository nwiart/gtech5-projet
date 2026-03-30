// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "SFXData.generated.h"

class USoundBase;

/**
 * Data structure representing a sound effect configuration.
 * Used by the Sound Subsystem to play and manage SFX.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FSFXData
{
	GENERATED_BODY()

	FSFXData()
		: Sound(nullptr)
		, BaseVolume(1.0f)
		, PitchMin(1.0f)
		, PitchMax(1.0f)
		, VolumeVariation(0.0f)
		, Cooldown(0.0f)
		, bIs2D(true)
		, AttenuationRadius(1000.0f)
	{
	}

	/** The sound asset to play (Sound Wave, Sound Cue, or MetaSound Source) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	TObjectPtr<USoundBase> Sound;

	/** Base volume level (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseVolume;

	/** Minimum pitch for randomization (1.0 = normal pitch) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX|Randomization", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float PitchMin;

	/** Maximum pitch for randomization (1.0 = normal pitch) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX|Randomization", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float PitchMax;

	/** Random volume variation applied on each play (+/- this value) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX|Randomization", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float VolumeVariation;

	/** Minimum time in seconds between two plays of this SFX (0 = no cooldown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX|Cooldown", meta = (ClampMin = "0.0"))
	float Cooldown;

	/** Whether this SFX plays as 2D (non-spatialized) by default */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	bool bIs2D;

	/** Attenuation radius for 3D spatialized sounds (in Unreal units) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX|Spatialization", meta = (ClampMin = "0.0", EditCondition = "!bIs2D"))
	float AttenuationRadius;

	/** Check if this SFX data is valid (has a sound assigned) */
	bool IsValid() const
	{
		return Sound != nullptr;
	}

	bool operator==(const FSFXData& Other) const
	{
		return Sound == Other.Sound
			&& BaseVolume == Other.BaseVolume
			&& PitchMin == Other.PitchMin
			&& PitchMax == Other.PitchMax
			&& VolumeVariation == Other.VolumeVariation
			&& Cooldown == Other.Cooldown
			&& bIs2D == Other.bIs2D
			&& AttenuationRadius == Other.AttenuationRadius;
	}

	bool operator!=(const FSFXData& Other) const
	{
		return !(*this == Other);
	}
};

/**
 * DataTable row structure for SFX configuration.
 * Allows defining SFX in a DataTable for centralized management.
 * Row Name is used as the typed SFX identifier for DataTable row handles.
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FSFXTableRow : public FTableRowBase
{
	GENERATED_BODY()

	/** SFX configuration data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	FSFXData SFXData;
};
