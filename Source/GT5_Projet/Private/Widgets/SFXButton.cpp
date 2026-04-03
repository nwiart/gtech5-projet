// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/SFXButton.h"

#include "Subsystems/SoundSubsystem.h"
#include "Kismet/GameplayStatics.h"

USFXButton::USFXButton()
{
}

TSharedRef<SWidget> USFXButton::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	OnHovered.AddUniqueDynamic(this, &USFXButton::HandleHovered);
	OnClicked.AddUniqueDynamic(this, &USFXButton::HandleClicked);

	return Widget;
}

void USFXButton::HandleHovered()
{
	PlaySFX(HoverSFXHandle);
}

void USFXButton::HandleClicked()
{
	PlaySFX(ClickSFXHandle);
}

void USFXButton::PlaySFX(const FDataTableRowHandle& Handle) const
{
	if (Handle.RowName.IsNone())
	{
		return;
	}

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI)
	{
		return;
	}

	USoundSubsystem* SoundSS = GI->GetSubsystem<USoundSubsystem>();
	if (!SoundSS)
	{
		return;
	}

	SoundSS->PlaySFXByHandle(Handle);
}
