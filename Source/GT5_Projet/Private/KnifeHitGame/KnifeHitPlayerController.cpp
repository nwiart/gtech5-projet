// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeHitGame/KnifeHitPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KnifeHitGame/KnifeHitGameMode.h"

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

	GameModeRef = Cast<AKnifeHitGameMode>(UGameplayStatics::GetGameMode(this));

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
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
		GameModeRef = Cast<AKnifeHitGameMode>(UGameplayStatics::GetGameMode(this));
	}

	if (GameModeRef)
	{
		GameModeRef->LaunchMatch();
	}
}


