#if WITH_DEV_AUTOMATION_TESTS

#include "Libraries/VNTileMapLibrary.h"

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTileMapLibraryWorldToTileTest,
	"GT5.TileMap.VNTileMapLibrary.WorldToTile.UsesFloorBoundaries",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTileMapLibraryWorldToTileTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Origin maps to tile 0,0"), UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(FVector(0.0, 0.0, 0.0)), FIntPoint(0, 0));
	TestEqual(TEXT("Positive values below tile size stay in tile 0"), UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(FVector(99.9, 99.9, 0.0)), FIntPoint(0, 0));
	TestEqual(TEXT("Positive tile boundary advances to next tile"), UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(FVector(100.0, 100.0, 0.0)), FIntPoint(1, 1));
	TestEqual(TEXT("Negative values floor toward the previous tile"), UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(FVector(-0.1, -0.1, 0.0)), FIntPoint(-1, -1));
	TestEqual(TEXT("Negative tile boundary stays exact"), UVNTileMapLibrary::GetTileCoordinatesFromWorldPos(FVector(-100.0, -100.0, 0.0)), FIntPoint(-1, -1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTileMapLibraryNearestTileTest,
	"GT5.TileMap.VNTileMapLibrary.NearestTile.UsesHalfTileOffset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTileMapLibraryNearestTileTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Below half-tile snaps to current tile"), UVNTileMapLibrary::GetNearestTileFromWorldPos(FVector(49.9, 49.9, 0.0)), FIntPoint(0, 0));
	TestEqual(TEXT("Half-tile boundary snaps forward"), UVNTileMapLibrary::GetNearestTileFromWorldPos(FVector(50.0, 50.0, 0.0)), FIntPoint(1, 1));
	TestEqual(TEXT("Negative half-tile boundary is deterministic"), UVNTileMapLibrary::GetNearestTileFromWorldPos(FVector(-50.0, -50.0, 0.0)), FIntPoint(0, 0));
	TestEqual(TEXT("Past negative half-tile snaps backward"), UVNTileMapLibrary::GetNearestTileFromWorldPos(FVector(-50.1, -50.1, 0.0)), FIntPoint(-1, -1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTileMapLibraryTileToWorldTest,
	"GT5.TileMap.VNTileMapLibrary.TileToWorld.ReturnsTileCenter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTileMapLibraryTileToWorldTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Tile 0,0 maps to center"), UVNTileMapLibrary::GetWorldPosFromTileCoordinates(FIntPoint(0, 0)), FVector(50.0, 50.0, 0.0));
	TestEqual(TEXT("Positive tile maps to center"), UVNTileMapLibrary::GetWorldPosFromTileCoordinates(FIntPoint(2, 3)), FVector(250.0, 350.0, 0.0));
	TestEqual(TEXT("Negative tile maps to center"), UVNTileMapLibrary::GetWorldPosFromTileCoordinates(FIntPoint(-1, -2)), FVector(-50.0, -150.0, 0.0));

	return true;
}

#endif
