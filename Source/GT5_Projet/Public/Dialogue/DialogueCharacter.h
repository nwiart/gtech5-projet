// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetDefinitionDefault.h"
#include "DialogueCharacter.generated.h"


class UDialogueCharacter;


/*
 * Dialogue "pose", used to select the appropriate sprite for the character speaking, depending
 * on which emotion we want them to show.
 */
UENUM(BlueprintType)
enum class EDialoguePose : uint8
{
	Normal    UMETA(DisplayName="Normal"),
	Joyful    UMETA(DisplayName="Joyful"),
	Intrigued UMETA(DisplayName="Intrigued"),
	Scared    UMETA(DisplayName="Scared"),
	Tired     UMETA(DisplayName="Tired"),
};


USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueLine
{
	GENERATED_BODY()

public:

	FDialogueLine()
		: Character(0), Pose(EDialoguePose::Normal)
	{
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Text;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDialogueCharacter* Character;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EDialoguePose Pose;
};


UCLASS(BlueprintType)
class GT5_PROJET_API UDialogueCharacter : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDialogueCharacter()
	{
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetCharacterName() const
	{
		return FText::FromString(Name);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTexture2D* GetTextureForPose(EDialoguePose pose) const
	{
		UTexture2D* const* valuePtr = Sprites.Find(pose);
		return valuePtr ? *valuePtr : 0;
	}


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

	virtual FText GetAssetDisplayName() const override
	{
		return FText::FromString(TEXT("Dialogue Character"));
	}
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override
	{
		const UDialogueCharacter* asset = Cast<const UDialogueCharacter>(AssetData.GetAsset());
		if (asset) {
			return FText::FromString(asset->Name);
		}
		return FText::GetEmpty();
	}

	virtual FLinearColor GetAssetColor() const override
	{
		return FLinearColor(0.6F, 0.2F, 0.8F);
	}

	virtual TSoftClassPtr<UObject> GetAssetClass() const override
	{
		return UDialogueCharacter::StaticClass();
	}

	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
	{
		static const auto Categories = {
			FAssetCategoryPath(FText::FromName("Visual Novel")),
		};
		return Categories;
	}
};

UCLASS(HideCategories = Object)
class UDialogueCharacterFactory : public UFactory
{
	GENERATED_BODY()
	 
public:

	UDialogueCharacterFactory(const FObjectInitializer&)
	{
		// Specify the asset class this factory is supposed to create
		SupportedClass = UDialogueCharacter::StaticClass();

		// Make sure to set this to true, otherwise it wont create the asset
		bCreateNew = true; 

		// Set this to true, if you want to immediately start editing the asset after creation
		bEditAfterNew = true;
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override
	{
		check(InClass->IsChildOf(UDialogueCharacter::StaticClass()));
		return NewObject<UDialogueCharacter>(InParent, InClass, InName, Flags);
	}
};


UCLASS(BlueprintType, Blueprintable)
class GT5_PROJET_API UDialogueLineObject : public UObject
{
	GENERATED_BODY()

public:

	UDialogueLineObject()
	{
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FDialogueLine> Lines;
};
