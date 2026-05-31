// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/PathfindingLibrary.h"
#include "Libraries/GridPathfinding.h"
#include "Libraries/VNTileMapLibrary.h"

#include "Core/PawnIsometric.h"
#include "Subsystems/VNChapterSubsystem.h"

#include "Subsystems/MapSubsystem.h"

#include "Core/VNGamemode.h"
#include "Map/VNMapEvent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

bool UPathfindingLibrary::FindPath(const FIntPoint& StartTile, const FIntPoint& EndTile, TArray<FIntPoint>& OutPath, const UObject* WorldContext, int32 MaxSearchDistance)
{
	FGridPathfinding::FIsWalkable IsWalkable;
	IsWalkable.BindLambda([WorldContext](const FIntPoint& TilePosition)
	{
		FHitResult HitResult;
		const bool bHit = FindTileAt(HitResult, TilePosition, WorldContext);
		DebugLogTile(HitResult, TilePosition);

		AActor* HitActor = HitResult.GetActor();
		return bHit && HitActor && (IsTileFloor(HitActor) || IsTileEvent(HitActor));
	});

	FGridPathfinding::FIsTerminal IsTerminal;
	IsTerminal.BindLambda([WorldContext](const FIntPoint& TilePosition)
	{
		return GetTileEvent(TilePosition, WorldContext) != nullptr;
	});

	return FGridPathfinding::FindPath(StartTile, EndTile, OutPath, IsWalkable, IsTerminal, MaxSearchDistance);
}

bool UPathfindingLibrary::IsTileWalkable(const FIntPoint& TilePosition, const UObject* WorldContext)
{
	FHitResult HitResult;
	bool bHitFloor = FindTileAt(HitResult, TilePosition, WorldContext);

	if (!bHitFloor || HitResult.GetActor() == nullptr)
	{
		return false;
	}

	return IsTileFloor(HitResult.GetActor()) || IsTileEvent(HitResult.GetActor());
}

AVNMapEvent* UPathfindingLibrary::GetTileEvent(const FIntPoint& TilePosition, const UObject* WorldContext)
{
	if (!WorldContext)
	{
		return nullptr;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UMapSubsystem* Subsys = World->GetSubsystem<UMapSubsystem>();
	return Subsys ? Subsys->GetMapEventAt(TilePosition) : nullptr;
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
	return HitActor->ActorHasTag(UVNTileMapLibrary::FloorTag);
}

bool UPathfindingLibrary::IsTileEvent(AActor* HitActor)
{
	return HitActor->ActorHasTag(UVNTileMapLibrary::EventTag);
}

void UPathfindingLibrary::DebugLogTile(const FHitResult& HitResult, const FIntPoint& TilePosition)
{
	AActor* Actor = HitResult.GetActor();

	// No collision detected (there's probably nothing here).
	if (Actor == nullptr)
	{
		return;
	}

	if (Actor->ActorHasTag(UVNTileMapLibrary::FloorTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Floor found at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else if (Actor->ActorHasTag(UVNTileMapLibrary::ObstacleTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit obstacle at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else if (Actor->ActorHasTag(UVNTileMapLibrary::EventTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit map event at tile (%d, %d) - Actor: %s"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile (%d, %d) hit actor '%s' but it doesn't have any known tag"),
			TilePosition.X, TilePosition.Y, *Actor->GetName());
	}
}
