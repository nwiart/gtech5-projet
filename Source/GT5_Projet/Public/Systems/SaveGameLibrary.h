// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveGameLibrary.generated.h"

class UVNSaveManager;

/**
 * 
 */
UCLASS()
class GT5_PROJET_API USaveGameLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UVNSaveManager* GetSaveManager(const UObject* WorldContextObject);
};
