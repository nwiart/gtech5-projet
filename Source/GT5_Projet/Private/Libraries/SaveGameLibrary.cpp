// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/SaveGameLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "Subsystems/VNSaveSubsystem.h"
#include "Core/VNGameInstance.h"


UVNSaveManager* USaveGameLibrary::GetSaveManager(const UObject* WorldContextObject)
{
	UVNSaveSubsystem* subsystem = UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UVNSaveSubsystem>();
	if (subsystem) {
		return subsystem->SaveManager;
	}
	return 0;
}
