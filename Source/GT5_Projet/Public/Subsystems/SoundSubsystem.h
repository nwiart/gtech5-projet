// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/SFXData.h"
#include "Data/SFXSaveData.h"

#include "SoundSubsystem.generated.h"

class UAudioComponent;
class UDataTable;

/** Log category for the Sound Subsystem */
DECLARE_LOG_CATEGORY_EXTERN(LogSound, Log, All);

/** Delegate broadcast when SFX volume changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSFXVolumeChanged,
	float,
	NewVolume);

/**
 * Sound Subsystem for managing sound effects (SFX) playback.
 *
 * Features:
 * - Play SFX in 2D (non-spatialized) or 3D (spatialized at location)
 * - Play SFX by name via DataTable lookup
 * - Master SFX volume control with persistence (save/load)
 * - Cooldown system to prevent SFX spam
 * - Pitch and volume randomization for natural variation
 * - AudioComponent pooling for efficient 3D sound playback
 */
UCLASS()
class GT5_PROJET_API USoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USoundSubsystem();

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// ==================== Playback Control ====================

	/**
	 * Play a sound effect in 2D (non-spatialized, heard everywhere).
	 * @param SFXData The SFX configuration to play
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFX2D(const FSFXData& SFXData);

	/**
	 * Play a sound effect at a specific world location (3D spatialized).
	 * @param SFXData The SFX configuration to play
	 * @param Location World location to play the sound at
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFXAtLocation(const FSFXData& SFXData, FVector Location);

	/**
	 * Play a sound effect by its name from the assigned DataTable.
	 * Uses the bIs2D flag from the DataTable entry to determine 2D or 3D playback.
	 * For 3D sounds, the Location parameter is used.
	 * @param SFXName Row name in the DataTable
	 * @param Location World location for 3D sounds (ignored for 2D)
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFXByName(FName SFXName, FVector Location = FVector::ZeroVector);

	/**
	 * Play a sound effect by its name at a specific location (always 3D).
	 * Convenience function for spatialized DataTable sounds.
	 * @param SFXName Row name in the DataTable
	 * @param Location World location to play the sound at
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFXByNameAtLocation(FName SFXName, FVector Location);

	// ==================== Volume Control ====================

	/**
	 * Set the master SFX volume.
	 * @param Volume New volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|Volume")
	void SetSFXVolume(float Volume);

	/**
	 * Get the current master SFX volume.
	 * @return Current volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sound|Volume")
	float GetSFXVolume() const { return MasterSFXVolume; }

	// ==================== DataTable ====================

	/**
	 * Assign a DataTable containing FSFXTableRow entries.
	 * This enables PlaySFXByName() and PlaySFXByNameAtLocation() lookups.
	 * @param DataTable The DataTable asset with SFX configurations
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|DataTable")
	void SetSFXDataTable(UDataTable* DataTable);

	/**
	 * Retrieve SFX data from the assigned DataTable by row name.
	 * @param SFXName Row name in the DataTable
	 * @param OutData Output SFX data
	 * @return True if the SFX was found in the DataTable
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|DataTable")
	bool GetSFXData(FName SFXName, FSFXData& OutData) const;

	// ==================== Save/Load ====================

	/**
	 * Save the current SFX volume to disk.
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|Save")
	void SaveSFXState();

	/**
	 * Load SFX volume from disk.
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|Save")
	void LoadSFXState();

	// ==================== Delegates ====================

	/** Called when SFX volume changes */
	UPROPERTY(BlueprintAssignable, Category = "Sound|Events")
	FOnSFXVolumeChanged OnSFXVolumeChanged;

protected:
	/**
	 * Get an available AudioComponent from the pool, or create a new one if needed.
	 * Used for 3D spatialized sounds that require an AudioComponent.
	 * @return An available AudioComponent, or nullptr if the pool is full
	 */
	UAudioComponent* AcquirePooledComponent();

	/**
	 * Check if the given SFX is allowed to play (cooldown check).
	 * @param SFXName The SFX identifier to check
	 * @param Cooldown The minimum interval between plays
	 * @return True if the SFX can play (cooldown elapsed or no cooldown)
	 */
	bool CanPlaySFX(FName SFXName, float Cooldown) const;

	/**
	 * Record that a SFX was just played (update cooldown timestamp).
	 * @param SFXName The SFX identifier that was played
	 */
	void RecordSFXPlay(FName SFXName);

	/**
	 * Calculate the final volume for a SFX with randomization applied.
	 * @param SFXData The SFX configuration
	 * @return Final volume (clamped 0-1)
	 */
	float CalculateFinalVolume(const FSFXData& SFXData) const;

	/**
	 * Calculate a randomized pitch for a SFX.
	 * @param SFXData The SFX configuration
	 * @return Randomized pitch value
	 */
	float CalculateRandomPitch(const FSFXData& SFXData) const;

private:
	/** Master SFX volume level (0.0 to 1.0) */
	UPROPERTY()
	float MasterSFXVolume;

	/** Assigned DataTable for SFX lookup by name */
	UPROPERTY()
	TObjectPtr<UDataTable> SFXDataTable;

	/** Pool of reusable AudioComponents for 3D spatialized sounds */
	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> AudioComponentPool;

	/** Maximum number of AudioComponents in the pool */
	static constexpr int32 MaxPoolSize = 16;

	/** Last play time for each SFX (for cooldown tracking) */
	TMap<FName, double> LastPlayTimes;

	/** Save slot name */
	static const FString SFXSaveSlot;

	/** Save user index */
	static const int32 SFXSaveUserIndex;
};
