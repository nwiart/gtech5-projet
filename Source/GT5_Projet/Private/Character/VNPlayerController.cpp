// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VNPlayerController.h"

#include "Map/VNMapCharacter.h"

#include "Core/VNGameInstance.h"
#include "Libraries/VNTileMapLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AVNPlayerController::AVNPlayerController()
	: PlayerCharacter(0)
{

}

void AVNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FIntPoint spawn(0, 0);
	UVNGameInstance* gameInstance = Cast<UVNGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance != NULL) {
		spawn = gameInstance->PlayerTilePosition;
	}

	FTransform transform;
	transform.SetLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(spawn) + FVector(0, 0, 60.0));
	PlayerCharacter = GetWorld()->SpawnActor<AVNMapCharacter>(PlayerCharacterClass, transform);
}
