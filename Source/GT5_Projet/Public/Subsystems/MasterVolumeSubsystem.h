#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/MasterVolumeSaveData.h"

#include "MasterVolumeSubsystem.generated.h"

/** Log category for the Master Volume Subsystem */
DECLARE_LOG_CATEGORY_EXTERN(LogMasterVolume, Log, All);

/** Delegate broadcast when the master volume changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMasterVolumeChanged,
	float,
	NewVolume);

/**
 * Master Volume Subsystem ("Volume Principal").
 *
 * Controls the global master output volume that scales the entire audio mix
 * (music, SFX and any other sound) on top of their individual group volumes.
 * Implemented via the audio device's transient primary volume.
 *
 * Features:
 * - Master volume control (0.0 to 1.0)
 * - Persistence (save/load) mirroring the Music/SFX subsystems
 */
UCLASS()
class GT5_PROJET_API UMasterVolumeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMasterVolumeSubsystem();

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// ==================== Volume Control ====================

	/**
	 * Set the master volume (scales the entire audio output).
	 * @param Volume New volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "MasterVolume")
	void SetMasterVolume(float Volume);

	/**
	 * Get the current master volume.
	 * @return Current volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MasterVolume")
	float GetMasterVolume() const { return MasterVolume; }

	// ==================== Save/Load ====================

	/**
	 * Save the current master volume to disk.
	 */
	UFUNCTION(BlueprintCallable, Category = "MasterVolume|Save")
	void SaveMasterVolumeState();

	/**
	 * Load the master volume from disk and apply it.
	 */
	UFUNCTION(BlueprintCallable, Category = "MasterVolume|Save")
	void LoadMasterVolumeState();

	// ==================== Delegates ====================

	/** Called when the master volume changes */
	UPROPERTY(BlueprintAssignable, Category = "MasterVolume|Events")
	FOnMasterVolumeChanged OnMasterVolumeChanged;

protected:
	/** Push the current master volume to the active audio device. */
	void ApplyMasterVolume();

private:
	/** Master volume level (0.0 to 1.0) */
	UPROPERTY()
	float MasterVolume;

	/** Save slot name */
	static const FString MasterVolumeSaveSlot;

	/** Save user index */
	static const int32 MasterVolumeSaveUserIndex;
};
