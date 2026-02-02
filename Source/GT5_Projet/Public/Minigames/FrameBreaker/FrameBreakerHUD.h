// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FrameBreakerHUD.generated.h"

class AFrameBreakerManager;

/**
 * HUD Widget for Frame Breaker Minigame
 * Displays knives remaining and frames destroyed during gameplay
 */
UCLASS()
class GT5_PROJET_API UFrameBreakerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// Reference to game mode (cached for performance)
	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	AFrameBreakerManager* GameMode;

	// Data binding functions (callable from Blueprint)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	int32 GetKnivesRemaining() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	int32 GetTotalKnives() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	int32 GetFramesDestroyed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	int32 GetTotalFrames() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	float GetKnivesPercent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	float GetFramesPercent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	FText GetKnivesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|HUD")
	FText GetFramesText() const;
};
