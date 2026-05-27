#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MapSubsystem.generated.h"

class ATileField;
class AVNMapEvent;


/**
 * Map subsystem.
 *
 * This subsystem holds the terrain (tiles) and can be used to perform
 * queries for various tasks, such as pathfinding.
 */
UCLASS()
class GT5_PROJET_API UMapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UMapSubsystem();

	/**
	 * Clears all registered tile fields and map events.
	 * Actors normally deregister themselves from EndPlay; this is a manual
	 * reset for edge cases (e.g. force-clear during editor PIE teardown).
	 */
	void Reset();

	/**
	 * Add a tile field to the terrain.
	 * This function is called automatically by every tile field instance from
	 * their BeginPlay(). Duplicate registrations are ignored.
	 */
	void AddTileField(ATileField* TileField);

	/**
	 * Remove a tile field from the terrain.
	 * Called automatically by every tile field instance from their EndPlay().
	 */
	void RemoveTileField(ATileField* TileField);

	/**
	 * Add a map event to the map.
	 * This function is called automatically by every map event instance from
	 * their BeginPlay(). Duplicate registrations are ignored.
	 */
	void AddMapEvent(AVNMapEvent* MapEvent);

	/**
	 * Remove a map event from the map.
	 * Called automatically by every map event instance from their EndPlay().
	 */
	void RemoveMapEvent(AVNMapEvent* MapEvent);

	/**
	 * Append the positions of all existing tiles to the output array.
	 * Tiles are not necessarily walkable (obstacles are ignored).
	 * The output array is not cleared by this function.
	 */
	UFUNCTION(BlueprintCallable)
	void GetAllTiles(TArray<FIntPoint>& TilePositions) const;

	/**
	 * Returns whether there is a tile (walkable or not) at the specified location.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTileAt(const FIntPoint& TilePos) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVNMapEvent* GetMapEventAt(const FIntPoint& TilePos) const;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AVNMapEvent*>& GetAllMapEvents() const { return MapEvents; }


private:

	// List of all tile fields in the current map. They represent the walkable terrain.
	// UPROPERTY so the GC nulls entries when an actor is destroyed instead of leaving
	// dangling pointers.
	UPROPERTY()
	TArray<ATileField*> TileFields;

	// List of map events. UPROPERTY for the same reason as TileFields.
	UPROPERTY()
	TArray<AVNMapEvent*> MapEvents;

	// Index of map events keyed by tile, kept in sync with MapEvents for O(1) lookup
	// in GetMapEventAt (called once per A* node during pathfinding).
	UPROPERTY()
	TMap<FIntPoint, AVNMapEvent*> MapEventsByTile;
};
