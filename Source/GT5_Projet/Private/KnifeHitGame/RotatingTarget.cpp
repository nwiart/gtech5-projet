// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeHitGame/RotatingTarget.h"

// Sets default values
ARotatingTarget::ARotatingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	TargetMesh->SetupAttachment(RootComponent);
	TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TargetMesh->SetCollisionResponseToAllChannels(ECR_Block);
	TargetMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	TargetMesh->SetNotifyRigidBodyCollision(true);
	TargetMesh->SetGenerateOverlapEvents(true);

	CriticalPoint1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CriticalPointMesh1"));
	CriticalPoint1->SetupAttachment(RootComponent);
	CriticalPoint1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CriticalPoint1->SetCollisionResponseToAllChannels(ECR_Overlap);
	CriticalPoint1->SetGenerateOverlapEvents(true);

	CriticalPoint2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CriticalPointMesh2"));
	CriticalPoint2->SetupAttachment(RootComponent);
	CriticalPoint2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CriticalPoint2->SetCollisionResponseToAllChannels(ECR_Overlap);
	CriticalPoint2->SetGenerateOverlapEvents(true);
	
	CriticalPoint3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CriticalPointMesh3"));
	CriticalPoint3->SetupAttachment(RootComponent);
	CriticalPoint3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CriticalPoint3->SetCollisionResponseToAllChannels(ECR_Overlap);
	CriticalPoint3->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ARotatingTarget::BeginPlay()
{
	Super::BeginPlay();

	if (bRandomizeSpeed)
	{
		CurrentRotationSpeed = FMath::RandRange(SpeedRange.X, SpeedRange.Y);
	}
	else
	{
		CurrentRotationSpeed = RotationSpeed;
	}
}

// Called every frame
void ARotatingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator DeltaRotation(CurrentRotationSpeed * DeltaTime, 0.0f, 0.0f);
	AddActorLocalRotation(DeltaRotation);
}

bool ARotatingTarget::IsNearCriticalPoint(FVector HitLocation, float Tolerance) {
	TArray<UStaticMeshComponent*> CriticalPoints = {CriticalPoint1, CriticalPoint2, CriticalPoint3};

	for (UStaticMeshComponent* Point : CriticalPoints)
	{
		if (Point && Point->IsVisible())
		{
			float Distance = FVector::Dist(Point->GetComponentLocation(), HitLocation);
			if (Distance <= Tolerance)
			{
				return true;
			}
		}
	}

	return false;
}

USceneComponent* ARotatingTarget::GetHitCriticalPoint(FVector HitLocation, float Tolerance) {
	TArray<UStaticMeshComponent*> CriticalPoints = {CriticalPoint1, CriticalPoint2, CriticalPoint3};

	for (UStaticMeshComponent* Point : CriticalPoints)
	{
		if (Point && Point->IsVisible())
		{
			float Distance = FVector::Dist(Point->GetComponentLocation(), HitLocation);
			if (Distance <= Tolerance)
			{
				return Point;
			}
		}
	}

	return nullptr;
}

void ARotatingTarget::DestroyCriticalPoint(USceneComponent* CriticalPoint) {
	if (CriticalPoint)
	{
		CriticalPoint->SetVisibility(false, true);
	}
}

