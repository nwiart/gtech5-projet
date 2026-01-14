// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerPerformanceSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnPerformanceChanged,
    int32,
    NewValue
);

UCLASS()
class GT5_PROJET_API UPlayerPerformanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    int32 GetPerformance() const { return Performance; }
    int32 GetMinValue() const { return MinValue; }
    int32 GetMaxValue() const { return MaxValue; }

    UFUNCTION(BlueprintCallable)
    void ModifyPerformance(int32 Delta);

    // called whenever the player performance changed
    UPROPERTY(BlueprintAssignable)
    FOnPerformanceChanged OnPerformanceChanged;

    UFUNCTION(BlueprintCallable)
    void GetPerformanceData(int32& OutMin, int32& OutMax, int32& OutValue) const;


protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
    int32 Performance = 0;

    static constexpr int32 MinValue = -100;
    static constexpr int32 MaxValue = 100;

    void ClampAndNotify();
};
