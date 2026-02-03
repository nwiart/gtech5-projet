// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "VNMapCharacter.generated.h"

UCLASS()
class GT5_PROJET_API AVNMapCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVNMapCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Enable();

	UFUNCTION(BlueprintCallable)
	void Disable();

	UFUNCTION(BlueprintCallable)
	void MoveTo(int X, int Y);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTilePosition(int& outX, int& outY) const;

	FIntPoint GetTilePosition() const;


private:

	// Path calculated by A* algorithm
	TArray<FIntPoint> PathToFollow;

	// Current waypoint index in the path
	int32 CurrentWaypointIndex;

	// Current position in world space for interpolation
	FVector CurrentWorldPosition;

	// Target world position for current waypoint
	FVector TargetWorldPosition;

	// Movement progress (0.0 to 1.0) between current and target waypoint
	float MovementProgress;

	bool bIsMoving;

	float CharacterZOffset;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed;
};
