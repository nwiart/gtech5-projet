// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNMapBounds.generated.h"

class UBoxComponent;

/*
 * Tile map boundaries.
 * 
 * This has no effect on map characters or events, and is only used to limit how far
 * the camera can travel.
 */
UCLASS()
class GT5_PROJET_API AVNMapBounds : public AActor
{
	GENERATED_BODY()
	
public:	

	AVNMapBounds();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Clamp a point (expressed in world space) within the view bounds.
	// The returned point is transformed back into world space.
	FVector ClampPoint(const FVector& WorldLocation) const;

	void GetViewCorners(TInlineComponentArray<FVector, 4>& OutCorners) const;

	// Get the bounding rectangle in view space (rotated -45 deg).
	void GetViewBoundsRect(FVector& OutMin, FVector& OutMax) const;


public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBoxComponent* Box;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int SizeX;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int SizeY;
};
