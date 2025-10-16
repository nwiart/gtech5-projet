// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KnifeHitGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API AKnifeHitGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKnifeHitGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Game state
	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	int32 RemainingMatches;

	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	int32 ConnectionScore;

	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	int32 TotalMatches = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	float MatchBurnDuration = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 CriticalPointsHit;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LaunchMatch();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnMatchHit(bool bHitCriticalPoint, bool bStillBurning);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnMatchCollision(); 

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void CalculateFinalScore();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void CheckTargetComplete();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnTargetComplete();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<class AMatchProjectile> MatchClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<class ARotatingTarget> TargetClass;

	UPROPERTY()
	class ARotatingTarget* CurrentTarget;

	UPROPERTY()
	TArray<class AMatchProjectile*> StuckMatches;

private:
	bool bGameActive;
};

	