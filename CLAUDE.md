# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

GT5_Projet is a narrative adventure game (visual novel with isometric exploration) built on **Unreal Engine 5.5** (C++ / Blueprints). The game features dialogue systems, tile-based character movement with A\* pathfinding, minigames, and an advanced audio system using MetaSound.

Target platform: Windows only. Two modules: `GT5_Projet` (Runtime) and `GT5_ProjetEditor` (Editor). No CI/CD pipelines or C++ tests exist — testing is Blueprint/level-based only.

## Build

Standard UE5 C++ project. Build through the Unreal Editor or via command line:

```bash
# Build (Development)
"<UE5_ROOT>/Engine/Build/BatchFiles/Build.bat" GT5_ProjetEditor Win64 Development -Project="<PROJECT_ROOT>/GT5_Projet.uproject"

# Generate project files
"<UE5_ROOT>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" -projectfiles -project="<PROJECT_ROOT>/GT5_Projet.uproject" -game -engine
```

Module dependencies are in `Source/GT5_Projet/GT5_Projet.Build.cs`. Key dependencies: EnhancedInput, Paper2D, PaperZD, Niagara, MetasoundEngine/MetasoundFrontend/AudioMixer, UMG/Slate/SlateCore.

**Important**: The `EDITOR_BUILD` define and editor deps (AssetDefinition, UnrealEd) are added conditionally in the **runtime** module's Build.cs (via `Target.bBuildEditor`), not just in the editor module. Use `#if defined(EDITOR_BUILD)` — not `WITH_EDITOR`.

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

- `AVNMapCharacter` — PaperZD-animated player with tile-based A\* pathfinding and a `HeightLevel` float property
- `APawnIsometric` — Isometric camera controller with pan, zoom, tile selection

**Critical architecture note**: `APawnIsometric` (the camera) is the actual UE Pawn possessed by `AVNPlayerController`. `AVNMapCharacter` is a separate actor spawned by `AVNChapterManager` and referenced via `AVNPlayerController::PlayerCharacter`. This is not standard UE pawn possession — the player character and the possessed pawn are different actors.

### Key Patterns

- **Subsystems as services**: All persistent global services are `UGameInstanceSubsystem`s — they persist across level transitions and communicate via delegates
- **Data-driven assets**: `UDialogueCharacter` and `UDialogueLineSequence` are `UPrimaryDataAsset`s with custom editor asset definitions and factories in `GT5_ProjetEditor` (enabling Content Browser right-click creation)
- **Event-driven**: Loose coupling through `DECLARE_DYNAMIC_MULTICAST_DELEGATE` (e.g., `OnConnectionChanged`, `OnMusicTrackChanged`, `OnLoadingFinished`, `OnMusicFadeComplete`, `OnSFXVolumeChanged`)
- **Blueprint-C++ hybrid**: C++ for systems, Blueprint for content. Uses `BlueprintCallable`/`BlueprintNativeEvent` for C++ with Blueprint override (e.g., `AVNChapterManager::Enable/Disable/TriggerMinigame`), `BlueprintImplementableEvent` for pure Blueprint hooks (e.g., `AVNMapEvent::OnPlayerHit`)
- **Manager pattern for minigames**: `ABaseMinigameManager` (extends `AGameModeBase`) is the abstract base — each minigame overrides it with its own level and HUD/win/lose widgets
- **Blueprint function libraries**: `UPathfindingLibrary` (A\* with Manhattan heuristic), `UVNTileMapLibrary` (tile↔world coordinate conversion), `USaveGameLibrary`, `UUtilsLibrary` (actors in streamed levels)
- **Save system**: `UVNSaveGame` uses `TMap<FGuid, FTableEvents>` for event tracking. The save game class is configurable via `UVNGameInstance::SaveGameClass` (supports Blueprint subclasses). Music/SFX settings save separately to their own slots.

### Source Organization

```
Source/GT5_Projet/
  Public/ and Private/ mirror each other, organized by domain:
    Character/    - AVNMapCharacter (PaperZD character)
    Components/   - UInteractibleComponent
    Core/         - UVNGameInstance, AVNGamemode, AVNChapterManager, AVNPlayerController
    Data/         - FMusicTrackData, FSFXData, FChapterData, FMinigameData, save structs
    Dialogue/     - UDialogueCharacter, UDialogueLineSequence, FDialogueLine, FDialogueChoice
    Libraries/    - Blueprint function libraries (SaveGame, Utils, TileMap, Pathfinding)
    Map/          - AVNMapEvent, AVNMapBounds, APawnIsometric, APointOfView
    Minigames/    - ABaseMinigameManager + FrameBreaker/, KnifeHitGame/, MatchingTilesGame/
    Save/         - UVNSaveGame, FTableEvents
    Subsystems/   - MusicSubsystem, SoundSubsystem, VNChapterSubsystem, VNSaveSubsystem, SceneTransitionSubsystem
    Widgets/      - UPerformanceBarWidget, minigame result widgets
```

### Class Rename History

`DefaultEngine.ini` has `ActiveClassRedirects` and path redirects. Current canonical names:

- `VNMapEvent` (was MapElement), `VNMapCharacter` (was MapCharacter)
- `VNChapterManager` (was VNChapterGamemode), `BaseMinigameManager` (was BaseMinigameGameMode)
- `FrameBreakerManager` (was FrameBreakerGameMode), `KnifeHitManager` (was KnifeHitGameMode)
- Path redirects also exist: `Character/VNMapCharacter` → `Map/VNMapCharacter`, `Save/VNSaveSubsystem` → `Subsystems/VNSaveSubsystem`, `Core/VNChapterSubsystem` → `Subsystems/VNChapterSubsystem`, `Libraries/` was previously `Systems/`

### Dialogue System

`UDialogueCharacter` holds a name and a map of `EDialoguePose` (17 emotion states: Normal, Blank, Angry, Worried, Sad, ClosedEyes1/2, Serious, Smile1/2/3, Intrigued1/2, Scared, Surprised, BlackEyes1/2) to sprite textures. `UDialogueLineSequence` chains lines with optional branching choices that modify the Connection score.

### Audio System

Two subsystems handle all audio. Only custom log categories in the project: `LogMusic` and `LogSound`.

- **MusicSubsystem**: Plays one track at a time via `UAudioComponent`. Supports crossfade (two components swapped on completion). Fade ticks at ~60Hz via `FTimerManager`. Volume formula: `BaseVolume * MasterVolume * FadeMultiplier`. Components are marked `bIsUISound = true` (plays during pause) and `bAllowSpatialization = false`. MetaSound parameters: sets `bool "Looping"` via `SetBoolParameter`, float params via `SetFloatParameter`, trigger params via `SetTriggerParameter`. All float parameters are persisted in `TMap<FName, float> StoredParameters` across save/load. Playback time resumption is not yet implemented.
- **SoundSubsystem**: Plays SFX via DataTable lookup (`FSFXTableRow`). 2D sounds use `UGameplayStatics::PlaySound2D` directly. 3D sounds use an `AudioComponent` pool (max 16, created on-demand). Supports pitch/volume randomization and per-SFX cooldowns.

### Rendering & Engine Config

- DX12 / SM6, Ray Tracing and Path Tracing enabled, Nanite disabled, Lumen software ray tracing (no hardware), static lighting disabled, virtual shadow maps disabled
- Paper2D: `DefaultPixelsPerUnrealUnit=0.200000` (1 pixel = 5 Unreal units) — critical for correct sprite scale
- Audio: 48kHz sample rate, Resonance Audio as spatialization and reverb plugin
- Translucent sort axis `Y=-1` configured for isometric view
- Input: EnhancedInput is the active system; legacy `ActionMappings`/`AxisMappings` remain in `DefaultInput.ini` from earlier development

## Conventions

- All exported runtime classes use the `GT5_PROJET_API` macro. Editor module classes are not exported (no API macro)
- Public headers in `Public/`, private implementations in `Private/` with matching subdirectory structure
- UProperties use `Category="..."` for organized Details panel grouping
- Editor-only code is guarded with `#if defined(EDITOR_BUILD)`
- Only 2 custom log categories exist: `LogMusic` and `LogSound` — other systems use default engine logging

## Documentation

A Cursor command (`.cursor/commands/generate-guide.md`, written in French) generates Blueprint developer guides from C++ source and publishes them directly to the GitHub wiki.
