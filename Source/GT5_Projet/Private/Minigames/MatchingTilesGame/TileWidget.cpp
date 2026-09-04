// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/MatchingTilesGame/TileWidget.h"
#include "Components/Image.h"
#include <Components/SizeBox.h>
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SoundSubsystem.h"
#include "Animation/WidgetAnimation.h"

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

    if (DisappearAnim)
    {
        FWidgetAnimationDynamicEvent FinishedDelegate;
        FinishedDelegate.BindDynamic(this, &UTileWidget::OnDisappearAnimationFinished);
        BindToAnimationFinished(DisappearAnim, FinishedDelegate);
    }
}

void UTileWidget::HandleClick()
{
    if (USoundSubsystem* SoundSubsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<USoundSubsystem>())
    {
        SoundSubsys->PlaySFXByHandle(ClickSFX);
    }

    OnTileClicked.Broadcast(this);
}

void UTileWidget::PlayDisappearAnimation()
{
    if (DisappearAnim)
    {
        PlayAnimation(DisappearAnim);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UTileWidget::OnDisappearAnimationFinished()
{
    SetVisibility(ESlateVisibility::Hidden);
}