// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VNChapterSubsystem.h"

#include "Kismet/GameplayStatics.h"


UVNChapterSubsystem::UVNChapterSubsystem()
{

}

void UVNChapterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UVNChapterSubsystem::Deinitialize()
{
}


bool UVNChapterSubsystem::OpenChapter(FName ChapterName, const UObject* WorldContextObject)
{
	if (!CurrentChapterName.IsNone()) {
		UE_LOG(LogTemp, Warning, TEXT("A chapter is already open (or the game's internal state is confused)."));
		return false;
	}

	UGameplayStatics::OpenLevel(WorldContextObject, ChapterName);

	Connection = 0;

	CurrentChapterName = ChapterName;
	LastMinigameGuid.Invalidate();
	return true;
}

void UVNChapterSubsystem::ModifyConnection(int32 Delta)
{
	Connection = FMath::Clamp(Connection + Delta, ConnectionMinValue, ConnectionMaxValue);
	OnConnectionChanged.Broadcast(Connection);
}

void UVNChapterSubsystem::TriggerMinigame(FGuid MapEventGuid)
{
	LastMinigameGuid = MapEventGuid;
}

void UVNChapterSubsystem::NotifyChapterComplete()
{
	CurrentChapterName = NAME_None;
}


void UVNChapterSubsystem::GetConnectionData(
	int32& OutMin,
	int32& OutMax,
	int32& OutValue
) const
{
	OutMin = ConnectionMinValue;
	OutMax = ConnectionMaxValue;
	OutValue = Connection;
}
