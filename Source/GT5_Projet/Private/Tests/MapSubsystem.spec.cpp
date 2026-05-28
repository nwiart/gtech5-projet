#if WITH_DEV_AUTOMATION_TESTS

#include "Subsystems/MapSubsystem.h"

#include "Engine/World.h"
#include "Map/TileField.h"
#include "Map/VNMapEvent.h"
#include "Misc/AutomationTest.h"

namespace
{
	ATileField* CreateTileField(UWorld* World, const FVector& Location, int32 SizeX, int32 SizeY)
	{
		ATileField* TileField = World->SpawnActor<ATileField>(ATileField::StaticClass(), Location, FRotator::ZeroRotator);
		TileField->SizeX = SizeX;
		TileField->SizeY = SizeY;
		TileField->OnConstruction(TileField->GetActorTransform());
		return TileField;
	}

	AVNMapEvent* CreateMapEvent(UWorld* World, const FVector& Location)
	{
		return World->SpawnActor<AVNMapEvent>(AVNMapEvent::StaticClass(), Location, FRotator::ZeroRotator);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMapSubsystemTileFieldRegistrationTest,
	"GT5.Map.MapSubsystem.TileFields.RegisterAndQueryTiles",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMapSubsystemTileFieldRegistrationTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Editor, false);
	TestNotNull(TEXT("Transient editor world should be created"), World);

	UMapSubsystem* MapSubsystem = World->GetSubsystem<UMapSubsystem>();
	TestNotNull(TEXT("Map subsystem should exist"), MapSubsystem);
	MapSubsystem->Reset();

	ATileField* TileField = CreateTileField(World, FVector::ZeroVector, 2, 3);
	TestNotNull(TEXT("Tile field should spawn"), TileField);

	MapSubsystem->AddTileField(nullptr);
	MapSubsystem->AddTileField(TileField);
	MapSubsystem->AddTileField(TileField);

	TArray<FIntPoint> Tiles;
	MapSubsystem->GetAllTiles(Tiles);
	TestEqual(TEXT("Duplicate tile fields should be ignored"), Tiles.Num(), 6);
	TestTrue(TEXT("Start tile should exist"), MapSubsystem->IsTileAt(FIntPoint(0, 0)));
	TestTrue(TEXT("Last covered tile should exist"), MapSubsystem->IsTileAt(FIntPoint(1, 2)));
	TestFalse(TEXT("X upper bound should be exclusive"), MapSubsystem->IsTileAt(FIntPoint(2, 0)));
	TestFalse(TEXT("Y upper bound should be exclusive"), MapSubsystem->IsTileAt(FIntPoint(0, 3)));

	MapSubsystem->RemoveTileField(TileField);
	Tiles.Reset();
	MapSubsystem->GetAllTiles(Tiles);
	TestEqual(TEXT("Removed field should no longer contribute tiles"), Tiles.Num(), 0);

	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMapSubsystemEventRegistrationTest,
	"GT5.Map.MapSubsystem.MapEvents.IndexByTile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMapSubsystemEventRegistrationTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Editor, false);
	TestNotNull(TEXT("Transient editor world should be created"), World);

	UMapSubsystem* MapSubsystem = World->GetSubsystem<UMapSubsystem>();
	TestNotNull(TEXT("Map subsystem should exist"), MapSubsystem);
	MapSubsystem->Reset();

	AVNMapEvent* FirstEvent = CreateMapEvent(World, FVector(50.0, 50.0, 0.0));
	AVNMapEvent* SecondEvent = CreateMapEvent(World, FVector(50.0, 50.0, 0.0));
	AVNMapEvent* InactiveEvent = CreateMapEvent(World, FVector(150.0, 50.0, 0.0));
	TestNotNull(TEXT("First map event should spawn"), FirstEvent);
	TestNotNull(TEXT("Second map event should spawn"), SecondEvent);
	TestNotNull(TEXT("Inactive map event should spawn"), InactiveEvent);

	MapSubsystem->AddMapEvent(nullptr);
	MapSubsystem->AddMapEvent(FirstEvent);
	MapSubsystem->AddMapEvent(FirstEvent);
	TestEqual(TEXT("Duplicate events should be ignored"), MapSubsystem->GetAllMapEvents().Num(), 1);
	TestEqual(TEXT("First event should be indexed by tile"), MapSubsystem->GetMapEventAt(FIntPoint(0, 0)), FirstEvent);

	MapSubsystem->AddMapEvent(SecondEvent);
	TestEqual(TEXT("Last added event wins by tile"), MapSubsystem->GetMapEventAt(FIntPoint(0, 0)), SecondEvent);

	MapSubsystem->RemoveMapEvent(FirstEvent);
	TestEqual(TEXT("Removing overwritten event should not clear current index"), MapSubsystem->GetMapEventAt(FIntPoint(0, 0)), SecondEvent);

	MapSubsystem->AddMapEvent(InactiveEvent);
	TestEqual(TEXT("Active event should be returned before deactivation"), MapSubsystem->GetMapEventAt(FIntPoint(1, 0)), InactiveEvent);
	InactiveEvent->SetInactive();
	TestNull(TEXT("Inactive events should be hidden from lookup"), MapSubsystem->GetMapEventAt(FIntPoint(1, 0)));

	MapSubsystem->RemoveMapEvent(SecondEvent);
	TestNull(TEXT("Removing indexed event should clear lookup"), MapSubsystem->GetMapEventAt(FIntPoint(0, 0)));

	World->DestroyWorld(false);
	return true;
}

#endif
