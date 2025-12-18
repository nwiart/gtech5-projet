// Fill out your copyright notice in the Description page of Project Settings.

#include "FrameBreaker/FrameRotationComponent.h"
#include "GameFramework/Actor.h"

UFrameRotationComponent::UFrameRotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFrameRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize pattern timer with random offset for variety
	PatternTimer = FMath::FRandRange(0.0f, 4.0f);
}

void UFrameRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// Update pattern timer
	PatternTimer += DeltaTime;

	// Get rotation delta based on pattern
	float DeltaRotation = GetPatternSpeed(DeltaTime);
	CurrentAngle += DeltaRotation;

	// Keep angle in 0-360 range
	if (CurrentAngle >= 360.0f)
		CurrentAngle -= 360.0f;
	else if (CurrentAngle < 0.0f)
		CurrentAngle += 360.0f;

	// Calculate new position on cylinder
	float RadAngle = FMath::DegreesToRadians(CurrentAngle);
	FVector NewLocation = CenterPoint;
	NewLocation.X += Radius * FMath::Cos(RadAngle);
	NewLocation.Y += Radius * FMath::Sin(RadAngle);

	// Update actor location
	Owner->SetActorLocation(NewLocation);

	// Update actor rotation (always face center)
	FRotator NewRotation = FRotator(0, CurrentAngle + 90, 0);
	Owner->SetActorRotation(NewRotation);
}

float UFrameRotationComponent::GetPatternSpeed(float DeltaTime)
{
	switch (CurrentPattern)
	{
		case ERotationPattern::Constant:
			return RotationSpeed * DeltaTime;

		case ERotationPattern::SpeedBurst:
		{
			float Multiplier = GetSpeedBurstMultiplier();
			return RotationSpeed * Multiplier * DeltaTime;
		}

		case ERotationPattern::Feint:
			return GetFeintSpeed(DeltaTime);

		case ERotationPattern::Flicker:
		{
			UpdateFlickerVisibility();
			return RotationSpeed * DeltaTime;
		}

		default:
			return RotationSpeed * DeltaTime;
	}
}

float UFrameRotationComponent::GetSpeedBurstMultiplier()
{
	// Use sine wave to vary speed: slow -> fast -> slow
	// Cycle time: 4 seconds
	float Cycle = FMath::Fmod(PatternTimer, 4.0f);
	float NormalizedCycle = Cycle / 4.0f; // 0 to 1

	// Speed multiplier: 0.5x to 3.0x
	float Multiplier = 1.0f + 2.0f * FMath::Abs(FMath::Sin(NormalizedCycle * PI * 2.0f));
	return Multiplier;
}

float UFrameRotationComponent::GetFeintSpeed(float DeltaTime)
{
	// Cycle: Rotate left -> Pause -> Reverse right
	// Total cycle: 4 seconds
	float Cycle = FMath::Fmod(PatternTimer, 4.0f);

	if (Cycle < 1.5f)
	{
		// Phase 1: Normal rotation (1.5 seconds)
		return RotationSpeed * DeltaTime;
	}
	else if (Cycle < 2.0f)
	{
		// Phase 2: Pause (0.5 seconds)
		return 0.0f;
	}
	else
	{
		// Phase 3: Reverse faster (2.0 seconds)
		return -RotationSpeed * 1.5f * DeltaTime;
	}
}

void UFrameRotationComponent::UpdateFlickerVisibility()
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// Random chance to toggle visibility (creates flicker effect)
	// 3% chance per frame
	if (FMath::FRandRange(0.0f, 1.0f) < 0.03f)
	{
		bool bCurrentlyHidden = Owner->IsHidden();
		Owner->SetActorHiddenInGame(!bCurrentlyHidden);
	}
}

void UFrameRotationComponent::SetCenterPoint(FVector NewCenter)
{
	CenterPoint = NewCenter;
}

void UFrameRotationComponent::SetRadius(float NewRadius)
{
	Radius = NewRadius;
}

void UFrameRotationComponent::SetPattern(ERotationPattern Pattern)
{
	CurrentPattern = Pattern;
	PatternTimer = 0.0f; // Reset timer when changing pattern
}