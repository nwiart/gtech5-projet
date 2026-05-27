#include "Map/TileField.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Libraries/VNTileMapLibrary.h"
#include "Subsystems/MapSubsystem.h"


// A small positive Z half-extent so the collision volume is not degenerate;
// pathfinding's downward line trace covers ~250 world units so anything > 0 hits.
static constexpr double TileFieldBoxZHalfExtent = 10.0;


// Sets default values
ATileField::ATileField()
	: SizeX(10), SizeY(10)
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComponent);
	// Explicit query-only collision: pathfinding line-traces against the box,
	// no physics interaction needed. Don't rely on engine default profile —
	// it can be overridden in Blueprint subclasses and silently break traces.
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	this->Tags.Add(UVNTileMapLibrary::FloorTag);
}

void ATileField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector Location = GetActorLocation();
	const FVector Extent(
		SizeX * UVNTileMapLibrary::TileHalfSize,
		SizeY * UVNTileMapLibrary::TileHalfSize,
		TileFieldBoxZHalfExtent);
	Box->SetBoxExtent(Extent);

	// Snap the box so its tiles align with the world tile grid, regardless of
	// the actor's raw transform. Use the shared snap helper so GetStartTile and
	// this snap agree across platforms.
	const FIntPoint NearestTile = UVNTileMapLibrary::GetNearestTileFromWorldPos(Location);
	const double SnapX = NearestTile.X * UVNTileMapLibrary::TileSize - Location.X;
	const double SnapY = NearestTile.Y * UVNTileMapLibrary::TileSize - Location.Y;
	Box->SetRelativeLocation(FVector(Extent.X + SnapX, Extent.Y + SnapY, 0.0));
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

void ATileField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld()) {
		if (UMapSubsystem* Subsys = World->GetSubsystem<UMapSubsystem>()) {
			Subsys->RemoveTileField(this);
		}
	}

	Super::EndPlay(EndPlayReason);
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
	return UVNTileMapLibrary::GetNearestTileFromWorldPos(GetActorLocation());
}
