// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/KnifeHitGame/KnifeHitPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Minigames/KnifeHitGame/KnifeHitManager.h"
#include "Blueprint/UserWidget.h"

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

	// Enable mouse cursor and allow both game and UI input
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
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


