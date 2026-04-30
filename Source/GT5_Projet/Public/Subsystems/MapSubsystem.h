#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MapSubsystem.generated.h"

class ATileField;

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UMapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UMapSubsystem();

	void Reset();
	void AddTileField(ATileField* field);

	UFUNCTION(BlueprintCallable)
	void GetAllTiles(TArray<FIntPoint>& TilePositions) const;

private:

	TArray<ATileField*> TileFields;
};
