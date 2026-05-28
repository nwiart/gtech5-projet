// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PawnIsometric.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Libraries/VNTileMapLibrary.h"
#include "Libraries/PathfindingLibrary.h"
#include "Character/VNPlayerController.h"
#include "Subsystems/VNChapterSubsystem.h"
#include "Map/VNMapCharacter.h"
#include "Map/VNMapBounds.h"


const float APawnIsometric::HIGHLIGHT_Z_OFFSET = 0.5;
const float APawnIsometric::CURSOR_Z_OFFSET = 1.0;

const float APawnIsometric::CAMERA_MOVE_DURATION = 0.3f;


// Sets default values
APawnIsometric::APawnIsometric()
	: CameraSpeed(1.0F), CameraMinWidth(500.0F), CameraMaxWidth(1000.0F)
	, CharacterHeightLevel(0.0F)
	, MapBounds(0), PlayerCharacter(0)
	, cursorActor(0), highlightActor(0)
	, cursorPosition(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min())
	, hoveredTile(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min())
	, cameraMode(ECameraMode::PLAYER), moveToPlayerTime(-1.0F)
	, bIsCursorActive(true), bIsPanning(false), bIsCameraCentered(true)
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

void APawnIsometric::BeginPlay()
{
	Super::BeginPlay();

	bIsCursorActive = true;
	if (CursorClass) {
		cursorActor = GetWorld()->SpawnActor<AActor>(CursorClass.Get(), FTransform::Identity);
		if (cursorActor) {
			cursorActor->SetActorHiddenInGame(true);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn cursor actor for APawnIsometric."));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No cursor actor class provided to APawnIsometric."));
	}

	if (HighlightClass) {
		highlightActor = GetWorld()->SpawnActor<AActor>(HighlightClass.Get());
		if (highlightActor) {
			highlightActor->SetActorHiddenInGame(true);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn highlight actor for APawnIsometric."));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No highlight actor class provided to APawnIsometric. Tiles will not be highlighted."));
	}
}

void APawnIsometric::EndPlay(const EEndPlayReason::Type reason)
{
	if (cursorActor) {
		cursorActor->Destroy();
		cursorActor = 0;
	}

	if (highlightActor) {
		highlightActor->Destroy();
		highlightActor = 0;
	}

	Super::EndPlay(reason);
}

void APawnIsometric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera move / transition.
	if (cameraMode != ECameraMode::FREE) {
		// Place camera continuously onto character.
		if (PlayerCharacter && cameraMode == ECameraMode::PLAYER) {
			moveToPlayerEnd = PlayerCharacter->GetActorLocation();
			moveToPlayerEnd.Z = CharacterHeightLevel;
		}

		if (moveToPlayerTime >= 0.0F) {
			moveToPlayerTime += DeltaTime / CAMERA_MOVE_DURATION;

			if (moveToPlayerTime >= 1.0F) {
				moveToPlayerTime = -1.0F;
				SetActorLocation(moveToPlayerEnd);
			}
			else {
				float t = 1.0F - (FMath::Cos(PI * moveToPlayerTime) * 0.5F + 0.5F);
				FVector interpPos = moveToPlayerStart + t * (moveToPlayerEnd - moveToPlayerStart);
				SetActorLocation(interpPos);
			}
		}
		else {
			SetActorLocation(moveToPlayerEnd);
		}
	}

	// Highlight hovered tile.
	if (highlightActor) {
		if (!bIsCursorActive) {
			highlightActor->SetActorHiddenInGame(true);
		}
		else {
			FVector2D mousePos = UVNTileMapLibrary::GetMousePositionInViewport(this);
			FIntPoint tilePos = GetPointedTile(mousePos.X, mousePos.Y);

			if (hoveredTile != tilePos) {
				hoveredTile = tilePos;

				bool valid = UPathfindingLibrary::IsTileWalkable(tilePos, this);
				highlightActor->SetActorHiddenInGame(!valid);
				if (valid) {
					highlightActor->SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(tilePos) + FVector(0, 0, CharacterHeightLevel + HIGHLIGHT_Z_OFFSET));
				}
			}
		}
	}
}

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
	if (bIsCameraCentered) return;
	bIsCameraCentered = true;

	cameraMode = ECameraMode::PLAYER;

	moveToPlayerTime = 0.0F;
	moveToPlayerStart = GetActorLocation();
}

void APawnIsometric::FocusViewOnTile(const FIntPoint& TilePos)
{
	cameraMode = ECameraMode::TILE;

	moveToPlayerTime = 0.0F;
	moveToPlayerStart = GetActorLocation();
	moveToPlayerEnd = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(TilePos);
}

void APawnIsometric::SetViewCenteredOnTile(const FIntPoint& TilePos)
{
	SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(TilePos));
}

void APawnIsometric::SetCursorActive(bool bActive)
{
	const bool bWasActive = bIsCursorActive;
	bIsCursorActive = bActive;
	if (cursorActor) {
		cursorActor->SetActorHiddenInGame(!bActive);
	}

	if (highlightActor && !bActive) {
		highlightActor->SetActorHiddenInGame(true);
	}

	// When re-activating, invalidate the cached tile so Tick re-evaluates and
	// re-shows the highlight even if the mouse hasn't moved.
	if (!bWasActive && bActive) {
		hoveredTile = FIntPoint(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min());
	}
}

void APawnIsometric::SetCursorHidden(bool bCursorHidden)
{
	if (!bIsCursorActive) return;

	if (cursorActor) {
		cursorActor->SetActorHiddenInGame(bCursorHidden);
	}
}


void APawnIsometric::Input_PanCameraX(float w)
{
	if (!bIsPanning) return;

	bIsCameraCentered = false;
	cameraMode = ECameraMode::FREE;
	moveToPlayerTime = -1.0F;

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
	cameraMode = ECameraMode::FREE;
	moveToPlayerTime = -1.0F;

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
	else {
		this->FocusViewOnTile(tilePos);
	}

	cursorPosition = tilePos;
	cursorActor->SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(cursorPosition) + FVector(0, 0, CharacterHeightLevel + CURSOR_Z_OFFSET));
}
