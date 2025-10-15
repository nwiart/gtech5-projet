
#include "Dialogue/DialogueLine.h"


FDialogueLine::FDialogueLine()
	: Character(0), Pose(EDialoguePose::Normal)
{
}


// Asset object.

UDialogueLineSequence::UDialogueLineSequence()
	: LeftCharacter(0), LeftCharacterPose(EDialoguePose::Normal)
	, RightCharacter(0), RightCharacterPose(EDialoguePose::Normal)
{
}


// Asset definition.

FText UAssetDefinition_DialogueLineSequence::GetAssetDisplayName() const
{
	return FText::FromString(TEXT("Dialogue Line Sequence"));
}

FText UAssetDefinition_DialogueLineSequence::GetAssetDescription(const FAssetData& AssetData) const
{
	const UDialogueLineSequence* asset = Cast<const UDialogueLineSequence>(AssetData.GetAsset());
	if (asset) {
		return FText::FromString(TEXT("Sequence of dialog lines, with an optional choice at the end."));
	}
	return FText::GetEmpty();
}

FLinearColor UAssetDefinition_DialogueLineSequence::GetAssetColor() const
{
	return FLinearColor(0.6F, 0.2F, 0.8F);
}

TSoftClassPtr<UObject> UAssetDefinition_DialogueLineSequence::GetAssetClass() const
{
	return UDialogueLineSequence::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_DialogueLineSequence::GetAssetCategories() const
{
	static const auto Categories = {
		FAssetCategoryPath(FText::FromName("Visual Novel")),
	};
	return Categories;
}


// Factory.

UDialogueLineSequenceFactory::UDialogueLineSequenceFactory(const FObjectInitializer&)
{
	// Specify the asset class this factory is supposed to create
	SupportedClass = UDialogueLineSequence::StaticClass();

	// Make sure to set this to true, otherwise it wont create the asset
	bCreateNew = true;

	// Set this to true, if you want to immediately start editing the asset after creation
	bEditAfterNew = true;
}

UObject* UDialogueLineSequenceFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(InClass->IsChildOf(UDialogueLineSequence::StaticClass()));
	return NewObject<UDialogueLineSequence>(InParent, InClass, InName, Flags);
}
