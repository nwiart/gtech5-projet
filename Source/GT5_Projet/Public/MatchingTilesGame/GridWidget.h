// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileWidget.h"
#include "GridGeneratorComponent.h"
#include "MatchingTileGame.h"
#include "Components/GridPanel.h"
#include <Components/SizeBox.h>
#include <Components/UniformGridPanel.h>
#include "Blueprint/UserWidget.h"
#include "GridWidget.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UGridWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
    UDataTable* MapDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UGridGeneratorComponent* Generator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MapRowName;

    virtual void NativeConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
    TSubclassOf<UTileWidget> TileWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
    UDataTable* TileTypeDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector2D GridSize = FVector2D(800.f, 600.f);

private:
    void BuildGrid(int32 Width, int32 Height, const TArray<int32>& Tiles);

    UFUNCTION()
    void OnTileClicked(UTileWidget* clickedTile);

    UPROPERTY(meta = (BindWidget))
    USizeBox* GridSizeBox;

    UPROPERTY(meta = (BindWidget))
    UGridPanel* GridPanel;
};
