// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigame/BaseMinigameGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ABaseMinigameGameMode::ABaseMinigameGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsMinigameActive = false;
	CurrentResultWidget = nullptr;
}

void ABaseMinigameGameMode::BeginPlay()
{
	Super::BeginPlay();
	bIsMinigameActive = true;

	// Show HUD if configured
	if (HUDWidgetClass)
	{
		ShowWidget(HUDWidgetClass, 0);
	}
}

void ABaseMinigameGameMode::ShowWinScreen()
{
	if (WinScreenClass)
	{
		CurrentResultWidget = ShowWidget(WinScreenClass, 10);
		UE_LOG(LogTemp, Log, TEXT("Win Screen Displayed for: %s"), *MinigameName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WinScreenClass is not set for %s"), *MinigameName);
	}
}

void ABaseMinigameGameMode::ShowLoseScreen()
{
	if (LoseScreenClass)
	{
		CurrentResultWidget = ShowWidget(LoseScreenClass, 10);
		UE_LOG(LogTemp, Log, TEXT("Lose Screen Displayed for: %s"), *MinigameName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LoseScreenClass is not set for %s"), *MinigameName);
	}
}

void ABaseMinigameGameMode::OnMinigameComplete_Implementation(bool bSuccess)
{
	bIsMinigameActive = false;

	// Build result data
	CurrentResult = BuildMinigameResult(bSuccess);
	CurrentResult.bSuccess = bSuccess;
	CurrentResult.MinigameName = MinigameName;

	// Show appropriate screen
	if (bSuccess)
	{
		ShowWinScreen();
	}
	else
	{
		ShowLoseScreen();
	}

	UE_LOG(LogTemp, Log, TEXT("Minigame '%s' completed. Success: %s, Score Delta: %d"),
		*MinigameName, bSuccess ? TEXT("Yes") : TEXT("No"), CurrentResult.ConnectionScoreDelta);
}

float ABaseMinigameGameMode::GetProgressPercentage_Implementation() const
{
	// Default implementation - override in child classes
	return 0.0f;
}

FText ABaseMinigameGameMode::GetObjectiveText_Implementation() const
{
	// Default implementation - override in child classes
	return FText::FromString(TEXT("Complete the minigame"));
}

FMinigameResult ABaseMinigameGameMode::BuildMinigameResult_Implementation(bool bSuccess)
{
	// Default implementation - override in child classes
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;
	Result.CompletionPercentage = GetProgressPercentage();
	return Result;
}

void ABaseMinigameGameMode::RestartMinigame()
{
	// Reload current level
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ABaseMinigameGameMode::ReturnToMainGame()
{
	// TODO: This should be connected to your main game flow
	// For now, just log - you'll need to implement this based on your game structure
	UE_LOG(LogTemp, Warning, TEXT("ReturnToMainGame called - implement level transition logic"));

	// Example: Load main game map
	// UGameplayStatics::OpenLevel(this, FName("MainGameMap"), false);
}

UUserWidget* ABaseMinigameGameMode::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
{
	if (!WidgetClass)
	{
		return nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return nullptr;
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
	if (Widget)
	{
		Widget->AddToViewport(ZOrder);
		return Widget;
	}

	return nullptr;
}