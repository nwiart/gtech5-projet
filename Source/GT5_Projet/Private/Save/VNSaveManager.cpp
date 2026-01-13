// Fill out your copyright notice in the Description page of Project Settings.

#include "Save/VNSaveManager.h"

#include "Save/VNSaveGame.h"
#include "Kismet/GameplayStatics.h"


UVNSaveManager::UVNSaveManager()
	: SaveGame(0)
{

}

void UVNSaveManager::Load(TSubclassOf<UVNSaveGame> saveGameClass)
{
	SaveGame = (UVNSaveGame*) UGameplayStatics::LoadGameFromSlot("Slot", 0);
	if (SaveGame == NULL) {
		SaveGame = NewObject<UVNSaveGame>(this, saveGameClass);
	}
}

void UVNSaveManager::Save()
{
	if (SaveGame == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("SaveGame was null! Did you forget to Load() the save?"));
		return;
	}

	UGameplayStatics::SaveGameToSlot(SaveGame, "Slot", 0);
}

const UClass* UVNSaveManager::GetSaveGameClass() const
{
	return SaveGame ? SaveGame->GetClass() : 0;
}

FTableEvents UVNSaveManager::GetEventData(const FGuid& Guid)
{
	if (SaveGame == NULL) {
		return FTableEvents();
	}

	FTableEvents* value = SaveGame->Events.Find(Guid);
	if (value != NULL) return *value;
	return FTableEvents();
}

void UVNSaveManager::SetEventData(const FGuid& Guid, const FTableEvents& Data)
{
	if (SaveGame == NULL) {
		return;
	}

	SaveGame->Events.Add(Guid, Data);
}
