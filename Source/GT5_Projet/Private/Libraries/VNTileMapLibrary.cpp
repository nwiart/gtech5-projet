// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/VNTileMapLibrary.h"

#include "Kismet/GameplayStatics.h"


const FName UVNTileMapLibrary::FloorTag = FName(TEXT("Floor"));
const FName UVNTileMapLibrary::EventTag = FName(TEXT("Event"));
const FName UVNTileMapLibrary::ObstacleTag = FName(TEXT("Obstacle"));


FIntPoint UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(const FVector& worldPos)
{
	return FIntPoint(FMath::FloorToInt(worldPos.X / TileSize), FMath::FloorToInt(worldPos.Y / TileSize));
}

FVector UVNTileMapLibrary::GetWorldPosFromTileCoordinates(const FIntPoint& tileCoords)
{
	return FVector(tileCoords.X * TileSize + TileHalfSize, tileCoords.Y * TileSize + TileHalfSize, 0.0);
}


FVector2D UVNTileMapLibrary::GetMousePositionInViewport(const UObject* worldContextObject)
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(worldContextObject, 0);
	FVector2D mousePos;
	FVector2D viewportSize;
	playerController->GetMousePosition(mousePos.X, mousePos.Y);
	GEngine->GameViewport->GetViewportSize(viewportSize);

	mousePos = mousePos / viewportSize * 2.0 - 1.0;
	mousePos.Y *= -1.0;

	return mousePos;
}
