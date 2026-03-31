// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "SceneTransitionSubsystem.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgress, float, Percent);

UCLASS()
class GT5_PROJET_API USceneTransitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Loading Events")
	FOnLoadingStateChanged OnLoadingStarted; // Started

	UPROPERTY(BlueprintAssignable, Category = "Loading Events")
	FOnLoadingStateChanged OnLoadingFinished; // Finished

	UPROPERTY(BlueprintAssignable, Category = "Loading Events")
	FOnLoadingProgress OnLoadingProgressUpdated; // Progress %

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UWorld> CurrentLevel;

	// --- Main Function ---

	/**
	 * Starts the loading process.
	 * @param LevelToLoad : The "Soft" reference of the level.
	 * @param WidgetClass : The visual to use (your WBP).
	 */
	UFUNCTION(BlueprintCallable, Category = "Scene Management")
	void LoadLevelAsync(TSoftObjectPtr<UWorld> LevelToLoad, TSubclassOf<UUserWidget> WidgetClass);

	/**
	* Starts the loading process for the first level, usually a main menu.
	* @param LevelToLoad : The "Soft" reference of the level.
	*/
	UFUNCTION(BlueprintCallable, Category = "Scene Management")
	void LoadStartLevel(TSoftObjectPtr<UWorld> LevelToLoad);

private:
	// Internal variables to store state
	TSoftObjectPtr<UWorld> PendingLevel;
	UPROPERTY()
	UUserWidget* CurrentWidget;
	FTimerHandle ProgressTimerHandle;

	// Internal functions
	void StartActualLoading();
	void CheckLoadProgress() const;

	UFUNCTION()
	void OnLoadCompleted();

	UFUNCTION()
	void OnLevelShown();
};
