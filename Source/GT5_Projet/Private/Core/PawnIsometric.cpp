// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PawnIsometric.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Libraries/VNTileMapLibrary.h"
#include "Character/VNPlayerController.h"
#include "Subsystems/VNChapterSubsystem.h"
#include "Map/VNMapCharacter.h"
#include "Map/VNMapBounds.h"


// Sets default values
APawnIsometric::APawnIsometric()
	: CameraSpeed(1.0F), CameraMinWidth(200.0F), CameraMaxWidth(4000.0F)
	, cursorActor(0), bIsCursorActive(true), bIsPanning(false), bIsCameraCentered(true), PlayerCharacter(0), MapBounds(0)
	, CharacterHeightLevel(0.0F)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostPhysics;

	const FRotator CameraRotation(-30.0F, 45.0F, 0.0F);
	cameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1400.0F;
	Camera->OrthoNearClipPlane = -1000.0F;
	Camera->bAutoCalculateOrthoPlanes = false;

	Camera->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SetActorRotation(CameraRotation);
}

// Called when the game starts or when spawned
void APawnIsometric::BeginPlay()
{
	Super::BeginPlay();
	
	bIsCursorActive = true;
	cursorActor = GetWorld()->SpawnActor<AActor>(CursorClass.Get(), FTransform::Identity);
	cursorActor->SetActorHiddenInGame(true);
}

void APawnIsometric::EndPlay(const EEndPlayReason::Type reason)
{
	cursorActor->Destroy();

	Super::EndPlay(reason);
}

void APawnIsometric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter && bIsCameraCentered) {
		FVector pos = PlayerCharacter->GetActorLocation();
		pos.Z = CharacterHeightLevel;

		double proj = cameraForwardVector.Dot(pos - GetActorLocation());
		pos -= cameraForwardVector * proj;
		SetActorLocation(pos);
	}
}

// Called to bind functionality to input
void APawnIsometric::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PanCameraX", this, &APawnIsometric::Input_PanCameraX);
	PlayerInputComponent->BindAxis("PanCameraY", this, &APawnIsometric::Input_PanCameraY);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &APawnIsometric::Input_ZoomCamera);

	PlayerInputComponent->BindAction("SelectTile", EInputEvent::IE_Released, this, &APawnIsometric::Input_SelectTile);
	PlayerInputComponent->BindAction("PanCamera", EInputEvent::IE_Pressed, this, &APawnIsometric::Input_PanCameraStart);
	PlayerInputComponent->BindAction("PanCamera", EInputEvent::IE_Released, this, &APawnIsometric::Input_PanCameraStop);
}

FVector APawnIsometric::ViewportToWorld(double viewportX, double viewportY) const
{
	FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const double halfHeight = Camera->OrthoWidth * 0.5;
	const double aspectRatio = viewportSize.X / viewportSize.Y;

	FVector off =
		viewportX * halfHeight * aspectRatio * Camera->GetRightVector() +
		viewportY * halfHeight * Camera->GetUpVector();

	// Project point onto Z = CharacterHeightLevel plane (ground level).
	double t = -((off.Z + GetActorLocation().Z) - CharacterHeightLevel) / cameraForwardVector.Z;
	return GetActorLocation() + off + t * cameraForwardVector;
}

FIntPoint APawnIsometric::GetPointedTile(double viewportX, double viewportY) const
{
	return UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(ViewportToWorld(viewportX, viewportY));
}

void APawnIsometric::SetZoomLevel(float Value)
{
	Camera->OrthoWidth = FMath::Lerp(CameraMinWidth, CameraMaxWidth, Value);
}

void APawnIsometric::RecenterViewOnPlayer()
{
	bIsCameraCentered = true;
}

void APawnIsometric::SetViewCenteredOnTile(const FIntPoint& TilePos)
{
	SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(TilePos));
}

void APawnIsometric::SetCursorActive(bool bActive)
{
	bIsCursorActive = bActive;
	cursorActor->SetActorHiddenInGame(!bActive);
}

void APawnIsometric::SetCursorHidden(bool bCursorHidden)
{
	if (!bIsCursorActive) return;

	cursorActor->SetActorHiddenInGame(bCursorHidden);
}


void APawnIsometric::Input_PanCameraX(float w)
{
	FVector2D mousePos = UVNTileMapLibrary::GetMousePositionInViewport(this);
	FIntPoint tilePos = GetPointedTile(mousePos.X, mousePos.Y);

	cursorActor->SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(tilePos) + FVector(0, 0, CharacterHeightLevel + 0.5));


	if (!bIsPanning) return;

	bIsCameraCentered = false;

	w = -w;   // Invert direction.
	const FVector off = Camera->GetRightVector() * w;
	float factor = Camera->OrthoWidth;
	float speed = factor * CameraSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);

	AddActorWorldOffset(off * speed);

	if (MapBounds) {
		SetActorLocation(MapBounds->ClampPoint(GetActorLocation()));
	}

	OnMoveCamera.Broadcast(off);
}

void APawnIsometric::Input_PanCameraY(float w)
{
	if (!bIsPanning) return;

	bIsCameraCentered = false;

	w = -w;   // Invert direction.
	const FVector off = Camera->GetUpVector() * w;
	float factor = Camera->OrthoWidth;
	float speed = factor * CameraSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);

	AddActorWorldOffset(off * speed);

	if (MapBounds) {
		SetActorLocation(MapBounds->ClampPoint(GetActorLocation()));
	}

	OnMoveCamera.Broadcast(off);
}

void APawnIsometric::Input_ZoomCamera(float w)
{
	w = 1.0F / (w * 0.1F + 1.0F);

	Camera->OrthoWidth = FMath::Clamp(Camera->OrthoWidth * w, CameraMinWidth, CameraMaxWidth);

	float relativeValue = (Camera->OrthoWidth - CameraMinWidth) / (CameraMaxWidth - CameraMinWidth);
	OnZoomChanged.Broadcast(relativeValue);
}

void APawnIsometric::Input_PanCameraStart()
{
	bIsPanning = true;
}

void APawnIsometric::Input_PanCameraStop()
{
	bIsPanning = false;
}

#include "Libraries/PathfindingLibrary.h"

void APawnIsometric::Input_SelectTile()
{
	if (!cursorActor || !bIsCursorActive) return;

	FVector2D mousePos = UVNTileMapLibrary::GetMousePositionInViewport(this);
	FIntPoint tilePos = GetPointedTile(mousePos.X, mousePos.Y);

	if (!UPathfindingLibrary::IsTileWalkable(tilePos, this)) {
		return;
	}

	cursorActor->SetActorHiddenInGame(false);

	// Clicked the same tile.
	if (tilePos == cursorPosition) {
		UVNChapterSubsystem* chapterSubsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
		chapterSubsys->GetMapCharacter()->MoveTo(cursorPosition.X, cursorPosition.Y);
	}

	cursorPosition = tilePos;
	cursorActor->SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(cursorPosition) + FVector(0, 0, CharacterHeightLevel + 0.5));
}
