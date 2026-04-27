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

	// Self-binding: OnHovered/OnClicked live on this UButton instance, so the
	// delegates and their target share lifetime. AddUniqueDynamic keeps the
	// bindings idempotent across rebuild cycles.
	OnHovered.AddUniqueDynamic(this, &USFXButton::HandleHovered);
	OnClicked.AddUniqueDynamic(this, &USFXButton::HandleClicked);

	return Widget;
}

void USFXButton::ReleaseSlateResources(bool bReleaseChildren) {
	// Paired cleanup with RebuildWidget. Not strictly required for self-bindings
	// (delegate and target die together), but kept for symmetry and clarity.
	OnHovered.RemoveDynamic(this, &USFXButton::HandleHovered);
	OnClicked.RemoveDynamic(this, &USFXButton::HandleClicked);

	Super::ReleaseSlateResources(bReleaseChildren);
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
