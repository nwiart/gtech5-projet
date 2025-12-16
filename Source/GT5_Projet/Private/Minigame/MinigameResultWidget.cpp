// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigame/MinigameResultWidget.h"
#include "Minigame/BaseMinigameGameMode.h"
#include "Kismet/GameplayStatics.h"

void UMinigameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

FString UMinigameResultWidget::FormatCustomStats(const TMap<FString, FText>& CustomStats)
{
	FString Result;

	for (const TPair<FString, FText>& StatPair : CustomStats)
	{
		Result += FString::Printf(TEXT("%s: %s\n"), *StatPair.Key, *StatPair.Value.ToString());
	}

	return Result;
}
