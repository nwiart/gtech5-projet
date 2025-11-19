// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigame/MinigameResultWidget.h"
#include "Minigame/BaseMinigameGameMode.h"
#include "Kismet/GameplayStatics.h"

void UMinigameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get the game mode and initialize with its result data
	CachedGameMode = Cast<ABaseMinigameGameMode>(UGameplayStatics::GetGameMode(this));
	if (CachedGameMode)
	{
		InitializeWithResult(CachedGameMode->CurrentResult);
	}
}

void UMinigameResultWidget::InitializeWithResult_Implementation(const FMinigameResult& Result)
{
	MinigameResult = Result;

	UE_LOG(LogTemp, Log, TEXT("MinigameResultWidget initialized with: %s (Success: %s)"),
		*Result.MinigameName, Result.bSuccess ? TEXT("Yes") : TEXT("No"));
}

int32 UMinigameResultWidget::GetCustomStat(const FString& StatName, bool& bFound) const
{
	const int32* StatValue = MinigameResult.CustomStats.Find(StatName);
	bFound = (StatValue != nullptr);
	return bFound ? *StatValue : 0;
}

FText UMinigameResultWidget::GetCustomTextData(const FString& Key, bool& bFound) const
{
	const FText* TextValue = MinigameResult.CustomTextData.Find(Key);
	bFound = (TextValue != nullptr);
	return bFound ? *TextValue : FText::GetEmpty();
}

bool UMinigameResultWidget::IsObjectiveCompleted(const FString& ObjectiveName, bool& bFound) const
{
	const bool* CompletionStatus = MinigameResult.ObjectivesCompleted.Find(ObjectiveName);
	bFound = (CompletionStatus != nullptr);
	return bFound ? *CompletionStatus : false;
}

void UMinigameResultWidget::OnContinuePressed()
{
	if (CachedGameMode)
	{
		CachedGameMode->ReturnToMainGame();
	}

	// Remove this widget from viewport
	RemoveFromParent();
}

void UMinigameResultWidget::OnRetryPressed()
{
	if (CachedGameMode)
	{
		CachedGameMode->RestartMinigame();
	}

	// Remove this widget from viewport
	RemoveFromParent();
}

void UMinigameResultWidget::OnReturnToMainGamePressed()
{
	if (CachedGameMode)
	{
		CachedGameMode->ReturnToMainGame();
	}

	// Remove this widget from viewport
	RemoveFromParent();
}
