#include "Subsystems/SoundSubsystem.h"

#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"

DEFINE_LOG_CATEGORY(LogSound);

const FString USoundSubsystem::SFXSaveSlot = TEXT("SFXSettings");
const int32 USoundSubsystem::SFXSaveUserIndex = 0;

USoundSubsystem::USoundSubsystem()
	: MasterSFXVolume(1.0f)
	, SFXDataTable(nullptr)
{
}

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSFXState();
}

void USoundSubsystem::Deinitialize()
{
	SaveSFXState();

	for (UAudioComponent* Comp : AudioComponentPool)
	{
		if (Comp)
		{
			if (Comp->IsPlaying())
			{
				Comp->Stop();
			}
			if (Comp->IsRegistered())
			{
				Comp->DestroyComponent();
			}
		}
	}
	AudioComponentPool.Empty();
	LastPlayTimes.Empty();

	Super::Deinitialize();
}

// ==================== Playback Control ====================

void USoundSubsystem::PlaySFX2D(const FSFXData& SFXData)
{
	PlayResolvedSFX2D(NAME_None, SFXData);
}

void USoundSubsystem::PlayResolvedSFX2D(FName SFXIdentifier, const FSFXData& SFXData)
{
	if (!SFXData.IsValid())
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: Attempted to play invalid SFX data"));
		return;
	}

	if (!CanPlaySFX(SFXIdentifier, SFXData.Cooldown))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: No valid world for 2D SFX playback"));
		return;
	}

	float FinalVolume = CalculateFinalVolume(SFXData);
	float FinalPitch = CalculateRandomPitch(SFXData);

	UGameplayStatics::PlaySound2D(World, SFXData.Sound, FinalVolume, FinalPitch);

	RecordSFXPlay(SFXIdentifier);

	UE_LOG(LogSound, Verbose, TEXT("SoundSubsystem: Playing 2D SFX '%s' (Vol: %.2f, Pitch: %.2f)"),
		*SFXIdentifier.ToString(), FinalVolume, FinalPitch);
}

void USoundSubsystem::PlaySFXAtLocation(const FSFXData& SFXData, FVector Location)
{
	PlayResolvedSFXAtLocation(NAME_None, SFXData, Location);
}

void USoundSubsystem::PlayResolvedSFXAtLocation(FName SFXIdentifier, const FSFXData& SFXData, FVector Location)
{
	if (!SFXData.IsValid())
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: Attempted to play invalid SFX data at location"));
		return;
	}

	if (!CanPlaySFX(SFXIdentifier, SFXData.Cooldown))
	{
		return;
	}

	float FinalVolume = CalculateFinalVolume(SFXData);
	float FinalPitch = CalculateRandomPitch(SFXData);

	UAudioComponent* AudioComp = AcquirePooledComponent();
	if (!AudioComp)
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: No available AudioComponent in pool for '%s'"),
			*SFXIdentifier.ToString());
		return;
	}

	AudioComp->SetSound(SFXData.Sound);
	AudioComp->SetVolumeMultiplier(FinalVolume);
	AudioComp->SetPitchMultiplier(FinalPitch);
	AudioComp->bAllowSpatialization = true;
	AudioComp->bIsUISound = false;
	AudioComp->bAutoDestroy = false;
	AudioComp->SetWorldLocation(Location);

	// Configure attenuation for proper 3D spatialization
	AudioComp->bOverrideAttenuation = true;

	FSoundAttenuationSettings Attenuation;
	Attenuation.bAttenuate = true;
	Attenuation.bSpatialize = true;
	Attenuation.AttenuationShape = EAttenuationShape::Sphere;
	Attenuation.AttenuationShapeExtents = FVector(200.0f);
	Attenuation.FalloffDistance = SFXData.AttenuationRadius;
	Attenuation.DistanceAlgorithm = EAttenuationDistanceModel::Logarithmic;
	Attenuation.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

	AudioComp->AdjustAttenuation(Attenuation);

	AudioComp->Play();

	RecordSFXPlay(SFXIdentifier);

	UE_LOG(LogSound, Verbose, TEXT("SoundSubsystem: Playing 3D SFX '%s' at (%.0f, %.0f, %.0f)"),
		*SFXIdentifier.ToString(), Location.X, Location.Y, Location.Z);
}

void USoundSubsystem::PlaySFXByHandle(const FDataTableRowHandle& SFXHandle, FVector Location)
{
	FSFXData Data;
	FName SFXIdentifier = NAME_None;
	if (!ResolveSFXHandle(SFXHandle, SFXIdentifier, Data))
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: SFX handle '%s' not found in DataTable"),
			*SFXHandle.RowName.ToString());
		return;
	}

	if (Data.bIs2D)
	{
		PlayResolvedSFX2D(SFXIdentifier, Data);
	}
	else
	{
		PlayResolvedSFXAtLocation(SFXIdentifier, Data, Location);
	}
}

void USoundSubsystem::PlaySFXByHandleAtLocation(const FDataTableRowHandle& SFXHandle, FVector Location)
{
	FSFXData Data;
	FName SFXIdentifier = NAME_None;
	if (!ResolveSFXHandle(SFXHandle, SFXIdentifier, Data))
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: SFX handle '%s' not found in DataTable"),
			*SFXHandle.RowName.ToString());
		return;
	}

	PlayResolvedSFXAtLocation(SFXIdentifier, Data, Location);
}

// ==================== Volume Control ====================

void USoundSubsystem::SetSFXVolume(float Volume)
{
	MasterSFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	OnSFXVolumeChanged.Broadcast(MasterSFXVolume);

	UE_LOG(LogSound, Display, TEXT("SoundSubsystem: SFX volume set to %.2f"), MasterSFXVolume);
}

// ==================== DataTable ====================

void USoundSubsystem::SetSFXDataTable(UDataTable* DataTable)
{
	SFXDataTable = DataTable;

	if (DataTable)
	{
		UE_LOG(LogSound, Display, TEXT("SoundSubsystem: SFX DataTable assigned ('%s', %d rows)"),
			*DataTable->GetName(), DataTable->GetRowMap().Num());
	}
	else
	{
		UE_LOG(LogSound, Display, TEXT("SoundSubsystem: SFX DataTable cleared"));
	}
}

bool USoundSubsystem::GetSFXDataFromHandle(const FDataTableRowHandle& SFXHandle, FSFXData& OutData) const
{
	FName SFXIdentifier = NAME_None;
	return ResolveSFXHandle(SFXHandle, SFXIdentifier, OutData);
}

bool USoundSubsystem::ResolveSFXHandle(const FDataTableRowHandle& SFXHandle, FName& OutSFXIdentifier, FSFXData& OutData) const
{
	const UDataTable* DataTable = SFXHandle.DataTable ? SFXHandle.DataTable : SFXDataTable;
	OutSFXIdentifier = NAME_None;
	if (!DataTable)
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: No DataTable assigned for SFX lookup"));
		return false;
	}

	if (SFXHandle.RowName.IsNone())
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: Invalid empty SFX row handle"));
		return false;
	}

	const FSFXTableRow* Row = DataTable->FindRow<FSFXTableRow>(SFXHandle.RowName, TEXT("SoundSubsystem::GetSFXDataFromHandle"));
	if (!Row)
	{
		return false;
	}

	OutData = Row->SFXData;
	OutSFXIdentifier = SFXHandle.RowName;

	return true;
}

// ==================== Save/Load ====================

void USoundSubsystem::SaveSFXState()
{
	USFXSaveGame* SaveGame = Cast<USFXSaveGame>(
		UGameplayStatics::CreateSaveGameObject(USFXSaveGame::StaticClass()));

	if (!SaveGame)
	{
		UE_LOG(LogSound, Error, TEXT("SoundSubsystem: Failed to create save game object"));
		return;
	}

	SaveGame->SFXState.MasterSFXVolume = MasterSFXVolume;

	if (UGameplayStatics::SaveGameToSlot(SaveGame, SFXSaveSlot, SFXSaveUserIndex))
	{
		UE_LOG(LogSound, Display, TEXT("SoundSubsystem: SFX state saved (Volume: %.2f)"), MasterSFXVolume);
	}
	else
	{
		UE_LOG(LogSound, Error, TEXT("SoundSubsystem: Failed to save SFX state"));
	}
}

void USoundSubsystem::LoadSFXState()
{
	if (!UGameplayStatics::DoesSaveGameExist(SFXSaveSlot, SFXSaveUserIndex))
	{
		UE_LOG(LogSound, Display, TEXT("SoundSubsystem: No saved SFX state found, using defaults"));
		return;
	}

	USFXSaveGame* SaveGame = Cast<USFXSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SFXSaveSlot, SFXSaveUserIndex));

	if (!SaveGame)
	{
		UE_LOG(LogSound, Error, TEXT("SoundSubsystem: Failed to load SFX save game"));
		return;
	}

	MasterSFXVolume = SaveGame->SFXState.MasterSFXVolume;

	UE_LOG(LogSound, Display, TEXT("SoundSubsystem: SFX state loaded (Volume: %.2f)"), MasterSFXVolume);
}

// ==================== Protected Methods ====================

UAudioComponent* USoundSubsystem::AcquirePooledComponent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSound, Warning, TEXT("SoundSubsystem: No valid world for pooled AudioComponent"));
		return nullptr;
	}

	// First pass: find an inactive component already registered in the current world
	for (UAudioComponent* Comp : AudioComponentPool)
	{
		if (Comp && !Comp->IsPlaying() && Comp->IsRegistered() && Comp->GetWorld() == World)
		{
			return Comp;
		}
	}

	// Second pass: reclaim a stale component registered in a different world
	for (int32 i = 0; i < AudioComponentPool.Num(); ++i)
	{
		UAudioComponent* Comp = AudioComponentPool[i];
		if (Comp && (!Comp->IsRegistered() || Comp->GetWorld() != World))
		{
			Comp->Stop();
			Comp->DestroyComponent();
			AudioComponentPool[i] = nullptr;
		}
	}

	// Create a new component if pool isn't full
	if (AudioComponentPool.Num() < MaxPoolSize)
	{
		UAudioComponent* NewComp = NewObject<UAudioComponent>(this,
			*FString::Printf(TEXT("SFXPoolComponent_%d"), AudioComponentPool.Num()));
		if (NewComp)
		{
			NewComp->bAutoActivate = false;
			NewComp->bAutoDestroy = false;
			NewComp->RegisterComponentWithWorld(World);
			AudioComponentPool.Add(NewComp);
			return NewComp;
		}
	}

	// Pool is full: try to reuse a null slot
	for (int32 i = 0; i < AudioComponentPool.Num(); ++i)
	{
		if (AudioComponentPool[i] == nullptr)
		{
			UAudioComponent* NewComp = NewObject<UAudioComponent>(this,
				*FString::Printf(TEXT("SFXPoolComponent_%d"), i));
			if (NewComp)
			{
				NewComp->bAutoActivate = false;
				NewComp->bAutoDestroy = false;
				NewComp->RegisterComponentWithWorld(World);
				AudioComponentPool[i] = NewComp;
				return NewComp;
			}
		}
	}

	return nullptr;
}

bool USoundSubsystem::CanPlaySFX(FName SFXIdentifier, float Cooldown) const
{
	if (Cooldown <= 0.0f || SFXIdentifier.IsNone())
	{
		return true;
	}

	const double* LastTime = LastPlayTimes.Find(SFXIdentifier);
	if (!LastTime)
	{
		return true;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	double CurrentTime = World->GetTimeSeconds();
	return (CurrentTime - *LastTime) >= static_cast<double>(Cooldown);
}

void USoundSubsystem::RecordSFXPlay(FName SFXIdentifier)
{
	if (SFXIdentifier.IsNone())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	LastPlayTimes.Add(SFXIdentifier, World->GetTimeSeconds());
}

float USoundSubsystem::CalculateFinalVolume(const FSFXData& SFXData) const
{
	float Variation = 0.0f;
	if (SFXData.VolumeVariation > 0.0f)
	{
		Variation = FMath::FRandRange(-SFXData.VolumeVariation, SFXData.VolumeVariation);
	}

	float FinalVolume = SFXData.BaseVolume * MasterSFXVolume + Variation;
	return FMath::Clamp(FinalVolume, 0.0f, 1.0f);
}

float USoundSubsystem::CalculateRandomPitch(const FSFXData& SFXData) const
{
	if (FMath::IsNearlyEqual(SFXData.PitchMin, SFXData.PitchMax))
	{
		return SFXData.PitchMin;
	}

	return FMath::FRandRange(SFXData.PitchMin, SFXData.PitchMax);
}
