// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/BaseMinigameGameMode.h"
#include "FrameBreakerGameMode.generated.h"

class APictureFrame;
class AFrameBreakerCharacter;

/**
 * Configuration for a single level
 */
USTRUCT(BlueprintType)
struct FFrameBreakerLevelConfig
{
	GENERATED_BODY()

	// Number of frames to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	int32 FrameCount = 12;

	// Number of knives available for this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	int32 KnifeCount = 15;

	// Rotation speed for frames
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	float RotationSpeed = 30.0f;

	// Radius of the cylinder
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	float CylinderRadius = 300.0f;

	// Difficulty level (affects rotation patterns)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	int32 DifficultyLevel = 1;

	// Score multiplier for this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Config")
	float ScoreMultiplier = 1.0f;

	FFrameBreakerLevelConfig()
		: FrameCount(12)
		, KnifeCount(15)
		, RotationSpeed(30.0f)
		, CylinderRadius(300.0f)
		, DifficultyLevel(1)
		, ScoreMultiplier(1.0f)
	{}
};

/**
 * Frame Breaker Minigame Mode
 * FPS game where player throws projectiles to destroy rotating 3D picture frames
 */
UCLASS()
class GT5_PROJET_API AFrameBreakerGameMode : public ABaseMinigameGameMode
{
	GENERATED_BODY()

public:
	AFrameBreakerGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// Frame management
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Breaker")
	TSubclassOf<APictureFrame> FrameClass;

	// Level configurations (configure different levels in Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Breaker|Levels")
	TArray<FFrameBreakerLevelConfig> LevelConfigs;

	// Current level index (0-based)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Breaker|Levels")
	int32 CurrentLevelIndex = 0;

	// Shared settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Breaker")
	FVector CylinderCenterPoint = FVector(500, 0, 100);

	// Runtime values (set from current level config)
	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	int32 TotalFrames = 12;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	int32 FramesDestroyed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	int32 CurrentScore = 0;

	// Knife inventory
	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	int32 TotalKnives = 7;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Breaker")
	int32 KnivesRemaining = 7;

	// Frame tracking
	UPROPERTY()
	TArray<APictureFrame*> Frames;

	// Level management
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker|Levels")
	void LoadLevel(int32 LevelIndex);

	UFUNCTION(BlueprintCallable, Category = "Frame Breaker|Levels")
	void StartNextLevel();

	UFUNCTION(BlueprintCallable, Category = "Frame Breaker|Levels")
	void RestartCurrentLevel();

	UFUNCTION(BlueprintPure, Category = "Frame Breaker|Levels")
	FFrameBreakerLevelConfig GetCurrentLevelConfig() const;

	// Spawn frames in cylindrical pattern
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void SpawnFramesInCylinder();

	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void ClearAllFrames();

	// Called when frame is destroyed
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void OnFrameDestroyed(APictureFrame* Frame);

	// Knife management
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void OnKnifeThrown();

	UFUNCTION(BlueprintPure, Category = "Frame Breaker")
	bool HasKnivesRemaining() const;

	// Set difficulty patterns
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void SetDifficulty(int32 Level);

	// Win/Lose conditions
	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void OnGameWon();

	UFUNCTION(BlueprintCallable, Category = "Frame Breaker")
	void OnGameLost();

protected:
	// Initialize default level configs if none are set
	void InitializeDefaultLevels();

	// Override base class methods
	virtual FMinigameResult BuildMinigameResult_Implementation(bool bSuccess) override;
};
