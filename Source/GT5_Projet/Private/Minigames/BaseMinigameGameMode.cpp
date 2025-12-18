// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/BaseMinigameGameMode.h"
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

FMinigameResult ABaseMinigameGameMode::BuildMinigameResult_Implementation(bool bSuccess)
{
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;
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
	UE_LOG(LogTemp, Warning, TEXT("ReturnToMainGame called - implement level transition logic"));

	UGameplayStatics::OpenLevel(this, FName("Test"), false);
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