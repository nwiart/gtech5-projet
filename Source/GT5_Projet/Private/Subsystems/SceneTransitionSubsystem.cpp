// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SceneTransitionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"

void USceneTransitionSubsystem::LoadLevelAsync(TSoftObjectPtr<UWorld> LevelToLoad, TSubclassOf<UUserWidget> WidgetClass)
{
    if (LevelToLoad.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadLevelAsync : Empty level reference!"));
        return;
    }

    PendingLevel = LevelToLoad;

    // 1. Create the widget (Loading screen)
    if (WidgetClass && !CurrentWidget)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport(9999); // Z-Order 9999 to be above everything
        }
    }

    // 2. SIGNAL: Notify the widget it should play its "Fade In" animation
    OnLoadingStarted.Broadcast();

    // 3. Safety delay: wait 1s so the Fade In is visible before loading
    // This avoids the small freeze that could block the animation at the start
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USceneTransitionSubsystem::StartActualLoading, 1.0f, false);
}

void USceneTransitionSubsystem::StartActualLoading()
{
    const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(PendingLevel.ToString()));

    // Callback configuration (Which function should be called when it's done?)
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = "OnLoadCompleted"; // Must match EXACTLY the name of the UFUNCTION
    LatentInfo.UUID = 696969;                         // Arbitrary unique ID
    LatentInfo.Linkage = 0;

    // Start ASYNCHRONOUS loading
    // bMakeVisibleAfterLoad = false (Keep the level invisible for now)
    // bShouldBlockOnLoad = false (This is what makes loading smooth!)
    UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, LatentInfo);

    // Start monitoring the percent (every 0.1s)
    GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, this, &USceneTransitionSubsystem::CheckLoadProgress, 0.1f, true);
}

void USceneTransitionSubsystem::CheckLoadProgress() const
{
    if (PendingLevel.IsNull())
        return;

    // Retrieve the real engine loading percentage
    float Percent = GetAsyncLoadPercentage(FName(*FPackageName::ObjectPathToPackageName(PendingLevel.ToString())));

    // If the level is already cached, GetAsyncLoadPercentage can return -1. Fix it to 100%.
    if (Percent < 0)
        Percent = 1.0f;

    // SIGNAL: Send the % to the widget for the progress bar
    OnLoadingProgressUpdated.Broadcast(Percent);
}

void USceneTransitionSubsystem::OnLoadCompleted()
{
    // 1. Stop monitoring the %
    GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
    
    // Ensure we send 100% at the end
    OnLoadingProgressUpdated.Broadcast(1.0f);

    // 2. Make the new level visible
    const FName NewLevelName = FName(*FPackageName::ObjectPathToPackageName(PendingLevel.ToString()));
    if (ULevelStreaming *LevelStream = UGameplayStatics::GetStreamingLevel(this, NewLevelName))
    {
        LevelStream->SetShouldBeVisible(true);
    }

    // Unload all other levels
    for (const TArray<ULevelStreaming*>& StreamingLevels = GetWorld()->GetStreamingLevels(); const ULevelStreaming* LevelStream : StreamingLevels)
    {
        if (LevelStream && LevelStream->IsLevelLoaded() && LevelStream->GetWorldAssetPackageFName() != NewLevelName)
        {
            const FLatentActionInfo LatentInfo;
            UGameplayStatics::UnloadStreamLevel(this, LevelStream->GetWorldAssetPackageFName(), LatentInfo, false);
        }
    }

    // 3. SIGNAL: Tell the widget to do its "Fade Out"
    OnLoadingFinished.Broadcast();

    // 4. Clean up the widget pointer (The widget will visually destroy itself at the end of its anim)
    CurrentWidget = nullptr;
}