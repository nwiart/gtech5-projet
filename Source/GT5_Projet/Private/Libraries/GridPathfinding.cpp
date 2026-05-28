#include "Libraries/GridPathfinding.h"

bool FGridPathfinding::FindPath(
	const FIntPoint& StartTile,
	const FIntPoint& EndTile,
	TArray<FIntPoint>& OutPath,
	const FIsWalkable& IsWalkable,
	const FIsTerminal& IsTerminal,
	int32 MaxSearchDistance)
{
	OutPath.Empty();

	if (StartTile == EndTile)
	{
		OutPath.Add(StartTile);
		return true;
	}

	TArray<FNode> Nodes;
	TArray<int32> OpenSet;
	TSet<FIntPoint> ClosedSet;
	TMap<FIntPoint, int32> NodeIndexByPosition;

	const int32 StartIndex = Nodes.Add({ StartTile, 0.0F, CalculateManhattanDistance(StartTile, EndTile), INDEX_NONE });
	NodeIndexByPosition.Add(StartTile, StartIndex);
	OpenSet.Add(StartIndex);

	int32 SearchCount = 0;
	int32 EndIndex = INDEX_NONE;

	while (OpenSet.Num() > 0 && SearchCount < MaxSearchDistance && EndIndex == INDEX_NONE)
	{
		SearchCount++;

		int32 LowestOpenSetIndex = 0;
		for (int32 Index = 1; Index < OpenSet.Num(); Index++)
		{
			const FNode& Candidate = Nodes[OpenSet[Index]];
			const FNode& CurrentLowest = Nodes[OpenSet[LowestOpenSetIndex]];
			if (Candidate.GetFCost() < CurrentLowest.GetFCost() ||
				(Candidate.GetFCost() == CurrentLowest.GetFCost() && Candidate.HCost < CurrentLowest.HCost))
			{
				LowestOpenSetIndex = Index;
			}
		}

		const int32 CurrentIndex = OpenSet[LowestOpenSetIndex];
		OpenSet.RemoveAt(LowestOpenSetIndex);
		const FIntPoint CurrentPosition = Nodes[CurrentIndex].Position;
		const float CurrentGCost = Nodes[CurrentIndex].GCost;
		ClosedSet.Add(CurrentPosition);

		if (CurrentPosition == EndTile || (IsTerminal.IsBound() && IsTerminal.Execute(CurrentPosition)))
		{
			EndIndex = CurrentIndex;
			break;
		}

		TArray<FIntPoint, TInlineAllocator<4>> Neighbors;
		GetNeighbors(Neighbors, CurrentPosition);
		for (const FIntPoint& NeighborPos : Neighbors)
		{
			if (ClosedSet.Contains(NeighborPos) || !IsWalkable.Execute(NeighborPos))
			{
				continue;
			}

			const float NewGCost = CurrentGCost + 1.0F;
			if (int32* ExistingIndex = NodeIndexByPosition.Find(NeighborPos))
			{
				FNode& ExistingNode = Nodes[*ExistingIndex];
				if (NewGCost < ExistingNode.GCost)
				{
					ExistingNode.GCost = NewGCost;
					ExistingNode.ParentIndex = CurrentIndex;
				}
			}
			else
			{
				const int32 NeighborIndex = Nodes.Add({
					NeighborPos,
					NewGCost,
					CalculateManhattanDistance(NeighborPos, EndTile),
					CurrentIndex
				});
				NodeIndexByPosition.Add(NeighborPos, NeighborIndex);
				OpenSet.Add(NeighborIndex);
			}
		}
	}

	if (EndIndex == INDEX_NONE)
	{
		return false;
	}

	for (int32 CurrentIndex = EndIndex; CurrentIndex != INDEX_NONE; CurrentIndex = Nodes[CurrentIndex].ParentIndex)
	{
		OutPath.Insert(Nodes[CurrentIndex].Position, 0);
	}

	return true;
}

float FGridPathfinding::CalculateManhattanDistance(const FIntPoint& A, const FIntPoint& B)
{
	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

void FGridPathfinding::GetNeighbors(TArray<FIntPoint, TInlineAllocator<4>>& OutNeighbors, const FIntPoint& Position)
{
	OutNeighbors.Add(FIntPoint(Position.X + 1, Position.Y));
	OutNeighbors.Add(FIntPoint(Position.X - 1, Position.Y));
	OutNeighbors.Add(FIntPoint(Position.X, Position.Y + 1));
	OutNeighbors.Add(FIntPoint(Position.X, Position.Y - 1));
}
