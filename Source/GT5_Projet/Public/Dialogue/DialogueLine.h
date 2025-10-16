#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetDefinitionDefault.h"

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

USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueChoice
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Text;
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

UCLASS()
class GT5_PROJET_API UAssetDefinition_DialogueLineSequence : public UAssetDefinitionDefault
{
	GENERATED_BODY()

protected:

	virtual FText GetAssetDisplayName() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};

UCLASS(HideCategories = Object)
class UDialogueLineSequenceFactory : public UFactory
{
	GENERATED_BODY()

public:

	UDialogueLineSequenceFactory(const FObjectInitializer&);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
