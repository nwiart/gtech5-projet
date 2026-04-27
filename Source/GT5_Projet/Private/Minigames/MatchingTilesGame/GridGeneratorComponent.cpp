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

    TArray<FString> Split;
    Row->TilesString.ParseIntoArray(Split, TEXT(","), true);

    for (FString& S : Split)
    {
        OutTiles.Add(FCString::Atoi(*S));
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded map %s: Width=%d Height=%d Tiles=%d"), *RowName.ToString(), OutWidth, OutHeight, OutTiles.Num());
}