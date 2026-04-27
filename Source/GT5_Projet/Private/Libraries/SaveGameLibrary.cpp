// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/SaveGameLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "Subsystems/VNSaveSubsystem.h"
#include "Core/VNGameInstance.h"


UVNSaveGame* USaveGameLibrary::GetSaveGame(const UObject* WorldContextObject)
{
	UVNSaveSubsystem* subsystem = UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVNSaveSubsystem>();
	if (!subsystem || !subsystem->SaveGame) {
		UE_LOG(LogTemp, Warning, TEXT("Non-existent save subsystem or save game object! Be sure that the save is loaded before calling USaveGameLibrary::GetSaveGame()."));
		return 0;
	}

	return subsystem->SaveGame;
}
