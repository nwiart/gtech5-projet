// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnIsometric.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APawnIsometric::APawnIsometric()
	: CameraSpeed(1.0F), CameraMinWidth(200.0F), CameraMaxWidth(4000.0F)
	, cursorActor(0)
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
	
	cursorActor = GetWorld()->SpawnActor<AActor>(CursorClass.Get(), FTransform::Identity);
}

// Called every frame
void APawnIsometric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (cursorActor) {
		// TODO : Move to function library.
		APlayerController* playerController = GetPlayerState()->GetPlayerController();
		FVector2D mousePos;
		FVector2D viewportSize;
		playerController->GetMousePosition(mousePos.X, mousePos.Y);
		GEngine->GameViewport->GetViewportSize(viewportSize);

		mousePos = mousePos / viewportSize * 2.0 - 1.0;
		mousePos.Y *= -1.0;

		FIntVector2 tilePos = GetPointedTile(mousePos.X, mousePos.Y);

		cursorActor->SetActorLocation(FVector(tilePos.X * 100 + 50, tilePos.Y * 100 + 50, 1.0F));
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
}

FVector APawnIsometric::ViewportToWorld(double viewportX, double viewportY) const
{
	const double halfHeight = Camera->OrthoWidth * 0.5;
	FVector off =
		viewportX * halfHeight * Camera->AspectRatio * Camera->GetRightVector() +
		viewportY * halfHeight * Camera->GetUpVector();

	double t = -(off.Z - GetActorLocation().Z) / cameraForwardVector.Z;

	return GetActorLocation() + off + t * cameraForwardVector;
}

FIntVector2 APawnIsometric::GetPointedTile(double viewportX, double viewportY) const
{
	FVector pos = ViewportToWorld(viewportX, viewportY);

	return FIntVector2(FMath::FloorToInt(pos.X / 100.0), FMath::FloorToInt(pos.Y / 100.0));
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
}
