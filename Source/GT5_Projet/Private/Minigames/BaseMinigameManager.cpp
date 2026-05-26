// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/BaseMinigameManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SoundSubsystem.h"

ABaseMinigameManager::ABaseMinigameManager()
	: HUDWidget(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsMinigameActive = false;
	CurrentResultWidget = nullptr;
}

void ABaseMinigameManager::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseMinigameManager::EndPlay(const EEndPlayReason::Type reason)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = 0;
	}

	Super::EndPlay(reason);
}

void ABaseMinigameManager::Initialize()
{
	bIsMinigameActive = true;

	// Show HUD if configured
	if (HUDWidgetClass)
	{
		HUDWidget = ShowWidget(HUDWidgetClass, 0);
	}
}

void ABaseMinigameManager::ShowWinScreen()
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

	if (VictorySFX.RowName != NAME_None)
	{
		if (USoundSubsystem* SoundSub = GetGameInstance()->GetSubsystem<USoundSubsystem>())
		{
			SoundSub->PlaySFXByHandle(VictorySFX);
		}
	}
}

void ABaseMinigameManager::ShowLoseScreen()
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

	if (LoseSFX.RowName != NAME_None)
	{
		if (USoundSubsystem* SoundSub = GetGameInstance()->GetSubsystem<USoundSubsystem>())
		{
			SoundSub->PlaySFXByHandle(LoseSFX);
		}
	}
}

void ABaseMinigameManager::OnMinigameComplete_Implementation(bool bSuccess)
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

FMinigameResult ABaseMinigameManager::BuildMinigameResult_Implementation(bool bSuccess)
{
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;
	return Result;
}

void ABaseMinigameManager::RestartMinigame()
{
	// Reload current level
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

UUserWidget* ABaseMinigameManager::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
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