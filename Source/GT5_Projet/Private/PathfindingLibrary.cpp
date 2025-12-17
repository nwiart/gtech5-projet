// Fill out your copyright notice in the Description page of Project Settings.

#include "PathfindingLibrary.h"
#include "VNGamemode.h"
#include "MapElement.h"
#include "VNTileMapLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

bool UPathfindingLibrary::FindPath(const FIntPoint& StartTile, const FIntPoint& EndTile, TArray<FIntPoint>& OutPath, const UObject* WorldContext, int32 MaxSearchDistance)
{
	OutPath.Empty();

	if (StartTile == EndTile)
	{
		OutPath.Add(StartTile);
		return true;
	}

	TMap<FIntPoint, FAStarNode*> AllNodes;
	TArray<FAStarNode*> OpenSet;
	TSet<FIntPoint> ClosedSet;

	FAStarNode* StartNode = new FAStarNode(StartTile, 0.f, CalculateManhattanDistance(StartTile, EndTile), nullptr);
	AllNodes.Add(StartTile, StartNode);
	OpenSet.Add(StartNode);

	int32 SearchCount = 0;
	bool bPathFound = false;
	FAStarNode* EndNode = nullptr;

	while (OpenSet.Num() > 0 && SearchCount < MaxSearchDistance)
	{
		SearchCount++;

		int32 LowestFCostIndex = 0;
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			if (OpenSet[i]->FCost < OpenSet[LowestFCostIndex]->FCost ||
				(OpenSet[i]->FCost == OpenSet[LowestFCostIndex]->FCost && OpenSet[i]->HCost < OpenSet[LowestFCostIndex]->HCost))
			{
				LowestFCostIndex = i;
			}
		}

		FAStarNode* CurrentNode = OpenSet[LowestFCostIndex];
		OpenSet.RemoveAt(LowestFCostIndex);
		ClosedSet.Add(CurrentNode->Position);

		if (CurrentNode->Position == EndTile)
		{
			bPathFound = true;
			EndNode = CurrentNode;
			break;
		}

		TArray<FIntPoint> Neighbors = GetNeighbors(CurrentNode->Position);
		for (const FIntPoint& NeighborPos : Neighbors)
		{
			// Skip if already evaluated
			if (ClosedSet.Contains(NeighborPos))
			{
				continue;
			}

			// Check if the tile is walkable
			if (!IsTileWalkable(NeighborPos, WorldContext))
			{
				continue;
			}

			// Calculate new G cost (1.0 for orthogonal movement)
			float NewGCost = CurrentNode->GCost + 1.0f;

			// Check if neighbor is already in open set
			FAStarNode** ExistingNodePtr = AllNodes.Find(NeighborPos);
			FAStarNode* NeighborNode = nullptr;

			if (ExistingNodePtr != nullptr)
			{
				NeighborNode = *ExistingNodePtr;
				// If we found a better path, update it
				if (NewGCost < NeighborNode->GCost)
				{
					NeighborNode->GCost = NewGCost;
					NeighborNode->FCost = NeighborNode->GCost + NeighborNode->HCost;
					NeighborNode->Parent = CurrentNode;
				}
			}
			else
			{
				// Create new node
				float HCost = CalculateManhattanDistance(NeighborPos, EndTile);
				NeighborNode = new FAStarNode(NeighborPos, NewGCost, HCost, CurrentNode);
				AllNodes.Add(NeighborPos, NeighborNode);
				OpenSet.Add(NeighborNode);
			}
		}
	}

	// Reconstruct path if found
	if (bPathFound && EndNode != nullptr)
	{
		FAStarNode* CurrentNode = EndNode;
		while (CurrentNode != nullptr)
		{
			OutPath.Insert(CurrentNode->Position, 0);
			CurrentNode = CurrentNode->Parent;
		}
	}

	// Cleanup allocated nodes
	for (auto& Pair : AllNodes)
	{
		delete Pair.Value;
	}

	return bPathFound;
}

bool UPathfindingLibrary::IsTileWalkable(const FIntPoint& TilePosition, const UObject* WorldContext)
{
	if (!WorldContext)
	{
		return true;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return true;
	}

	FVector TileWorldPos = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(TilePosition);

	FVector TraceStart = TileWorldPos + FVector(0, 0, 200.0f);
	FVector TraceEnd = TileWorldPos + FVector(0, 0, -200.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	bool bHitFloor = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_WorldStatic,
		QueryParams
	);


	bool bHasFloor = false;
	if (bHitFloor)
	{
		// Check if we hit something with the "Floor" tag
		if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(FName("Floor")))
		{
			bHasFloor = true;
			UE_LOG(LogTemp, VeryVerbose, TEXT("Floor found at tile (%d, %d) - Actor: %s"),
				TilePosition.X, TilePosition.Y, *HitResult.GetActor()->GetName());
		}
		else if (HitResult.GetActor())
		{
			// Hit something but it doesn't have the "Floor" tag
			UE_LOG(LogTemp, Warning, TEXT("Tile (%d, %d) hit actor '%s' but it doesn't have 'Floor' tag"),
				TilePosition.X, TilePosition.Y, *HitResult.GetActor()->GetName());
		}
	}
	else
	{
		// No collision detected
		UE_LOG(LogTemp, Warning, TEXT("No floor collision at tile (%d, %d) - Check collision settings"),
			TilePosition.X, TilePosition.Y);
	}

	if (!bHasFloor)
	{
		return false;
	}

	AVNGamemode* Gamemode = Cast<AVNGamemode>(UGameplayStatics::GetGameMode(WorldContext));
	if (Gamemode)
	{
		for (AMapElement* Element : Gamemode->GetAllMapElements())
		{
			if (Element && Element->bBlocksMovement)
			{
				FIntPoint ElementPos = Element->GetTilePosition();
				if (ElementPos == TilePosition)
				{
					return false; // This tile is blocked by a MapElement
				}
			}
		}
	}

	return true; // Floor exists and no blocking elements found, tile is walkable
}

float UPathfindingLibrary::CalculateManhattanDistance(const FIntPoint& A, const FIntPoint& B)
{
	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

TArray<FIntPoint> UPathfindingLibrary::GetNeighbors(const FIntPoint& Position)
{
	TArray<FIntPoint> Neighbors;

	Neighbors.Add(FIntPoint(Position.X + 1, Position.Y));     // Right
	Neighbors.Add(FIntPoint(Position.X - 1, Position.Y));     // Left
	Neighbors.Add(FIntPoint(Position.X, Position.Y + 1));     // Up
	Neighbors.Add(FIntPoint(Position.X, Position.Y - 1));     // Down

	return Neighbors;
}