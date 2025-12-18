// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/PictureFrame.h"
#include "Minigames/FrameBreaker/FrameRotationComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

APictureFrame::APictureFrame()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root scene component
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Create frame mesh
	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	FrameMesh->SetupAttachment(RootComponent);
	FrameMesh->SetCollisionProfileName(TEXT("BlockAll"));
	FrameMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FrameMesh->SetNotifyRigidBodyCollision(true);

	// Create rotation component
	RotationComponent = CreateDefaultSubobject<UFrameRotationComponent>(TEXT("RotationComponent"));
}

void APictureFrame::BeginPlay()
{
	Super::BeginPlay();
}

void APictureFrame::Shatter()
{
	if (bIsDestroyed)
		return;

	bIsDestroyed = true;

	UE_LOG(LogTemp, Log, TEXT("Frame shattered: %s"), *GetName());

	// Spawn shatter VFX
	if (ShatterFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ShatterFX,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	// Play shatter sound
	if (ShatterSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShatterSound, GetActorLocation());
	}

	// Disable collision
	FrameMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Hide mesh
	FrameMesh->SetVisibility(false);

	// Disable rotation
	if (RotationComponent)
	{
		RotationComponent->SetComponentTickEnabled(false);
	}

	// Destroy actor after delay (allow VFX to play)
	SetLifeSpan(2.0f);
}
