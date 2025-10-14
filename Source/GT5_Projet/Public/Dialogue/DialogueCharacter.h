// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetDefinitionDefault.h"

#include "Dialogue/DialoguePose.h"

#include "DialogueCharacter.generated.h"


class UDialogueCharacter;


UCLASS(BlueprintType)
class GT5_PROJET_API UDialogueCharacter : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDialogueCharacter();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetCharacterName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTexture2D* GetTextureForPose(EDialoguePose pose) const;


	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	TMap<EDialoguePose, UTexture2D*> Sprites;
};

UCLASS()
class GT5_PROJET_API UAssetDefinition_DialogueCharacter : public UAssetDefinitionDefault
{
	GENERATED_BODY()

protected:

	virtual FText GetAssetDisplayName() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const;
};

UCLASS(HideCategories = Object)
class UDialogueCharacterFactory : public UFactory
{
	GENERATED_BODY()
	 
public:

	UDialogueCharacterFactory(const FObjectInitializer&);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};
