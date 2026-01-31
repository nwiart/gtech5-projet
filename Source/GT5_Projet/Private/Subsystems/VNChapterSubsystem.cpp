// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VNChapterSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Core/VNGameInstance.h"
#include "Libraries/VNTileMapLibrary.h"


UVNChapterSubsystem::UVNChapterSubsystem()
{

}

void UVNChapterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UVNChapterSubsystem::Deinitialize()
{
}


bool UVNChapterSubsystem::OpenChapter(TSoftObjectPtr<UWorld> ChapterLevel, FName ChapterName, TSubclassOf<AActor> ManagerClass, TSubclassOf<APawnIsometric> PawnClass, TSubclassOf<AVNMapCharacter> CharacterClass, const UObject* WorldContextObject)
{
	if (!CurrentChapterName.IsNone()) {
		UE_LOG(LogTemp, Warning, TEXT("A chapter is already open (or the game's internal state is confused)."));
		return false;
	}

	// Place map character at spawn location.
	// TODO : Hardcoded character height offset (60.0).
	PlayerPosition = FIntPoint::ZeroValue;
	FVector characterLocation = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(PlayerPosition) + FVector(0, 0, 60.0);

	ManagerActor = GetWorld()->SpawnActor(ManagerClass);
	PawnCamera = GetWorld()->SpawnActor<APawnIsometric>(PawnClass);
	MapCharacter = GetWorld()->SpawnActor<AVNMapCharacter>(CharacterClass, characterLocation, FRotator());

	// Possess pawn.
	APlayerController* pc = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	pc->Possess(PawnCamera);

	// Configure mouse input.
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	pc->bShowMouseCursor = true;
	pc->bEnableClickEvents = true;
	pc->SetInputMode(inputMode);

	Connection = 0;

	CurrentChapterName = ChapterName;
	CurrentChapterLevel = ChapterLevel;
	LastMinigameGuid.Invalidate();
	return true;
}

void UVNChapterSubsystem::CloseChapter()
{
	MapCharacter->Destroy();
	MapCharacter = 0;

	PawnCamera->Destroy();
	PawnCamera = 0;

	ManagerActor->Destroy();
	ManagerActor = 0;

	CurrentChapterName = NAME_None;
	CurrentChapterLevel.Reset();
}

bool UVNChapterSubsystem::InitializeMinigame(TSubclassOf<ABaseMinigameGameMode> ManagerClass, TSubclassOf<APawn> PawnClass, const UObject* WorldContextObject)
{
	MapCharacter->SetActorHiddenInGame(true);

	MinigameManager = GetWorld()->SpawnActor<ABaseMinigameGameMode>(ManagerClass);
	MinigamePawn = GetWorld()->SpawnActor<APawn>(PawnClass);

	APlayerController* pc = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	pc->Possess(MinigamePawn);

	MinigameManager->Initialize();

	return true;
}

void UVNChapterSubsystem::ExitMinigame(const UObject* WorldContextObject)
{
	MapCharacter->SetActorHiddenInGame(false);

	APlayerController* pc = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	pc->Possess(PawnCamera);

	MinigameManager->Destroy();
	MinigameManager = 0;

	if (MinigamePawn) {
		MinigamePawn->Destroy();
		MinigamePawn = 0;
	}
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
