// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VNChapterSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Core/VNGameInstance.h"
#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/SceneTransitionSubsystem.h"


UVNChapterSubsystem::UVNChapterSubsystem()
{

}

void UVNChapterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	USceneTransitionSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<USceneTransitionSubsystem>();
	subsys->OnLoadingFinished.AddDynamic(this, &UVNChapterSubsystem::OnLevelLoadingDone);
}

void UVNChapterSubsystem::Deinitialize()
{
}


bool UVNChapterSubsystem::OpenChapter(const FChapterData& ChapterData)
{
	if (!CurrentChapterName.IsNone()) {
		UE_LOG(LogTemp, Error, TEXT("A chapter is already open (or the game's internal state is confused)."));
		return false;
	}

	CurrentChapterName = FName(ChapterData.Title);
	CurrentChapterLevel = ChapterData.Level;
	LastMinigameGuid.Invalidate();

	ChapterManager = GetWorld()->SpawnActor<AVNChapterManager>(ChapterData.ManagerClass);
	ChapterManager->Enable();

	Connection = 0;

	return true;
}

void UVNChapterSubsystem::CloseChapter()
{
	ChapterManager->Destroy();
	ChapterManager = 0;

	CurrentChapterName = NAME_None;
	CurrentChapterLevel.Reset();
}

bool UVNChapterSubsystem::InitializeMinigame(TSubclassOf<ABaseMinigameManager> ManagerClass, TSubclassOf<APawn> PawnClass, const UObject* WorldContextObject)
{
	ChapterManager->Disable();

	MinigameManager = GetWorld()->SpawnActor<ABaseMinigameManager>(ManagerClass);
	MinigamePawn = GetWorld()->SpawnActor<APawn>(PawnClass);

	APlayerController* pc = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	pc->Possess(MinigamePawn);

	MinigameManager->Initialize();

	return true;
}

void UVNChapterSubsystem::ExitMinigame(const UObject* WorldContextObject)
{
	MinigameManager->Destroy();
	MinigameManager = 0;

	if (MinigamePawn) {
		MinigamePawn->Destroy();
		MinigamePawn = 0;
	}

	ChapterManager->Enable();
}


void UVNChapterSubsystem::ModifyConnection(int32 Delta)
{
	Connection = FMath::Clamp(Connection + Delta, ConnectionMinValue, ConnectionMaxValue);
	OnConnectionChanged.Broadcast(Connection);
}

void UVNChapterSubsystem::TriggerMinigame(const FMinigameData& MinigameData, FGuid MapEventGuid)
{
	ScheduledMinigame = MinigameData;
	LastMinigameGuid = MapEventGuid;

	if (ChapterManager) {
		ChapterManager->TriggerMinigame(MinigameData);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Tried to launch minigame, but no chapter manager is spawned in!"));
	}
}

void UVNChapterSubsystem::NotifyChapterComplete()
{
	CurrentChapterName = NAME_None;
	CurrentChapterLevel.Reset();
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


void UVNChapterSubsystem::OnLevelLoadingDone()
{
	USceneTransitionSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<USceneTransitionSubsystem>();

	// If loading back into our current chapter level (likely after completing a minigame).
	if (subsys->CurrentLevel == CurrentChapterLevel) {
		ChapterManager->Enable();
	}
	// If loading the chapter level in (from chapter selection).
	else if (subsys->CurrentLevel == ScheduledChapter.Level) {
		OpenChapter(ScheduledChapter);

		// Hide cursor initially when spawning in.
		if (GetPawn()) {
			GetPawn()->SetCursorHidden(true);
		}
	}
}
