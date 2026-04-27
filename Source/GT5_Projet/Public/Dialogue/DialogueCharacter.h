// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Dialogue/DialoguePose.h"

#include "DialogueCharacter.generated.h"


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
