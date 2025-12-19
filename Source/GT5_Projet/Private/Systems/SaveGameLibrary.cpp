// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/SaveGameLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "Core/VNGameInstance.h"


UVNSaveManager* USaveGameLibrary::GetSaveManager(const UObject* WorldContextObject)
{
	UVNGameInstance* gameInstance = Cast<UVNGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (gameInstance) {
		return gameInstance->SaveManager;
	}
	return 0;
}
