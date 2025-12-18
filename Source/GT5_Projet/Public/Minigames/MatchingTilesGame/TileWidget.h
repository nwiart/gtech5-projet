// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/Image.h>
#include <Components/Button.h>
#include <Components/SizeBox.h>
#include "Blueprint/UserWidget.h"
#include "TileWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTileClickedEvent, class UTileWidget*, ClickedTile);


UCLASS()
class GT5_PROJET_API UTileWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile")
    int32 TypeID;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile")
    int32 Row;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile")
    int32 Col;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UImage* TileImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    USizeBox* TileSizeBox;

    UPROPERTY(meta = (BindWidget))
    UButton* TileButton;

    UPROPERTY(BlueprintAssignable, Category = "Tile")
    FTileClickedEvent OnTileClicked;

private:
    UFUNCTION()
    void HandleClick();
};