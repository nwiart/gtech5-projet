#if WITH_DEV_AUTOMATION_TESTS

#include "Subsystems/VNSaveSubsystem.h"

#include "Misc/AutomationTest.h"
#include "Save/VNSaveGame.h"

namespace
{
	UVNSaveSubsystem* CreateSaveSubsystem()
	{
		return NewObject<UVNSaveSubsystem>();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSaveSubsystemNullSaveGameTest,
	"GT5.Save.VNSaveSubsystem.NullSaveGame.ReturnsDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSaveSubsystemNullSaveGameTest::RunTest(const FString& Parameters)
{
	UVNSaveSubsystem* SaveSubsystem = CreateSaveSubsystem();
	TestNotNull(TEXT("Save subsystem should be created"), SaveSubsystem);

	const FGuid Guid = FGuid::NewGuid();
	FTableEvents Data;
	Data.bTriggered = true;

	TestNull(TEXT("Save game class should be null without a save game"), SaveSubsystem->GetSaveGameClass());

	FTableEvents DefaultData = SaveSubsystem->GetEventData(Guid);
	TestFalse(TEXT("Missing save game should return default event data"), DefaultData.bTriggered);

	SaveSubsystem->SetEventData(Guid, Data);
	DefaultData = SaveSubsystem->GetEventData(Guid);
	TestFalse(TEXT("SetEventData should no-op without a save game"), DefaultData.bTriggered);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSaveSubsystemEventRoundTripTest,
	"GT5.Save.VNSaveSubsystem.Events.RoundTripInMemory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSaveSubsystemEventRoundTripTest::RunTest(const FString& Parameters)
{
	UVNSaveSubsystem* SaveSubsystem = CreateSaveSubsystem();
	TestNotNull(TEXT("Save subsystem should be created"), SaveSubsystem);

	SaveSubsystem->SaveGame = NewObject<UVNSaveGame>(SaveSubsystem);
	TestNotNull(TEXT("Transient save game should be created"), SaveSubsystem->SaveGame.Get());
	TestTrue(TEXT("Save game class should reflect assigned save game"), SaveSubsystem->GetSaveGameClass() == UVNSaveGame::StaticClass());

	const FGuid Guid = FGuid::NewGuid();
	FTableEvents Data;
	Data.bTriggered = true;
	SaveSubsystem->SetEventData(Guid, Data);

	const FTableEvents StoredData = SaveSubsystem->GetEventData(Guid);
	TestTrue(TEXT("Stored event data should round-trip"), StoredData.bTriggered);

	const FTableEvents MissingData = SaveSubsystem->GetEventData(FGuid::NewGuid());
	TestFalse(TEXT("Unknown events should return default data"), MissingData.bTriggered);

	Data.bTriggered = false;
	SaveSubsystem->SetEventData(Guid, Data);
	const FTableEvents ReplacedData = SaveSubsystem->GetEventData(Guid);
	TestFalse(TEXT("SetEventData should replace existing event data"), ReplacedData.bTriggered);

	return true;
}

#endif
