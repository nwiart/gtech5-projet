// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VNPlayerController.h"

#include "Character/MapCharacter.h"

#include "Kismet/KismetMathLibrary.h"


AVNPlayerController::AVNPlayerController()
	: PlayerCharacter(0)
{

}

void AVNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FTransform transform;
	transform.SetLocation(FVector(50, 50, 0));
	PlayerCharacter = GetWorld()->SpawnActor<AMapCharacter>(PlayerCharacterClass, transform);
}
