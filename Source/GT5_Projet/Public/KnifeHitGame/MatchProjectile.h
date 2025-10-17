// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MatchProjectile.generated.h"

UCLASS()
class GT5_PROJET_API AMatchProjectile : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchProjectile();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MatchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UParticleSystemComponent* FireParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Match Settings")
	float BurnDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Match Settings")
	float LaunchSpeed = 1000.0f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Match")
	void Launch(class ARotatingTarget* Target);

	UFUNCTION(BlueprintCallable, Category = "Match")
	bool IsStillBurning() const { return bIsBurning; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	void Extinguish();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float BurnTimer;
	bool bIsBurning;
	bool bHasStuck;
    
	class ARotatingTarget* TargetRef;
	class AKnifeHitGameMode* GameModeRef;

};
