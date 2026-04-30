// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VNGamemode.generated.h"

class AVNMapEvent;

/**
 * 
 */
UCLASS()
class GT5_PROJET_API AVNGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AVNGamemode();

	void BeginPlay();
};
