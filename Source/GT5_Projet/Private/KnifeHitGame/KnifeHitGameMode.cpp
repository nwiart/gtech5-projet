// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeHitGame/KnifeHitGameMode.h"
#include "KnifeHitGame/MatchProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "KnifeHitGame/KnifeHitPlayerController.h"
#include "KnifeHitGame/RotatingTarget.h"
#include "Blueprint/UserWidget.h"


AKnifeHitGameMode::AKnifeHitGameMode() {
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AKnifeHitPlayerController::StaticClass();

	ConnectionScore = 0;
	CriticalPointsHit = 0;
	bGameActive = false;
	ReadyMatch = nullptr;

	// Set minigame name
	MinigameName = TEXT("KNIFE HIT GAME");
	MinigameDescription = FText::FromString(
		TEXT("Throw all matches at the rotating target without hitting other matches!"));
}

void AKnifeHitGameMode::BeginPlay() {
	Super::BeginPlay();

	RemainingMatches = TotalMatches;
	bGameActive = true;

	if (TargetClass) {
		FVector SpawnLocation(0.0f, 0.0f, 200.0f);
		FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

		CurrentTarget = GetWorld()->SpawnActor<ARotatingTarget>(
			TargetClass,
			SpawnLocation,
			SpawnRotation
		);
	}

	SpawnReadyMatch();
}

void AKnifeHitGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	bool bAnyBurning = false;
	for (AMatchProjectile* Match : StuckMatches) {
		if (Match && Match->IsStillBurning()) {
			bAnyBurning = true;
			break;
		}
	}
}

void AKnifeHitGameMode::LaunchMatch() {
	if (!bGameActive || RemainingMatches <= 0 || !CurrentTarget || !ReadyMatch) return;

	ReadyMatch->Launch(CurrentTarget);
	RemainingMatches--;

	ReadyMatch = nullptr;
}

void AKnifeHitGameMode::SpawnReadyMatch() {
	if (!MatchClass || !CurrentTarget) return;

	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FVector SpawnLocation = TargetLocation + FVector(0.0f, 0.0f, -ReadyMatchDistance);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	ReadyMatch = GetWorld()->SpawnActor<AMatchProjectile>(
		MatchClass,
		SpawnLocation,
		SpawnRotation
	);

	if (ReadyMatch) {
		ReadyMatch->SetReadyState(true);
	}
}

void AKnifeHitGameMode::OnMatchHit(bool bHitCriticalPoint, bool bStillBurning, AMatchProjectile* Match) {
	if (!bStillBurning) {
		OnMatchCollision();
		return;
	}

	if (Match) {
		StuckMatches.Add(Match);
	}

	if (bHitCriticalPoint) {
		CriticalPointsHit++;
	}

	if (RemainingMatches > 0) {
		SpawnReadyMatch();
	}

	CheckTargetComplete();

	if (RemainingMatches == 0) {
		CalculateFinalScore();
	}
}

void AKnifeHitGameMode::OnMatchCollision() {
	bGameActive = false;

	for (AMatchProjectile* Match : StuckMatches) {
		if (Match) {
			Match->Extinguish();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Over - Collision!"));

	// Trigger minigame complete with failure
	OnMinigameComplete(false);
}

void AKnifeHitGameMode::RestartGame() {
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AKnifeHitGameMode::CalculateFinalScore() {
	bGameActive = false;

	// Calculate connection score delta
	if (CriticalPointsHit == 3) {
		ConnectionScore = 10;
	}
	else if (CriticalPointsHit == 2) {
		ConnectionScore = 5;
	}
	else if (CriticalPointsHit == 1) {
		ConnectionScore = 0;
	}
	else {
		ConnectionScore = -5;
	}

	UE_LOG(LogTemp, Log, TEXT("Final Score - Critical Points: %d, Connection Delta: %d"),
	       CriticalPointsHit, ConnectionScore);

	// Determine success/failure
	bool bSuccess = ConnectionScore > -100;

	// Trigger minigame complete
	OnMinigameComplete(bSuccess);
}

void AKnifeHitGameMode::CheckTargetComplete() {
	int32 MatchesStuckSuccessfully = TotalMatches - RemainingMatches;

	if (MatchesStuckSuccessfully == TotalMatches) {
		OnTargetComplete();
	}
}

void AKnifeHitGameMode::OnTargetComplete() {
	if (CurrentTarget) {
		for (AMatchProjectile* Match : StuckMatches) {
			if (Match) {
				Match->Destroy();
			}
		}
		StuckMatches.Empty();

		CurrentTarget->Destroy();
		CurrentTarget = nullptr;

		UE_LOG(LogTemp, Log, TEXT("Target Complete! Target and projectiles destroyed."));
	}
}

// Override base class methods to provide knife-specific data
float AKnifeHitGameMode::GetProgressPercentage_Implementation() const {
	if (TotalMatches <= 0) return 0.0f;
	return static_cast<float>(TotalMatches - RemainingMatches) / static_cast<float>(TotalMatches);
}

FText AKnifeHitGameMode::GetObjectiveText_Implementation() const {
	FString ObjectiveText = FString::Printf(TEXT("Hit %d/%d critical points - %d matches remaining"),
	                                        CriticalPointsHit, TotalCriticalPoints, RemainingMatches);
	return FText::FromString(ObjectiveText);
}

FMinigameResult AKnifeHitGameMode::BuildMinigameResult_Implementation(bool bSuccess) {
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;
	Result.ConnectionScoreDelta = ConnectionScore;
	Result.CompletionPercentage = GetProgressPercentage();

	// Add custom statistics
	Result.CustomStats.Add(TEXT("Critical Points Hit"), CriticalPointsHit);
	Result.CustomStats.Add(TEXT("Total Critical Points"), TotalCriticalPoints);
	Result.CustomStats.Add(TEXT("Matches Used"), TotalMatches - RemainingMatches);
	Result.CustomStats.Add(TEXT("Total Matches"), TotalMatches);

	// Add objectives completion status
	Result.ObjectivesCompleted.Add(TEXT("Hit all critical points"), IsMissionCriticalPointsComplete());
	Result.ObjectivesCompleted.Add(TEXT("Use all knives"), IsMissionAllKnivesComplete());

	// Add text data
	FString RankText;
	if (CriticalPointsHit == TotalCriticalPoints) {
		RankText = TEXT("Perfect!");
	}
	else if (CriticalPointsHit >= TotalCriticalPoints / 2) {
		RankText = TEXT("Good");
	}
	else {
		RankText = TEXT("Try Again");
	}
	Result.CustomTextData.Add(TEXT("Rank"), FText::FromString(RankText));

	return Result;
}
