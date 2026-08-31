// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/PictureFrame.h"
#include "Minigames/FrameBreaker/FrameRotationComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/BodySetup.h"
#include "TimerManager.h"

APictureFrame::APictureFrame()
{
	// Tick is only used briefly for the hit reaction; it starts disabled.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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

	// Cache the base scale so the hit reaction can punch it and restore it.
	if (FrameMesh)
	{
		FrameBaseScale = FrameMesh->GetRelativeScale3D();
	}
}

void APictureFrame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHitReacting)
	{
		UpdateHitReaction(DeltaSeconds);
	}
}

void APictureFrame::Shatter(FVector HitImpulseDirection, FVector HitLocation)
{
	if (bIsDestroyed)
		return;

	bIsDestroyed = true;
	PendingHitImpulseDir = HitImpulseDirection;

	UE_LOG(LogTemp, Log, TEXT("Frame hit: %s"), *GetName());

	// Disable collision right away so no other knife can hit it during the delay
	FrameMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Immediate impact feedback: scale punch (C++),
	// plus a Blueprint hook for anything extra (impact VFX, hit-stop...).
	StartHitReaction();
	OnHitReaction();

	// Delay the actual disappearance for a beat of feedback
	if (VanishDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			VanishTimerHandle, this, &APictureFrame::CompleteShatter, VanishDelay, false);
	}
	else
	{
		CompleteShatter();
	}
}

void APictureFrame::CompleteShatter()
{
	UE_LOG(LogTemp, Log, TEXT("Frame shattered: %s"), *GetName());

	// Stop the reaction in case it was still running (duration >= VanishDelay)
	EndHitReaction();

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

	// Stop the orbiting rotation before the frame drops / hides
	if (RotationComponent)
	{
		RotationComponent->SetComponentTickEnabled(false);
	}

	float RemoveAfter = 2.0f;
	if (bPhysicsFall && StartPhysicsFall())
	{
		// Frame is now free-falling to the ground; give it time to settle.
		RemoveAfter = FallLifeSpan;
	}
	else
	{
		// No physics fall available: fall back to the old instant hide.
		FrameMesh->SetVisibility(false);
	}

	// Let Blueprint handle extra components (e.g. delayed destroy of BP-added meshes)
	OnShatter();

	// Destroy actor after delay (allow the VFX / fall to finish)
	SetLifeSpan(RemoveAfter);
}

bool APictureFrame::StartPhysicsFall()
{
	if (!FrameMesh)
		return false;

	// Simulating physics needs simple collision on the static mesh.
	UBodySetup* BodySetup = FrameMesh->GetBodySetup();
	if (!BodySetup || BodySetup->AggGeom.GetElementCount() == 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("PictureFrame '%s': frame mesh has no simple collision, skipping physics fall"),
			*GetName());
		return false;
	}

	// Detach so physics can move the mesh freely in world space.
	FrameMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	// Collide with the world, simulate, and drop under gravity.
	FrameMesh->SetMobility(EComponentMobility::Movable);
	FrameMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	FrameMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FrameMesh->SetSimulatePhysics(true);
	FrameMesh->SetEnableGravity(true);

	// Push the frame over along the throw direction, plus a bit of random spin.
	FVector ImpulseDir = PendingHitImpulseDir.GetSafeNormal();
	if (ImpulseDir.IsNearlyZero())
	{
		ImpulseDir = GetActorForwardVector();
	}
	FrameMesh->AddImpulse(ImpulseDir * FallImpulseStrength, NAME_None, /*bVelChange=*/true);

	const FVector RandomAxis = FVector(
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal();
	FrameMesh->AddAngularImpulseInDegrees(RandomAxis * FallAngularImpulse, NAME_None, /*bVelChange=*/true);

	return true;
}

void APictureFrame::StartHitReaction()
{
	bHitReacting = true;
	HitReactionElapsed = 0.0f;
	SetActorTickEnabled(true);
}

void APictureFrame::UpdateHitReaction(float DeltaSeconds)
{
	HitReactionElapsed += DeltaSeconds;

	const float Duration = FMath::Max(HitReactionDuration, KINDA_SMALL_NUMBER);
	const float Alpha = FMath::Clamp(HitReactionElapsed / Duration, 0.0f, 1.0f);

	// Scale punch: 0 -> peak -> 0 (half sine)
	if (FrameMesh)
	{
		const float Punch = FMath::Sin(Alpha * PI) * ScalePunchStrength;
		FrameMesh->SetRelativeScale3D(FrameBaseScale * (1.0f + Punch));
	}

	if (Alpha >= 1.0f)
	{
		EndHitReaction();
	}
}

void APictureFrame::EndHitReaction()
{
	if (!bHitReacting)
		return;

	bHitReacting = false;
	HitReactionElapsed = 0.0f;
	SetActorTickEnabled(false);

	if (FrameMesh)
	{
		FrameMesh->SetRelativeScale3D(FrameBaseScale);
	}
}
