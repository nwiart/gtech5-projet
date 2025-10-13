// Fill out your copyright notice in the Description page of Project Settings.

#include "PointOfView.h"

#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
APointOfView::APointOfView()
	: Back(NULL)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FVector eye = GetEyeHeight();

	RootComponent = CreateDefaultSubobject<USceneComponent>("Scene Root");

	ForwardDirectionComponent = CreateDefaultSubobject<UArrowComponent>("Forward Direction");
	ForwardDirectionComponent->SetRelativeLocation(eye);
	ForwardDirectionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetRelativeLocation(eye);
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void APointOfView::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APointOfView::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector APointOfView::GetEyeLocation() const
{
	return CameraComponent->GetComponentLocation();
}

FVector APointOfView::GetEyeHeight() const
{
	return { 0.0F, 0.0F, EyeHeight };
}
