// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/VNMapCharacter.h"

#include "Core/VNGamemode.h"
#include "Libraries/VNTileMapLibrary.h"
#include "Libraries/PathfindingLibrary.h"

#include "Map/VNMapEvent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AVNMapCharacter::AVNMapCharacter()
	: CurrentWaypointIndex(0)
	, MovementProgress(0.f)
	, bIsMoving(false)
	, CharacterZOffset(0.f)
	, MovementSpeed(3.0f)
	, HeightLevel(0.0F)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

// Called when the game starts or when spawned
void AVNMapCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterZOffset = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	CurrentWorldPosition = GetActorLocation();
	TargetWorldPosition = CurrentWorldPosition;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FRotator(CurrentRotation.Pitch, 40, CurrentRotation.Roll));
}

// Called every frame
void AVNMapCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving || PathToFollow.Num() == 0)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	MovementProgress += DeltaTime * MovementSpeed;

	// Arrived at end of node.
	if (MovementProgress >= 1.0f)
	{
		FIntPoint TilePos = PathToFollow[CurrentWaypointIndex];
		CurrentWaypointIndex++;

		// Check for hit event.
		AVNMapEvent* elem = UPathfindingLibrary::GetTileEvent(TilePos, this);

		// Last point reached.
		if (CurrentWaypointIndex >= PathToFollow.Num() || elem != NULL)
		{
			SetActorLocation(TargetWorldPosition);
			bIsMoving = false;

			GetCharacterMovement()->Velocity = FVector::ZeroVector;

			PathToFollow.Empty();

			if (elem != NULL) {
				elem->OnPlayerHit();
			}
			return;
		}

		MovementProgress = 0.0f;
		CurrentWorldPosition = TargetWorldPosition;
		FVector NextPos = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(PathToFollow[CurrentWaypointIndex]);
		TargetWorldPosition = FVector(NextPos.X, NextPos.Y, HeightLevel + CharacterZOffset);
	}

	FVector NewPosition = FMath::Lerp(CurrentWorldPosition, TargetWorldPosition, MovementProgress);
	SetActorLocation(NewPosition);

	// Calculate velocity for animation (direction * speed)
	FVector MovementDirection = (TargetWorldPosition - CurrentWorldPosition).GetSafeNormal();
	FVector TileSize = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(FIntPoint(1, 0)) - UVNTileMapLibrary::GetWorldPosFromTileCoordinates(FIntPoint(0, 0));
	float TileDistance = TileSize.Size();
	GetCharacterMovement()->Velocity = MovementDirection * (MovementSpeed * TileDistance);

	if (!MovementDirection.IsNearlyZero())
	{
		float TargetYaw = 40.0f; // Default

		if (FMath::Abs(MovementDirection.X) > FMath::Abs(MovementDirection.Y))
		{
			TargetYaw = (MovementDirection.X > 0) ? 40.0f : 46.0f;  // +X → 40°, -X → 46°
		}
		else
		{
			TargetYaw = (MovementDirection.Y > 0) ? 46.0f : 40.0f;  // +Y → 46°, -Y → 40°
		}

		FRotator CurrentRotation = GetActorRotation();
		SetActorRotation(FRotator(CurrentRotation.Pitch, TargetYaw, CurrentRotation.Roll));
	}
}


void AVNMapCharacter::Enable()
{
	SetActorHiddenInGame(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AVNMapCharacter::Disable()
{
	SetActorHiddenInGame(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVNMapCharacter::MoveTo(int X, int Y)
{
	FIntPoint StartTile = GetTilePosition();
	FIntPoint EndTile(X, Y);

	// Calculate path using A* algorithm with obstacle detection
	TArray<FIntPoint> NewPath;
	bool bPathFound = UPathfindingLibrary::FindPath(StartTile, EndTile, NewPath, this);

	if (bPathFound && NewPath.Num() > 0)
	{
		PathToFollow = NewPath;
		CurrentWaypointIndex = 0;
		MovementProgress = 0.0f;
		bIsMoving = true;

		// Set initial positions
		CurrentWorldPosition = GetActorLocation();
		FVector FirstTilePos = UVNTileMapLibrary::GetWorldPosFromTileCoordinates(PathToFollow[CurrentWaypointIndex]);
		TargetWorldPosition = FVector(FirstTilePos.X, FirstTilePos.Y, CharacterZOffset + HeightLevel);
	}
	else
	{
		// No path found, optionally log a warning
		UE_LOG(LogTemp, Warning, TEXT("No path found from (%d, %d) to (%d, %d)"), StartTile.X, StartTile.Y, EndTile.X, EndTile.Y);
	}
}

void AVNMapCharacter::GetTilePosition(int& outX, int& outY) const
{
	FIntPoint pos = UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
	outX = pos.X;
	outY = pos.Y;
}

FIntPoint AVNMapCharacter::GetTilePosition() const
{
	return UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(GetActorLocation());
}
