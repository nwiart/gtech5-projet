// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PerformanceBarWidget.h"
#include "Subsystems/VNChapterSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UPerformanceBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ProgressBar_Performance)
    {
        UE_LOG(LogTemp, Error, TEXT("ProgressBar_Performance unbinded"));
        return;
    }

    if (UVNChapterSubsystem* PerfSubsystem =
        GetGameInstance()->GetSubsystem<UVNChapterSubsystem>())
    {
        PerfSubsystem->OnConnectionChanged.AddDynamic(
            this,
            &UPerformanceBarWidget::OnPerformanceUpdated
        );

        int32 InitialValue;
        PerfSubsystem->GetConnectionData(MinValue, MaxValue, InitialValue);

        UpdateBar(InitialValue);
    }
}


void UPerformanceBarWidget::OnPerformanceInit(
    int32 InMin,
    int32 InMax,
    int32 InitialValue
)
{
    MinValue = InMin;
    MaxValue = InMax;

    UpdateBar(InitialValue);
}

void UPerformanceBarWidget::OnPerformanceUpdated(int32 NewValue)
{
    UpdateBar(NewValue);
}

void UPerformanceBarWidget::UpdateBar(int32 Value)
{
    const float Normalized = FMath::GetMappedRangeValueClamped(
        FVector2D(MinValue, MaxValue),
        FVector2D(0.f, 1.f),
        Value
    );

    ProgressBar_Performance->SetPercent(Normalized);

    const FLinearColor BarColor = FLinearColor::LerpUsingHSV(
        FLinearColor::Red,
        FLinearColor::Green,
        Normalized
    );

    ProgressBar_Performance->SetFillColorAndOpacity(BarColor);
}
