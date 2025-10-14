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
	Normal    UMETA(DisplayName="Normal"),
	Joyful    UMETA(DisplayName="Joyful"),
	Intrigued UMETA(DisplayName="Intrigued"),
	Scared    UMETA(DisplayName="Scared"),
	Tired     UMETA(DisplayName="Tired"),
};
