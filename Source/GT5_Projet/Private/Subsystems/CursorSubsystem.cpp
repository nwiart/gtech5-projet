#include "Subsystems/CursorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UCursorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ApplyCursorState();
}

void UCursorSubsystem::SetMode(ECursorMode NewMode)
{
	if (Mode == NewMode) return;

	Mode = NewMode;
	bIsFocusUIHeld = false;
	ApplyCursorState();
	OnCursorModeChanged.Broadcast(NewMode);
}

void UCursorSubsystem::SetFocusUI(bool bFocusUI)
{
	if (Mode != ECursorMode::Locked) return;

	bIsFocusUIHeld = bFocusUI;
	ApplyCursorState();
}

void UCursorSubsystem::ApplyCursorState()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetGameInstance(), 0);
	if (!PC) return;

	switch (Mode)
	{
	case ECursorMode::Locked:
		if (bIsFocusUIHeld)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		}
		else
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
		break;

	case ECursorMode::Free:
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		break;
	}
}
