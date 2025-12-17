// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchingTilesGame/MatchingTileGameManager.h"
#include <Kismet/GameplayStatics.h>

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

FMinigameResult AMatchingTileGameManager::BuildMinigameResult_Implementation(bool bSuccess) {
    FMinigameResult Result;
    Result.bSuccess = bSuccess;
    Result.MinigameName = "Matching Tiles";
    Result.ConnectionScoreDelta = bSuccess ? 10 : -5;

    Result.CustomStats.Add("Tiles Remaining", FText::AsNumber(Tiles.Num()));
    return Result;
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
            CheckEndConditions();
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


bool AMatchingTileGameManager::AreTilesConnected(UTileWidget* A, UTileWidget* B)
{
    /*  int32 dx = FMath::Abs(TileA->Col - TileB->Col);
   int32 dy = FMath::Abs(TileA->Row - TileB->Row);
    return (dx <= 1 && dy <= 1);*/

    if (!A || !B) return false;
    if (A->TypeID != B->TypeID) return false;

    const int ax = A->Col;
    const int ay = A->Row;
    const int bx = B->Col;
    const int by = B->Row;

    auto IsEmpty = [&](int x, int y)
        {
            for (UTileWidget* T : Tiles)
            {
                if (T->Col == x && T->Row == y)
                {
                    return T->TypeID == -1;
                }
            }
            return true;
        };

    // Adjacent check
    if (FMath::Abs(ax - bx) <= 1 && FMath::Abs(ay - by) <= 1)
        return true;

    // Same Row 
    if (ay == by)
    {
        int start = FMath::Min(ax, bx) + 1;
        int end = FMath::Max(ax, bx);
        bool clear = true;

        for (int x = start; x < end; x++)
            if (!IsEmpty(x, ay)) clear = false;

        if (clear) return true;
    }

    //  Same Column
    if (ax == bx)
    {
        int start = FMath::Min(ay, by) + 1;
        int end = FMath::Max(ay, by);
        bool clear = true;

        for (int y = start; y < end; y++)
            if (!IsEmpty(ax, y)) clear = false;

        if (clear) return true;
    }

    // Diagonal
    if (FMath::Abs(ax - bx) == FMath::Abs(ay - by))
    {
        int dx = (bx > ax) ? 1 : -1;
        int dy = (by > ay) ? 1 : -1;

        int x = ax + dx;
        int y = ay + dy;

        bool clear = true;
        while (x != bx)
        {
            if (!IsEmpty(x, y)) clear = false;
            x += dx;
            y += dy;
        }

        if (clear) return true;
    }

    return false;
}


void AMatchingTileGameManager::ClearMatchedTiles()
{
    if (FirstSelectedTile && SecondSelectedTile)
    {
        FirstSelectedTile->SetVisibility(ESlateVisibility::Hidden);
        SecondSelectedTile->SetVisibility(ESlateVisibility::Hidden);

        Tiles.Remove(FirstSelectedTile);
        Tiles.Remove(SecondSelectedTile);
    }
}


void AMatchingTileGameManager::CheckEndConditions()
{
    if (Tiles.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("You win !"));
        OnMinigameComplete(true);
        return;
    }

    if (!HasAnyPossibleMatch())
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You loose !"));
        OnMinigameComplete(false);
        return;
    }
}

bool AMatchingTileGameManager::HasAnyPossibleMatch()
{
    for (int32 i = 0; i < Tiles.Num(); i++)
    {
        for (int32 j = i + 1; j < Tiles.Num(); j++)
        {
            UTileWidget* A = Tiles[i];
            UTileWidget* B = Tiles[j];

            if (!A || !B)
                continue;

            if (A->TypeID != B->TypeID)
                continue;

            if (AreTilesConnected(A, B))
                return true;
        }
    }

    return false;
}
