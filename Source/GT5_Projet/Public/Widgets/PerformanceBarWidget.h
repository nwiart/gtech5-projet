// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "PerformanceBarWidget.generated.h"

/**
 * 
 */

class UPlayerPerformanceSubsystem;

UCLASS()
class GT5_PROJET_API UPerformanceBarWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ProgressBar_Performance;

private:
    int32 MinValue = -100;
    int32 MaxValue = 100;

    UFUNCTION()
    void OnPerformanceInit(
        int32 InMin,
        int32 InMax,
        int32 InitialValue
    );

    UFUNCTION()
    void OnPerformanceUpdated(int32 NewValue);

    void UpdateBar(int32 Value);
};