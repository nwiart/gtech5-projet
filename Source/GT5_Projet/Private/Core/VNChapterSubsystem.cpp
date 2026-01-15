// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/VNChapterSubsystem.h"

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

	CurrentChapterName = ChapterName;
	LastMinigameGuid.Invalidate();
	return true;
}

void UVNChapterSubsystem::TriggerMinigame(FGuid MapEventGuid)
{
	LastMinigameGuid = MapEventGuid;
}

void UVNChapterSubsystem::NotifyChapterComplete()
{
	CurrentChapterName = NAME_None;
}
