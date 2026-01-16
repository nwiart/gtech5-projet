// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapElement.h"
#include "Systems/VNTileMapLibrary.h"
#include "Core/VNGamemode.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMapElement::AMapElement()
	: bBlocksMovement(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
}

// Called when the game starts or when spawned
void AMapElement::BeginPlay()
{
	Super::BeginPlay();
	
	AVNGamemode* gamemode = Cast<AVNGamemode>(UGameplayStatics::GetGameMode(this));
	if (gamemode) {
		gamemode->GetAllMapElements().Add(this);
	}
}


void AMapElement::GetTilePosition(FIntPoint& out) const
{
	FIntPoint pos = UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
	out = pos;
}

FIntPoint AMapElement::GetTilePosition() const
{
	return UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
}
