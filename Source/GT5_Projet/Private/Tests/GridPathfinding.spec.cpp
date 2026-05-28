#if WITH_DEV_AUTOMATION_TESTS

#include "Libraries/GridPathfinding.h"

#include "Misc/AutomationTest.h"

namespace
{
	FGridPathfinding::FIsWalkable WalkableExcept(const TSet<FIntPoint>& BlockedTiles)
	{
		FGridPathfinding::FIsWalkable IsWalkable;
		IsWalkable.BindLambda([BlockedTiles](const FIntPoint& TilePosition)
		{
			return !BlockedTiles.Contains(TilePosition);
		});
		return IsWalkable;
	}

	FGridPathfinding::FIsTerminal TerminalAt(const FIntPoint& TerminalTile)
	{
		FGridPathfinding::FIsTerminal IsTerminal;
		IsTerminal.BindLambda([TerminalTile](const FIntPoint& TilePosition)
		{
			return TilePosition == TerminalTile;
		});
		return IsTerminal;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGridPathfindingStartEqualsEndTest,
	"GT5.Pathfinding.GridPathfinding.StartEqualsEnd.ReturnsSingleTile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGridPathfindingStartEqualsEndTest::RunTest(const FString& Parameters)
{
	TArray<FIntPoint> Path;
	const bool bFoundPath = FGridPathfinding::FindPath(
		FIntPoint(2, 3),
		FIntPoint(2, 3),
		Path,
		WalkableExcept({}),
		FGridPathfinding::FIsTerminal());

	TestTrue(TEXT("Start equals end should find a path"), bFoundPath);
	TestEqual(TEXT("Path should contain exactly the start tile"), Path, TArray<FIntPoint>({ FIntPoint(2, 3) }));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGridPathfindingObstacleTest,
	"GT5.Pathfinding.GridPathfinding.Obstacles.FindsAlternateRoute",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGridPathfindingObstacleTest::RunTest(const FString& Parameters)
{
	TArray<FIntPoint> Path;
	const bool bFoundPath = FGridPathfinding::FindPath(
		FIntPoint(0, 0),
		FIntPoint(2, 0),
		Path,
		WalkableExcept({ FIntPoint(1, 0) }),
		FGridPathfinding::FIsTerminal(),
		32);

	TestTrue(TEXT("Path should route around the blocked tile"), bFoundPath);
	TestEqual(TEXT("Path should start at requested tile"), Path[0], FIntPoint(0, 0));
	TestEqual(TEXT("Path should end at requested tile"), Path.Last(), FIntPoint(2, 0));
	TestFalse(TEXT("Path should not include blocked tile"), Path.Contains(FIntPoint(1, 0)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGridPathfindingTerminalTest,
	"GT5.Pathfinding.GridPathfinding.Terminal.StopsBeforeEndTile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGridPathfindingTerminalTest::RunTest(const FString& Parameters)
{
	TArray<FIntPoint> Path;
	const bool bFoundPath = FGridPathfinding::FindPath(
		FIntPoint(0, 0),
		FIntPoint(4, 0),
		Path,
		WalkableExcept({}),
		TerminalAt(FIntPoint(2, 0)),
		32);

	TestTrue(TEXT("Terminal tile should produce a path"), bFoundPath);
	TestEqual(TEXT("Path should stop at the terminal tile"), Path.Last(), FIntPoint(2, 0));

	return true;
}

#endif
