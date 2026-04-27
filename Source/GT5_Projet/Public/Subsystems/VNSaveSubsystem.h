// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Save/VNSaveGame.h"

#include "VNSaveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UVNSaveSubsystem();

	// Subsystem implementation.
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void Load(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION(BlueprintCallable)
	bool DeleteSave();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool SaveExists() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UClass* GetSaveGameClass() const;


	UFUNCTION(BlueprintCallable)
	FTableEvents GetEventData(const FGuid& Guid);

	UFUNCTION(BlueprintCallable)
	void SetEventData(const FGuid& Guid, const FTableEvents& Data);


	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVNSaveGame> SaveGame;


private:

	static const FString SLOT_NAME;
};
