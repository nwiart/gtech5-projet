// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Map/VNMapCharacter.h"
#include "Core/PawnIsometric.h"
#include "Minigames/BaseMinigameGameMode.h"

#include "VNChapterSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnConnectionChanged,
	int32,
	NewValue
);

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UVNChapterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UVNChapterSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


	inline int32 GetConnection() const { return Connection; }
	inline int32 GetConnectionMinValue() const { return ConnectionMinValue; }
	inline int32 GetConnectionMaxValue() const { return ConnectionMaxValue; }


	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	bool OpenChapter(TSoftObjectPtr<UWorld> ChapterLevel, FName ChapterName, TSubclassOf<AActor> ManagerClass, TSubclassOf<APawnIsometric> PawnClass, TSubclassOf<AVNMapCharacter> CharacterClass, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void CloseChapter();

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	bool InitializeMinigame(TSubclassOf<ABaseMinigameGameMode> ManagerClass, TSubclassOf<APawn> PawnClass, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	void ExitMinigame(const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable)
	void ModifyConnection(int32 Delta);

	UFUNCTION(BlueprintCallable)
	void TriggerMinigame(FGuid MapEventGuid);

	UFUNCTION(BlueprintCallable)
	void NotifyChapterComplete();

	UFUNCTION(BlueprintCallable)
	void GetConnectionData(int32& OutMin, int32& OutMax, int32& OutValue) const;


	UPROPERTY(BlueprintReadOnly)
	FName CurrentChapterName;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UWorld> CurrentChapterLevel;

	UPROPERTY(BlueprintReadOnly)
	FGuid LastMinigameGuid;

	UPROPERTY(BlueprintReadOnly)
	int32 Connection = 0;


	UPROPERTY(BlueprintReadOnly, Category="Chapter Runtime")
	AActor* ManagerActor;

	UPROPERTY(BlueprintReadOnly, Category="Chapter Runtime")
	AVNMapCharacter* MapCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Chapter Runtime")
	APawnIsometric* PawnCamera;

	UPROPERTY(BlueprintReadOnly, Category="Chapter Runtime")
	FIntPoint PlayerPosition;


	UPROPERTY(BlueprintReadOnly, Category="Minigame Runtime")
	APawn* MinigamePawn;

	UPROPERTY(BlueprintReadOnly, Category = "Minigame Runtime")
	ABaseMinigameGameMode* MinigameManager;


	// Called whenever the player connection changed
	UPROPERTY(BlueprintAssignable)
	FOnConnectionChanged OnConnectionChanged;


	static constexpr int32 ConnectionMinValue = -100;
	static constexpr int32 ConnectionMaxValue = 100;

private:

	void ClampAndNotify();
};
