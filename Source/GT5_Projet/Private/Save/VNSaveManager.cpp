// Fill out your copyright notice in the Description page of Project Settings.

#include "Save/VNSaveManager.h"

#include "Save/VNSaveGame.h"
#include "Kismet/GameplayStatics.h"


UVNSaveManager::UVNSaveManager()
	: SaveGame(0)
{

}

void UVNSaveManager::Load()
{
	SaveGame = (UVNSaveGame*) UGameplayStatics::LoadGameFromSlot("Slot", 0);
	if (SaveGame == NULL) {
		SaveGame = NewObject<UVNSaveGame>(this);
	}
}

void UVNSaveManager::Save()
{
	UGameplayStatics::SaveGameToSlot(SaveGame, "Slot", 0);
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
	SaveGame->Events.Add(Guid, Data);
}
