// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/KnifeHitGame/MatchProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Minigames/KnifeHitGame/KnifeHitManager.h"
#include "Minigames/KnifeHitGame/RotatingTarget.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMatchProjectile::AMatchProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f); 
	CollisionComp->SetNotifyRigidBodyCollision(true);

	RootComponent = CollisionComp;

	MatchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MatchMesh"));
	MatchMesh->SetupAttachment(RootComponent);
	MatchMesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	MatchMesh->SetNotifyRigidBodyCollision(false);
	MatchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	FireParticle->SetupAttachment(MatchMesh);
	FireParticle->bAutoActivate = false;
	FireParticle->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = LaunchSpeed;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	bIsBurning = true;
	bHasStuck = false;
	BurnTimer = 0.0f;

}

// Called when the game starts or when spawned
void AMatchProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentHit.AddDynamic(this, &AMatchProjectile::OnHit);

	GameModeRef = Cast<AKnifeHitManager>(UGameplayStatics::GetGameMode(this));
}

void AMatchProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bIsBurning)
	{
		BurnTimer += DeltaTime;
        
		if (BurnTimer >= BurnDuration)
		{
			Extinguish();
		}
	}
}

void AMatchProjectile::Extinguish() {
	bIsBurning = false;

	if (FireParticle)
	{
		FireParticle->Deactivate();
	}
}

void AMatchProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit) {
	if (bHasStuck) return;

	AMatchProjectile* OtherMatch = Cast<AMatchProjectile>(OtherActor);
	if (OtherMatch)
	{
		if (GameModeRef)
		{
			GameModeRef->OnMatchCollision();
		}
		return;
	}

	ARotatingTarget* HitTarget = Cast<ARotatingTarget>(OtherActor);
	if (HitTarget && HitTarget == TargetRef)
	{
		bHasStuck = true;

		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetActive(false);
		
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		const FVector DirectionIntoTarget = -Hit.Normal;
		const FRotator BaseRotation = DirectionIntoTarget.Rotation();
		const FRotator FinalRotation = BaseRotation + FRotator(-90.0f, 0.0f, 0.0f);
		SetActorRotation(FinalRotation);

		FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
		AttachToComponent(OtherComp, AttachRules);

		USceneComponent* HitCriticalPoint = HitTarget->GetHitCriticalPoint(Hit.Location, CriticalPointTolerance);
		bool bHitCritical = HitCriticalPoint != nullptr;

		if (bHitCritical)
		{
			HitTarget->DestroyCriticalPoint(HitCriticalPoint);
		}

		if (GameModeRef)
		{
			GameModeRef->OnMatchHit(bHitCritical, bIsBurning, this);
		}
	}
}

void AMatchProjectile::Launch(class ARotatingTarget* Target) {
	TargetRef = Target;

	// Start burning when launched
	bIsBurning = true;
	BurnTimer = 0.0f;

	// Activate fire particle effect
	if (FireParticle)
	{
		FireParticle->Activate(true);
	}

	// Enable collision
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (ProjectileMovement)
	{
		ProjectileMovement->SetActive(true);
		FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = DirectionToTarget * LaunchSpeed;

		FRotator BaseRotation = DirectionToTarget.Rotation();
		FRotator FinalRotation = BaseRotation + FRotator(-90.0f, 0.0f, 0.0f);

		SetActorRotation(FinalRotation);
	}
}

void AMatchProjectile::SetReadyState(bool bReady) {
	if (bReady)
	{
		bIsBurning = true;  // Keep fire burning in ready state
		BurnTimer = 0.0f;
		bHasStuck = false;

		// Activate fire particle for ready match
		if (FireParticle)
		{
			FireParticle->Activate(true);
		}

		if (ProjectileMovement)
		{
			ProjectileMovement->SetActive(false);
			ProjectileMovement->Velocity = FVector::ZeroVector;
		}

		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMatchProjectile::UpdateFireScale(float FireIntensity) {
	if (FireParticle && bIsBurning)
	{
		// Keep minimum scale at 0.3 so fire doesn't disappear too early
		float MinScale = 0.3f;
		float FireScale = FMath::Lerp(MinScale, 1.0f, FireIntensity);
		FireParticle->SetRelativeScale3D(FVector(FireScale));
	}
}


