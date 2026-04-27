#pragma once

#include "CoreMinimal.h"
#include "DialoguePose.generated.h"

/*
 * Dialogue "pose", used to select the appropriate sprite for the character speaking, depending
 * on which emotion we want them to show.
 */
UENUM(BlueprintType)
enum class EDialoguePose : uint8
{
	Normal			UMETA(DisplayName="Normal"),
	Blank			UMETA(DisplayName="Blank"),
	Angry			UMETA(DisplayName="Angry"),
	Worried			UMETA(DisplayName="Worried"),
	Sad				UMETA(DisplayName="Sad"),
	ClosedEyes1		UMETA(DisplayName="Closed Eyes 1"),
	ClosedEyes2		UMETA(DisplayName="Closed Eyes 2"),
	Serious			UMETA(DisplayName="Serious"),
	Smile1			UMETA(DisplayName="Smile 1"),
	Smile2			UMETA(DisplayName="Smile 2"),
	Smile3			UMETA(DisplayName="Smile 3"),
	Intrigued1		UMETA(DisplayName="Intrigued 1"),
	Intrigued2		UMETA(DisplayName="Intrigued 2"),
	Scared			UMETA(DisplayName="Scared"),
	Surprised		UMETA(DisplayName="Surprised"),
	BlackEyes1		UMETA(DisplayName="Black Eyes 1"),
	BlackEyes2		UMETA(DisplayName="Black Eyes 2"),
};
