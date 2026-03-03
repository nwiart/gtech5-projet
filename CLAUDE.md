# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

GT5_Projet is a narrative adventure game (visual novel with isometric exploration) built on **Unreal Engine 5.5** (C++ / Blueprints). The game features dialogue systems, tile-based character movement with A\* pathfinding, minigames, and an advanced audio system using MetaSound.

Target platform: Windows only. Two modules: `GT5_Projet` (Runtime) and `GT5_ProjetEditor` (Editor).

## Build

This is a standard UE5 C++ project. Build through the Unreal Editor or via command line:

```bash
# Build (Development)
"<UE5_ROOT>/Engine/Build/BatchFiles/Build.bat" GT5_ProjetEditor Win64 Development -Project="<PROJECT_ROOT>/GT5_Projet.uproject"

# Generate project files
"<UE5_ROOT>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" -projectfiles -project="<PROJECT_ROOT>/GT5_Projet.uproject" -game -engine
```

Module dependencies are defined in `Source/GT5_Projet/GT5_Projet.Build.cs`. Key dependencies: EnhancedInput, Paper2D, PaperZD, Niagara, MetasoundEngine/MetasoundFrontend/AudioMixer, UMG/Slate/SlateCore. Editor-only: AssetDefinition, UnrealEd (guarded by `EDITOR_BUILD` define).

## Architecture

### Game Flow

Entry point is `Map_Persistent` level with `BP_GameInstance` (extends `UVNGameInstance`) and `BP_GamemodeGlobal`.

```
UVNGameInstance (persists across levels)
  ├── MusicSubsystem        - Background music with MetaSound, crossfade, fade in/out
  ├── SoundSubsystem        - SFX with 2D/3D playback, pooling (16 max), cooldowns
  ├── VNChapterSubsystem    - Chapter progression, Connection score (-100 to +100)
  ├── VNSaveSubsystem       - GUID-based event tracking, save/load/delete
  └── SceneTransitionSubsystem - Async level loading with progress
```

Chapters load their own levels with `AVNChapterManager` (extends `AVNGamemode`) as GameMode, which spawns:

- `AVNMapCharacter` - PaperZD-animated player with tile-based A\* pathfinding
- `APawnIsometric` - Isometric camera controller with pan, zoom, tile selection

### Key Patterns

- **Subsystems as services**: All persistent global services are `UGameInstanceSubsystem`s — they persist across level transitions and communicate via delegates
- **Data-driven assets**: `UDialogueCharacter` and `UDialogueLineSequence` are `UPrimaryDataAsset`s with custom editor asset definitions in `GT5_ProjetEditor`
- **Event-driven**: Loose coupling through multicast delegates (e.g., `OnConnectionChanged`, `OnMusicTrackChanged`, `OnLoadingFinished`)
- **Blueprint-C++ hybrid**: C++ for systems, Blueprint for content. Classes use `BlueprintCallable`/`BlueprintImplementableEvent` extensively
- **Manager pattern for minigames**: `ABaseMinigameManager` (extends `AGameModeBase`) is the abstract base — each minigame (FrameBreaker, KnifeHitGame, MatchingTilesGame) overrides it with its own level and HUD/win/lose widgets

### Source Organization

```
Source/GT5_Projet/
  Public/ and Private/ mirror each other, organized by domain:
    Character/    - AVNMapCharacter (PaperZD character)
    Components/   - UInteractibleComponent
    Core/         - UVNGameInstance, AVNGamemode, AVNChapterManager
    Data/         - FMusicTrackData, FSFXData, FChapterData, FMinigameData, save structs
    Dialogue/     - UDialogueCharacter, UDialogueLineSequence, FDialogueLine, FDialogueChoice
    Libraries/    - Blueprint function libraries (SaveGame, Utils, TileMap, Pathfinding)
    Map/          - AVNMapEvent, AVNMapBounds, APawnIsometric
    Minigames/    - ABaseMinigameManager + FrameBreaker/, KnifeHitGame/, MatchingTilesGame/
    Save/         - UVNSaveGame, FTableEvents
    Subsystems/   - MusicSubsystem, SoundSubsystem, VNChapterSubsystem, VNSaveSubsystem, SceneTransitionSubsystem
    Widgets/      - UPerformanceBarWidget, minigame result widgets
```

### Class Rename History

The engine config has `ActiveClassRedirects` for several renames. Current canonical names:

- `VNMapEvent` (was MapElement), `VNMapCharacter` (was MapCharacter)
- `VNChapterManager` (was VNChapterGamemode), `BaseMinigameManager` (was BaseMinigameGameMode)
- `FrameBreakerManager` (was FrameBreakerGameMode), `KnifeHitManager` (was KnifeHitGameMode)

### Dialogue System

`UDialogueCharacter` holds a name and a map of `EDialoguePose` (17 emotion states) to sprite textures. `UDialogueLineSequence` chains lines with optional branching choices that modify the Connection score.

### Audio System

Two subsystems handle all audio:

- **MusicSubsystem**: Plays one track at a time with crossfade support, MetaSound parameter control, auto-saves/restores state. Log category: `LogMusic`
- **SoundSubsystem**: Plays SFX via DataTable lookup (`FSFXTableRow`), supports pitch/volume randomization and per-SFX cooldowns. AudioComponent pool for 3D sounds (max 16). Log category: `LogSound`

## Conventions

- All exported classes use the `GT5_PROJET_API` macro
- Public headers in `Public/`, private implementations in `Private/` with matching subdirectory structure
- UProperties use `Category="..."` for organized Details panel grouping
- Editor-only code is guarded with `#if defined(EDITOR_BUILD)`
