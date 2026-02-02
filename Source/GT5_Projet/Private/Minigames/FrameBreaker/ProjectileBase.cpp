// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/ProjectileBase.h"
#include "Minigames/FrameBreaker/PictureFrame.h"
#include "Minigames/FrameBreaker/FrameBreakerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/VNChapterSubsystem.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	MeshComponent->SetNotifyRigidBodyCollision(true); // Enable hit events
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Bind hit event
	MeshComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity

	// Create trail component
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	TrailComponent->SetupAttachment(RootComponent);
	TrailComponent->bAutoActivate = true;

	// Auto-destroy after timeout
	InitialLifeSpan = 5.0f;
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Set trail effect if specified
	if (TrailFX && TrailComponent)
	{
		TrailComponent->SetAsset(TrailFX);
		TrailComponent->Activate();
	}
}

void AProjectileBase::LaunchTowardTarget(FVector TargetLocation)
{
	FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	ProjectileMovement->Velocity = Direction * ProjectileSpeed;

	UE_LOG(LogTemp, Log, TEXT("Projectile launched toward %s"), *TargetLocation.ToString());
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
		return;

	UE_LOG(LogTemp, Log, TEXT("Projectile hit: %s"), *OtherActor->GetName());

	// Check if we hit a picture frame
	if (APictureFrame* Frame = Cast<APictureFrame>(OtherActor))
	{
		HandleFrameHit(Frame, Hit);
	}

	// Destroy projectile
	Destroy();
}

void AProjectileBase::HandleFrameHit(APictureFrame* Frame, const FHitResult& Hit)
{
	if (!Frame)
		return;

	UE_LOG(LogTemp, Log, TEXT("Projectile hit frame!"));

	// 1. Spawn glass shatter VFX
	if (GlassShatterFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GlassShatterFX,
			Hit.Location,
			Hit.ImpactNormal.Rotation(),
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	// 2. Spawn spatial tear decal
	if (SpatialTearDecal)
	{
		UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			SpatialTearDecal,
			DecalSize,
			Hit.Location,
			Hit.ImpactNormal.Rotation(),
			DecalLifeSpan
		);
	}

	// 3. Camera shake
	if (HitShake)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->ClientStartCameraShake(HitShake);
		}
	}

	// 4. Slow motion effect
	if (bEnableSlowMotion)
	{
		ApplySlowMotionEffect();
	}

	// 5. Play shatter sound
	if (ShatterSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShatterSound, Hit.Location);
	}

	// 6. Destroy the frame
	Frame->Shatter();

	// 7. Notify GameMode
	UVNChapterSubsystem* subsys = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVNChapterSubsystem>();
	AFrameBreakerManager* GameMode = Cast<AFrameBreakerManager>(subsys->MinigameManager);
	if (GameMode)
	{
		GameMode->OnFrameDestroyed(Frame);
	}
}

void AProjectileBase::ApplySlowMotionEffect()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	// Apply slow motion
	UGameplayStatics::SetGlobalTimeDilation(World, SlowMotionScale);

	// Reset after duration
	World->GetTimerManager().SetTimer(
		SlowMotionTimerHandle,
		[World]()
		{
			if (World)
			{
				UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			}
		},
		SlowMotionDuration,
		false
	);
}
