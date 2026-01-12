#include "UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartGameBtn)
	{
		StartGameBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
	}

	if (ContinueBtn)
	{
		ContinueBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueClicked);
		
		if (!HasSaveGame())
		{
			ContinueBtn->SetIsEnabled(false);
			ContinueBtn->SetRenderOpacity(0.5f); 
		}
		else
		{
			ContinueBtn->SetIsEnabled(true);
			ContinueBtn->SetRenderOpacity(1.0f);
		}
	}

	if (SettingsBtn)
	{
		SettingsBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsClicked);
	}

	if (CreditsBtn)
	{
		CreditsBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreditsClicked);
	}

	if (QuitBtn)
	{
		QuitBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}


}

void UMainMenuWidget::OnStartGameClicked()
{
	UGameplayStatics::OpenLevel(this, FirstLevelName);
}

void UMainMenuWidget::OnContinueClicked()
{
	if (HasSaveGame())
	{
		// Load the saved game. 
		// UGameplayStatics::OpenLevel(this, LoadedLevelName);
	}
}

void UMainMenuWidget::OnSettingsClicked()
{
}

void UMainMenuWidget::OnCreditsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Credits Clicked"));
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

bool UMainMenuWidget::HasSaveGame()
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}