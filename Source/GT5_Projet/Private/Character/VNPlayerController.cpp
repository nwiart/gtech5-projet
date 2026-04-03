// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VNPlayerController.h"

#include "Map/VNMapCharacter.h"

#include "Core/VNGameInstance.h"
#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/CursorSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


AVNPlayerController::AVNPlayerController()
	: PlayerCharacter(0)
{

}

void AVNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (CursorMappingContext)
		{
			Subsystem->AddMappingContext(CursorMappingContext, 0);
		}
	}

	FIntPoint spawn(0, 0);
	UVNGameInstance* gameInstance = Cast<UVNGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance != NULL) {
		spawn = gameInstance->PlayerTilePosition;
	}

	FTransform transform;
	transform.SetLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(spawn) + FVector(0, 0, 60.0));
	PlayerCharacter = GetWorld()->SpawnActor<AVNMapCharacter>(PlayerCharacterClass, transform);
}

void AVNPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (FocusUIAction)
		{
			EnhancedInput->BindAction(FocusUIAction, ETriggerEvent::Started, this, &AVNPlayerController::Input_FocusUIStart);
			EnhancedInput->BindAction(FocusUIAction, ETriggerEvent::Completed, this, &AVNPlayerController::Input_FocusUIStop);
		}
	}
}

void AVNPlayerController::Input_FocusUIStart()
{
	UCursorSubsystem* CursorSub = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UCursorSubsystem>();
	CursorSub->SetFocusUI(true);
}

void AVNPlayerController::Input_FocusUIStop()
{
	UCursorSubsystem* CursorSub = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UCursorSubsystem>();
	CursorSub->SetFocusUI(false);
}
