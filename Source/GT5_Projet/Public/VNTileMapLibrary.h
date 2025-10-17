// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VNTileMapLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNTileMapLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FIntPoint GetTileCoordinatesFromWorldPos(const FVector& worldPos);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetWorldPosFromTileCoordinates(const FIntPoint& tileCoords);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="worldContextObject", UnsafeDuringActorConstruction="true"))
	static FVector2D GetMousePositionInViewport(const UObject* worldContextObject);
};
