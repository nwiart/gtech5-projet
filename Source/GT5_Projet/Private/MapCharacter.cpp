// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCharacter.h"

#include "VNTileMapLibrary.h"


// Sets default values
AMapCharacter::AMapCharacter()
	: moving(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
}

// Called when the game starts or when spawned
void AMapCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	targetPosition = GetTilePosition();
}

// Called every frame
void AMapCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (moving) {
		SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(targetPosition));
		moving = false;
	}
}


void AMapCharacter::MoveTo(int X, int Y)
{
	targetPosition = FIntPoint(X, Y);
	moving = true;
}

void AMapCharacter::GetTilePosition(int& outX, int& outY) const
{
	FVector pos = GetActorLocation();
	outX = FMath::FloorToInt(pos.X / 100.0);
	outY = FMath::FloorToInt(pos.Y / 100.0);
}

FIntPoint AMapCharacter::GetTilePosition() const
{
	FVector pos = GetActorLocation();
	return FIntPoint(FMath::FloorToInt(pos.X / 100.0), FMath::FloorToInt(pos.Y / 100.0));
}
