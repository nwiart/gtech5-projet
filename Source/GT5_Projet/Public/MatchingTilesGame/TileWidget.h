// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/Image.h>
#include "Blueprint/UserWidget.h"
#include "TileWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTileClickedSignature, class UTileWidget*, ClickedTile);


UCLASS()
class GT5_PROJET_API UTileWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    int32 TileValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UImage* TileImage;
};