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
	 * Clears all registered tile fields.
	 * This is to be called when a new level starts getting loaded (currently
	 *   in USceneTransitionSubsystem::StartActualLoading()).
	 */
	void Reset();

	/**
	 * Add a tile field to the terrain.
	 * This function is called automatically by every tile field instance from
	 * their BeginPlay().
	 */
	void AddTileField(ATileField* field);

	/**
	 * Add a map event to the map.
	 * This function is called automatically by every map event instance from
	 * their BeginPlay().
	 */
	void AddMapEvent(AVNMapEvent* mapEvent);

	/**
	 * Get a list of all existing tiles, represented by their position.
	 * These are not necessarily walkable tiles (obstacles are ignored).
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
	TArray<AVNMapEvent*>& GetAllMapEvents() { return MapEvents; }


private:

	// List of all tile fields in the current map. They represent the walkable terrain.
	TArray<ATileField*> TileFields;

	// List of map events.
	TArray<AVNMapEvent*> MapEvents;
};
