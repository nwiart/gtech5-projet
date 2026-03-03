// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/PathfindingLibrary.h"
#include "Libraries/VNTileMapLibrary.h"

#include "Subsystems/VNChapterSubsystem.h"

#include "Core/VNGamemode.h"
#include "Map/VNMapEvent.h"

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

	while (OpenSet.Num() > 0 && SearchCount < MaxSearchDistance && !bPathFound)
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

		// Stop on first met event.
		if (GetTileEvent(CurrentNode->Position, WorldContext) != NULL)
		{
			bPathFound = true;
			EndNode = CurrentNode;
			break;
		}

		TInlineComponentArray<FIntPoint, 4> Neighbors;
		GetNeighbors(Neighbors, CurrentNode->Position);
		for (const FIntPoint& NeighborPos : Neighbors)
		{
			// Skip if already evaluated
			if (ClosedSet.Contains(NeighborPos))
			{
				continue;
			}

			FHitResult HitResult;
			bool bHit = FindTileAt(HitResult, NeighborPos, WorldContext);
			DebugLogTile(HitResult, NeighborPos);

			if (!bHit || HitResult.GetActor() == NULL)
			{
				continue;			
			}

			// Check if the tile is walkable
			if (!IsTileFloor(HitResult.GetActor()) && !IsTileEvent(HitResult.GetActor()))
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
	FHitResult HitResult;
	bool bHitFloor = FindTileAt(HitResult, TilePosition, WorldContext);

	if (!bHitFloor || HitResult.GetActor() == NULL)
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.GetActor()->GetName());

	return IsTileFloor(HitResult.GetActor()) || IsTileEvent(HitResult.GetActor());
}

AVNMapEvent* UPathfindingLibrary::GetTileEvent(const FIntPoint& TilePosition, const UObject* WorldContext)
{
	FHitResult HitResult;
	bool bHitFloor = FindTileAt(HitResult, TilePosition, WorldContext);
	AActor* Actor = HitResult.GetActor();

	if (!bHitFloor || Actor == NULL) {
		return NULL;
	}

	return IsTileEvent(Actor) ? Cast<AVNMapEvent>(Actor) : 0;
}

float UPathfindingLibrary::CalculateManhattanDistance(const FIntPoint& A, const FIntPoint& B)
{
	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

bool UPathfindingLibrary::FindTileAt(FHitResult& OutResult, const FIntPoint& Pos, const UObject* WorldContext)
{
	if (!WorldContext)
	{
		return false;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return false;
	}

	UVNChapterSubsystem* chapterSubsys = UGameplayStatics::GetGameInstance(WorldContext)->GetSubsystem<UVNChapterSubsystem>();
	if (!chapterSubsys || !chapterSubsys->GetMapCharacter())
	{
		return false;
	}

	const float Z = chapterSubsys->GetPawn()->CharacterHeightLevel;
	
	// Perform a downward cast.
	// Any obstacle obstructing a walkable tile will be hit first, so no need to manually filter.
	const FVector TileWorldPos = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(Pos);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	return World->LineTraceSingleByChannel(
		OutResult,
		TileWorldPos + FVector(0, 0, Z + 200.0f),
		TileWorldPos + FVector(0, 0, Z - 50.0f),
		ECC_Visibility,
		QueryParams
	);
}

bool UPathfindingLibrary::IsTileFloor(AActor* HitActor)
{
	return HitActor->ActorHasTag(FName("Floor"));
}

bool UPathfindingLibrary::IsTileEvent(AActor* HitActor)
{
	return HitActor->ActorHasTag(FName("Event"));
}

void UPathfindingLibrary::GetNeighbors(TInlineComponentArray<FIntPoint, 4>& outNeighbors, const FIntPoint& Position)
{
	outNeighbors.Add(FIntPoint(Position.X + 1, Position.Y));     // Right
	outNeighbors.Add(FIntPoint(Position.X - 1, Position.Y));     // Left
	outNeighbors.Add(FIntPoint(Position.X, Position.Y + 1));     // Up
	outNeighbors.Add(FIntPoint(Position.X, Position.Y - 1));     // Down
}

void UPathfindingLibrary::DebugLogTile(const FHitResult& HitResult, const FIntPoint& TilePosition)
{
	AActor* Actor = HitResult.GetActor();

	// No collision detected (there's probably nothing here).
	if (Actor == NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("No floor collision at tile (%d, %d) - Check collision settings"),
			//TilePosition.X, TilePosition.Y);
		return;	
	}

	// Check if we hit something with the "Floor" tag
	if (Actor->ActorHasTag(FName("Floor")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Floor found at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else if (Actor->ActorHasTag(FName("Obstacle")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit obstacle at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else if (Actor->ActorHasTag(FName("Event")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit map event at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else
	{
		// Hit something but it doesn't have a known tag
		UE_LOG(LogTemp, Warning, TEXT("Tile (%d, %d) hit actor '%s' but it doesn't have any known tag"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
}
