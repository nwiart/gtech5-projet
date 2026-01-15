// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VNChapterSubsystem.generated.h"

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


	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	bool OpenChapter(FName ChapterName, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void TriggerMinigame(FGuid MapEventGuid);

	UFUNCTION(BlueprintCallable)
	void NotifyChapterComplete();


	UPROPERTY(BlueprintReadOnly)
	FName CurrentChapterName;

	UPROPERTY(BlueprintReadOnly)
	FGuid LastMinigameGuid;
};
