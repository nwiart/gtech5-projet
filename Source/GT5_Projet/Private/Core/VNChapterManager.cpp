// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNChapterManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/VNChapterSubsystem.h"


AVNChapterManager::AVNChapterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVNChapterManager::BeginPlay()
{
	Super::BeginPlay();

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


void AVNChapterManager::Enable()
{
	APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0);

	// Place map character at spawn location.
	// TODO : Hardcoded character height offset (60.0).
	if (MapCharacter) {
		FIntPoint playerSpawnPosition = FIntPoint::ZeroValue;

		UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
		ULevelStreaming* LevelStream = UGameplayStatics::GetStreamingLevel(this, subsys->CurrentChapterLevel.GetLongPackageFName());
		TArray<AActor*> starts = LevelStream->GetLoadedLevel()->Actors.FilterByPredicate([](const AActor* actor) {
			return actor && actor->GetClass() == APlayerStart::StaticClass();
		});

		if (starts.Num() >= 1) {
			if (starts.Num() > 1) {
				UE_LOG(LogTemp, Warning, TEXT("%d player starts found. An arbitrary one will be chosen."), starts.Num());
			}
			playerSpawnPosition = UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(starts[0]->GetActorLocation());
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No player starts found. Defaulting to (0, 0, 0)."));
		}

		FVector location = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(playerSpawnPosition) + FVector(0, 0, 60.0);
		MapCharacter->SetActorLocation(location);
	}

	// Possess pawn.
	if (PawnCamera) {
		pc->Possess(PawnCamera);

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

void AVNChapterManager::Disable()
{
	if (PawnCamera) {
		PawnCamera->SetCursorActive(false);
	}

	if (MapCharacter) {
		MapCharacter->Disable();
	}
}
