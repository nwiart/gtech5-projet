// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Save/VNSaveManager.h"

#include "VNGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UVNGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;


	UPROPERTY(BlueprintReadWrite)
	FIntPoint PlayerTilePosition;

	UPROPERTY(BlueprintReadOnly)
	UVNSaveManager* SaveManager;
};
