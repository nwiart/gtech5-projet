#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ChapterData.generated.h"

/**
 * Structure décrivant un chapitre du jeu
 */
USTRUCT(BlueprintType)
struct FChapterData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    UTexture2D* Thumbnail = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    FName LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    bool bIsLocked = false;

};
