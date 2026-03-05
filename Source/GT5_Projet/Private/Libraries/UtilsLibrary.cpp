// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/UtilsLibrary.h"

#include "Kismet/GameplayStatics.h"


bool UUtilsLibrary::GetActorsOfClassInStreamedLevel(TArray<AActor*>& OutActors, ULevelStreaming* LevelStreaming, const UClass* Class, const UObject* WorldContextObject)
{
	if (!LevelStreaming || !Class) {
		UE_LOG(LogTemp, Warning, TEXT("UUtilsLibrary::GetActorsOfClassInStreamedLevel() : LevelStreaming and Class must be non-null!"));
		return false;
	}

	if (!LevelStreaming->GetLoadedLevel()) {
		UE_LOG(LogTemp, Warning, TEXT("UUtilsLibrary::GetActorsOfClassInStreamedLevel() : LevelStreaming reports level not being loaded."));
		return false;
	}

	OutActors = LevelStreaming->GetLoadedLevel()->Actors.FilterByPredicate([Class](const AActor* actor) {
		return actor && actor->GetClass()->IsChildOf(Class);
	});
	return true;
}
