// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/VNChapterManager.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/GameInstance.h"

#include "Core/PawnIsometric.h"
#include "Map/VNMapCharacter.h"
#include "Map/VNMapBounds.h"
#include "Libraries/UtilsLibrary.h"
#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/VNChapterSubsystem.h"
#include "Subsystems/CursorSubsystem.h"


AVNChapterManager::AVNChapterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVNChapterManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnChapterActors();
	PlaceCharacterAtPlayerStart();
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

	if (PawnCamera && pc) {
		pc->Possess(PawnCamera);
	}
	ConfigureCameraBounds(LevelStream);

	if (MapCharacter) {
		MapCharacter->Enable();
	}

	ConfigureMapInput(pc);
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
	GetPawn()->SetCharacterHeightLevel(SwitcherB->GetActorLocation().Z);
	ch->HeightLevel = SwitcherB->GetActorLocation().Z;
}

void AVNChapterManager::SpawnChapterActors()
{
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
		PawnCamera->SetMapCharacter(MapCharacter);
	}
}

void AVNChapterManager::PlaceCharacterAtPlayerStart()
{
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

void AVNChapterManager::ConfigureCameraBounds(ULevelStreaming* LevelStream)
{
	if (PawnCamera) {
		TArray<AVNMapBounds*> bounds; UUtilsLibrary::GetActorsOfClassInStreamedLevel<AVNMapBounds>(bounds, LevelStream, this);

		PawnCamera->SetMapBounds(bounds.IsEmpty() ? 0 : bounds[0]);
		PawnCamera->SetCursorActive(true);
	}
}

void AVNChapterManager::ConfigureMapInput(APlayerController* PlayerController) const
{
	if (PlayerController) {
		PlayerController->bEnableClickEvents = true;
	}
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this)) {
		if (UCursorSubsystem* CursorSubsys = GameInstance->GetSubsystem<UCursorSubsystem>()) {
			CursorSubsys->SetMode(ECursorMode::Free);
		}
	}
}
