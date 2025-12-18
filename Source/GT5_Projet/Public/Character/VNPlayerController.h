// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VNPlayerController.generated.h"


class AMapCharacter;

/**
 * 
 */
UCLASS()
class GT5_PROJET_API AVNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AVNPlayerController();

protected:

	void BeginPlay() override;

public:

	// The character we control on the map.
	// Not to be mistaken with the Unreal pawn!! The pawn is the isometric camera.
	UPROPERTY(BlueprintReadOnly)
	AMapCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AMapCharacter> PlayerCharacterClass;
};
