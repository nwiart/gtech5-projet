// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilsLibrary.generated.h"

/**
 *
 */
UCLASS()
class GT5_PROJET_API UUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static void GetActorsOfClassInStreamedLevel(TArray<AActor*>& OutActors, ULevelStreaming* LevelStreaming, const UClass* Class, const UObject* WorldContextObject);

	template<class T>
	static void GetActorsOfClassInStreamedLevel(TArray<T*>& OutActors, ULevelStreaming* LevelStreaming, const UObject* WorldContextObject)
	{
		GetActorsOfClassInStreamedLevel((TArray<AActor*>&) OutActors, LevelStreaming, T::StaticClass(), WorldContextObject);
	}
};
