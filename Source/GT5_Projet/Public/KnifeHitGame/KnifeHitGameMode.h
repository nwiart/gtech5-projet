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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Game Settings")
	int32 TotalMatches = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	float MatchBurnDuration = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 CriticalPointsHit;

	// UI Display Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FString MapName = TEXT("KNIFE HIT GAME");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	int32 TotalCriticalPoints = 3;

	// UI Helper Functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	int32 GetMatchesUsed() const { return TotalMatches - RemainingMatches; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	float GetCriticalPointsProgress() const { return TotalCriticalPoints > 0 ? CriticalPointsHit / TotalCriticalPoints : 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsGameActive() const { return bGameActive; }

	// Mission Status
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsMissionCriticalPointsComplete() const { return CriticalPointsHit >= TotalCriticalPoints; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsMissionAllKnivesComplete() const { return RemainingMatches == 0; }

	// Win/Lose Screens
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> WinScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> LoseScreenClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowWinScreen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLoseScreen();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LaunchMatch();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnMatchHit(bool bHitCriticalPoint, bool bStillBurning, class AMatchProjectile* Match = nullptr);

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

	UPROPERTY()
	class AMatchProjectile* ReadyMatch;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float ReadyMatchDistance = 200.0f;

private:
	bool bGameActive;

	void SpawnReadyMatch();
};

	