#include "Map/TileField.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/MapSubsystem.h"


// Sets default values
ATileField::ATileField()
	: SizeX(10), SizeY(10)
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComponent);

	this->Tags.Add(UVNTileMapLibrary::FloorTag);
}

void ATileField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector Location = GetActorLocation();
	const FVector Extent(SizeX * UVNTileMapLibrary::TileHalfSize, SizeY * UVNTileMapLibrary::TileHalfSize, 0.0);
	Box->SetBoxExtent(Extent);

	// Snap the box so its tiles align with the world tile grid, regardless of the actor's raw transform.
	const double SnapX = FMath::RoundToDouble(Location.X / UVNTileMapLibrary::TileSize) * UVNTileMapLibrary::TileSize - Location.X;
	const double SnapY = FMath::RoundToDouble(Location.Y / UVNTileMapLibrary::TileSize) * UVNTileMapLibrary::TileSize - Location.Y;
	Box->SetRelativeLocation(Extent + FVector(SnapX, SnapY, 0.0));
}

void ATileField::BeginPlay()
{
	Super::BeginPlay();

	// Register ourselves as terrain.
	UMapSubsystem* Subsys = GetWorld()->GetSubsystem<UMapSubsystem>();
	if (Subsys) {
		Subsys->AddTileField(this);
	}
}


bool ATileField::IsPointInside(const FVector& WorldLocation) const
{
	// The box is offset in OnConstruction() to snap to the tile grid, so its
	// world center is not the actor location — use the component's own location.
	const FVector Center = Box->GetComponentLocation();
	const FVector Extent = Box->GetScaledBoxExtent();
	const FVector P0 = Center - Extent;
	const FVector P1 = Center + Extent;

	return (WorldLocation.X >= P0.X && WorldLocation.X < P1.X) && (WorldLocation.Y >= P0.Y && WorldLocation.Y < P1.Y);
}

FIntPoint ATileField::GetStartTile() const
{
	const FVector Location = GetActorLocation();
	return FIntPoint(
		FMath::RoundToInt(Location.X / UVNTileMapLibrary::TileSize),
		FMath::RoundToInt(Location.Y / UVNTileMapLibrary::TileSize));
}
