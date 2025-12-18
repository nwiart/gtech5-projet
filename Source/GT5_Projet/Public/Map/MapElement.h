// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapElement.generated.h"

UCLASS()
class GT5_PROJET_API AMapElement : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapElement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTilePosition(int& outX, int& outY) const;

	FIntPoint GetTilePosition() const;


	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerHit();

	// If true, this element blocks character movement (walls, obstacles, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bBlocksMovement;
};
