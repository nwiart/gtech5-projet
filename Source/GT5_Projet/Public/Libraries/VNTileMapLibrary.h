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

	// World units per tile (a tile is a TileSize x TileSize square in world space).
	static constexpr double TileSize = 100.0;
	static constexpr double TileHalfSize = 50.0;

	// Tags used to identify map actors during traces and queries.
	static const FName FloorTag;
	static const FName EventTag;
	static const FName ObstacleTag;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FIntPoint GetTileCoordinatesFromWorldPos(const FVector& worldPos);

	// Returns the integer tile coordinate nearest to a world position. Uses
	// floor-of-(value + half-tile), which is deterministic across platforms
	// (FMath::RoundToInt's tie-breaking can vary).
	static FIntPoint GetNearestTileFromWorldPos(const FVector& worldPos);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetWorldPosFromTileCoordinates(const FIntPoint& tileCoords);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="worldContextObject", UnsafeDuringActorConstruction="true"))
	static FVector2D GetMousePositionInViewport(const UObject* worldContextObject);
};
