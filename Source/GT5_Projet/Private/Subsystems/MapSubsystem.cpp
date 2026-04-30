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
}

void UMapSubsystem::AddTileField(ATileField* field)
{
	TileFields.Add(field);
}

void UMapSubsystem::AddMapEvent(AVNMapEvent* mapEvent)
{
	MapEvents.Add(mapEvent);
}

void UMapSubsystem::GetAllTiles(TArray<FIntPoint>& pos) const
{
	for (ATileField* field : TileFields) {
		FIntPoint tile0 = field->GetStartTile();
		for (int x = tile0.X; x < tile0.X + field->SizeX; x++) {
			for (int y = tile0.Y; y < tile0.Y + field->SizeY; y++) {
				pos.Add(FIntPoint(x, y));
			}
		}
	}
}

bool UMapSubsystem::IsTileAt(const FIntPoint& TilePos) const
{
	for (ATileField* field : TileFields) {
		FIntPoint p0 = field->GetStartTile();
		FIntPoint p1 = p0 + FIntPoint(field->SizeX, field->SizeY);

		if (TilePos.X < p0.X || TilePos.X >= p1.X || TilePos.Y < p0.Y || TilePos.Y >= p1.Y) continue;

		return true;
	}
	return false;
}

AVNMapEvent* UMapSubsystem::GetMapEventAt(const FIntPoint& TilePos) const
{
	for (AVNMapEvent* me : MapEvents) {
		if (me->GetTilePosition() == TilePos && !me->bIsInactive) {
			return me;
		}
	}
	return NULL;
}
