// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/MusicTrackData.h"
#include "Data/MusicSaveData.h"

#include "MusicSubsystem.generated.h"

class UAudioComponent;

/** Delegate broadcast when the current music track changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMusicTrackChanged,
	const FMusicTrackData &,
	NewTrack);

/** Delegate broadcast when music volume changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMusicVolumeChanged,
	float,
	NewVolume);

/** Delegate broadcast when a fade operation completes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMusicFadeComplete);

/** Delegate broadcast when music starts or stops */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMusicPlayStateChanged,
	bool,
	bIsPlaying);

/**
 * Music Subsystem for managing background music playback.
 *
 * Features:
 * - Play/Stop music with fade in/out
 * - Crossfade between tracks
 * - Volume control with MetaSound parameter support
 * - Loop support (intro + loop sections via MetaSound)
 * - State persistence (save/load music state)
 */
UCLASS()
class GT5_PROJET_API UMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMusicSubsystem();

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// ==================== Playback Control ====================

	/**
	 * Play a music track with optional fade in.
	 * @param TrackData The track configuration to play
	 * @param FadeInDuration Duration of the fade in (0 for immediate)
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayMusic(const FMusicTrackData &TrackData, float FadeInDuration = 0.0f);

	/**
	 * Stop the currently playing music with optional fade out.
	 * @param FadeOutDuration Duration of the fade out (0 for immediate)
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic(float FadeOutDuration = 0.0f);

	/**
	 * Crossfade from current track to a new track.
	 * @param NewTrackData The new track to crossfade to
	 * @param CrossfadeDuration Duration of the crossfade transition
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void CrossfadeTo(const FMusicTrackData &NewTrackData, float CrossfadeDuration = 1.0f);

	/**
	 * Pause the currently playing music.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void PauseMusic();

	/**
	 * Resume paused music.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void ResumeMusic();

	// ==================== Volume Control ====================

	/**
	 * Set the master music volume.
	 * @param Volume New volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetMusicVolume(float Volume);

	/**
	 * Get the current master music volume.
	 * @return Current volume level (0.0 to 1.0)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	float GetMusicVolume() const { return MasterVolume; }

	// ==================== MetaSound Parameters ====================

	/**
	 * Set a float parameter on the current MetaSound.
	 * Useful for controlling intro/loop transitions, intensity, etc.
	 * @param ParameterName Name of the MetaSound parameter
	 * @param Value Value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Parameters")
	void SetMusicParameter(FName ParameterName, float Value);

	/**
	 * Trigger a MetaSound trigger parameter.
	 * @param ParameterName Name of the trigger parameter
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Parameters")
	void TriggerMusicParameter(FName ParameterName);

	/**
	 * Get a stored parameter value.
	 * @param ParameterName Name of the parameter
	 * @param OutValue Output parameter value
	 * @return True if parameter exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Parameters")
	bool GetMusicParameter(FName ParameterName, float &OutValue) const;

	// ==================== State Queries ====================

	/**
	 * Check if music is currently playing.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	bool IsMusicPlaying() const;

	/**
	 * Check if music is currently paused.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	bool IsMusicPaused() const { return bIsPaused; }

	/**
	 * Check if a fade operation is in progress.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	bool IsFading() const { return bIsFading; }

	/**
	 * Get the currently playing track data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
	const FMusicTrackData &GetCurrentTrack() const { return CurrentTrack; }

	// ==================== Save/Load ====================

	/**
	 * Save the current music state to disk.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Save")
	void SaveMusicState();

	/**
	 * Load and restore music state from disk.
	 * @param bAutoPlay If true, automatically resume playback if music was playing
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Save")
	void LoadMusicState(bool bAutoPlay = true);

	/**
	 * Get the current save state (without saving to disk).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music|Save")
	FMusicSaveState GetCurrentSaveState() const;

	/**
	 * Restore from a specific save state.
	 * @param State The state to restore
	 * @param bAutoPlay If true, automatically resume playback
	 */
	UFUNCTION(BlueprintCallable, Category = "Music|Save")
	void RestoreFromSaveState(const FMusicSaveState &State, bool bAutoPlay = true);

	// ==================== Delegates ====================

	/** Called when the current track changes */
	UPROPERTY(BlueprintAssignable, Category = "Music|Events")
	FOnMusicTrackChanged OnMusicTrackChanged;

	/** Called when volume changes */
	UPROPERTY(BlueprintAssignable, Category = "Music|Events")
	FOnMusicVolumeChanged OnMusicVolumeChanged;

	/** Called when a fade operation completes */
	UPROPERTY(BlueprintAssignable, Category = "Music|Events")
	FOnMusicFadeComplete OnMusicFadeComplete;

	/** Called when music play state changes */
	UPROPERTY(BlueprintAssignable, Category = "Music|Events")
	FOnMusicPlayStateChanged OnMusicPlayStateChanged;

protected:
	/** Create or get the audio component for playback */
	UAudioComponent *GetOrCreateAudioComponent();

	/** Create or get the crossfade audio component */
	UAudioComponent *GetOrCreateCrossfadeComponent();

	/** Internal tick for fade operations */
	void TickFade(float DeltaTime);

	/** Handle fade completion */
	void OnFadeComplete();

	/** Apply current volume to audio component */
	void ApplyVolume();

	/** Timer callback for fade updates */
	UFUNCTION()
	void FadeTickCallback();

private:
	/** Main audio component for music playback */
	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponent;

	/** Secondary audio component for crossfading */
	UPROPERTY()
	TObjectPtr<UAudioComponent> CrossfadeComponent;

	/** Currently playing track data */
	UPROPERTY()
	FMusicTrackData CurrentTrack;

	/** Track data for pending crossfade */
	UPROPERTY()
	FMusicTrackData PendingTrack;

	/** Master volume level (0.0 to 1.0) */
	UPROPERTY()
	float MasterVolume;

	/** Current fade multiplier (0.0 to 1.0) */
	float FadeMultiplier;

	/** Target fade multiplier */
	float FadeTargetMultiplier;

	/** Fade speed (change per second) */
	float FadeSpeed;

	/** Stored MetaSound parameters */
	UPROPERTY()
	TMap<FName, float> StoredParameters;

	/** Timer handle for fade tick */
	FTimerHandle FadeTimerHandle;

	/** Whether music is currently paused */
	bool bIsPaused;

	/** Whether a fade operation is in progress */
	bool bIsFading;

	/** Whether we're doing a crossfade (vs simple fade) */
	bool bIsCrossfading;

	/** Whether to stop music after fade out completes */
	bool bStopAfterFade;

	/** Save slot name */
	static const FString MusicSaveSlot;

	/** Save user index */
	static const int32 MusicSaveUserIndex;
};
