// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/VNMapBounds.h"

#include "Components/BoxComponent.h"


// Sets default values
AVNMapBounds::AVNMapBounds()
	: SizeX(10), SizeY(10)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVNMapBounds::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	Box->SetBoxExtent(FVector(SizeX * 50.0, SizeY * 50.0, 0.0));
}

FVector AVNMapBounds::ClampPoint(const FVector& WorldLocation) const
{
	FVector localLoc;
	FVector min, max;

	localLoc = (WorldLocation - GetActorLocation()).RotateAngleAxis(-45.0, FVector::ZAxisVector);

	GetViewBoundsRect(min, max);

	localLoc.X = FMath::Clamp(localLoc.X, min.X, max.X);
	localLoc.Y = FMath::Clamp(localLoc.Y, min.Y, max.Y);

	return localLoc.RotateAngleAxis(45.0, FVector::ZAxisVector) + GetActorLocation();
}

void AVNMapBounds::GetViewCorners(TInlineComponentArray<FVector, 4>& OutCorners) const
{
	const double extX = SizeX * 50.0, extY = SizeY * 50.0;

	const FVector corners[4] = {
		{  extX, -extY, 0.0 },
		{  extX,  extY, 0.0 },
		{ -extX,  extY, 0.0 },
		{ -extX, -extY, 0.0 },
	};

	for (int i = 0; i < 4; i++) {
		OutCorners.Add(corners[i].RotateAngleAxis(-45.0, FVector::ZAxisVector));
	}
}

void AVNMapBounds::GetViewBoundsRect(FVector& OutMin, FVector& OutMax) const
{
	TInlineComponentArray<FVector, 4> corners;
	GetViewCorners(corners);

	OutMin.Set(corners[0].Y, corners[3].X, 0.0);
	OutMax.Set(corners[2].Y, corners[1].X, 0.0);
}
