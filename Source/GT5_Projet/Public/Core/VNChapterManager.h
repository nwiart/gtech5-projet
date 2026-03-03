// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/VNGamemode.h"

#include "Map/VNMapCharacter.h"
#include "Core/PawnIsometric.h"
#include "Data/MinigameData.h"

#include "VNChapterManager.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API AVNChapterManager : public AVNGamemode
{
	GENERATED_BODY()

public:

	AVNChapterManager();

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type Reason) override;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Enable();
	virtual void Enable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Disable();
	virtual void Disable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TriggerMinigame(const FMinigameData& Minigame);
	virtual void TriggerMinigame_Implementation(const FMinigameData& Minigame) { }

	UFUNCTION(BlueprintCallable)
	void TravelOnHeightSwitcher(AActor* SwitcherA, AActor* SwitcherB);


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
