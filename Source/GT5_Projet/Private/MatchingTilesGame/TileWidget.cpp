// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Image.h"
#include <Components/SizeBox.h>
#include "Blueprint/WidgetTree.h"
#include "MatchingTilesGame/TileWidget.h"

void UTileWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!TileSizeBox)
    {
        TileSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("TileSizeBox"));
    }

    if (!TileImage)
    {
        TileImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("TileImage"));
        TileSizeBox->AddChild(TileImage);
    }

    if (!WidgetTree->RootWidget)
        WidgetTree->RootWidget = TileSizeBox;
}
