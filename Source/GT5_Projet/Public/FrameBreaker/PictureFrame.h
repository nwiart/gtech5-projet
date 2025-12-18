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

	// Shatter the frame (called when hit by projectile)
	UFUNCTION(BlueprintCallable, Category = "Frame")
	void Shatter();

	// Check if frame is destroyed
	UPROPERTY(BlueprintReadOnly, Category = "Frame")
	bool bIsDestroyed = false;
};
