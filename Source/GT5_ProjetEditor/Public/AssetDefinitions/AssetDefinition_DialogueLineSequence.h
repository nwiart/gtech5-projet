#pragma once

#include "CoreMinimal.h"

#include "Factories/Factory.h"
#include "AssetDefinitionDefault.h"

#include "AssetDefinition_DialogueLineSequence.generated.h"


UCLASS()
class UAssetDefinition_DialogueLineSequence : public UAssetDefinitionDefault
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
