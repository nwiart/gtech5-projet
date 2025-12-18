// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/MatchingTilesGame/TileWidget.h"
#include "Components/Image.h"
#include <Components/SizeBox.h>
#include "Blueprint/WidgetTree.h"

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
    if (!TileButton)
    {
        TileButton = WidgetTree->ConstructWidget<UButton>(UImage::StaticClass(), TEXT("TileButton"));
        TileButton->AddChild(TileSizeBox);
    }

    if (!WidgetTree->RootWidget)
        WidgetTree->RootWidget = TileButton;

     TileButton->OnClicked.AddDynamic(this, &UTileWidget::HandleClick);
}

void UTileWidget::HandleClick()
{
    OnTileClicked.Broadcast(this);
}