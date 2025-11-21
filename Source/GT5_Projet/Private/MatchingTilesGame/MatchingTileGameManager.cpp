// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchingTilesGame/MatchingTileGameManager.h"

// Sets default values
AMatchingTileGameManager::AMatchingTileGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMatchingTileGameManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMatchingTileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMatchingTileGameManager::InitializeGame(const TArray<UTileWidget*>& InTiles, int32 InWidth, int32 InHeight)
{
    Tiles = InTiles;
    Width = InWidth;
    Height = InHeight;
}

void AMatchingTileGameManager::OnTileClicked(UTileWidget* ClickedTile)
{
    if (!FirstSelectedTile)
    {
        FirstSelectedTile = ClickedTile;
        UE_LOG(LogTemp, Log, TEXT("First tile selected: %d (%d,%d)"), ClickedTile->TypeID, ClickedTile->Row, ClickedTile->Col);
        return;
    }

    if (!SecondSelectedTile && ClickedTile != FirstSelectedTile)
    {
        SecondSelectedTile = ClickedTile;
        UE_LOG(LogTemp, Log, TEXT("Second tile selected: %d (%d,%d)"), ClickedTile->TypeID, ClickedTile->Row, ClickedTile->Col);
        CheckMatch();
    }
}

void AMatchingTileGameManager::CheckMatch()
{
    if (!FirstSelectedTile || !SecondSelectedTile)
        return;

    if (FirstSelectedTile->TypeID == SecondSelectedTile->TypeID)
    {
        if (AreTilesConnected(FirstSelectedTile, SecondSelectedTile))
        {
            UE_LOG(LogTemp, Log, TEXT("Tiles match and are connected!"));
            ClearMatchedTiles();
            // anim
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Tiles match but not connected!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Tiles do not match."));
    }

    FirstSelectedTile = nullptr;
    SecondSelectedTile = nullptr;
}

bool AMatchingTileGameManager::AreTilesConnected(UTileWidget* TileA, UTileWidget* TileB)
{
    int32 dx = FMath::Abs(TileA->Col - TileB->Col);
    int32 dy = FMath::Abs(TileA->Row - TileB->Row);

    return (dx <= 1 && dy <= 1);
}

void AMatchingTileGameManager::ClearMatchedTiles()
{
    if (FirstSelectedTile && SecondSelectedTile)
    {
        if (FirstSelectedTile->IsValidLowLevel()) FirstSelectedTile->RemoveFromParent();
        if (SecondSelectedTile->IsValidLowLevel()) SecondSelectedTile->RemoveFromParent();

        Tiles.Remove(FirstSelectedTile);
        Tiles.Remove(SecondSelectedTile);
    }
}


