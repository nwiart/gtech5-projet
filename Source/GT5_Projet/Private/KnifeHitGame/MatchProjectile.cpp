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

	MatchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MatchMesh"));
	RootComponent = MatchMesh;
	MatchMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	MatchMesh->SetNotifyRigidBodyCollision(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = MatchMesh;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = LaunchSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	bIsBurning = true;
	bHasStuck = false;
	BurnTimer = 0.0f;

}

// Called when the game starts or when spawned
void AMatchProjectile::BeginPlay()
{
	Super::BeginPlay();

	MatchMesh->OnComponentHit.AddDynamic(this, &AMatchProjectile::OnHit);

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

		MatchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetActorLocation(Hit.Location);

		FRotator HitRotation = Hit.Normal.Rotation();
		HitRotation.Yaw += 180.0f;
		SetActorRotation(HitRotation);

		FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
		AttachToComponent(OtherComp, AttachRules);

		USceneComponent* HitCriticalPoint = HitTarget->GetHitCriticalPoint(Hit.Location, 50.0f);
		bool bHitCritical = HitCriticalPoint != nullptr;

		if (bHitCritical)
		{
			HitTarget->DestroyCriticalPoint(HitCriticalPoint);
		}

		if (GameModeRef)
		{
			GameModeRef->OnMatchHit(bHitCritical, bIsBurning);
		}
	}
}

void AMatchProjectile::Launch(class ARotatingTarget* Target) {
	TargetRef = Target;

	if (ProjectileMovement)
	{
		FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = DirectionToTarget * LaunchSpeed;
	}
}


