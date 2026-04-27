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

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bHideName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bSoloSpeaker;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Note;
};

USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueChoice
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Text;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDialogueLineSequence* NextDialogue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int ConnectionPoints;
};


UCLASS(BlueprintType)
class GT5_PROJET_API UDialogueLineSequence : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDialogueLineSequence();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasChoice() const;


	// Start character sprites (displayed before the first line).
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Starting Characters")
	UDialogueCharacter* LeftCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Starting Characters")
	EDialoguePose LeftCharacterPose;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Starting Characters")
	UDialogueCharacter* RightCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Starting Characters")
	EDialoguePose RightCharacterPose;


	// List of lines to play in order.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FDialogueLine> Lines;


	// Optional choice at the end of this sequence. Leave empty for no selection.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Choice")
	FString ChoiceQuestion;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Choice")
	TArray<FDialogueChoice> ChoiceOptions;
};
