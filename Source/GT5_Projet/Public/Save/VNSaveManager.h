// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Save/TableEvents.h"

#include "VNSaveManager.generated.h"

class UVNSaveGame;

/**
 *
 */
UCLASS()
class GT5_PROJET_API UVNSaveManager : public UObject
{
	GENERATED_BODY()

public:

	UVNSaveManager();

	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	void Save();


	UFUNCTION(BlueprintCallable)
	FTableEvents GetEventData(const FGuid& Guid);

	UFUNCTION(BlueprintCallable)
	void SetEventData(const FGuid& Guid, const FTableEvents& Data);


	UPROPERTY()
	TObjectPtr<UVNSaveGame> SaveGame;
};
