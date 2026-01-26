// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MinigameData.h"
#include "MinigameResultWidget.generated.h"

/**
 * Base widget for displaying minigame results (Win/Lose screens)
 * Can be inherited in Blueprint to create custom result screens
 */
UCLASS()
class GT5_PROJET_API UMinigameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Called automatically to initialize the widget with minigame result data
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Minigame Result")
	void InitializeWithResult(const FMinigameResult& Result);
	
	// String formatting helpers static
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result", meta = (CompactNodeTitle = "Format Stats"))
	static FString FormatCustomStats(const TMap<FString, FText>& CustomStats);
	
	// Button actions
	UFUNCTION(BlueprintCallable, Category = "Minigame Result")
	void OnRetryPressed();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Minigame Result")
	FMinigameResult MinigameResult;

	UPROPERTY()
	class ABaseMinigameGameMode* CachedGameMode;
};
