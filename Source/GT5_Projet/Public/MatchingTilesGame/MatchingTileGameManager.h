// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/BaseMinigameGameMode.h"
#include "TileWidget.h"
#include "MatchingTileGameManager.generated.h"

UCLASS()
class GT5_PROJET_API AMatchingTileGameManager : public ABaseMinigameGameMode
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchingTileGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
    void InitializeGame(const TArray<UTileWidget*>& InTiles, int32 InWidth, int32 InHeight);

    UFUNCTION()
    void OnTileClicked(UTileWidget* ClickedTile);

	virtual FMinigameResult BuildMinigameResult_Implementation(bool bSuccess) override;


private:
    UPROPERTY()
    TArray<UTileWidget*> Tiles;

    int32 Width;
    int32 Height;

    UTileWidget* FirstSelectedTile = nullptr;
    UTileWidget* SecondSelectedTile = nullptr;

    void CheckMatch();
    bool AreTilesConnected(UTileWidget* TileA, UTileWidget* TileB);
    void ClearMatchedTiles();
    bool HasAnyPossibleMatch();
    void CheckEndConditions();
};
