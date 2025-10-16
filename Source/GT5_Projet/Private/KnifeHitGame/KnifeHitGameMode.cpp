// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeHitGame/KnifeHitGameMode.h"
#include "KnifeHitGame/MatchProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "KnifeHitGame/KnifeHitPlayerController.h"
#include "KnifeHitGame/RotatingTarget.h"


AKnifeHitGameMode::AKnifeHitGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AKnifeHitPlayerController::StaticClass();
    
	ConnectionScore = 0;
	CriticalPointsHit = 0;
	bGameActive = false;
}

void AKnifeHitGameMode::BeginPlay() {
	Super::BeginPlay();

	RemainingMatches = TotalMatches;
	bGameActive = true;

	if (TargetClass)
	{
		FVector SpawnLocation(0.0f, 0.0f, 200.0f);
		FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
        
		CurrentTarget = GetWorld()->SpawnActor<ARotatingTarget>(
			TargetClass, 
			SpawnLocation, 
			SpawnRotation
		);
	}

}

void AKnifeHitGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	bool bAnyBurning = false;
	for (AMatchProjectile* Match : StuckMatches)
	{
		if (Match && Match->IsStillBurning())
		{
			bAnyBurning = true;
			break;
		}
	}
}

void AKnifeHitGameMode::LaunchMatch() {
	if (!bGameActive || RemainingMatches <= 0 || !CurrentTarget) return;

	if (MatchClass)
	{
		FVector TargetLocation = CurrentTarget->GetActorLocation();
        
		FVector SpawnLocation = TargetLocation + FVector(0.0f, 0.0f, -300.0f);
        
		FRotator SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
        
		AMatchProjectile* NewMatch = GetWorld()->SpawnActor<AMatchProjectile>(
			MatchClass,
			SpawnLocation,
			SpawnRotation
		);

		if (NewMatch)
		{
			NewMatch->Launch(CurrentTarget);
			RemainingMatches--;
		}
	}
}

void AKnifeHitGameMode::OnMatchHit(bool bHitCriticalPoint, bool bStillBurning) {
	if (!bStillBurning)
	{
		OnMatchCollision();
		return;
	}

	if (bHitCriticalPoint)
	{
		CriticalPointsHit++;
	}

	CheckTargetComplete();

	if (RemainingMatches == 0)
	{
		CalculateFinalScore();
	}
}

void AKnifeHitGameMode::OnMatchCollision() {
	bGameActive = false;

	for (AMatchProjectile* Match : StuckMatches)
	{
		if (Match)
		{
			Match->Extinguish();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Over - Collision!"));
	
}

void AKnifeHitGameMode::RestartGame() {
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AKnifeHitGameMode::CalculateFinalScore() {
	bGameActive = false;

	if (CriticalPointsHit == 3)
	{
		ConnectionScore += 10;
	}
	else if (CriticalPointsHit == 2)
	{
		ConnectionScore += 5;
	}
	else if (CriticalPointsHit == 1)
	{
		ConnectionScore += 0;
	}
	else
	{
		ConnectionScore -= 5;
	}

	UE_LOG(LogTemp, Log, TEXT("Final Score - Critical Points: %d, Connection: %d"),
		CriticalPointsHit, ConnectionScore);

	if (ConnectionScore <= -100)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection too low - Chapter Failed!"));
	}
}

void AKnifeHitGameMode::CheckTargetComplete() {
	int32 MatchesStuckSuccessfully = TotalMatches - RemainingMatches;

	if (MatchesStuckSuccessfully == TotalMatches)
	{
		OnTargetComplete();
	}
}

void AKnifeHitGameMode::OnTargetComplete() {
	if (CurrentTarget)
	{
		CurrentTarget->Destroy();
		CurrentTarget = nullptr;

		UE_LOG(LogTemp, Log, TEXT("Target Complete! Target destroyed."));
	}
}
