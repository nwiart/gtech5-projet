// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "MapCharacter.generated.h"

UCLASS()
class GT5_PROJET_API AMapCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void MoveTo(int X, int Y);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetTilePosition(int& outX, int& outY) const;

	FIntPoint GetTilePosition() const;


private:

	FIntPoint targetPosition;

	bool moving;
};
