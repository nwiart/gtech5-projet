#include "AssetDefinitions/AssetDefinition_DialogueCharacter.h"

#include "GT5_Projet/Public/Dialogue/DialogueCharacter.h"


// Asset definition.

FText UAssetDefinition_DialogueCharacter::GetAssetDisplayName() const
{
	return FText::FromString(TEXT("Dialogue Character"));
}

FText UAssetDefinition_DialogueCharacter::GetAssetDescription(const FAssetData& AssetData) const
{
	const UDialogueCharacter* asset = Cast<const UDialogueCharacter>(AssetData.GetAsset());
	if (asset) {
		return FText::FromString(asset->Name);
	}
	return FText::GetEmpty();
}

FLinearColor UAssetDefinition_DialogueCharacter::GetAssetColor() const
{
	return FLinearColor(0.6F, 0.2F, 0.8F);
}

TSoftClassPtr<UObject> UAssetDefinition_DialogueCharacter::GetAssetClass() const
{
	return UDialogueCharacter::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_DialogueCharacter::GetAssetCategories() const
{
	static const auto Categories = {
		FAssetCategoryPath(FText::FromName("Visual Novel")),
	};
	return Categories;
}


// Factory.

UDialogueCharacterFactory::UDialogueCharacterFactory(const FObjectInitializer&)
{
	// Specify the asset class this factory is supposed to create
	SupportedClass = UDialogueCharacter::StaticClass();

	// Make sure to set this to true, otherwise it wont create the asset
	bCreateNew = true;

	// Set this to true, if you want to immediately start editing the asset after creation
	bEditAfterNew = true;
}

UObject* UDialogueCharacterFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(InClass->IsChildOf(UDialogueCharacter::StaticClass()));
	return NewObject<UDialogueCharacter>(InParent, InClass, InName, Flags);
}
