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

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	void Load(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UClass* GetSaveGameClass() const;


	UFUNCTION(BlueprintCallable)
	FTableEvents GetEventData(const FGuid& Guid);

	UFUNCTION(BlueprintCallable)
	void SetEventData(const FGuid& Guid, const FTableEvents& Data);


	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVNSaveGame> SaveGame;
};
