#include "Subsystems/MusicSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"

DEFINE_LOG_CATEGORY(LogMusic);

// Static constants
const FString UMusicSubsystem::MusicSaveSlot = TEXT("MusicSettings");
const int32 UMusicSubsystem::MusicSaveUserIndex = 0;

// Fade tick interval in seconds
static constexpr float FadeTickInterval = 0.016f; // ~60Hz

UMusicSubsystem::UMusicSubsystem()
	: MusicComponent(nullptr)
	, CrossfadeComponent(nullptr)
	, MasterVolume(1.0f)
	, FadeMultiplier(1.0f)
	, FadeTargetMultiplier(1.0f)
	, FadeSpeed(0.0f)
	, bIsPaused(false)
	, bIsFading(false)
	, bIsCrossfading(false)
	, bStopAfterFade(false)
{
}

void UMusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load saved music state on startup
	LoadMusicState(false); // Don't auto-play, let the game decide when to start music
}

void UMusicSubsystem::Deinitialize()
{
	// Save current state before shutting down
	SaveMusicState();

	// Stop any playing music
	if (MusicComponent)
	{
		if (MusicComponent->IsPlaying())
		{
			MusicComponent->Stop();
		}
		if (MusicComponent->IsRegistered())
		{
			MusicComponent->DestroyComponent();
		}
		MusicComponent = nullptr;
	}

	if (CrossfadeComponent)
	{
		if (CrossfadeComponent->IsPlaying())
		{
			CrossfadeComponent->Stop();
		}
		if (CrossfadeComponent->IsRegistered())
		{
			CrossfadeComponent->DestroyComponent();
		}
		CrossfadeComponent = nullptr;
	}

	// Clear timer
	if (FadeTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(FadeTimerHandle);
		}
	}

	Super::Deinitialize();
}

// ==================== Playback Control ====================

void UMusicSubsystem::PlayMusic(const FMusicTrackData& TrackData, float FadeInDuration)
{
	if (!TrackData.IsValid())
	{
		UE_LOG(LogMusic, Warning, TEXT("MusicSubsystem: Attempted to play invalid track data"));
		return;
	}

	// Stop any current playback
	if (IsMusicPlaying())
	{
		StopMusic(0.0f);
	}

	CurrentTrack = TrackData;

	UAudioComponent* AudioComp = GetOrCreateAudioComponent();
	if (!AudioComp)
	{
		UE_LOG(LogMusic, Error, TEXT("MusicSubsystem: Failed to create audio component"));
		return;
	}

	// Configure audio component
	AudioComp->SetSound(TrackData.Sound);
	AudioComp->bIsUISound = true; // Ensure it plays regardless of pause state
	AudioComp->bAllowSpatialization = false;
	AudioComp->bAutoDestroy = false;
	AudioComp->bIsMusic = true;

	// Apply loop setting from track data
	// For Sound Waves, set the bLooping property directly on the asset
	if (USoundWave* SoundWave = Cast<USoundWave>(TrackData.Sound))
	{
		SoundWave->bLooping = TrackData.bLoop;
	}
	// For MetaSound Sources, set the looping parameter via the audio component
	AudioComp->SetBoolParameter(FName(TEXT("Looping")), TrackData.bLoop);

	// Setup fade
	if (FadeInDuration > 0.0f)
	{
		FadeMultiplier = 0.0f;
		FadeTargetMultiplier = 1.0f;
		FadeSpeed = 1.0f / FadeInDuration;
		bIsFading = true;
		bStopAfterFade = false;

		// Start fade timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				FadeTimerHandle,
				this,
				&UMusicSubsystem::FadeTickCallback,
				FadeTickInterval,
				true);
		}
	}
	else
	{
		FadeMultiplier = 1.0f;
		bIsFading = false;
	}

	ApplyVolume();

	// Start playback
	AudioComp->Play();
	bIsPaused = false;

	// Broadcast events
	OnMusicTrackChanged.Broadcast(CurrentTrack);
	OnMusicPlayStateChanged.Broadcast(true);

	UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: Playing track '%s'"), *TrackData.TrackName.ToString());
}

void UMusicSubsystem::StopMusic(float FadeOutDuration)
{
	if (!MusicComponent || !IsMusicPlaying())
	{
		return;
	}

	if (FadeOutDuration > 0.0f)
	{
		FadeTargetMultiplier = 0.0f;
		FadeSpeed = FadeMultiplier / FadeOutDuration;
		bIsFading = true;
		bStopAfterFade = true;

		// Start fade timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				FadeTimerHandle,
				this,
				&UMusicSubsystem::FadeTickCallback,
				FadeTickInterval,
				true);
		}
	}
	else
	{
		// Immediate stop
		MusicComponent->Stop();
		FadeMultiplier = 0.0f;
		bIsFading = false;
		CurrentTrack = FMusicTrackData();

		OnMusicPlayStateChanged.Broadcast(false);
	}
}

void UMusicSubsystem::CrossfadeTo(const FMusicTrackData& NewTrackData, float CrossfadeDuration)
{
	if (!NewTrackData.IsValid())
	{
		UE_LOG(LogMusic, Warning, TEXT("MusicSubsystem: Attempted crossfade to invalid track"));
		return;
	}

	// If nothing is playing, just play normally
	if (!IsMusicPlaying())
	{
		PlayMusic(NewTrackData, CrossfadeDuration);
		return;
	}

	// Setup crossfade
	PendingTrack = NewTrackData;
	bIsCrossfading = true;

	// Get or create crossfade component
	UAudioComponent* CrossfadeComp = GetOrCreateCrossfadeComponent();
	if (!CrossfadeComp)
	{
		UE_LOG(LogMusic, Error, TEXT("MusicSubsystem: Failed to create crossfade component"));
		PlayMusic(NewTrackData, CrossfadeDuration);
		return;
	}

	// Configure new track on crossfade component
	CrossfadeComp->SetSound(NewTrackData.Sound);
	CrossfadeComp->bIsUISound = true;
	CrossfadeComp->bAllowSpatialization = false;
	CrossfadeComp->bAutoDestroy = false;
	CrossfadeComp->bIsMusic = true;

	// Apply loop setting from track data
	if (USoundWave* SoundWave = Cast<USoundWave>(NewTrackData.Sound))
	{
		SoundWave->bLooping = NewTrackData.bLoop;
	}
	CrossfadeComp->SetBoolParameter(FName(TEXT("Looping")), NewTrackData.bLoop);

	CrossfadeComp->SetVolumeMultiplier(0.0f);
	CrossfadeComp->Play();

	// Setup fade: current fades out, new fades in
	FadeTargetMultiplier = 0.0f;
	FadeSpeed = 1.0f / CrossfadeDuration;
	bIsFading = true;
	bStopAfterFade = false;

	// Start fade timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FadeTimerHandle,
			this,
			&UMusicSubsystem::FadeTickCallback,
			FadeTickInterval,
			true);
	}

	UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: Crossfading to '%s'"), *NewTrackData.TrackName.ToString());
}

void UMusicSubsystem::PauseMusic()
{
	if (MusicComponent && MusicComponent->IsPlaying())
	{
		MusicComponent->SetPaused(true);
		bIsPaused = true;
		OnMusicPlayStateChanged.Broadcast(false);
	}
}

void UMusicSubsystem::ResumeMusic()
{
	if (MusicComponent && bIsPaused)
	{
		MusicComponent->SetPaused(false);
		bIsPaused = false;
		OnMusicPlayStateChanged.Broadcast(true);
	}
}

// ==================== Volume Control ====================

void UMusicSubsystem::SetMusicVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	ApplyVolume();

	OnMusicVolumeChanged.Broadcast(MasterVolume);
}

// ==================== MetaSound Parameters ====================

void UMusicSubsystem::SetMusicParameter(FName ParameterName, float Value)
{
	StoredParameters.Add(ParameterName, Value);

	if (MusicComponent)
	{
		MusicComponent->SetFloatParameter(ParameterName, Value);
	}
}

void UMusicSubsystem::TriggerMusicParameter(FName ParameterName)
{
	if (MusicComponent)
	{
		MusicComponent->SetTriggerParameter(ParameterName);
	}
}

bool UMusicSubsystem::GetMusicParameter(FName ParameterName, float& OutValue) const
{
	const float* FoundValue = StoredParameters.Find(ParameterName);
	if (FoundValue)
	{
		OutValue = *FoundValue;
		return true;
	}
	return false;
}

// ==================== State Queries ====================

bool UMusicSubsystem::IsMusicPlaying() const
{
	return MusicComponent && MusicComponent->IsPlaying() && !bIsPaused;
}

// ==================== Save/Load ====================

void UMusicSubsystem::SaveMusicState()
{
	UMusicSaveGame* SaveGame = Cast<UMusicSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMusicSaveGame::StaticClass()));

	if (!SaveGame)
	{
		UE_LOG(LogMusic, Error, TEXT("MusicSubsystem: Failed to create save game object"));
		return;
	}

	SaveGame->MusicState = GetCurrentSaveState();

	if (UGameplayStatics::SaveGameToSlot(SaveGame, MusicSaveSlot, MusicSaveUserIndex))
	{
		UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: Music state saved"));
	}
	else
	{
		UE_LOG(LogMusic, Error, TEXT("MusicSubsystem: Failed to save music state"));
	}
}

void UMusicSubsystem::LoadMusicState(bool bAutoPlay)
{
	if (!UGameplayStatics::DoesSaveGameExist(MusicSaveSlot, MusicSaveUserIndex))
	{
		UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: No saved music state found"));
		return;
	}

	UMusicSaveGame* SaveGame = Cast<UMusicSaveGame>(
		UGameplayStatics::LoadGameFromSlot(MusicSaveSlot, MusicSaveUserIndex));

	if (!SaveGame)
	{
		UE_LOG(LogMusic, Error, TEXT("MusicSubsystem: Failed to load music save game"));
		return;
	}

	// Restore volume (always)
	MasterVolume = SaveGame->MusicState.MasterVolume;

	// Restore parameters
	StoredParameters = SaveGame->MusicState.Parameters;

	UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: Music state loaded (Volume: %.2f)"), MasterVolume);

	// Note: We don't auto-restore track playback here because we don't have
	// access to the sound asset. The game should call RestoreFromSaveState
	// with the appropriate FMusicTrackData when ready.
}

FMusicSaveState UMusicSubsystem::GetCurrentSaveState() const
{
	FMusicSaveState State;
	State.CurrentTrackName = CurrentTrack.TrackName;
	State.MasterVolume = MasterVolume;
	State.bWasPlaying = IsMusicPlaying();
	State.Parameters = StoredParameters;

	// Get playback time if available
	if (MusicComponent && MusicComponent->IsPlaying())
	{
		// Note: Getting exact playback time requires additional setup with MetaSounds
		// For now, we store 0 and the track will restart from beginning
		State.PlaybackTime = 0.0f;
	}

	return State;
}

void UMusicSubsystem::RestoreFromSaveState(const FMusicSaveState& State, bool bAutoPlay)
{
	MasterVolume = State.MasterVolume;
	StoredParameters = State.Parameters;

	ApplyVolume();
	OnMusicVolumeChanged.Broadcast(MasterVolume);

	UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: State restored (Volume: %.2f, Track: '%s', WasPlaying: %s)"),
		MasterVolume, *State.CurrentTrackName.ToString(), State.bWasPlaying ? TEXT("true") : TEXT("false"));
}

void UMusicSubsystem::RestoreFromSaveStateWithTrack(const FMusicSaveState& State, const FMusicTrackData& TrackData, bool bAutoPlay)
{
	MasterVolume = State.MasterVolume;
	StoredParameters = State.Parameters;

	ApplyVolume();
	OnMusicVolumeChanged.Broadcast(MasterVolume);

	// Resume playback if requested and music was playing when saved
	if (bAutoPlay && State.bWasPlaying && TrackData.IsValid())
	{
		PlayMusic(TrackData, 0.0f);

		// Restore MetaSound parameters to the now-playing audio component
		if (MusicComponent)
		{
			for (const auto& Param : StoredParameters)
			{
				MusicComponent->SetFloatParameter(Param.Key, Param.Value);
			}
		}

		UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: State restored and playback resumed (Track: '%s')"),
			*TrackData.TrackName.ToString());
	}
	else
	{
		UE_LOG(LogMusic, Display, TEXT("MusicSubsystem: State restored without playback (Volume: %.2f)"), MasterVolume);
	}
}

// ==================== Protected Methods ====================

UAudioComponent* UMusicSubsystem::GetOrCreateAudioComponent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogMusic, Warning, TEXT("MusicSubsystem: No valid world available for audio component"));
		return nullptr;
	}

	if (MusicComponent && (!MusicComponent->IsRegistered() || MusicComponent->GetWorld() != World))
	{
		MusicComponent->Stop();
		MusicComponent->DestroyComponent();
		MusicComponent = nullptr;
	}

	if (!MusicComponent)
	{
		MusicComponent = NewObject<UAudioComponent>(this, TEXT("MusicComponent"));
		if (MusicComponent)
		{
			MusicComponent->bAutoActivate = false;
			MusicComponent->bAutoDestroy = false;
			MusicComponent->RegisterComponentWithWorld(World);
		}
	}
	return MusicComponent;
}

UAudioComponent* UMusicSubsystem::GetOrCreateCrossfadeComponent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogMusic, Warning, TEXT("MusicSubsystem: No valid world available for crossfade component"));
		return nullptr;
	}

	if (CrossfadeComponent && (!CrossfadeComponent->IsRegistered() || CrossfadeComponent->GetWorld() != World))
	{
		CrossfadeComponent->Stop();
		CrossfadeComponent->DestroyComponent();
		CrossfadeComponent = nullptr;
	}

	if (!CrossfadeComponent)
	{
		CrossfadeComponent = NewObject<UAudioComponent>(this, TEXT("CrossfadeComponent"));
		if (CrossfadeComponent)
		{
			CrossfadeComponent->bAutoActivate = false;
			CrossfadeComponent->bAutoDestroy = false;
			CrossfadeComponent->RegisterComponentWithWorld(World);
		}
	}
	return CrossfadeComponent;
}

void UMusicSubsystem::TickFade(float DeltaTime)
{
	if (!bIsFading)
	{
		return;
	}

	// Update fade multiplier
	if (FadeMultiplier < FadeTargetMultiplier)
	{
		FadeMultiplier = FMath::Min(FadeMultiplier + FadeSpeed * DeltaTime, FadeTargetMultiplier);
	}
	else if (FadeMultiplier > FadeTargetMultiplier)
	{
		FadeMultiplier = FMath::Max(FadeMultiplier - FadeSpeed * DeltaTime, FadeTargetMultiplier);
	}

	// Apply volume
	ApplyVolume();

	// Handle crossfade - update the other component inversely
	if (bIsCrossfading && CrossfadeComponent)
	{
		float CrossfadeVolume = (1.0f - FadeMultiplier) * PendingTrack.BaseVolume * MasterVolume;
		CrossfadeComponent->SetVolumeMultiplier(CrossfadeVolume);
	}

	// Check if fade is complete
	if (FMath::IsNearlyEqual(FadeMultiplier, FadeTargetMultiplier, 0.001f))
	{
		OnFadeComplete();
	}
}

void UMusicSubsystem::OnFadeComplete()
{
	bIsFading = false;

	// Clear timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTimerHandle);
	}

	if (bIsCrossfading)
	{
		// Crossfade complete: swap components and stop old one
		if (MusicComponent)
		{
			MusicComponent->Stop();
		}

		// Swap: crossfade becomes main
		UAudioComponent* Temp = MusicComponent;
		MusicComponent = CrossfadeComponent;
		CrossfadeComponent = Temp;

		// Update current track
		CurrentTrack = PendingTrack;
		PendingTrack = FMusicTrackData();

		// Reset fade multiplier for the new track
		FadeMultiplier = 1.0f;
		ApplyVolume();

		bIsCrossfading = false;

		OnMusicTrackChanged.Broadcast(CurrentTrack);
	}
	else if (bStopAfterFade)
	{
		// Fade out complete: stop music
		if (MusicComponent)
		{
			MusicComponent->Stop();
		}

		CurrentTrack = FMusicTrackData();
		bStopAfterFade = false;

		OnMusicPlayStateChanged.Broadcast(false);
	}

	OnMusicFadeComplete.Broadcast();
}

void UMusicSubsystem::ApplyVolume()
{
	if (MusicComponent)
	{
		float FinalVolume = CurrentTrack.BaseVolume * MasterVolume * FadeMultiplier;
		MusicComponent->SetVolumeMultiplier(FinalVolume);
	}
}

void UMusicSubsystem::FadeTickCallback()
{
	TickFade(FadeTickInterval);
}
