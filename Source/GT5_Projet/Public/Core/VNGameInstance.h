// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Core/VNSaveManager.h"

#include "VNGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;

	virtual void Shutdown() override;

private:

	UVNSaveManager* SaveManager;
};
