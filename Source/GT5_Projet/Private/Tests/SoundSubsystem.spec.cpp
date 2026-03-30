#if WITH_DEV_AUTOMATION_TESTS

#include "Subsystems/SoundSubsystem.h"

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Misc/AutomationTest.h"

namespace
{
	UDataTable* CreateSFXDataTable(const TArray<FName>& RowNames)
	{
		UDataTable* DataTable = NewObject<UDataTable>();
		DataTable->RowStruct = FSFXTableRow::StaticStruct();

		for (const FName RowName : RowNames)
		{
			FSFXTableRow Row;
			DataTable->AddRow(RowName, Row);
		}

		return DataTable;
	}

	USoundSubsystem* CreateSoundSubsystem()
	{
		UGameInstance* GameInstance = NewObject<UGameInstance>();
		return NewObject<USoundSubsystem>(GameInstance);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSoundSubsystemResolveHandleTest,
	"GT5.Sound.SoundSubsystem.ResolveSFXHandle.WithExplicitTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSoundSubsystemResolveHandleTest::RunTest(const FString& Parameters)
{
	const FName RowName(TEXT("Ui_Click"));
	UDataTable* DataTable = CreateSFXDataTable({ RowName });
	TestNotNull(TEXT("Transient data table should be created"), DataTable);

	USoundSubsystem* SoundSubsystem = CreateSoundSubsystem();
	TestNotNull(TEXT("Sound subsystem should be created"), SoundSubsystem);

	FDataTableRowHandle Handle;
	Handle.DataTable = DataTable;
	Handle.RowName = RowName;

	FSFXData ResolvedData;
	FName ResolvedIdentifier = NAME_None;
	const bool bResolved = SoundSubsystem->ResolveSFXHandle(Handle, ResolvedIdentifier, ResolvedData);

	TestTrue(TEXT("A valid row handle should resolve to SFX data"), bResolved);
	TestTrue(TEXT("Resolved identifier should match the row name"), ResolvedIdentifier == RowName);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSoundSubsystemResolveHandleFallbackTest,
	"GT5.Sound.SoundSubsystem.ResolveSFXHandle.FallsBackToSubsystemTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSoundSubsystemResolveHandleFallbackTest::RunTest(const FString& Parameters)
{
	const FName RowName(TEXT("Footstep_Stone"));
	UDataTable* DataTable = CreateSFXDataTable({ RowName });
	TestNotNull(TEXT("Transient data table should be created"), DataTable);

	USoundSubsystem* SoundSubsystem = CreateSoundSubsystem();
	TestNotNull(TEXT("Sound subsystem should be created"), SoundSubsystem);
	SoundSubsystem->SetSFXDataTable(DataTable);

	FDataTableRowHandle Handle;
	Handle.RowName = RowName;

	FSFXData ResolvedData;
	FName ResolvedIdentifier = NAME_None;
	const bool bResolved = SoundSubsystem->ResolveSFXHandle(Handle, ResolvedIdentifier, ResolvedData);

	TestTrue(TEXT("A handle without DataTable should use the subsystem DataTable"), bResolved);
	TestTrue(TEXT("Fallback lookup should keep the row name as identifier"), ResolvedIdentifier == RowName);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSoundSubsystemResolveHandleEmptyRowTest,
	"GT5.Sound.SoundSubsystem.ResolveSFXHandle.RejectsEmptyRowName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSoundSubsystemResolveHandleEmptyRowTest::RunTest(const FString& Parameters)
{
	USoundSubsystem* SoundSubsystem = CreateSoundSubsystem();
	TestNotNull(TEXT("Sound subsystem should be created"), SoundSubsystem);

	FSFXData ResolvedData;
	FName ResolvedIdentifier = NAME_None;
	const bool bResolved = SoundSubsystem->ResolveSFXHandle(FDataTableRowHandle(), ResolvedIdentifier, ResolvedData);

	TestFalse(TEXT("An empty row handle should be rejected"), bResolved);
	TestTrue(TEXT("No identifier should be produced when the handle is invalid"), ResolvedIdentifier.IsNone());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSoundSubsystemResolveHandleMissingRowTest,
	"GT5.Sound.SoundSubsystem.ResolveSFXHandle.RejectsMissingRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSoundSubsystemResolveHandleMissingRowTest::RunTest(const FString& Parameters)
{
	UDataTable* DataTable = CreateSFXDataTable({ FName(TEXT("Existing_Row")) });
	TestNotNull(TEXT("Transient data table should be created"), DataTable);

	USoundSubsystem* SoundSubsystem = CreateSoundSubsystem();
	TestNotNull(TEXT("Sound subsystem should be created"), SoundSubsystem);

	FDataTableRowHandle Handle;
	Handle.DataTable = DataTable;
	Handle.RowName = FName(TEXT("Missing_Row"));

	FSFXData ResolvedData;
	FName ResolvedIdentifier = NAME_None;
	const bool bResolved = SoundSubsystem->ResolveSFXHandle(Handle, ResolvedIdentifier, ResolvedData);

	TestFalse(TEXT("A missing row should fail resolution"), bResolved);
	TestTrue(TEXT("Missing rows should not leak an identifier"), ResolvedIdentifier.IsNone());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSoundSubsystemGetDataFromHandleTest,
	"GT5.Sound.SoundSubsystem.GetSFXDataFromHandle.ReturnsPayloadOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSoundSubsystemGetDataFromHandleTest::RunTest(const FString& Parameters)
{
	const FName RowName(TEXT("Menu_Confirm"));
	UDataTable* DataTable = CreateSFXDataTable({ RowName });
	TestNotNull(TEXT("Transient data table should be created"), DataTable);

	USoundSubsystem* SoundSubsystem = CreateSoundSubsystem();
	TestNotNull(TEXT("Sound subsystem should be created"), SoundSubsystem);

	FDataTableRowHandle Handle;
	Handle.DataTable = DataTable;
	Handle.RowName = RowName;

	FSFXData ResolvedData;
	const bool bResolved = SoundSubsystem->GetSFXDataFromHandle(Handle, ResolvedData);

	TestTrue(TEXT("Public handle lookup should still resolve"), bResolved);

	return true;
}

#endif
