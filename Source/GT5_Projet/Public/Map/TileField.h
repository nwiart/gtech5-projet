#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileField.generated.h"

class UBoxComponent;


UCLASS()
class GT5_PROJET_API ATileField : public AActor
{
	GENERATED_BODY()

public:	

	ATileField();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPointInside(const FVector& WorldLocation) const;

	FIntPoint GetStartTile() const;


public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UBoxComponent* Box;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 SizeX;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 SizeY;
};
