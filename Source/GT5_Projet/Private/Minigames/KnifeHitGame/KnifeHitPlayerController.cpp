// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/KnifeHitGame/KnifeHitPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Minigames/KnifeHitGame/KnifeHitManager.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/CursorSubsystem.h"

AKnifeHitPlayerController::AKnifeHitPlayerController()
{
	// Constructor
}

void AKnifeHitPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

		if (KnifeHitMappingContext)
		{
			Subsystem->AddMappingContext(KnifeHitMappingContext, MappingPriority);
		}
	}

	GameModeRef = Cast<AKnifeHitManager>(UGameplayStatics::GetGameMode(this));

	// Create and display HUD widget
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UCursorSubsystem* CursorSubsys = GameInstance->GetSubsystem<UCursorSubsystem>())
		{
			CursorSubsys->SetMode(ECursorMode::Free);
		}
	}
}

void AKnifeHitPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (LaunchMatchAction)
		{
			EnhancedInputComponent->BindAction(
				LaunchMatchAction,
				ETriggerEvent::Started,
				this,
				&AKnifeHitPlayerController::OnLaunchMatch
			);
		}
	}
}

void AKnifeHitPlayerController::OnLaunchMatch() {
	if (!GameModeRef)
	{
		GameModeRef = Cast<AKnifeHitManager>(UGameplayStatics::GetGameMode(this));
	}

	if (GameModeRef)
	{
		GameModeRef->LaunchMatch();
	}
}
