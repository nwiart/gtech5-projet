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
 * - Play SFX from typed DataTable row handles
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
	 * Play a sound effect from a typed DataTable row handle.
	 * Uses the bIs2D flag from the DataTable entry to determine 2D or 3D playback.
	 * If the handle does not specify a DataTable, the subsystem DataTable is used.
	 * @param SFXHandle Typed handle to a FSFXTableRow entry
	 * @param Location World location for 3D sounds (ignored for 2D)
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFXByHandle(const FDataTableRowHandle& SFXHandle, FVector Location = FVector::ZeroVector);

	/**
	 * Play a sound effect from a typed DataTable row handle at a specific location (always 3D).
	 * If the handle does not specify a DataTable, the subsystem DataTable is used.
	 * @param SFXHandle Typed handle to a FSFXTableRow entry
	 * @param Location World location to play the sound at
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|SFX")
	void PlaySFXByHandleAtLocation(const FDataTableRowHandle& SFXHandle, FVector Location);

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
	 * This enables typed row handle lookups when the handle does not specify a DataTable.
	 * @param DataTable The DataTable asset with SFX configurations
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|DataTable")
	void SetSFXDataTable(UDataTable* DataTable);

	/**
	 * Retrieve SFX data from a typed DataTable row handle.
	 * If the handle does not specify a DataTable, the subsystem DataTable is used.
	 * @param SFXHandle Typed handle to a FSFXTableRow entry
	 * @param OutData Output SFX data
	 * @return True if the SFX row was found
	 */
	UFUNCTION(BlueprintCallable, Category = "Sound|DataTable")
	bool GetSFXDataFromHandle(const FDataTableRowHandle& SFXHandle, FSFXData& OutData) const;

	/**
	 * Resolve a typed row handle into SFX configuration and its logical identifier.
	 * C++ helper used by playback and tests; not exposed to Blueprint.
	 * @param SFXHandle Typed handle to a FSFXTableRow entry
	 * @param OutSFXIdentifier Resolved row identifier used for cooldown/logging
	 * @param OutData Output SFX data payload
	 * @return True if the SFX row was found
	 */
	bool ResolveSFXHandle(const FDataTableRowHandle& SFXHandle, FName& OutSFXIdentifier, FSFXData& OutData) const;

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
	 * Play a 2D SFX with an optional logical identifier for cooldown/logging.
	 * @param SFXIdentifier Resolved row identifier, or NAME_None for direct payload playback
	 * @param SFXData The SFX configuration to play
	 */
	void PlayResolvedSFX2D(FName SFXIdentifier, const FSFXData& SFXData);

	/**
	 * Play a 3D SFX with an optional logical identifier for cooldown/logging.
	 * @param SFXIdentifier Resolved row identifier, or NAME_None for direct payload playback
	 * @param SFXData The SFX configuration to play
	 * @param Location World location to play the sound at
	 */
	void PlayResolvedSFXAtLocation(FName SFXIdentifier, const FSFXData& SFXData, FVector Location);

	/**
	 * Check if the given SFX is allowed to play (cooldown check).
	 * @param SFXIdentifier The SFX identifier to check
	 * @param Cooldown The minimum interval between plays
	 * @return True if the SFX can play (cooldown elapsed or no cooldown)
	 */
	bool CanPlaySFX(FName SFXIdentifier, float Cooldown) const;

	/**
	 * Record that a SFX was just played (update cooldown timestamp).
	 * @param SFXIdentifier The SFX identifier that was played
	 */
	void RecordSFXPlay(FName SFXIdentifier);

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

	/** Assigned DataTable used as the default source for typed row handle lookups */
	UPROPERTY()
	TObjectPtr<UDataTable> SFXDataTable;

	/** Pool of reusable AudioComponents for 3D spatialized sounds */
	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> AudioComponentPool;

	/** Maximum number of AudioComponents in the pool */
	static constexpr int32 MaxPoolSize = 16;

	/** Last play time for each resolved row identifier (for cooldown tracking) */
	TMap<FName, double> LastPlayTimes;

	/** Save slot name */
	static const FString SFXSaveSlot;

	/** Save user index */
	static const int32 SFXSaveUserIndex;
};
