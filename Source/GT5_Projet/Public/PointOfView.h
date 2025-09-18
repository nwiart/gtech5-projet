// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointOfView.generated.h"

class UArrowComponent;
class UCameraComponent;

UCLASS()
class GT5_PROJET_API APointOfView : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointOfView();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetEyeLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetEyeHeight() const;


protected:

	UArrowComponent* ForwardDirectionComponent;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APointOfView* Back;

	static constexpr float EyeHeight = 150.0F;
};
