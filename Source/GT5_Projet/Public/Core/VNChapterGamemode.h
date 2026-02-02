// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/VNGamemode.h"

#include "Map/VNMapCharacter.h"
#include "Core/PawnIsometric.h"

#include "VNChapterGamemode.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API AVNChapterGamemode : public AVNGamemode
{
	GENERATED_BODY()

public:

	AVNChapterGamemode();

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type Reason) override;


	UFUNCTION(BlueprintCallable)
	void Enable();

	UFUNCTION(BlueprintCallable)
	void Disable();


	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVNMapCharacter* GetMapCharacter() const { return MapCharacter; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	APawnIsometric* GetPawn() const { return PawnCamera; }


protected:

	UPROPERTY(EditDefaultsOnly, Category="Chapter Runtime")
	TSubclassOf<AVNMapCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category="Chapter Runtime")
	TSubclassOf<APawnIsometric> PawnCameraClass;


	UPROPERTY(BlueprintReadOnly, Category="Chapter Runtime")
	AVNMapCharacter* MapCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Chapter Runtime")
	APawnIsometric* PawnCamera;
};
