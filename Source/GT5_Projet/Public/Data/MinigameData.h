// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinigameData.generated.h"

/**
 * Result data structure for minigames
 * Contains all information needed to display results and update main game state
 */
USTRUCT(BlueprintType)
struct FMinigameResult
{
	GENERATED_BODY()

	// Success/Failure
	UPROPERTY(BlueprintReadWrite, Category = "Result")
	bool bSuccess = false;

	// Connection Score Delta (how much to add/subtract from main game connection score)
	UPROPERTY(BlueprintReadWrite, Category = "Result")
	int32 ConnectionScoreDelta = 0;

	// Minigame Name
	UPROPERTY(BlueprintReadWrite, Category = "Result")
	FString MinigameName;

	// Custom Statistics (e.g., "Critical Hits", "Time Taken", "Accuracy")
	UPROPERTY(BlueprintReadWrite, Category = "Result")
	TMap<FString, FText> CustomStats;

	FMinigameResult()
		: bSuccess(false)
		, ConnectionScoreDelta(0)
		, MinigameName(TEXT(""))
	{}
};

/**
 * Objective definition for minigames
 */
USTRUCT(BlueprintType)
struct FMinigameObjective
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objective")
	FText ObjectiveText;

	UPROPERTY(BlueprintReadWrite, Category = "Objective")
	bool bIsCompleted = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objective")
	bool bIsOptional = false;

	FMinigameObjective()
		: ObjectiveText(FText::FromString(TEXT("")))
		, bIsCompleted(false)
		, bIsOptional(false)
	{}
};
