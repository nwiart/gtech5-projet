// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnIsometric.generated.h"

class UCameraComponent;


UCLASS()
class GT5_PROJET_API APawnIsometric : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnIsometric();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Input_PanCameraX(float w);
	void Input_PanCameraY(float w);
	void Input_ZoomCamera(float w);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	float CameraSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	float CameraMinWidth;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	float CameraMaxWidth;


private:

	FVector cameraForwardVector;
};
