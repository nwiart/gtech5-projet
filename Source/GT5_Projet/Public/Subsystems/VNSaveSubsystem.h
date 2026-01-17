// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Save/VNSaveManager.h"

#include "VNSaveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UVNSaveSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;


	UPROPERTY(BlueprintReadOnly)
	UVNSaveManager* SaveManager;
};
