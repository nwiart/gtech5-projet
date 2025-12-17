// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnProjectile.generated.h"

/**
 * Animation Notify for spawning projectile during throw animation
 * Calls SpawnProjectile() on FrameBreakerCharacter
 */
UCLASS()
class GT5_PROJET_API UAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// Override to display custom name in editor
	virtual FString GetNotifyName_Implementation() const override;
};
