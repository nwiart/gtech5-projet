// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/UtilsLibrary.h"

#include "Kismet/GameplayStatics.h"


void UUtilsLibrary::GetActorsOfClassInStreamedLevel(TArray<AActor*>& OutActors, ULevelStreaming* LevelStreaming, const UClass* Class, const UObject* WorldContextObject)
{
	OutActors = LevelStreaming->GetLoadedLevel()->Actors.FilterByPredicate([Class](const AActor* actor) {
		return actor && actor->GetClass()->IsChildOf(Class);
	});
}
