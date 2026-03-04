// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNChapterManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "Map/VNMapBounds.h"
#include "Libraries/UtilsLibrary.h"
#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/VNChapterSubsystem.h"


AVNChapterManager::AVNChapterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVNChapterManager::BeginPlay()
{
	Super::BeginPlay();

	// Spawn camera pawn and map character (distinct actors!)
	if (PawnCameraClass != NULL) {
		PawnCamera = GetWorld()->SpawnActor<APawnIsometric>(PawnCameraClass);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Pawn Camera Class was not specified. No pawn will be spawned in."));
		PawnCamera = 0;
	}

	if (CharacterClass != NULL) {
		MapCharacter = GetWorld()->SpawnActor<AVNMapCharacter>(CharacterClass);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Character Class was not specified. No map character will be spawned in."));
		MapCharacter = 0;
	}

	if (PawnCamera) {
		PawnCamera->PlayerCharacter = MapCharacter;
	}

	// Place map character at spawn location.
	// TODO : Hardcoded character height offset (60.0).
	if (MapCharacter) {
		FIntPoint playerSpawnPosition = FIntPoint::ZeroValue;

		UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
		ULevelStreaming* LevelStream = UGameplayStatics::GetStreamingLevel(this, subsys->CurrentChapterLevel.GetLongPackageFName());

		TArray<APlayerStart*> starts; UUtilsLibrary::GetActorsOfClassInStreamedLevel<APlayerStart>(starts, LevelStream, this);

		if (starts.Num() >= 1) {
			if (starts.Num() > 1) {
				UE_LOG(LogTemp, Warning, TEXT("%d player starts found. An arbitrary one will be chosen."), starts.Num());
			}

			FVector pos = starts[0]->GetActorLocation();
			playerSpawnPosition = UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(pos);

			UE_LOG(LogTemp, Warning, TEXT("Chose player start at world position (%lf, %lf)."), pos.X, pos.Y);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No player starts found. Defaulting to (0, 0, 0)."));
		}

		FVector location = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(playerSpawnPosition) + FVector(0, 0, 60.0);
		MapCharacter->SetActorLocation(location);
	}
}

void AVNChapterManager::EndPlay(EEndPlayReason::Type Reason)
{
	if (MapCharacter) {
		MapCharacter->Destroy();
	}

	if (PawnCamera) {
		PawnCamera->Destroy();
	}

	Super::EndPlay(Reason);
}

void AVNChapterManager::Enable_Implementation()
{
	APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0);
	UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
	ULevelStreaming* LevelStream = UGameplayStatics::GetStreamingLevel(this, subsys->CurrentChapterLevel.GetLongPackageFName());

	// Possess pawn.
	if (PawnCamera) {
		pc->Possess(PawnCamera);

		// Get map bounds actor.
		TArray<AVNMapBounds*> bounds; UUtilsLibrary::GetActorsOfClassInStreamedLevel<AVNMapBounds>(bounds, LevelStream, this);

		PawnCamera->MapBounds = bounds.IsEmpty() ? 0 : bounds[0];
		PawnCamera->SetCursorActive(true);
	}

	// Show map character.
	if (MapCharacter) {
		MapCharacter->Enable();
	}

	// Configure mouse input.
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	pc->bShowMouseCursor = true;
	pc->bEnableClickEvents = true;
	pc->SetInputMode(inputMode);
}

void AVNChapterManager::Disable_Implementation()
{
	if (PawnCamera) {
		PawnCamera->SetCursorActive(false);
	}

	if (MapCharacter) {
		MapCharacter->Disable();
	}
}

void AVNChapterManager::TravelOnHeightSwitcher_Implementation(AActor* SwitcherA, AActor* SwitcherB)
{
	AVNMapCharacter* ch = GetMapCharacter();

	// Teleport character to destination switcher (on the tile in front of it).
	const FVector Loc = ch->GetActorLocation() - SwitcherA->GetActorLocation() + SwitcherB->GetActorLocation() + (SwitcherB->GetActorForwardVector() * -100.0);
	ch->SetActorLocation(Loc);

	// Set plane height.
	GetPawn()->CharacterHeightLevel = SwitcherB->GetActorLocation().Z;
	ch->HeightLevel = SwitcherB->GetActorLocation().Z;
}
