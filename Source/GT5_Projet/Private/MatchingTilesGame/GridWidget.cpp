// Fill out your copyright notice in the Description page of Project Settings.

#include "MatchingTilesGame/GridWidget.h"
#include "Components/GridSlot.h"

void UGridWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (GridSizeBox)
    {
        GridSizeBox->SetWidthOverride(GridSize.X);
        GridSizeBox->SetHeightOverride(GridSize.Y);
    }

    if (!Generator)
    {
        Generator = NewObject<UGridGeneratorComponent>(this);
        Generator->MapDataTable = MapDataTable;
    }

    int32 Width = 0;
    int32 Height = 0;
    TArray<int32> Tiles;

    Generator->LoadMap(MapRowName, Width, Height, Tiles);

    if (Tiles.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Map is empty!"));
        return;
    }

    BuildGrid(Width, Height, Tiles);
}

void UGridWidget::BuildGrid(int32 Width, int32 Height, const TArray<int32>& Tiles)
{
    if (!GridPanel)
    {
        UE_LOG(LogTemp, Error, TEXT("GridPanel not assigned!"));
        return;
    }

    GridPanel->ClearChildren();
    float TileW = GridSize.X / Width;
    float TileH = GridSize.Y / Height;
    float FinalTileSize = FMath::Min(TileW, TileH);
    UE_LOG(LogTemp, Log, TEXT("size %f"), FinalTileSize);

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            int index = y * Width + x;
            int TileID = Tiles[index];

            UTileWidget* Tile = CreateWidget<UTileWidget>(this, TileWidgetClass);
            Tile->TileValue = Tiles[index];
            Tile->TileSizeBox->SetWidthOverride(FinalTileSize);
            Tile->TileSizeBox->SetHeightOverride(FinalTileSize);

            FString RowNameStr = FString::Printf(TEXT("Tile_%d"), TileID);
            FTileTypeData* TileData = TileTypeDataTable->FindRow<FTileTypeData>(FName(*RowNameStr), TEXT(""));
            if (TileData && Tile->TileImage)
                Tile->TileImage->SetBrushFromTexture(TileData->TileTexture);
         

            UGridSlot* slot = GridPanel->AddChildToGrid(Tile, y, x);
            slot->SetPadding(FMargin(2.f));
            

        }
    }

    UE_LOG(LogTemp, Log, TEXT("Grid built successfully"));
}