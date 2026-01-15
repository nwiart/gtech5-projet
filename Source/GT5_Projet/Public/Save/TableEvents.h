// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TableEvents.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FTableEvents
{
	GENERATED_BODY()
	

	UPROPERTY(BlueprintReadWrite)
	bool bTriggered;
};
