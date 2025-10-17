// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeHitGame/MatchProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KnifeHitGame/KnifeHitGameMode.h"
#include "KnifeHitGame/RotatingTarget.h"

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

	GameModeRef = Cast<AKnifeHitGameMode>(UGameplayStatics::GetGameMode(this));
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

		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		const FVector DirectionIntoTarget = -Hit.Normal;
		const FRotator BaseRotation = DirectionIntoTarget.Rotation();
		const FRotator FinalRotation = BaseRotation + FRotator(90.0f, 0.0f, 0.0f);
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

	// Enable collision
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (ProjectileMovement)
	{
		ProjectileMovement->SetActive(true);
		FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = DirectionToTarget * LaunchSpeed;

		FRotator BaseRotation = DirectionToTarget.Rotation();
		FRotator FinalRotation = BaseRotation + FRotator(90.0f, 0.0f, 0.0f);

		SetActorRotation(FinalRotation);
	}
}

void AMatchProjectile::SetReadyState(bool bReady) {
	if (bReady)
	{
		// In ready state: not burning, not moving, no collision with other matches
		bIsBurning = false;
		BurnTimer = 0.0f;

		if (ProjectileMovement)
		{
			ProjectileMovement->SetActive(false);
			ProjectileMovement->Velocity = FVector::ZeroVector;
		}

		MatchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
}


