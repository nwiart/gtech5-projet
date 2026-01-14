// Fill out your copyright notice in the Description page of Project Settings.

#include "Save/VNSaveSubsystem.h"


UVNSaveSubsystem::UVNSaveSubsystem()
{

}

void UVNSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SaveManager = NewObject<UVNSaveManager>();
	SaveManager->Load(this);
}

void UVNSaveSubsystem::Deinitialize()
{
	SaveManager = 0;
}
