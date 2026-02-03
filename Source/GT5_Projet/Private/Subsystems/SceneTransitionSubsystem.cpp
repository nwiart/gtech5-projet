// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SceneTransitionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"

void USceneTransitionSubsystem::LoadLevelAsync(const TSoftObjectPtr<UWorld> LevelToLoad, const TSubclassOf<UUserWidget> WidgetClass)
{
    if (LevelToLoad.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadLevelAsync : Empty level reference!"));
        return;
    }

    PendingLevel = LevelToLoad;

    // Create the loading UI (if provided) and keep it above other widgets.
    if (WidgetClass && !CurrentWidget)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport(9999); // High Z-order so it stays on top.
        }
    }

    // Let the UI play its intro animation (fade in).
    OnLoadingStarted.Broadcast();

    // Small delay so the fade-in starts before streaming potentially causes a tiny hitch.
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USceneTransitionSubsystem::StartActualLoading, 1.0f, false);
}

void USceneTransitionSubsystem::StartActualLoading()
{
    const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(PendingLevel.ToString()));

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = "OnLoadCompleted"; // Must match the UFUNCTION name exactly.
    LatentInfo.UUID = 696969;
    LatentInfo.Linkage = 0;

    // Non-blocking streaming to avoid freezing the game thread.
    UGameplayStatics::LoadStreamLevel(this, LevelName, false, false, LatentInfo);

    // Lightweight polling to feed the loading UI.
    GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, this, &USceneTransitionSubsystem::CheckLoadProgress, 0.1f, true);
}

void USceneTransitionSubsystem::CheckLoadProgress() const
{
    if (PendingLevel.IsNull())
        return;

    float Percent = GetAsyncLoadPercentage(FName(*FPackageName::ObjectPathToPackageName(PendingLevel.ToString())));

    // Can return -1 if already cached/complete: normalize to 100%.
    if (Percent < 0)
        Percent = 1.0f;

    OnLoadingProgressUpdated.Broadcast(Percent);
}

void USceneTransitionSubsystem::OnLoadCompleted()
{
    CurrentLevel = PendingLevel;

    GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);

    // Always send a final 100% (useful if the last poll didn't hit).
    OnLoadingProgressUpdated.Broadcast(1.0f);

    const FName NewLevelName = PendingLevel.GetLongPackageFName();
    if (ULevelStreaming *LevelStream = UGameplayStatics::GetStreamingLevel(this, NewLevelName))
    {
        LevelStream->SetShouldBeVisible(true);
    }

    // Prevent stacking streamed levels in memory: unload everything except the target.
    for (const TArray<ULevelStreaming*>& StreamingLevels = GetWorld()->GetStreamingLevels(); ULevelStreaming* LevelStream : StreamingLevels)
    {
        if (LevelStream && LevelStream->IsLevelLoaded() && LevelStream->PackageNameToLoad != NewLevelName)
        {
            const FLatentActionInfo LatentInfo;
            UGameplayStatics::UnloadStreamLevel(this, LevelStream->PackageNameToLoad, LatentInfo, false);
        }
    }

    // Let the UI play its outro animation (fade out).
    OnLoadingFinished.Broadcast();

    // The widget handles its own visual teardown; we only drop the C++ reference.
    CurrentWidget = nullptr;
}
