// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/PictureFrame.h"
#include "Minigames/FrameBreaker/FrameRotationComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
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

		// The painting canvas is authored in Blueprint as static mesh component(s)
		// parented under FrameMesh. Build dynamic materials so the hit reaction
		// can drain the life out of the image.
		TArray<USceneComponent*> ChildComponents;
		FrameMesh->GetChildrenComponents(/*bIncludeAllDescendants=*/true, ChildComponents);

		for (USceneComponent* Child : ChildComponents)
		{
			UStaticMeshComponent* CanvasMesh = Cast<UStaticMeshComponent>(Child);
			if (!CanvasMesh)
				continue;

			CanvasMeshes.Add(CanvasMesh);

			const int32 NumMaterials = CanvasMesh->GetNumMaterials();
			for (int32 MatIndex = 0; MatIndex < NumMaterials; ++MatIndex)
			{
				if (UMaterialInstanceDynamic* MID = CanvasMesh->CreateAndSetMaterialInstanceDynamic(MatIndex))
				{
					CanvasMIDs.Add(MID);
				}
			}
		}
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

	// End the scale punch and restore the base scale before physics takes over.
	// The life drain keeps running: the image finishes fading while the frame falls.
	bPunchActive = false;
	if (FrameMesh)
	{
		FrameMesh->SetRelativeScale3D(FrameBaseScale);
	}

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
	bPunchActive = true;
	HitReactionElapsed = 0.0f;
	SetActorTickEnabled(true);

	SetLifeDrain(0.0f);

	// Wisps escaping the canvas. Attached so they follow the frame, which is
	// still orbiting while the image drains.
	if (LifeDrainFX)
	{
		USceneComponent* AttachTo = CanvasMeshes.Num() > 0 ? Cast<USceneComponent>(CanvasMeshes[0]) : FrameMesh;
		if (AttachTo)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				LifeDrainFX,
				AttachTo,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				/*bAutoDestroy=*/true);
		}
	}
}

void APictureFrame::UpdateHitReaction(float DeltaSeconds)
{
	HitReactionElapsed += DeltaSeconds;

	// Scale punch: 0 -> peak -> 0 (half sine) over HitReactionDuration.
	// Stops as soon as the frame starts falling, so it never fights physics.
	const float PunchDuration = FMath::Max(HitReactionDuration, KINDA_SMALL_NUMBER);
	const float PunchAlpha = FMath::Clamp(HitReactionElapsed / PunchDuration, 0.0f, 1.0f);
	if (bPunchActive && FrameMesh)
	{
		const float Punch = FMath::Sin(PunchAlpha * PI) * ScalePunchStrength;
		FrameMesh->SetRelativeScale3D(FrameBaseScale * (1.0f + Punch));
	}

	// Life drain: 0 -> 1 over LifeDrainDuration. This outlives VanishDelay on
	// purpose, so the image keeps fading while the frame is already falling.
	const float DrainDuration = FMath::Max(LifeDrainDuration, KINDA_SMALL_NUMBER);
	const float DrainAlpha = FMath::Clamp(HitReactionElapsed / DrainDuration, 0.0f, 1.0f);
	SetLifeDrain(DrainAlpha);

	const bool bPunchDone = !bPunchActive || PunchAlpha >= 1.0f;
	if (bPunchDone && DrainAlpha >= 1.0f)
	{
		EndHitReaction();
	}
}

void APictureFrame::EndHitReaction()
{
	if (!bHitReacting)
		return;

	bHitReacting = false;
	bPunchActive = false;
	HitReactionElapsed = 0.0f;
	SetActorTickEnabled(false);

	if (FrameMesh)
	{
		FrameMesh->SetRelativeScale3D(FrameBaseScale);
	}

	// Leave the canvas fully drained - the image never comes back.
	SetLifeDrain(1.0f);
}

void APictureFrame::SetLifeDrain(float Value)
{
	for (UMaterialInstanceDynamic* MID : CanvasMIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(LifeDrainParamName, Value);
		}
	}
}
