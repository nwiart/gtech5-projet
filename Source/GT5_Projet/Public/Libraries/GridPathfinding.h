#pragma once

#include "CoreMinimal.h"

/**
 * Pure grid pathfinding helpers. Unreal/world queries stay outside this class
 * so the A* behavior can be characterized independently.
 */
class GT5_PROJET_API FGridPathfinding
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(bool, FIsWalkable, const FIntPoint&);
	DECLARE_DELEGATE_RetVal_OneParam(bool, FIsTerminal, const FIntPoint&);

	static bool FindPath(
		const FIntPoint& StartTile,
		const FIntPoint& EndTile,
		TArray<FIntPoint>& OutPath,
		const FIsWalkable& IsWalkable,
		const FIsTerminal& IsTerminal,
		int32 MaxSearchDistance = 1000);

private:
	struct FNode
	{
		FIntPoint Position;
		float GCost = 0.0F;
		float HCost = 0.0F;
		int32 ParentIndex = INDEX_NONE;

		float GetFCost() const { return GCost + HCost; }
	};

	static float CalculateManhattanDistance(const FIntPoint& A, const FIntPoint& B);
	static void GetNeighbors(TArray<FIntPoint, TInlineAllocator<4>>& OutNeighbors, const FIntPoint& Position);
};
