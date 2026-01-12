// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GT5_PROJET_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartGameBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* ContinueBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* SettingsBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditsBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitBtn;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* VersionText;

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	FName FirstLevelName = FName("Chapter1Map");

	UPROPERTY(EditDefaultsOnly, Category = "Game Config")
	FString SaveSlotName = TEXT("MainSaveSlot");
	
	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnCreditsClicked();

	UFUNCTION()
	void OnQuitClicked();
    
	bool HasSaveGame();
};
