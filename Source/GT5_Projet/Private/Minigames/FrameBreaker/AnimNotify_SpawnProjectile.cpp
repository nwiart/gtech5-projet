// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/AnimNotify_SpawnProjectile.h"
#include "Minigames/FrameBreaker/FrameBreakerCharacter.h"

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify_SpawnProjectile: MeshComp is null!"));
		return;
	}

	// Get the owning actor
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify_SpawnProjectile: Owner is null!"));
		return;
	}

	// Try to cast to FrameBreakerCharacter
	AFrameBreakerCharacter* Character = Cast<AFrameBreakerCharacter>(Owner);
	if (Character)
	{
		// Call SpawnProjectile on the character
		Character->SpawnProjectile();
		UE_LOG(LogTemp, Log, TEXT("AnimNotify_SpawnProjectile: Called SpawnProjectile on FrameBreakerCharacter"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify_SpawnProjectile: Owner is not a FrameBreakerCharacter!"));
	}
}

FString UAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
	return TEXT("Spawn Projectile");
}
