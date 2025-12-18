// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FrameBreakerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class AProjectileBase;

/**
 * First-Person Character for Frame Breaker minigame
 * Has visible arms that play throw animation
 */
UCLASS()
class GT5_PROJET_API AFrameBreakerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFrameBreakerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Camera component (first-person)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	// Arms skeletal mesh (visible in FPS view)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ArmsMesh;

	// Knife mesh in hand (attached to ProjectileSocket)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* KnifeInHandMesh;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrowAction;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ThrowMontage;

	// Projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName ProjectileSocketName = "ProjectileSocket";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FVector ProjectileTargetOffset = FVector(500, 0, 0);

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<UCameraShakeBase> BreathingShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<UCameraShakeBase> ThrowShake;

	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ThrowSound;

	// Called by animation notify to spawn projectile
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile();

	// Show/hide knife in hand
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void ShowKnifeInHand();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void HideKnifeInHand();

protected:
	// Input handlers
	void OnThrowPressed(const FInputActionValue& Value);

	// Animation handlers
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayThrowAnimation();

	// Montage callbacks
	UFUNCTION()
	void OnThrowMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Check if can throw
	bool CanThrow() const;

private:
	bool bIsThrowing = false;
};
