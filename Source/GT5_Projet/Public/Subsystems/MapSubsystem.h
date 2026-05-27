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
	 * This is to be called when a new level starts getting loaded (currently
	 *   in USceneTransitionSubsystem::StartActualLoading()).
	 * Tile fields and map events from the newly loaded level re-register
	 * themselves from their BeginPlay().
	 */
	void Reset();

	/**
	 * Add a tile field to the terrain.
	 * This function is called automatically by every tile field instance from
	 * their BeginPlay(). Duplicate registrations are ignored.
	 */
	void AddTileField(ATileField* TileField);

	/**
	 * Add a map event to the map.
	 * This function is called automatically by every map event instance from
	 * their BeginPlay(). Duplicate registrations are ignored.
	 */
	void AddMapEvent(AVNMapEvent* MapEvent);

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
	TArray<ATileField*> TileFields;

	// List of map events.
	TArray<AVNMapEvent*> MapEvents;
};
