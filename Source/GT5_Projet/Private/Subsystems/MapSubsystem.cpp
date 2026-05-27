#include "Subsystems/MapSubsystem.h"

#include "Map/TileField.h"
#include "Map/VNMapEvent.h"


UMapSubsystem::UMapSubsystem()
{
}

void UMapSubsystem::Reset()
{
	TileFields.Empty();
	MapEvents.Empty();
	MapEventsByTile.Empty();
}

void UMapSubsystem::AddTileField(ATileField* TileField)
{
	if (!TileField) return;
	TileFields.AddUnique(TileField);
}

void UMapSubsystem::RemoveTileField(ATileField* TileField)
{
	if (!TileField) return;
	TileFields.RemoveSingleSwap(TileField);
}

void UMapSubsystem::AddMapEvent(AVNMapEvent* MapEvent)
{
	if (!MapEvent || MapEvents.Contains(MapEvent)) return;
	MapEvents.Add(MapEvent);
	// Last-add-wins on collisions; multiple events on a single tile is not currently supported.
	MapEventsByTile.Add(MapEvent->GetTilePosition(), MapEvent);
}

void UMapSubsystem::RemoveMapEvent(AVNMapEvent* MapEvent)
{
	if (!MapEvent) return;
	MapEvents.RemoveSingleSwap(MapEvent);
	const FIntPoint Pos = MapEvent->GetTilePosition();
	// Only clear the index entry if it still points at this event — another
	// event may have overwritten the slot via last-add-wins.
	if (AVNMapEvent* const* Existing = MapEventsByTile.Find(Pos); Existing && *Existing == MapEvent)
	{
		MapEventsByTile.Remove(Pos);
	}
}

void UMapSubsystem::GetAllTiles(TArray<FIntPoint>& TilePositions) const
{
	int32 TotalTiles = 0;
	for (const ATileField* Field : TileFields) {
		if (!Field) continue;
		TotalTiles += Field->SizeX * Field->SizeY;
	}
	TilePositions.Reserve(TilePositions.Num() + TotalTiles);

	for (const ATileField* Field : TileFields) {
		if (!Field) continue;
		const FIntPoint Start = Field->GetStartTile();
		for (int32 X = Start.X; X < Start.X + Field->SizeX; X++) {
			for (int32 Y = Start.Y; Y < Start.Y + Field->SizeY; Y++) {
				TilePositions.Add(FIntPoint(X, Y));
			}
		}
	}
}

bool UMapSubsystem::IsTileAt(const FIntPoint& TilePos) const
{
	for (const ATileField* Field : TileFields) {
		if (!Field) continue;

		const FIntPoint P0 = Field->GetStartTile();
		const FIntPoint P1 = P0 + FIntPoint(Field->SizeX, Field->SizeY);

		if (TilePos.X < P0.X || TilePos.X >= P1.X || TilePos.Y < P0.Y || TilePos.Y >= P1.Y) continue;

		return true;
	}
	return false;
}

AVNMapEvent* UMapSubsystem::GetMapEventAt(const FIntPoint& TilePos) const
{
	AVNMapEvent* const* Found = MapEventsByTile.Find(TilePos);
	if (!Found) return nullptr;

	AVNMapEvent* Event = *Found;
	if (!Event || Event->IsInactive()) return nullptr;

	return Event;
}
