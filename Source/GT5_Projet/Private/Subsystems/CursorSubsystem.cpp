#include "Subsystems/CursorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"

void UCursorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Don't call ApplyCursorState() here: GameInstance subsystems initialize
	// before any LocalPlayer/PC exists, so the call would be a no-op. The first
	// SetMode/SetFocusUI from gameplay code applies the state once a PC is up.
}

void UCursorSubsystem::SetMode(ECursorMode NewMode)
{
	const bool bModeChanged = Mode != NewMode;

	Mode = NewMode;
	if (bModeChanged)
	{
		bIsFocusUIHeld = false;
	}

	ApplyCursorState();

	if (bModeChanged)
	{
		OnCursorModeChanged.Broadcast(NewMode);
	}
}

void UCursorSubsystem::SetFocusUI(bool bFocusUI)
{
	if (Mode != ECursorMode::Locked)
	{
		bIsFocusUIHeld = false;
		return;
	}

	bIsFocusUIHeld = bFocusUI;
	ApplyCursorState();
}

void UCursorSubsystem::ApplyCursorState()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GetWorld() || !GameInstance) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GameInstance, 0);
	if (!PC) return;

	switch (Mode)
	{
	case ECursorMode::Locked:
		if (bIsFocusUIHeld)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetShowMouseCursor(true);
			PC->SetInputMode(InputMode);
		}
		else
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
		break;

	case ECursorMode::Free:
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetShowMouseCursor(true);
			PC->SetInputMode(InputMode);
		}
		break;
	}
}
