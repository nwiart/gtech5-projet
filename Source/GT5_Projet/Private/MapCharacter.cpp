// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCharacter.h"

#include "VNGamemode.h"
#include "VNTileMapLibrary.h"

#include "MapElement.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AMapCharacter::AMapCharacter()
	: moving(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

		AVNGamemode* gamemode = Cast<AVNGamemode>(UGameplayStatics::GetGameMode(this));
		if (gamemode) {
			for (AMapElement* elem : gamemode->GetAllMapElements()) {
				FIntPoint pos = elem->GetTilePosition();
				if (pos == targetPosition) {
					elem->OnPlayerHit();
					break;
				}
			}
		}
	}
}


void AMapCharacter::MoveTo(int X, int Y)
{
	targetPosition = FIntPoint(X, Y);
	moving = true;
}

void AMapCharacter::GetTilePosition(int& outX, int& outY) const
{
	FIntPoint pos = UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
	outX = pos.X;
	outY = pos.Y;
}

FIntPoint AMapCharacter::GetTilePosition() const
{
	return UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
}
