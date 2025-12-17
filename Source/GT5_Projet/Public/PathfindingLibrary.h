// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathfindingLibrary.generated.h"

class AMapElement;


/**
 * Pathfinding utility library with A* algorithm implementation for grid-based navigation
 */
UCLASS()
class GT5_PROJET_API UPathfindingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Find a path from start tile to end tile using A* algorithm
	 * @param StartTile - Starting tile coordinates
	 * @param EndTile - Target tile coordinates
	 * @param OutPath - Resulting path as array of tile coordinates (includes start and end)
	 * @param WorldContext - World context for checking obstacles
	 * @param MaxSearchDistance - Maximum search distance to prevent infinite loops (default: 1000)
	 * @return True if path was found, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding", meta=(WorldContext="WorldContext"))
	static bool FindPath(const FIntPoint& StartTile, const FIntPoint& EndTile, TArray<FIntPoint>& OutPath, const UObject* WorldContext, int32 MaxSearchDistance = 1000);

	/**
	 * Check if a tile is walkable (can be overridden to add obstacle detection)
	 * @param TilePosition - Tile coordinates to check
	 * @param WorldContext - World context for checking obstacles
	 * @return True if tile is walkable
	 */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding", meta=(WorldContext="WorldContext"))
	static bool IsTileWalkable(const FIntPoint& TilePosition, const UObject* WorldContext);

	/**
	 * Check if a tile is a map event, and return it if valid.
	 * @param TilePosition - Tile coordinates to check
	 * @param WorldContext - World context for checking obstacles
	 * @return A pointer to the map event actor, if valid. Otherwise, NULL is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding", meta=(WorldContext="WorldContext"))
	static AMapElement* GetTileEvent(const FIntPoint& TilePosition, const UObject* WorldContext);

private:

	// A* Node structure for pathfinding
	struct FAStarNode
	{
		FIntPoint Position;
		float GCost; // Distance from start
		float HCost; // Heuristic distance to end
		float FCost; // Total cost (G + H)
		FAStarNode* Parent;

		FAStarNode()
			: Position(0, 0), GCost(0.f), HCost(0.f), FCost(0.f), Parent(nullptr)
		{
		}

		FAStarNode(const FIntPoint& InPosition, float InGCost, float InHCost, FAStarNode* InParent = nullptr)
			: Position(InPosition), GCost(InGCost), HCost(InHCost), FCost(InGCost + InHCost), Parent(InParent)
		{
		}

		bool operator<(const FAStarNode& Other) const
		{
			return FCost < Other.FCost;
		}
	};

	static float CalculateManhattanDistance(const FIntPoint& A, const FIntPoint& B);

	static bool FindTileAt(FHitResult& OutResult, const FIntPoint& TilePosition, const UObject* WorldContext);

	static bool IsTileFloor(AActor* HitActor);
	static bool IsTileEvent(AActor* HitActor);

	static void GetNeighbors(TInlineComponentArray<FIntPoint, 4>& out, const FIntPoint& Position);

	static void DebugLogTile(const FHitResult& HitResult, const FIntPoint& TilePosition);
};
