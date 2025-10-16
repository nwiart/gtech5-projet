// Fill out your copyright notice in the Description page of Project Settings.


#include "VNTileMapLibrary.h"

#include "Kismet/GameplayStatics.h"


FIntPoint UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(const FVector& worldPos)
{
	return FIntPoint(FMath::FloorToInt(worldPos.X / 100.0), FMath::FloorToInt(worldPos.Y / 100.0));
}

FVector UVNTileMapLibrary::GetWorldPosFromTileCoordinates(const FIntPoint& tileCoords)
{
	return FVector(tileCoords.X * 100 + 50, tileCoords.Y * 100 + 50, 0.0);
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
