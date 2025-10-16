// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingTarget.generated.h"

UCLASS()
class GT5_PROJET_API ARotatingTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* TargetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* CriticalPoint1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* CriticalPoint2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* CriticalPoint3;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 100.0f; 

	UPROPERTY(EditAnywhere, Category = "Rotation")
	bool bRandomizeSpeed = true;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FVector2D SpeedRange = FVector2D(30.0f, 120.0f);

	UFUNCTION(BlueprintCallable, Category = "Target")
	bool IsNearCriticalPoint(FVector HitLocation, float Tolerance = 20.0f);

	UFUNCTION(BlueprintCallable, Category = "Target")
	USceneComponent* GetHitCriticalPoint(FVector HitLocation, float Tolerance = 20.0f);

	UFUNCTION(BlueprintCallable, Category = "Target")
	void DestroyCriticalPoint(USceneComponent* CriticalPoint);

private:
	float CurrentRotationSpeed;

};
