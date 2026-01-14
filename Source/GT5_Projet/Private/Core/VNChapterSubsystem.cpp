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
	if (!CurrentChapterName.IsNone()) return false;

	UGameplayStatics::OpenLevel(WorldContextObject, ChapterName);

	CurrentChapterName = ChapterName;
	return true;
}

void UVNChapterSubsystem::NotifyChapterComplete()
{
	CurrentChapterName = NAME_None;
}
