// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/MatchingTilesGame/GridGeneratorComponent.h"


void UGridGeneratorComponent::LoadMap(FName RowName, int32& OutWidth, int32& OutHeight, TArray<int32>& OutTiles)
{
    OutTiles.Empty();
    OutWidth = 0;
    OutHeight = 0;

    if (!MapDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("MapDataTable not assigned!"));
        return;
    }

    FMapData* Row = MapDataTable->FindRow<FMapData>(RowName, TEXT(""));

    if (!Row)
    {
        UE_LOG(LogTemp, Error, TEXT("Row %s not found in DataTable"), *RowName.ToString());
        return;
    }

    OutWidth = Row->Width;
    OutHeight = Row->Height;

    const int32 CellCount = OutWidth * OutHeight;
    if (CellCount % 2 != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Map %s has an odd tile count (%d) - a matching game needs an even number of tiles."), *RowName.ToString(), CellCount);
        return;
    }

    const int32 NumTileTypes = TileTypeDataTable ? TileTypeDataTable->GetRowNames().Num() : 0;
    if (NumTileTypes <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("TileTypeDataTable not assigned or empty!"));
        return;
    }

    // Build tiles as pairs so every type used appears an even number of times, then shuffle.
    const int32 PairCount = CellCount / 2;
    OutTiles.Reserve(CellCount);
    for (int32 i = 0; i < PairCount; ++i)
    {
        const int32 TypeID = i % NumTileTypes;
        OutTiles.Add(TypeID);
        OutTiles.Add(TypeID);
    }

    for (int32 i = OutTiles.Num() - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        OutTiles.Swap(i, j);
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded map %s: Width=%d Height=%d Tiles=%d"), *RowName.ToString(), OutWidth, OutHeight, OutTiles.Num());
}