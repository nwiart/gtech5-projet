// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystems/SoundSubsystem.h"


UVNGameInstance::UVNGameInstance()
{
}

void UVNGameInstance::Init()
{
	Super::Init();

	if (USoundSubsystem* SoundSubsys = GetSubsystem<USoundSubsystem>())
	{
		SoundSubsys->SetSFXDataTable(SFXDataTable);
	}
}

void UVNGameInstance::Shutdown()
{
	Super::Shutdown();
}
