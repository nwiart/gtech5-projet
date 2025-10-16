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

	// Target mesh (cylinder or custom mesh)
	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	TargetMesh->SetupAttachment(RootComponent);
	TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TargetMesh->SetCollisionResponseToAllChannels(ECR_Block);
	TargetMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	TargetMesh->SetNotifyRigidBodyCollision(true);
	TargetMesh->SetGenerateOverlapEvents(true);

	// Critical points (glyphs/symbols) - positioned around the target
	CriticalPoint1 = CreateDefaultSubobject<USceneComponent>(TEXT("CriticalPoint1"));
	CriticalPoint1->SetupAttachment(TargetMesh);
	CriticalPoint1->SetRelativeLocation(FVector(100.0f, 0.0f, 50.0f));

	CriticalPoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("CriticalPoint2"));
	CriticalPoint2->SetupAttachment(TargetMesh);
	CriticalPoint2->SetRelativeLocation(FVector(100.0f, 0.0f, -50.0f));

	CriticalPoint3 = CreateDefaultSubobject<USceneComponent>(TEXT("CriticalPoint3"));
	CriticalPoint3->SetupAttachment(TargetMesh);
	CriticalPoint3->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));

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
	TArray<USceneComponent*> CriticalPoints = {CriticalPoint1, CriticalPoint2, CriticalPoint3};

	for (USceneComponent* Point : CriticalPoints)
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
	TArray<USceneComponent*> CriticalPoints = {CriticalPoint1, CriticalPoint2, CriticalPoint3};

	for (USceneComponent* Point : CriticalPoints)
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

