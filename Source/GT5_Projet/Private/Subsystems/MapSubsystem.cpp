#include "Subsystems/MapSubsystem.h"


UMapSubsystem::UMapSubsystem()
{
	UE_LOG(LogTemp, Warning, TEXT("UMapSubsystem was created!!"));
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
		
	}
}
