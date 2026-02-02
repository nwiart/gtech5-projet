// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/FrameBreakerGameMode.h"
#include "Minigames/FrameBreaker/FrameBreakerCharacter.h"
#include "Minigames/FrameBreaker/PictureFrame.h"
#include "Minigames/FrameBreaker/FrameRotationComponent.h"
#include "Kismet/GameplayStatics.h"

AFrameBreakerGameMode::AFrameBreakerGameMode()
{
	// Set default pawn class to Frame Breaker Character
	DefaultPawnClass = AFrameBreakerCharacter::StaticClass();

	MinigameName = TEXT("Frame Breaker");
	MinigameDescription = FText::FromString(TEXT("Destroy all rotating picture frames!"));

	bIsMinigameActive = true;
}

void AFrameBreakerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Show mouse cursor
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		// Set input mode to Game and UI (allows both game input and UI interaction)
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Mouse cursor enabled"));
	}

	// Initialize default levels if no configs are set
	InitializeDefaultLevels();

	// Load the first level
	LoadLevel(CurrentLevelIndex);
}

void AFrameBreakerGameMode::EndPlay(const EEndPlayReason::Type reason)
{
	ClearAllFrames();

	Super::EndPlay(reason);
}

void AFrameBreakerGameMode::InitializeDefaultLevels()
{
	// If no levels configured, create default level
	if (LevelConfigs.Num() == 0)
	{
		FFrameBreakerLevelConfig DefaultLevel;
		DefaultLevel.FrameCount = 12;
		DefaultLevel.KnifeCount = 15;
		DefaultLevel.RotationSpeed = 30.0f;
		DefaultLevel.CylinderRadius = 300.0f;
		DefaultLevel.DifficultyLevel = 1;
		DefaultLevel.ScoreMultiplier = 1.0f;
		LevelConfigs.Add(DefaultLevel);

		UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Initialized default level with %d frames and %d knives"),
			DefaultLevel.FrameCount, DefaultLevel.KnifeCount);
	}
}

void AFrameBreakerGameMode::LoadLevel(int32 LevelIndex)
{
	// Validate level index
	if (LevelIndex < 0 || LevelIndex >= LevelConfigs.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("FrameBreakerGameMode: Invalid level index %d (max: %d)"), LevelIndex, LevelConfigs.Num() - 1);
		return;
	}

	CurrentLevelIndex = LevelIndex;
	FFrameBreakerLevelConfig Config = LevelConfigs[CurrentLevelIndex];

	UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Loading Level %d - %d frames at speed %.1f"),
		CurrentLevelIndex + 1, Config.FrameCount, Config.RotationSpeed);

	// Clear existing frames
	ClearAllFrames();

	// Apply level configuration
	TotalFrames = Config.FrameCount;
	TotalKnives = Config.KnifeCount;

	// Reset counters
	FramesDestroyed = 0;
	CurrentScore = 0;
	KnivesRemaining = TotalKnives;

	// Spawn frames
	SpawnFramesInCylinder();

	// Set difficulty patterns
	SetDifficulty(Config.DifficultyLevel);
}

void AFrameBreakerGameMode::StartNextLevel()
{
	int32 NextLevel = CurrentLevelIndex + 1;

	if (NextLevel >= LevelConfigs.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerGameMode: No more levels! Already at level %d"), CurrentLevelIndex + 1);
		return;
	}

	LoadLevel(NextLevel);
}

void AFrameBreakerGameMode::RestartCurrentLevel()
{
	LoadLevel(CurrentLevelIndex);
}

FFrameBreakerLevelConfig AFrameBreakerGameMode::GetCurrentLevelConfig() const
{
	if (CurrentLevelIndex >= 0 && CurrentLevelIndex < LevelConfigs.Num())
	{
		return LevelConfigs[CurrentLevelIndex];
	}

	// Return default config if invalid
	return FFrameBreakerLevelConfig();
}

void AFrameBreakerGameMode::SpawnFramesInCylinder()
{
	if (!FrameClass)
	{
		UE_LOG(LogTemp, Error, TEXT("FrameBreakerGameMode: FrameClass is not set!"));
		return;
	}

	// Get current level config
	FFrameBreakerLevelConfig Config = GetCurrentLevelConfig();
	float CylinderRadius = Config.CylinderRadius;

	// Calculate angle step for even distribution
	float AngleStep = 360.0f / TotalFrames;

	for (int32 i = 0; i < TotalFrames; i++)
	{
		// Calculate angle for this frame
		float Angle = i * AngleStep;
		float RadAngle = FMath::DegreesToRadians(Angle);

		// Calculate position on cylinder
		FVector SpawnLocation = CylinderCenterPoint;
		SpawnLocation.X += CylinderRadius * FMath::Cos(RadAngle);
		SpawnLocation.Y += CylinderRadius * FMath::Sin(RadAngle);

		// Calculate rotation (face inward toward center)
		FRotator SpawnRotation = FRotator(0, Angle + 90, 0);

		// Spawn frame
		APictureFrame* Frame = GetWorld()->SpawnActor<APictureFrame>(
			FrameClass,
			SpawnLocation,
			SpawnRotation
		);

		if (Frame && Frame->RotationComponent)
		{
			// Configure rotation component
			Frame->RotationComponent->SetCenterPoint(CylinderCenterPoint);
			Frame->RotationComponent->SetRadius(CylinderRadius);
			Frame->RotationComponent->CurrentAngle = Angle;

			Frames.Add(Frame);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Spawned %d frames in cylinder"), Frames.Num());
}

void AFrameBreakerGameMode::ClearAllFrames()
{
	// Destroy all existing frames
	for (APictureFrame* Frame : Frames)
	{
		if (Frame)
		{
			Frame->Destroy();
		}
	}

	Frames.Empty();
	UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Cleared all frames"));
}

void AFrameBreakerGameMode::OnFrameDestroyed(APictureFrame* Frame)
{
	if (!Frame)
		return;

	FramesDestroyed++;

	// Apply score multiplier from current level config
	FFrameBreakerLevelConfig Config = GetCurrentLevelConfig();
	int32 BaseScore = 100;
	int32 ScoreGained = FMath::RoundToInt(BaseScore * Config.ScoreMultiplier);
	CurrentScore += ScoreGained;

	UE_LOG(LogTemp, Log, TEXT("Frame destroyed! %d / %d (Score: +%d)"), FramesDestroyed, TotalFrames, ScoreGained);

	// Check win condition
	if (FramesDestroyed >= TotalFrames)
	{
		OnGameWon();
	}
}

void AFrameBreakerGameMode::SetDifficulty(int32 Level)
{
	if (Frames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerGameMode: No frames to set difficulty for!"));
		return;
	}

	// Get rotation speed from current level config
	FFrameBreakerLevelConfig Config = GetCurrentLevelConfig();
	float RotationSpeed = Config.RotationSpeed;

	switch (Level)
	{
		case 1: // Easy - All constant rotation
		{
			for (APictureFrame* Frame : Frames)
			{
				if (Frame && Frame->RotationComponent)
				{
					Frame->RotationComponent->SetPattern(ERotationPattern::Constant);
					Frame->RotationComponent->RotationSpeed = RotationSpeed;
				}
			}
			break;
		}

		case 2: // Medium - Mix of constant and speed burst
		{
			for (int32 i = 0; i < Frames.Num(); i++)
			{
				if (Frames[i] && Frames[i]->RotationComponent)
				{
					ERotationPattern Pattern = (i % 2 == 0) ? ERotationPattern::Constant : ERotationPattern::SpeedBurst;
					Frames[i]->RotationComponent->SetPattern(Pattern);
					Frames[i]->RotationComponent->RotationSpeed = RotationSpeed;
				}
			}
			break;
		}

		case 3: // Hard - All patterns
		{
			for (int32 i = 0; i < Frames.Num(); i++)
			{
				if (Frames[i] && Frames[i]->RotationComponent)
				{
					ERotationPattern Pattern = static_cast<ERotationPattern>(i % 4);
					Frames[i]->RotationComponent->SetPattern(Pattern);
					Frames[i]->RotationComponent->RotationSpeed = RotationSpeed;
				}
			}
			break;
		}

		default:
			UE_LOG(LogTemp, Warning, TEXT("FrameBreakerGameMode: Invalid difficulty level %d"), Level);
			break;
	}

	UE_LOG(LogTemp, Log, TEXT("FrameBreakerGameMode: Set difficulty to level %d with speed %.1f"), Level, RotationSpeed);
}

void AFrameBreakerGameMode::OnKnifeThrown()
{
	if (!bIsMinigameActive)
		return;

	// Decrease knife count
	KnivesRemaining--;

	UE_LOG(LogTemp, Log, TEXT("Knife thrown! %d / %d remaining"), KnivesRemaining, TotalKnives);

	// Check lose condition
	if (KnivesRemaining <= 0 && FramesDestroyed < TotalFrames)
	{
		OnGameLost();
	}
}

bool AFrameBreakerGameMode::HasKnivesRemaining() const
{
	return KnivesRemaining > 0;
}

void AFrameBreakerGameMode::OnGameLost()
{
	UE_LOG(LogTemp, Log, TEXT("Frame Breaker: DEFEAT! Ran out of knives"));

	OnMinigameComplete(false);
}

void AFrameBreakerGameMode::OnGameWon()
{
	UE_LOG(LogTemp, Log, TEXT("Frame Breaker: VICTORY!"));

	OnMinigameComplete(true);
}

FMinigameResult AFrameBreakerGameMode::BuildMinigameResult_Implementation(bool bSuccess)
{
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;

	// Calculate Connection Score based on performance
	int32 ConnectionDelta = 0;

	if (bSuccess)
	{
		// Victory - Calculate based on knives remaining
		if (KnivesRemaining >= 3)
		{
			ConnectionDelta = 10; // Excellent performance
			UE_LOG(LogTemp, Log, TEXT("Victory with %d knives remaining: +10 Connection"), KnivesRemaining);
		}
		else if (KnivesRemaining == 2)
		{
			ConnectionDelta = 5; // Good performance
			UE_LOG(LogTemp, Log, TEXT("Victory with 2 knives remaining: +5 Connection"));
		}
		else if (KnivesRemaining == 1)
		{
			ConnectionDelta = 0; // Acceptable performance
			UE_LOG(LogTemp, Log, TEXT("Victory with 1 knife remaining: 0 Connection"));
		}
		else // KnivesRemaining == 0
		{
			ConnectionDelta = -5; // Won with exact amount (risky)
			UE_LOG(LogTemp, Log, TEXT("Victory with 0 knives remaining: -5 Connection"));
		}
	}
	else
	{
		// Defeat - Negative connection
		ConnectionDelta = -10;
		UE_LOG(LogTemp, Log, TEXT("Defeat: -10 Connection"));
	}

	Result.ConnectionScoreDelta = ConnectionDelta;

	Result.CustomStats.Add(TEXT("Tableaux Détruits"),
		FText::FromString(FString::Printf(TEXT("%d / %d"), FramesDestroyed, TotalFrames)));

	Result.CustomStats.Add(TEXT("Couteaux Utilisés"),
		FText::FromString(FString::Printf(TEXT("%d / %d"), TotalKnives - KnivesRemaining, TotalKnives)));

	Result.CustomStats.Add(TEXT("Couteaux Restants"),
		FText::AsNumber(KnivesRemaining));

	return Result;
}

