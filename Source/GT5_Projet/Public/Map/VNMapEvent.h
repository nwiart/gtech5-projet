// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNMapEvent.generated.h"

UCLASS()
class GT5_PROJET_API AVNMapEvent : public AActor
{
	GENERATED_BODY()

public:
	AVNMapEvent();

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTilePosition(FIntPoint& TilePos) const;

	FIntPoint GetTilePosition() const;


	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerHit();

	UFUNCTION(BlueprintCallable)
	void SetInactive();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInactive() const { return bIsInactive; }


	// If true, this element blocks character movement (walls, obstacles, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bBlocksMovement;

private:

	UPROPERTY(VisibleAnywhere)
	bool bIsInactive;
};
