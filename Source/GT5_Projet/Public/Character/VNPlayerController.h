// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VNPlayerController.generated.h"

class AVNMapCharacter;
class UInputMappingContext;
class UInputAction;

UCLASS()
class GT5_PROJET_API AVNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AVNPlayerController();

protected:

	void BeginPlay() override;

	virtual void SetupInputComponent() override;

public:

	// The character we control on the map.
	// Not to be mistaken with the Unreal pawn!! The pawn is the isometric camera.
	UPROPERTY(BlueprintReadOnly)
	AVNMapCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AVNMapCharacter> PlayerCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* CursorMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FocusUIAction;

private:

	void Input_FocusUIStart();
	void Input_FocusUIStop();
};
