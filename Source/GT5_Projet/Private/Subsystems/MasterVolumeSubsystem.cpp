#include "Subsystems/MasterVolumeSubsystem.h"

#include "AudioDevice.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogMasterVolume);

const FString UMasterVolumeSubsystem::MasterVolumeSaveSlot = TEXT("MasterVolumeSettings");
const int32 UMasterVolumeSubsystem::MasterVolumeSaveUserIndex = 0;

UMasterVolumeSubsystem::UMasterVolumeSubsystem()
	: MasterVolume(1.0f)
{
}

void UMasterVolumeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadMasterVolumeState();
}

void UMasterVolumeSubsystem::Deinitialize()
{
	SaveMasterVolumeState();

	Super::Deinitialize();
}

// ==================== Volume Control ====================

void UMasterVolumeSubsystem::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	ApplyMasterVolume();

	OnMasterVolumeChanged.Broadcast(MasterVolume);

	UE_LOG(LogMasterVolume, Display, TEXT("MasterVolumeSubsystem: Master volume set to %.2f"), MasterVolume);
}

void UMasterVolumeSubsystem::ApplyMasterVolume()
{
	// Prefer the world's audio device; fall back to the engine's main device
	// (e.g. during early startup before a world has been created).
	FAudioDeviceHandle DeviceHandle;
	if (UWorld* World = GetWorld())
	{
		DeviceHandle = World->GetAudioDevice();
	}
	if (!DeviceHandle.IsValid() && GEngine)
	{
		DeviceHandle = GEngine->GetMainAudioDevice();
	}

	if (DeviceHandle.IsValid())
	{
		DeviceHandle->SetTransientPrimaryVolume(MasterVolume);
	}
	else
	{
		UE_LOG(LogMasterVolume, Warning, TEXT("MasterVolumeSubsystem: No audio device available to apply master volume"));
	}
}

// ==================== Save/Load ====================

void UMasterVolumeSubsystem::SaveMasterVolumeState()
{
	UMasterVolumeSaveGame* SaveGame = Cast<UMasterVolumeSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMasterVolumeSaveGame::StaticClass()));

	if (!SaveGame)
	{
		UE_LOG(LogMasterVolume, Error, TEXT("MasterVolumeSubsystem: Failed to create save game object"));
		return;
	}

	SaveGame->MasterVolumeState.MasterVolume = MasterVolume;

	if (UGameplayStatics::SaveGameToSlot(SaveGame, MasterVolumeSaveSlot, MasterVolumeSaveUserIndex))
	{
		UE_LOG(LogMasterVolume, Display, TEXT("MasterVolumeSubsystem: Master volume saved (Volume: %.2f)"), MasterVolume);
	}
	else
	{
		UE_LOG(LogMasterVolume, Error, TEXT("MasterVolumeSubsystem: Failed to save master volume"));
	}
}

void UMasterVolumeSubsystem::LoadMasterVolumeState()
{
	if (!UGameplayStatics::DoesSaveGameExist(MasterVolumeSaveSlot, MasterVolumeSaveUserIndex))
	{
		UE_LOG(LogMasterVolume, Display, TEXT("MasterVolumeSubsystem: No saved master volume found, using default"));
		ApplyMasterVolume();
		return;
	}

	UMasterVolumeSaveGame* SaveGame = Cast<UMasterVolumeSaveGame>(
		UGameplayStatics::LoadGameFromSlot(MasterVolumeSaveSlot, MasterVolumeSaveUserIndex));

	if (!SaveGame)
	{
		UE_LOG(LogMasterVolume, Error, TEXT("MasterVolumeSubsystem: Failed to load master volume save game"));
		return;
	}

	MasterVolume = FMath::Clamp(SaveGame->MasterVolumeState.MasterVolume, 0.0f, 1.0f);

	ApplyMasterVolume();

	UE_LOG(LogMasterVolume, Display, TEXT("MasterVolumeSubsystem: Master volume loaded (Volume: %.2f)"), MasterVolume);
}
