// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PictureFrame.generated.h"

class UStaticMeshComponent;
class UFrameRotationComponent;
class UNiagaraSystem;
class USoundBase;

/**
 * Picture Frame Actor
 * Destructible frame that rotates in cylindrical trajectory
 */
UCLASS()
class GT5_PROJET_API APictureFrame : public AActor
{
	GENERATED_BODY()

public:
	APictureFrame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	// Frame mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FrameMesh;

	// Rotation component (handles cylindrical movement)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFrameRotationComponent* RotationComponent;

	// VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* ShatterFX;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ShatterSound;

	// Delay between the knife impact and the frame actually disappearing (seconds).
	// The hit reaction plays immediately; the mesh only vanishes after this beat.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame", meta = (ClampMin = "0.0"))
	float VanishDelay = 0.4f;

	// --- Hit reaction (plays during VanishDelay) ---

	// Duration of the scale punch (seconds). Keep <= VanishDelay.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Hit Reaction", meta = (ClampMin = "0.0"))
	float HitReactionDuration = 0.3f;

	// Peak extra scale of the punch (0.12 = +12% at the peak, back to normal at the end).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Hit Reaction", meta = (ClampMin = "0.0"))
	float ScalePunchStrength = 0.12f;

	// --- Physics fall (frame drops to the ground when shattered) ---

	// If true, the frame detaches and drops with physics instead of just hiding.
	// Requires the frame static mesh to have simple collision; otherwise it just hides.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Fall")
	bool bPhysicsFall = true;

	// Linear impulse (velocity change) pushing the frame over, along the throw direction.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Fall", meta = (ClampMin = "0.0"))
	float FallImpulseStrength = 350.0f;

	// Random spin (angular velocity change, deg/s) so the fall looks natural.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Fall", meta = (ClampMin = "0.0"))
	float FallAngularImpulse = 180.0f;

	// Seconds the fallen frame stays on the ground before being removed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame|Fall", meta = (ClampMin = "0.0"))
	float FallLifeSpan = 3.0f;

	// Shatter the frame (called when hit by projectile).
	// HitImpulseDirection drives which way the physics fall tips.
	UFUNCTION(BlueprintCallable, Category = "Frame")
	void Shatter(FVector HitImpulseDirection = FVector::ZeroVector, FVector HitLocation = FVector::ZeroVector);

	// Fired immediately on impact, before the frame vanishes.
	// Use for hit feedback: impact VFX, hit-stop, etc. (C++ already does a scale punch).
	UFUNCTION(BlueprintImplementableEvent, Category = "Frame")
	void OnHitReaction();

	// Hook for Blueprint to handle extra components on shatter (e.g. delayed destroy of BP-added meshes)
	UFUNCTION(BlueprintImplementableEvent, Category = "Frame")
	void OnShatter();

	// Check if frame is destroyed
	UPROPERTY(BlueprintReadOnly, Category = "Frame")
	bool bIsDestroyed = false;

private:
	// Second phase of Shatter(): break VFX/sound, hide mesh, schedule destroy.
	void CompleteShatter();

	// Scale punch, driven from Tick during the reaction window.
	void StartHitReaction();
	void UpdateHitReaction(float DeltaSeconds);
	void EndHitReaction();

	// Detach the frame mesh and let it drop with physics.
	// Returns false if the mesh has no simple collision to simulate.
	bool StartPhysicsFall();

	FTimerHandle VanishTimerHandle;

	bool bHitReacting = false;
	float HitReactionElapsed = 0.0f;
	FVector FrameBaseScale = FVector::OneVector;
	FVector PendingHitImpulseDir = FVector::ZeroVector;
};
