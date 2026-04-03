#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CursorSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECursorMode : uint8
{
	Locked,
	Free
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCursorModeChanged,
	ECursorMode,
	NewMode);

UCLASS()
class GT5_PROJET_API UCursorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Cursor")
	void SetMode(ECursorMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Cursor")
	void SetFocusUI(bool bFocusUI);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cursor")
	ECursorMode GetMode() const { return Mode; }

	UPROPERTY(BlueprintAssignable, Category = "Cursor|Events")
	FOnCursorModeChanged OnCursorModeChanged;

private:
	void ApplyCursorState();

	ECursorMode Mode = ECursorMode::Free;
	bool bIsFocusUIHeld = false;
};
