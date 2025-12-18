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

	void Input_SelectTile();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector ViewportToWorld(double viewportX, double viewportY) const;

	// Gets the tile coordinate based on a cursor position in viewport space (-1; 1).
	FIntPoint GetPointedTile(double viewportX, double viewportY) const;

	UFUNCTION(BlueprintCallable)
	void RecenterViewOnPlayer();

	UFUNCTION(BlueprintCallable)
	void SetCursorActive(bool bActive);


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	float CameraSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	float CameraMinWidth;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Camera")
	float CameraMaxWidth;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CursorClass;


private:

	AActor* cursorActor;

	FVector cameraForwardVector;

	FIntPoint cursorPosition;
};
