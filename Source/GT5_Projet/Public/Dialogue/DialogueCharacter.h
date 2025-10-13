// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueCharacter.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FString, UTexture2D*> Sprites;
};


USTRUCT(BlueprintType)
struct GT5_PROJET_API FDialogueLine
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Text;
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
