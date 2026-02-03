// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/MinigameResultWidget.h"
#include "Minigames/BaseMinigameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/VNChapterSubsystem.h"

void UMinigameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
	CachedGameMode = Cast<ABaseMinigameManager>(subsys->MinigameManager);
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

void UMinigameResultWidget::OnRetryPressed()
{
	if (CachedGameMode)
	{
		CachedGameMode->RestartMinigame();
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
