// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/DialogueCharacter.h"


UDialogueCharacter::UDialogueCharacter()
{
}

FText UDialogueCharacter::GetCharacterName() const
{
	return FText::FromString(Name);
}

UTexture2D* UDialogueCharacter::GetTextureForPose(EDialoguePose pose) const
{
	UTexture2D* const* valuePtr = Sprites.Find(pose);
	return valuePtr ? *valuePtr : 0;
}
