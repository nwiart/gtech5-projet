// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinigameData.h"
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

	// Blueprint-accessible getters for result data
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	bool WasSuccessful() const { return MinigameResult.bSuccess; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	FString GetMinigameName() const { return MinigameResult.MinigameName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	int32 GetConnectionScoreDelta() const { return MinigameResult.ConnectionScoreDelta; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	float GetCompletionPercentage() const { return MinigameResult.CompletionPercentage; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	int32 GetCustomStat(const FString& StatName, bool& bFound) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	FText GetCustomTextData(const FString& Key, bool& bFound) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	bool IsObjectiveCompleted(const FString& ObjectiveName, bool& bFound) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	TMap<FString, int32> GetAllCustomStats() const { return MinigameResult.CustomStats; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Minigame Result")
	TMap<FString, bool> GetAllObjectives() const { return MinigameResult.ObjectivesCompleted; }

	// Button actions
	UFUNCTION(BlueprintCallable, Category = "Minigame Result")
	void OnContinuePressed();

	UFUNCTION(BlueprintCallable, Category = "Minigame Result")
	void OnRetryPressed();

	UFUNCTION(BlueprintCallable, Category = "Minigame Result")
	void OnReturnToMainGamePressed();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Minigame Result")
	FMinigameResult MinigameResult;

	UPROPERTY()
	class ABaseMinigameGameMode* CachedGameMode;
};
