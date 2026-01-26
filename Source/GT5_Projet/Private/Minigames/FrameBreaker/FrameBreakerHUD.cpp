// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/FrameBreakerHUD.h"
#include "Minigames/FrameBreaker/FrameBreakerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/VNChapterSubsystem.h"

void UFrameBreakerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache reference to game mode
	UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
	GameMode = Cast<AFrameBreakerGameMode>(subsys->MinigameManager);

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("FrameBreakerHUD: Could not find FrameBreakerGameMode!"));
	}
}

void UFrameBreakerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

int32 UFrameBreakerHUD::GetKnivesRemaining() const
{
	return GameMode ? GameMode->KnivesRemaining : 0;
}

int32 UFrameBreakerHUD::GetTotalKnives() const
{
	return GameMode ? GameMode->TotalKnives : 0;
}

int32 UFrameBreakerHUD::GetFramesDestroyed() const
{
	return GameMode ? GameMode->FramesDestroyed : 0;
}

int32 UFrameBreakerHUD::GetTotalFrames() const
{
	return GameMode ? GameMode->TotalFrames : 0;
}

float UFrameBreakerHUD::GetKnivesPercent() const
{
	if (!GameMode || GameMode->TotalKnives == 0)
		return 0.0f;

	return (float)GameMode->KnivesRemaining / (float)GameMode->TotalKnives;
}

float UFrameBreakerHUD::GetFramesPercent() const
{
	if (!GameMode || GameMode->TotalFrames == 0)
		return 0.0f;

	return (float)GameMode->FramesDestroyed / (float)GameMode->TotalFrames;
}

FText UFrameBreakerHUD::GetKnivesText() const
{
	if (!GameMode)
		return FText::FromString(TEXT("0/0"));

	return FText::FromString(FString::Printf(TEXT("%d/%d"),
		GameMode->KnivesRemaining,
		GameMode->TotalKnives));
}

FText UFrameBreakerHUD::GetFramesText() const
{
	if (!GameMode)
		return FText::FromString(TEXT("0/0"));

	return FText::FromString(FString::Printf(TEXT("%d/%d"),
		GameMode->FramesDestroyed,
		GameMode->TotalFrames));
}
