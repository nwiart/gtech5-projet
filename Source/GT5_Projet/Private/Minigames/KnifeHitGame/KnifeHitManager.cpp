// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/KnifeHitGame/KnifeHitManager.h"
#include "Minigames/KnifeHitGame/MatchProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Minigames/KnifeHitGame/KnifeHitPlayerController.h"
#include "Minigames/KnifeHitGame/RotatingTarget.h"
#include "Blueprint/UserWidget.h"


AKnifeHitManager::AKnifeHitManager() {
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AKnifeHitPlayerController::StaticClass();

	// Disable default pawn spawning (we don't need a physical player)
	DefaultPawnClass = nullptr;

	ConnectionScore = 0;
	CriticalPointsHit = 0;
	bGameActive = false;
	ReadyMatch = nullptr;
	FlyingMatch = nullptr;

	// Set minigame name
	MinigameName = TEXT("KNIFE HIT GAME");
	MinigameDescription = FText::FromString(
		TEXT("Throw all matches at the rotating target without hitting other matches!"));
}

void AKnifeHitManager::BeginPlay() {
	Super::BeginPlay();

	RemainingMatches = TotalMatches;
	bGameActive = true;
	CurrentFireTime = InitialFireTime;

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

void AKnifeHitManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bGameActive)
	{
		// Countdown global fire timer
		CurrentFireTime -= DeltaTime;

		if (CurrentFireTime <= 0.0f)
		{
			CurrentFireTime = 0.0f;
			OnFireTimerExpired();
			return;
		}

		float FireIntensity = GetFireIntensity();

		if (ReadyMatch)
		{
			ReadyMatch->UpdateFireScale(FireIntensity);
		}

		for (AMatchProjectile* Match : StuckMatches)
		{
			if (Match)
			{
				Match->UpdateFireScale(FireIntensity);
			}
		}
	}
}

void AKnifeHitManager::LaunchMatch() {
	// Prevent launching if match already in flight
	if (FlyingMatch)
	{
		return;
	}

	if (!bGameActive || RemainingMatches <= 0 || !CurrentTarget || !ReadyMatch)
	{
		return;
	}

	FlyingMatch = ReadyMatch;
	FlyingMatch->Launch(CurrentTarget);
	RemainingMatches--;

	ReadyMatch = nullptr;
}

void AKnifeHitManager::SpawnReadyMatch() {
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

void AKnifeHitManager::OnMatchHit(bool bHitCriticalPoint, bool bStillBurning, AMatchProjectile* Match) {
	if (!bStillBurning) {
		OnMatchCollision();
		return;
	}

	// Clear flying match since it has now stuck to target
	FlyingMatch = nullptr;

	if (Match) {
		StuckMatches.Add(Match);
	}

	if (bHitCriticalPoint) {
		CriticalPointsHit++;
	}

	// Add time bonus for successful throw (decreasing as game progresses)
	float TimeBonus = CalculateTimeBonusForThrow();
	CurrentFireTime += TimeBonus;

	if (RemainingMatches > 0) {
		SpawnReadyMatch();
	}

	CheckTargetComplete();

	if (RemainingMatches == 0) {
		CalculateFinalScore();
	}
}

void AKnifeHitManager::OnMatchCollision() {
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

void AKnifeHitManager::RestartGame() {
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AKnifeHitManager::CalculateFinalScore() {
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

	bool bSuccess = ConnectionScore > -100;

	OnMinigameComplete(bSuccess);
}

void AKnifeHitManager::CheckTargetComplete() {
	int32 MatchesStuckSuccessfully = TotalMatches - RemainingMatches;

	if (MatchesStuckSuccessfully == TotalMatches) {
		OnTargetComplete();
	}
}

void AKnifeHitManager::OnTargetComplete() {
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

FMinigameResult AKnifeHitManager::BuildMinigameResult_Implementation(bool bSuccess) {
	FMinigameResult Result;
	Result.bSuccess = bSuccess;
	Result.MinigameName = MinigameName;
	Result.ConnectionScoreDelta = ConnectionScore;

	// Add custom statistics with formatted text
	Result.CustomStats.Add(TEXT("Critical Points"),
		FText::FromString(FString::Printf(TEXT("%d / %d"), CriticalPointsHit, TotalCriticalPoints)));

	Result.CustomStats.Add(TEXT("Matches Used"),
		FText::FromString(FString::Printf(TEXT("%d / %d"), TotalMatches - RemainingMatches, TotalMatches)));

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

	return Result;
}

void AKnifeHitManager::OnFireTimerExpired() {
	bGameActive = false;

	if (ReadyMatch)
	{
		ReadyMatch->Extinguish();
	}

	for (AMatchProjectile* Match : StuckMatches)
	{
		if (Match)
		{
			Match->Extinguish();
		}
	}

	OnMinigameComplete(false);
}

float AKnifeHitManager::CalculateTimeBonusForThrow() const {
	float Progress = TotalMatches > 0 ? static_cast<float>(TotalMatches - RemainingMatches) / static_cast<float>(TotalMatches) : 1.0f;

	float TimeBonus = FMath::Lerp(MaxBonusTime, MinBonusTime, Progress);

	return TimeBonus;
}
