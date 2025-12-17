// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class APictureFrame;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;

/**
 * Base class for projectiles (knife, glass shard, etc.)
 * Handles movement, collision, and visual effects
 */
UCLASS()
class GT5_PROJET_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

protected:
	virtual void BeginPlay() override;

public:
	// Mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// Trail VFX
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* TrailComponent;

	// Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileSpeed = 2000.0f;

	// VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* TrailFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* GlassShatterFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UMaterialInterface* SpatialTearDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FVector DecalSize = FVector(50, 50, 50);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float DecalLifeSpan = 10.0f;

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<UCameraShakeBase> HitShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	bool bEnableSlowMotion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float SlowMotionScale = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float SlowMotionDuration = 0.2f;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ShatterSound;

	// Launch projectile toward target
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void LaunchTowardTarget(FVector TargetLocation);

protected:
	// Collision handler
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Handle frame hit
	void HandleFrameHit(APictureFrame* Frame, const FHitResult& Hit);

	// Slow motion effect
	void ApplySlowMotionEffect();

private:
	FTimerHandle SlowMotionTimerHandle;
};