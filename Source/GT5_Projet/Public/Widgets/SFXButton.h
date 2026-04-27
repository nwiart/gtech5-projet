// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"

#include "SFXButton.generated.h"

/**
 * Button subclass that automatically plays SFX on hover and click
 * using the SoundSubsystem and DataTable row handles.
 *
 * Drop-in replacement for UButton — just set the row handles in the details panel.
 */
UCLASS()
class GT5_PROJET_API USFXButton : public UButton
{
	GENERATED_BODY()

public:
	USFXButton();

	/** DataTable row handle for the hover SFX (from DT_SFX) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX",
		meta = (RowType = "/Script/GT5_Projet.SFXTableRow"))
	FDataTableRowHandle HoverSFXHandle;

	/** DataTable row handle for the click/pressed SFX (from DT_SFX) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX",
		meta = (RowType = "/Script/GT5_Projet.SFXTableRow"))
	FDataTableRowHandle ClickSFXHandle;

protected:
	/** Bind hover/click handlers once the underlying SButton exists. */
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/** Unbind handlers when the SButton is torn down (paired with RebuildWidget). */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	/** Called when the button is hovered */
	UFUNCTION()
	void HandleHovered();

	/** Called when the button is clicked */
	UFUNCTION()
	void HandleClicked();

	/** Play a SFX from a DataTable row handle via the SoundSubsystem */
	void PlaySFX(const FDataTableRowHandle& Handle) const;
};
