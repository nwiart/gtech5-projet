
#include "Dialogue/DialogueLine.h"


FDialogueLine::FDialogueLine()
	: Character(0), Pose(EDialoguePose::Normal)
{
}


UDialogueLineSequence::UDialogueLineSequence()
	: LeftCharacter(0), LeftCharacterPose(EDialoguePose::Normal)
	, RightCharacter(0), RightCharacterPose(EDialoguePose::Normal)
{
}

bool UDialogueLineSequence::HasChoice() const
{
	return !ChoiceOptions.IsEmpty();
}
