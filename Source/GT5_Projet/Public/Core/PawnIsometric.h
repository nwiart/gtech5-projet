// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnIsometric.generated.h"

class UCameraComponent;

class AVNMapCharacter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoomChanged, float, Value);


UCLASS()
class GT5_PROJET_API APawnIsometric : public APawn
{
	GENERATED_BODY()

	friend class AVNChapterManager;

public:
	// Sets default values for this pawn's properties
	APawnIsometric();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type reason) override;

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

	// Programmatically set the zoom level.
	// 0 is zoomed in, 1 is zoomed out.
	UFUNCTION(BlueprintCallable)
	void SetZoomLevel(float Value);

	UFUNCTION(BlueprintCallable)
	void RecenterViewOnPlayer();

	UFUNCTION(BlueprintCallable)
	void SetViewCenteredOnTile(const FIntPoint& TilePos);

	UFUNCTION(BlueprintCallable)
	void SetCursorActive(bool bActive);

	UFUNCTION(BlueprintCallable)
	void SetCursorHidden(bool bCursorHidden);


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


public:

	UPROPERTY(BlueprintAssignable)
	FOnZoomChanged OnZoomChanged;

private:

	AVNMapCharacter* PlayerCharacter;

	AActor* cursorActor;

	FVector cameraForwardVector;

	FIntPoint cursorPosition;

	bool bIsCursorActive;
};
