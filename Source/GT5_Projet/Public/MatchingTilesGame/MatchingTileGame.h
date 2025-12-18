#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MatchingTileGame.generated.h"

USTRUCT(BlueprintType)
struct FMapData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Height;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TilesString;
};

USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TileType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* TileTexture;
};