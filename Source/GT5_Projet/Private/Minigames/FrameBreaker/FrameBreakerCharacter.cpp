// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigames/FrameBreaker/FrameBreakerCharacter.h"
#include "Minigames/FrameBreaker/ProjectileBase.h"
#include "Minigames/FrameBreaker/FrameBreakerGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

AFrameBreakerCharacter::AFrameBreakerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create first-person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), FName("head")); // Attach to head socket if available
	FirstPersonCamera->bUsePawnControlRotation = false; // Lock rotation
	FirstPersonCamera->SetRelativeLocation(FVector(0, 0, 60)); // Eye height

	// Create arms mesh (separate from body)
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(FirstPersonCamera);
	ArmsMesh->SetOnlyOwnerSee(true); // Only visible to player
	ArmsMesh->bCastDynamicShadow = false;
	ArmsMesh->CastShadow = false;
	ArmsMesh->SetRelativeLocation(FVector(0, 0, -160)); // Position arms below camera

	// Create knife mesh in hand (will be attached to ProjectileSocket in BeginPlay)
	KnifeInHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KnifeInHandMesh"));
	KnifeInHandMesh->SetupAttachment(ArmsMesh); // Temporarily attach to ArmsMesh
	KnifeInHandMesh->SetOnlyOwnerSee(true);
	KnifeInHandMesh->bCastDynamicShadow = false;
	KnifeInHandMesh->CastShadow = false;
	KnifeInHandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Hide body mesh from owner
	GetMesh()->SetOwnerNoSee(true);
}

void AFrameBreakerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Attach knife to ProjectileSocket if it exists
	if (KnifeInHandMesh && ArmsMesh && ArmsMesh->DoesSocketExist(ProjectileSocketName))
	{
		KnifeInHandMesh->AttachToComponent(ArmsMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ProjectileSocketName);
		UE_LOG(LogTemp, Log, TEXT("FrameBreakerCharacter: Knife attached to socket '%s'"), *ProjectileSocketName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerCharacter: Could not attach knife to socket '%s'"), *ProjectileSocketName.ToString());
	}

	// Add input mapping context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

		// Start breathing camera shake
		if (BreathingShake)
		{
			PC->ClientStartCameraShake(BreathingShake);
		}
	}
}

void AFrameBreakerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Enhanced Input actions
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ThrowAction)
		{
			EnhancedInput->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AFrameBreakerCharacter::OnThrowPressed);
		}
	}
}

void AFrameBreakerCharacter::OnThrowPressed(const FInputActionValue& Value)
{
	if (CanThrow())
	{
		PlayThrowAnimation();
	}
}

void AFrameBreakerCharacter::PlayThrowAnimation()
{
	if (!ThrowMontage || !ArmsMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerCharacter: ThrowMontage or ArmsMesh is null!"));

		SpawnProjectile();
		return;
	}

	bIsThrowing = true;

	UAnimInstance* AnimInstance = ArmsMesh->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(ThrowMontage);

		// Bind to montage end event to show knife again
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &AFrameBreakerCharacter::OnThrowMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ThrowMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerCharacter: No AnimInstance on ArmsMesh!"));
		bIsThrowing = false;
		SpawnProjectile();
	}

	if (ThrowSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ThrowSound);
	}

	if (ThrowShake)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ClientStartCameraShake(ThrowShake);
		}
	}
}

void AFrameBreakerCharacter::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("FrameBreakerCharacter: ProjectileClass is not set!"));
		bIsThrowing = false;
		return;
	}

	// Notify game mode that a knife is being thrown
	AFrameBreakerGameMode* GameMode = Cast<AFrameBreakerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnKnifeThrown();
	}

	// Hide knife in hand when throwing
	HideKnifeInHand();

	// Get spawn location from hand socket
	FVector SpawnLocation;
	FRotator SpawnRotation;

	if (ArmsMesh && ArmsMesh->DoesSocketExist(ProjectileSocketName))
	{
		SpawnLocation = ArmsMesh->GetSocketLocation(ProjectileSocketName);
		SpawnRotation = ArmsMesh->GetSocketRotation(ProjectileSocketName);
	}
	else
	{
		// Fallback: spawn from camera
		SpawnLocation = FirstPersonCamera->GetComponentLocation();
		SpawnRotation = FirstPersonCamera->GetComponentRotation();
		SpawnLocation += SpawnRotation.Vector() * 50.0f; // Offset forward

		UE_LOG(LogTemp, Warning, TEXT("FrameBreakerCharacter: Socket '%s' not found, using camera location"), *ProjectileSocketName.ToString());
	}

	// Calculate target location (forward from camera)
	FVector TargetLocation = FirstPersonCamera->GetComponentLocation() + (FirstPersonCamera->GetForwardVector() * 1000.0f);
	TargetLocation += ProjectileTargetOffset;

	// Spawn projectile
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation
	);

	if (Projectile)
	{
		Projectile->LaunchTowardTarget(TargetLocation);
		UE_LOG(LogTemp, Log, TEXT("FrameBreakerCharacter: Spawned projectile at %s"), *SpawnLocation.ToString());
	}
}

bool AFrameBreakerCharacter::CanThrow() const
{
	if (bIsThrowing)
		return false;

	// Check if game mode has knives remaining
	AFrameBreakerGameMode* GameMode = Cast<AFrameBreakerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		return GameMode->HasKnivesRemaining();
	}

	return true; // Fallback if game mode not found
}

void AFrameBreakerCharacter::ShowKnifeInHand()
{
	if (KnifeInHandMesh)
	{
		KnifeInHandMesh->SetVisibility(true);
		UE_LOG(LogTemp, Log, TEXT("FrameBreakerCharacter: Knife shown"));
	}
}

void AFrameBreakerCharacter::HideKnifeInHand()
{
	if (KnifeInHandMesh)
	{
		KnifeInHandMesh->SetVisibility(false);
		UE_LOG(LogTemp, Log, TEXT("FrameBreakerCharacter: Knife hidden"));
	}
}

void AFrameBreakerCharacter::OnThrowMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("FrameBreakerCharacter: Throw montage ended"));

	// Show knife again when throw animation completes
	ShowKnifeInHand();

	// Allow throwing again
	bIsThrowing = false;
}