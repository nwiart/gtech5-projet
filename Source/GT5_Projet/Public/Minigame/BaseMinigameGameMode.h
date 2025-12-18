// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MinigameData.h"
#include "BaseMinigameGameMode.generated.h"

/**
 * Base class for all minigames, providing shared win/lose screen functionality
 * and standardized result handling
 */
UCLASS(Abstract)
class GT5_PROJET_API ABaseMinigameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseMinigameGameMode();

	// Win/Lose Screen Management
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> WinScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> LoseScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	// Display Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FString MinigameName = TEXT("Minigame");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText MinigameDescription;

	// Game State
	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	bool bIsMinigameActive;

	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	FMinigameResult CurrentResult;

	// Standardized Win/Lose handling
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowWinScreen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLoseScreen();

	// Called when minigame completes (success or failure)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Minigame")
	void OnMinigameComplete(bool bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Minigame")
	FMinigameResult BuildMinigameResult(bool bSuccess);

	// Common actions
	UFUNCTION(BlueprintCallable, Category = "Game")
	virtual void RestartMinigame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	virtual void ReturnToMainGame();

protected:
	virtual void BeginPlay() override;

	// Helper to show any widget
	class UUserWidget* ShowWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 10);

	UPROPERTY()
	class UUserWidget* CurrentResultWidget;
};
