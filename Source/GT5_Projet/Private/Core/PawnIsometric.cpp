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


// Sets default values
APawnIsometric::APawnIsometric()
	: CameraSpeed(1.0F), CameraMinWidth(200.0F), CameraMaxWidth(4000.0F)
	, cursorActor(0), bIsCursorActive(true)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	const FRotator CameraRotation(-30.0F, 45.0F, 0.0F);
	cameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1400.0F;
	Camera->OrthoNearClipPlane = -10000.0F;
	Camera->bAutoCalculateOrthoPlanes = false;
	Camera->SetRelativeRotation(CameraRotation);
	Camera->SetRelativeLocation(cameraForwardVector * -100.0F);

	Camera->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
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

// Called every frame
void APawnIsometric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawnIsometric::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("PanCameraX", this, &APawnIsometric::Input_PanCameraX);
	PlayerInputComponent->BindAxis("PanCameraY", this, &APawnIsometric::Input_PanCameraY);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &APawnIsometric::Input_ZoomCamera);

	PlayerInputComponent->BindAction("SelectTile", EInputEvent::IE_Released, this, &APawnIsometric::Input_SelectTile);
}

FVector APawnIsometric::ViewportToWorld(double viewportX, double viewportY) const
{
	FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const double halfHeight = Camera->OrthoWidth * 0.5;
	const double aspectRatio = viewportSize.X / viewportSize.Y;

	FVector off =
		viewportX * halfHeight * aspectRatio * Camera->GetRightVector() +
		viewportY * halfHeight * Camera->GetUpVector();

	// Project point onto Z = 0 plane (ground level).
	double t = -(off.Z - GetActorLocation().Z) / cameraForwardVector.Z;
	return GetActorLocation() + off + t * cameraForwardVector;
}

FIntPoint APawnIsometric::GetPointedTile(double viewportX, double viewportY) const
{
	return UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(ViewportToWorld(viewportX, viewportY));
}

void APawnIsometric::RecenterViewOnPlayer()
{
	AVNPlayerController* playerController = Cast<AVNPlayerController>(GetPlayerState()->GetPlayerController());
	if (!playerController || !playerController->PlayerCharacter) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't recenter view on player (could not find playerController, or map character isn't spawned yet."));
		return;
	}

	FIntPoint tilePos = playerController->PlayerCharacter->GetTilePosition();
	SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(tilePos));
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
	const FVector off(-w, w, 0.0F);
	float factor = Camera->OrthoWidth;
	float speed = factor * CameraSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);

	AddActorWorldOffset(off * speed);
}

void APawnIsometric::Input_PanCameraY(float w)
{
	const FVector off(w, w, 0.0F);
	float factor = Camera->OrthoWidth / -cameraForwardVector.Z;
	float speed = factor * CameraSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);

	AddActorWorldOffset(off * speed);
}

void APawnIsometric::Input_ZoomCamera(float w)
{
	w = 1.0F / (w * 0.1F + 1.0F);

	Camera->OrthoWidth = FMath::Clamp(Camera->OrthoWidth * w, CameraMinWidth, CameraMaxWidth);
}

void APawnIsometric::Input_SelectTile()
{
	if (!cursorActor || !bIsCursorActive) return;

	cursorActor->SetActorHiddenInGame(false);

	FVector2D mousePos = UVNTileMapLibrary::GetMousePositionInViewport(this);
	FIntPoint tilePos = GetPointedTile(mousePos.X, mousePos.Y);

	// Clicked the same tile.
	if (tilePos == cursorPosition) {
		UVNChapterSubsystem* chapterSubsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
		chapterSubsys->GetMapCharacter()->MoveTo(cursorPosition.X, cursorPosition.Y);
	}

	cursorPosition = tilePos;
	cursorActor->SetActorLocation(UVNTileMapLibrary::GetWorldPosFromTileCoordinates(cursorPosition) + FVector(0, 0, 0.5));
}
