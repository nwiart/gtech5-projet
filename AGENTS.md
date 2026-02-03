# AGENTS.md - GT5_Projet Development Guide

## Project Overview
- **Engine**: Unreal Engine 5.5
- **Project Name**: GT5_Projet
- **Language**: C++ (C++20 standard)
- **Modules**: GT5_Projet (Runtime), GT5_ProjetEditor (Editor)
- **Type**: 2D Visual Novel game with minigames using Paper2D and PaperZD
- **Platform**: Windows (Win64)

## Build Commands

### Using Unreal Build Tool (UBT)
Build from UE5 installation directory (`C:\Program Files\Epic Games\UE_5.5`):

```bash
# Development build (most common)
Engine\Build\BatchFiles\Build.bat GT5_ProjetEditor Win64 Development "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex

# Debug build (full symbols)
Engine\Build\BatchFiles\Build.bat GT5_ProjetEditor Win64 Debug "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex

# DebugGame build (game module debug only)
Engine\Build\BatchFiles\Build.bat GT5_ProjetEditor Win64 DebugGame "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex

# Clean build
Engine\Build\BatchFiles\Clean.bat GT5_ProjetEditor Win64 Development "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex
```

### Standalone Game Builds
```bash
# Development game build
Engine\Build\BatchFiles\Build.bat GT5_Projet Win64 Development "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex

# Shipping build (optimized, no debug info)
Engine\Build\BatchFiles\Build.bat GT5_Projet Win64 Shipping "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -waitmutex
```

### Generate Project Files
```bash
Engine\Build\BatchFiles\RunUBT.bat -projectfiles -vscode -project="C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -game -engine -dotnet
```

## Testing

### Running Tests
Unreal Engine uses automation testing framework. Tests are run through the editor or command line.

```bash
# Run all tests
Engine\Binaries\Win64\UnrealEditor.exe "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -ExecCmds="Automation RunTests Now" -unattended -nopause -NullRHI -log

# Run specific test by name
Engine\Binaries\Win64\UnrealEditor.exe "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject" -ExecCmds="Automation RunTests Now <TestName>" -unattended -nopause -NullRHI -log
```

### Launching Editor
```bash
# Development Editor
"C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\User\Documents\repository\gtech5-projet\GT5_Projet.uproject"
```

## Code Style Guidelines

### File Organization
- **Headers**: `Source/GT5_Projet/Public/<Category>/<ClassName>.h`
- **Implementation**: `Source/GT5_Projet/Private/<Category>/<ClassName>.cpp`
- **Categories**: Character, Core, Components, Dialogue, Map, Minigames, Save, Systems, Widgets, Data

### Header Files (.h)

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentClass.h"
// Additional includes
#include "ClassName.generated.h"

/**
 * Brief description of class purpose
 */
UCLASS()
class GT5_PROJET_API UClassName : public UParentClass
{
    GENERATED_BODY()

public:
    // Public interface
    UFUNCTION(BlueprintCallable, Category = "CategoryName")
    void PublicFunction();

protected:
    // Protected members
    virtual void BeginPlay() override;

private:
    // Private members
    UPROPERTY(EditAnywhere, Category = "CategoryName")
    float PrivateVariable;
};
```

### Implementation Files (.cpp)

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "Category/ClassName.h"

#include "Core/OtherClass.h"
#include "Systems/SystemClass.h"

#include "Kismet/GameplayStatics.h"

// Constructor initializer list - one per line, tab-indented
UClassName::UClassName()
    : PrivateVariable(0.f)
    , AnotherVariable(false)
{
    // Constructor body
}

void UClassName::PublicFunction()
{
    // Implementation
}
```

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Classes (Actor) | `A` prefix | `AMapCharacter` |
| Classes (Object) | `U` prefix | `UVNGameInstance` |
| Classes (Struct) | `F` prefix | `FIntPoint`, `FTableEvents` |
| Enums | `E` prefix | `ESpawnActorCollisionHandlingMethod` |
| Interfaces | `I` prefix | `IInteractible` |
| Member variables | PascalCase | `MovementSpeed` |
| Private members | No special prefix | `bIsMoving`, `CurrentWaypointIndex` |
| Boolean variables | `b` prefix | `bIsMoving`, `bPathFound` |
| Function parameters | PascalCase | `DeltaTime`, `WorldContext` |
| Out parameters | `Out` prefix | `OutPath`, `outX`, `outY` |
| Local variables | PascalCase | `NewPath`, `StartTile` |
| Constants | `k` prefix or UPPER_CASE | `MaxSearchDistance` |
| Blueprint exposed | Use `UPROPERTY` or `UFUNCTION` | N/A |

### Import Order
1. Own header file (in .cpp)
2. Blank line
3. Project headers (alphabetically by category, then name)
4. Blank line
5. Engine headers (e.g., `Kismet/GameplayStatics.h`)
6. Blank line
7. Third-party headers

### Formatting
- **Indentation**: Tabs (as per UE5 standard)
- **Braces**: Opening brace on same line for functions/classes, next line for control flow acceptable
- **Line Length**: No strict limit, but keep readable (~120 chars recommended)
- **Spaces**: Space after keywords (`if`, `for`, `while`), no space before `(`

### Types and Nullability
- Use Unreal types: `int32`, `float`, `FString`, `TArray<>`, `TMap<>`, `TSet<>`
- Use `nullptr` instead of `NULL` (both present in codebase, prefer `nullptr`)
- Check pointers before dereferencing: `if (Ptr != nullptr)` or `if (Ptr)`
- Use `const` for parameters that won't be modified
- Use references (`&`) for out parameters and large objects

### UPROPERTY Specifiers
```cpp
// Editable in editor, visible in category
UPROPERTY(EditAnywhere, Category = "Movement")
float MovementSpeed;

// Blueprint read-only
UPROPERTY(BlueprintReadOnly)
FIntPoint PlayerTilePosition;

// Blueprint read-write
UPROPERTY(BlueprintReadWrite)
TSubclassOf<UVNSaveGame> SaveGameClass;

// Replicated (multiplayer)
UPROPERTY(Replicated)
int32 Health;
```

### UFUNCTION Specifiers
```cpp
// Callable from blueprints
UFUNCTION(BlueprintCallable, Category = "Pathfinding")
void MoveTo(int X, int Y);

// Pure function (no side effects)
UFUNCTION(BlueprintCallable, BlueprintPure)
void GetTilePosition(int &outX, int &outY) const;

// World context for static functions
UFUNCTION(BlueprintCallable, Category = "Pathfinding", meta=(WorldContext="WorldContext"))
static bool FindPath(const FIntPoint& StartTile, const FIntPoint& EndTile);
```

### Error Handling
- Use `UE_LOG` for logging (categories: `LogTemp`, custom categories)
- Log levels: `Warning`, `Error`, `Display`, `Verbose`
- Use `check()` for critical assertions (crashes in all builds)
- Use `ensure()` for non-critical assertions (logs in all builds, crashes in debug)

```cpp
// Logging
UE_LOG(LogTemp, Warning, TEXT("No path found from (%d, %d) to (%d, %d)"), 
    StartTile.X, StartTile.Y, EndTile.X, EndTile.Y);

// Assertions
check(IsValid(Actor)); // Critical - will crash if false
ensure(IsValid(Actor)); // Non-critical - logs but continues
```

### Comments
- Use `//` for single-line comments
- Use `/** */` for documentation comments above functions/classes
- Keep comments concise and explain *why*, not *what*
- Document public APIs, especially Blueprint-exposed functions

### Module Dependencies
**GT5_Projet.Build.cs** Public Dependencies:
- Core, CoreUObject, Engine, InputCore, EnhancedInput
- Paper2D, PaperZD, Niagara
- UMG, Slate, SlateCore
- AssetDefinition, UnrealEd (Editor only)

**GT5_ProjetEditor.Build.cs** Public Dependencies:
- Core, CoreUObject, Engine, GT5_Projet, InputCore, EnhancedInput
- AssetDefinition, UnrealEd

## Common Patterns

### Actor Movement
- Use `SetActorLocation()` and `SetActorRotation()`
- Interpolate with `FMath::Lerp()`
- Update velocity via `CharacterMovementComponent`

### Pathfinding
- Use `UPathfindingLibrary::FindPath()` for A* pathfinding
- Use `UVNTileMapLibrary` for tile coordinate conversions
- Check walkability with `IsTileWalkable()`

### Save System
- Extend `UVNSaveGame` for custom save data
- Use `TMap<FGuid, FTableEvents>` pattern for event storage
- Access via `VNSaveSubsystem` or `VNSaveManager`

### Blueprint Integration
- Mark classes with `UCLASS(Blueprintable)` for Blueprint inheritance
- Use `BlueprintCallable` for functions called from Blueprints
- Use `BlueprintImplementableEvent` for Blueprint-implemented functions
- Use `BlueprintNativeEvent` for overridable functions with C++ default

## Best Practices
1. Always initialize member variables in constructor initializer lists
2. Use `virtual void BeginPlay() override;` for initialization
3. Clean up dynamic memory in `BeginDestroy()` or use smart pointers
4. Prefer Unreal's garbage collection over manual memory management
5. Use `IsValid()` to check object validity, not just `!= nullptr`
6. Keep Tick functions optimized - disable if not needed
7. Use subsystems for singleton-like behavior
8. Follow Unreal's reflection system (UCLASS, UPROPERTY, UFUNCTION)
9. Test in-editor and in standalone builds
10. Keep Blueprint and C++ responsibilities clear

## Debugging
- Use Visual Studio debugger with .natvis support
- Set breakpoints in C++ code
- Use `DrawDebugHelpers.h` for visual debugging
- Check Output Log in editor for `UE_LOG` messages
- Use Unreal Insights for performance profiling
