// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNMapEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapEventTrigger);


UCLASS()
class GT5_PROJET_API AVNMapEvent : public AActor
{
	GENERATED_BODY()

public:
	AVNMapEvent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTilePosition(FIntPoint& TilePos) const;

	FIntPoint GetTilePosition() const;


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnPlayerHit();
	virtual void OnPlayerHit_Implementation();

	UFUNCTION(BlueprintCallable)
	void SetInactive();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInactive() const { return bIsInactive; }


	// If true, this element blocks character movement (walls, obstacles, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bBlocksMovement;

	UPROPERTY(BlueprintAssignable)
	FOnMapEventTrigger OnTrigger;

private:

	UPROPERTY(VisibleAnywhere)
	bool bIsInactive;
};
