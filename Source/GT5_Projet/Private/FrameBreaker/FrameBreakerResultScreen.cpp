// Fill out your copyright notice in the Description page of Project Settings.

#include "FrameBreaker/FrameBreakerResultScreen.h"
#include "FrameBreaker/FrameBreakerGameMode.h"
#include "Kismet/GameplayStatics.h"

void UFrameBreakerResultScreen::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache reference to game mode
	GameMode = Cast<AFrameBreakerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("FrameBreakerResultScreen: Could not find FrameBreakerGameMode!"));
	}
}

bool UFrameBreakerResultScreen::IsVictory() const
{
	if (!GameMode)
		return false;

	return GameMode->CurrentResult.bSuccess;
}

bool UFrameBreakerResultScreen::IsDefeat() const
{
	if (!GameMode)
		return false;

	return !GameMode->CurrentResult.bSuccess;
}

FText UFrameBreakerResultScreen::GetHeaderText() const
{
	if (IsVictory())
	{
		return FText::FromString(TEXT("ALL FRAMES BROKEN!"));
	}
	else
	{
		return FText::FromString(TEXT("OUT OF KNIVES"));
	}
}

FText UFrameBreakerResultScreen::GetRemainingKnivesText() const
{
	if (!GameMode)
		return FText::FromString(TEXT("Remaining Knives: 0"));

	return FText::FromString(FString::Printf(TEXT("Remaining Knives: %d"),
		GameMode->KnivesRemaining));
}

FText UFrameBreakerResultScreen::GetFramesDestroyedText() const
{
	if (!GameMode)
		return FText::FromString(TEXT("Frames Destroyed: 0/0"));

	return FText::FromString(FString::Printf(TEXT("Frames Destroyed: %d/%d"),
		GameMode->FramesDestroyed,
		GameMode->TotalFrames));
}

FText UFrameBreakerResultScreen::GetConnectionRewardText() const
{
	if (!GameMode)
		return FText::FromString(TEXT("Reward: Connection: +0"));

	int32 ConnectionDelta = GameMode->CurrentResult.ConnectionScoreDelta;

	// Format with + or - sign
	if (ConnectionDelta >= 0)
	{
		return FText::FromString(FString::Printf(TEXT("Reward: Connection: +%d"), ConnectionDelta));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Reward: Connection: %d"), ConnectionDelta));
	}
}

int32 UFrameBreakerResultScreen::GetKnivesRemaining() const
{
	return GameMode ? GameMode->KnivesRemaining : 0;
}

int32 UFrameBreakerResultScreen::GetFramesDestroyed() const
{
	return GameMode ? GameMode->FramesDestroyed : 0;
}

int32 UFrameBreakerResultScreen::GetTotalFrames() const
{
	return GameMode ? GameMode->TotalFrames : 0;
}

int32 UFrameBreakerResultScreen::GetConnectionDelta() const
{
	return GameMode ? GameMode->CurrentResult.ConnectionScoreDelta : 0;
}
