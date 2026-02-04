// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNChapterManager.h"

#include "Kismet/GameplayStatics.h"

#include "Libraries/VNTileMapLibrary.h"


AVNChapterManager::AVNChapterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVNChapterManager::BeginPlay()
{
	Super::BeginPlay();

	// Place map character at spawn location.
	// TODO : Hardcoded character height offset (60.0).
	FIntPoint playerSpawnPosition = FIntPoint::ZeroValue;
	FVector characterLocation = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(playerSpawnPosition) + FVector(0, 0, 60.0);

	if (PawnCameraClass != NULL) {
		PawnCamera = GetWorld()->SpawnActor<APawnIsometric>(PawnCameraClass);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Pawn Camera Class was not specified. No pawn will be spawned in."));
		PawnCamera = 0;
	}

	if (CharacterClass != NULL) {
		MapCharacter = GetWorld()->SpawnActor<AVNMapCharacter>(CharacterClass, characterLocation, FRotator());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Character Class was not specified. No map character will be spawned in."));
		MapCharacter = 0;
	}

	if (PawnCamera) {
		PawnCamera->PlayerCharacter = MapCharacter;
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

void AVNChapterManager::Disable_Implementation()
{
	if (PawnCamera) {
		PawnCamera->SetCursorActive(false);
	}

	if (MapCharacter) {
		MapCharacter->Disable();
	}
}
