// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FrameBreakerResultScreen.generated.h"

class AFrameBreakerGameMode;

/**
 * Combined Result Screen Widget for Frame Breaker Minigame
 * Displays victory or defeat based on game state
 * Shows remaining knives, frames destroyed, and connection reward
 */
UCLASS()
class GT5_PROJET_API UFrameBreakerResultScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// Reference to game mode (cached for performance)
	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	AFrameBreakerGameMode* GameMode;

	// Check win/lose state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	bool IsVictory() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	bool IsDefeat() const;

	// Header text based on result
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	FText GetHeaderText() const;

	// Data display functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	FText GetRemainingKnivesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	FText GetFramesDestroyedText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	FText GetConnectionRewardText() const;

	// Individual stat getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	int32 GetKnivesRemaining() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	int32 GetFramesDestroyed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	int32 GetTotalFrames() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frame Breaker|Result Screen")
	int32 GetConnectionDelta() const;
};
