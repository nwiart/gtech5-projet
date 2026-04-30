#include "Map/TileField.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Subsystems/MapSubsystem.h"


// Sets default values
ATileField::ATileField()
	: SizeX(10), SizeY(10)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->Tags.Add(TEXT("Floor"));
}

void ATileField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector loc = this->GetActorLocation();
	FVector he(SizeX * 50.0, SizeY * 50.0, 0.0);
	Box->SetBoxExtent(he);
	Box->SetRelativeLocation(he + FVector(FMath::RoundToDouble(loc.X / 100.0) * 100.0 - loc.X, FMath::RoundToDouble(loc.Y / 100.0) * 100.0 - loc.Y, 0.0));
}

void ATileField::BeginPlay()
{
	Super::BeginPlay();

	// Register ourselves as terrain.
	UMapSubsystem* subsys = this->GetWorld()->GetSubsystem<UMapSubsystem>();
	if (subsys) {
		subsys->AddTileField(this);
	}
}


bool ATileField::IsPointInside(const FVector& WorldLocation) const
{
	FVector center = GetActorLocation();
	FVector p0 = center - Box->GetScaledBoxExtent();
	FVector p1 = center + Box->GetScaledBoxExtent();

	return (WorldLocation.X >= p0.X && WorldLocation.X < p1.X) && (WorldLocation.Y >= p0.Y && WorldLocation.Y < p1.Y);
}
