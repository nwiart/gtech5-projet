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

void UMapSubsystem::AddTileField(ATileField* TileField)
{
	TileFields.AddUnique(TileField);
}

void UMapSubsystem::AddMapEvent(AVNMapEvent* MapEvent)
{
	MapEvents.AddUnique(MapEvent);
}

void UMapSubsystem::GetAllTiles(TArray<FIntPoint>& TilePositions) const
{
	int32 TotalTiles = 0;
	for (const ATileField* Field : TileFields) {
		TotalTiles += Field->SizeX * Field->SizeY;
	}
	TilePositions.Reserve(TilePositions.Num() + TotalTiles);

	for (const ATileField* Field : TileFields) {
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
		const FIntPoint P0 = Field->GetStartTile();
		const FIntPoint P1 = P0 + FIntPoint(Field->SizeX, Field->SizeY);

		if (TilePos.X < P0.X || TilePos.X >= P1.X || TilePos.Y < P0.Y || TilePos.Y >= P1.Y) continue;

		return true;
	}
	return false;
}

AVNMapEvent* UMapSubsystem::GetMapEventAt(const FIntPoint& TilePos) const
{
	for (AVNMapEvent* Event : MapEvents) {
		if (Event->GetTilePosition() == TilePos && !Event->IsInactive()) {
			return Event;
		}
	}
	return nullptr;
}
