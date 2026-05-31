#if WITH_DEV_AUTOMATION_TESTS

#include "Subsystems/VNChapterSubsystem.h"

#include "Misc/AutomationTest.h"

namespace
{
	UVNChapterSubsystem* CreateChapterSubsystem()
	{
		return NewObject<UVNChapterSubsystem>();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FChapterSubsystemConnectionClampTest,
	"GT5.Chapter.VNChapterSubsystem.Connection.ClampsAndReportsData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FChapterSubsystemConnectionClampTest::RunTest(const FString& Parameters)
{
	UVNChapterSubsystem* ChapterSubsystem = CreateChapterSubsystem();
	TestNotNull(TEXT("Chapter subsystem should be created"), ChapterSubsystem);

	ChapterSubsystem->ModifyConnection(150);
	TestEqual(TEXT("Connection should clamp to max"), ChapterSubsystem->GetConnection(), UVNChapterSubsystem::ConnectionMaxValue);

	ChapterSubsystem->ModifyConnection(-300);
	TestEqual(TEXT("Connection should clamp to min"), ChapterSubsystem->GetConnection(), UVNChapterSubsystem::ConnectionMinValue);

	int32 OutMin = 0;
	int32 OutMax = 0;
	int32 OutValue = 0;
	ChapterSubsystem->GetConnectionData(OutMin, OutMax, OutValue);
	TestEqual(TEXT("Connection data should expose min"), OutMin, UVNChapterSubsystem::ConnectionMinValue);
	TestEqual(TEXT("Connection data should expose max"), OutMax, UVNChapterSubsystem::ConnectionMaxValue);
	TestEqual(TEXT("Connection data should expose current value"), OutValue, UVNChapterSubsystem::ConnectionMinValue);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FChapterSubsystemStateResetTest,
	"GT5.Chapter.VNChapterSubsystem.State.NotifyChapterCompleteResetsCurrentChapter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FChapterSubsystemStateResetTest::RunTest(const FString& Parameters)
{
	UVNChapterSubsystem* ChapterSubsystem = CreateChapterSubsystem();
	TestNotNull(TEXT("Chapter subsystem should be created"), ChapterSubsystem);

	ChapterSubsystem->CurrentChapterName = FName(TEXT("Chapter_01"));
	ChapterSubsystem->CurrentChapterLevel = TSoftObjectPtr<UWorld>(FSoftObjectPath(TEXT("/Game/Test/Chapter_01.Chapter_01")));

	ChapterSubsystem->NotifyChapterComplete();

	TestTrue(TEXT("Current chapter name should reset"), ChapterSubsystem->CurrentChapterName.IsNone());
	TestTrue(TEXT("Current chapter level should reset"), ChapterSubsystem->CurrentChapterLevel.IsNull());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FChapterSubsystemTriggerMinigameTest,
	"GT5.Chapter.VNChapterSubsystem.Minigame.StoresScheduledDataWithoutManager",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FChapterSubsystemTriggerMinigameTest::RunTest(const FString& Parameters)
{
	UVNChapterSubsystem* ChapterSubsystem = CreateChapterSubsystem();
	TestNotNull(TEXT("Chapter subsystem should be created"), ChapterSubsystem);

	FMinigameData MinigameData;
	MinigameData.Name = FName(TEXT("KnifeHit"));
	MinigameData.Description = TEXT("Throw matches at the target");

	const FGuid EventGuid = FGuid::NewGuid();
	ChapterSubsystem->TriggerMinigame(MinigameData, EventGuid);

	TestEqual(TEXT("Scheduled minigame name should be stored"), ChapterSubsystem->ScheduledMinigame.Name, MinigameData.Name);
	TestEqual(TEXT("Scheduled minigame description should be stored"), ChapterSubsystem->ScheduledMinigame.Description, MinigameData.Description);
	TestEqual(TEXT("Last minigame guid should be stored"), ChapterSubsystem->LastMinigameGuid, EventGuid);

	return true;
}

#endif
