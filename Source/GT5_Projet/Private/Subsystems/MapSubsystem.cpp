#include "Subsystems/MapSubsystem.h"

#include "Map/TileField.h"


UMapSubsystem::UMapSubsystem()
{
}

void UMapSubsystem::Reset()
{
	TileFields.Empty();
}

void UMapSubsystem::AddTileField(ATileField* field)
{
	TileFields.Add(field);
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
