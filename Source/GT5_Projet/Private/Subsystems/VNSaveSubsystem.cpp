// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/VNSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Core/VNGameInstance.h"


/*static*/ const FString UVNSaveSubsystem::SLOT_NAME = "Slot";


UVNSaveSubsystem::UVNSaveSubsystem()
	: SaveGame(0)
{

}

void UVNSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	this->Load(this);
}

void UVNSaveSubsystem::Deinitialize()
{
	
}


void UVNSaveSubsystem::Load(const UObject* WorldContextObject)
{
	UVNGameInstance* gameInstance = Cast<UVNGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (gameInstance == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Game instance not found!"));
		return;
	}

	if (SaveGame != NULL) {
		SaveGame->ConditionalBeginDestroy();
	}

	SaveGame = (UVNSaveGame*) UGameplayStatics::LoadGameFromSlot(SLOT_NAME, 0);

	if (SaveGame == NULL || SaveGame->GetClass() != gameInstance->SaveGameClass) {
		SaveGame = NewObject<UVNSaveGame>(this, gameInstance->SaveGameClass);
	}
}

void UVNSaveSubsystem::Save()
{
	if (SaveGame == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("SaveGame was null! Did you forget to Load() the save?"));
		return;
	}

	UGameplayStatics::SaveGameToSlot(SaveGame, SLOT_NAME, 0);
}

bool UVNSaveSubsystem::DeleteSave()
{
	bool res = UGameplayStatics::DeleteGameInSlot(SLOT_NAME, 0);
	if (res && SaveGame != NULL) {
		Load(this);
	}
	return res;
}

bool UVNSaveSubsystem::SaveExists() const
{
	return UGameplayStatics::DoesSaveGameExist(SLOT_NAME, 0);
}

const UClass* UVNSaveSubsystem::GetSaveGameClass() const
{
	return SaveGame ? SaveGame->GetClass() : 0;
}

FTableEvents UVNSaveSubsystem::GetEventData(const FGuid& Guid)
{
	if (SaveGame == NULL) {
		return FTableEvents();
	}

	FTableEvents* value = SaveGame->Events.Find(Guid);
	if (value != NULL) return *value;
	return FTableEvents();
}

void UVNSaveSubsystem::SetEventData(const FGuid& Guid, const FTableEvents& Data)
{
	if (SaveGame == NULL) {
		return;
	}

	SaveGame->Events.Add(Guid, Data);
}
