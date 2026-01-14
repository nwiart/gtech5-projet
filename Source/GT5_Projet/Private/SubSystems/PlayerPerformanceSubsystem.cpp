// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystems/PlayerPerformanceSubsystem.h"

void UPlayerPerformanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Performance = 0;
}

void UPlayerPerformanceSubsystem::ModifyPerformance(int32 Delta)
{
    Performance += Delta;
    ClampAndNotify();
}

void UPlayerPerformanceSubsystem::ClampAndNotify()
{
    Performance = FMath::Clamp(Performance, MinValue, MaxValue);
    OnPerformanceChanged.Broadcast(Performance);
}

void UPlayerPerformanceSubsystem::GetPerformanceData(
    int32& OutMin,
    int32& OutMax,
    int32& OutValue
) const
{
    OutMin = MinValue;
    OutMax = MaxValue;
    OutValue = Performance;
}
