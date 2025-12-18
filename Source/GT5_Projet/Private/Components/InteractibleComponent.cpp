// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractibleComponent.h"


// Sets default values for this component's properties
UInteractibleComponent::UInteractibleComponent()
	: TooltipText("Interact"), bEnabled(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UInteractibleComponent::BeginPlay()
{
	Super::BeginPlay();

	// Update collidability.
	SetEnabled(bEnabled);
}


// Called every frame
void UInteractibleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UInteractibleComponent::SetEnabled(bool enabled)
{
	bEnabled = enabled;

	// Make sure at least one collidable component can be hit by the trace.
	ECollisionResponse response = enabled ? ECR_Block : ECR_Ignore;
	UPrimitiveComponent* sm = GetOwner()->GetComponentByClass<UPrimitiveComponent>();
	sm->SetCollisionResponseToChannel(ECC_Camera, response);
	sm->SetCollisionResponseToChannel(ECC_Visibility, response);
}
