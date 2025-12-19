// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"

#include "Core/VNSaveGame.h"


UVNGameInstance::UVNGameInstance()
	: SaveManager(0)
{
}

void UVNGameInstance::Init()
{
	Super::Init();

	SaveManager = NewObject<UVNSaveManager>();
	SaveManager->Load();
}

void UVNGameInstance::Shutdown()
{
	SaveManager->Save();

	Super::Shutdown();
}
