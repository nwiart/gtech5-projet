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

	//void OnWorldBeginPlay(UWorld& InWorld) override;

	void Reset();
	void AddTileField(ATileField* field);

	UFUNCTION(BlueprintCallable)
	void GetAllTiles(TArray<FIntPoint>& TilePositions) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTileAt(const FIntPoint& TilePos) const;


private:

	TArray<ATileField*> TileFields;
};
