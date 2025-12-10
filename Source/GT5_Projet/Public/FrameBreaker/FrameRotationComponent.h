// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FrameRotationComponent.generated.h"

/**
 * Rotation patterns for frames
 */
UENUM(BlueprintType)
enum class ERotationPattern : uint8
{
	Constant     UMETA(DisplayName = "Constant"),
	SpeedBurst   UMETA(DisplayName = "Speed Burst"),
	Feint        UMETA(DisplayName = "Feint"),
	Flicker      UMETA(DisplayName = "Flicker")
};

/**
 * Frame Rotation Component
 * Handles cylindrical trajectory rotation with various patterns
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GT5_PROJET_API UFrameRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFrameRotationComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Rotation settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FVector CenterPoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed = 30.0f; // Degrees per second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	ERotationPattern CurrentPattern = ERotationPattern::Constant;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float CurrentAngle = 0.0f;

	// Setters
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void SetCenterPoint(FVector NewCenter);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void SetRadius(float NewRadius);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void SetPattern(ERotationPattern Pattern);

protected:
	// Get rotation speed based on pattern
	float GetPatternSpeed(float DeltaTime);

	// Pattern-specific helpers
	float GetSpeedBurstMultiplier();
	float GetFeintSpeed(float DeltaTime);
	void UpdateFlickerVisibility();

private:
	float PatternTimer = 0.0f;
};
