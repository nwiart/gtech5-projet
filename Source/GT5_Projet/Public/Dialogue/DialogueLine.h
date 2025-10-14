#pragma once

#include "CoreMinimal.h"
#include "Dialogue/DialoguePose.h"

#include "DialogueLine.generated.h"


class UDialogueCharacter;

USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueLine
{
	GENERATED_BODY()

public:

	FDialogueLine();


	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Text;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDialogueCharacter* Character;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EDialoguePose Pose;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool LeftSide;
};


UCLASS(BlueprintType, Blueprintable)
class GT5_PROJET_API UDialogueLineObject : public UObject
{
	GENERATED_BODY()

public:

	UDialogueLineObject();


	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FDialogueLine> Lines;
};
