// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchingTileGame.h"
#include "Components/ActorComponent.h"
#include "GridGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GT5_PROJET_API UGridGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UDataTable* MapDataTable;

    void LoadMap(FName RowName, int32& OutWidth, int32& OutHeight, TArray<int32>& OutTiles);
};
