# AGENTS.md

## Overview & Scope
GT5_Projet is an Unreal Engine 5.5 Windows game project with C++ runtime/editor modules and Blueprint/content assets. Applies to the entire repo unless a nested `AGENTS.md` is added later; closest `AGENTS.md` to the edited file wins.

## Agent Role
- Act as an experienced Unreal Engine 5.5 C++ gameplay/editor engineer.
- Allowed: edit C++ in `Source/`, project config in `Config/`, `.uproject` metadata, and documentation.
- Allowed: inspect asset paths and references in `Content/`; modify binary assets only when explicitly requested and only through Unreal-aware tooling.
- Must not: edit generated/cache/build outputs, commit secrets, run destructive git commands, or rewrite large gameplay systems without approval.

## Build, Test & Validation Commands

```bash
git lfs version
```

```bash
"/c/Program Files/Epic Games/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe" "C:/Users/User/Documents/repository/gtech5-projet/GT5_Projet.uproject"  # (unverified)
```

```bash
"/c/Program Files/Epic Games/UE_5.5/Engine/Build/BatchFiles/Build.bat" GT5_ProjetEditor Win64 Development "C:/Users/User/Documents/repository/gtech5-projet/GT5_Projet.uproject" -WaitMutex  # (unverified)
```

```bash
"/c/Program Files/Epic Games/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "C:/Users/User/Documents/repository/gtech5-projet/GT5_Projet.uproject" -ExecCmds="Automation RunTests GT5.Sound; Quit" -TestExit="Automation Test Queue Empty" -unattended -nop4  # (unverified)
```

- No package-manager install command is present.
- No standalone lint/format/test script is present.
- Unreal build/test commands are verified as local paths only; they were not executed during generation.

## Conventions & Patterns
- Use CRLF, UTF-8, final newline, and trim trailing whitespace from `.editorconfig`.
- Use tabs with width 4 for C++, C#, JSON, `.uproject`, `.uplugin`, HLSL, USF, and USH files.
- Use spaces with width 2 for `.ini`, `.cfg`, YAML, XML, HTML, and UXML files.
- Use Allman braces for C++ and C#.
- Keep runtime C++ in `Source/GT5_Projet`; headers in `Public/`, implementations in `Private/`.
- Keep editor-only C++ in `Source/GT5_ProjetEditor`.
- Mirror feature folders between `Public/` and `Private/` when adding C++ types.
- Current runtime feature folders include `Character/`, `Components/`, `Core/`, `Data/`, `Dialogue/`, `Libraries/`, `Map/`, `Minigames/`, `Save/`, `Subsystems/`, and `Widgets/`.
- Prefer Unreal naming: `U` for UObject classes, `A` for Actors, `F` for structs, `E` for enums, `I` for interfaces.
- Use `GT5_PROJET_API` on exported runtime classes that need cross-module visibility.
- Expose gameplay/editor hooks to Blueprints with `UCLASS`, `USTRUCT`, `UFUNCTION`, and `UPROPERTY` metadata only when needed.
- Use existing modules before adding dependencies: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `Paper2D`, `PaperZD`, `Niagara`, `MetasoundEngine`, `MetasoundFrontend`, `AudioMixer`, `UMG`, `Slate`, `SlateCore`.
- Put automation tests under `Source/GT5_Projet/Private/Tests` using Unreal Automation Test macros; existing test namespace prefix is `GT5.Sound`.
- Keep Unreal config in `Config/Default*.ini`.
- Keep game-owned assets under `Content/Game/`; third-party/sample assets remain in their existing `Content/*` folders.
- Search code with `rg` scoped to `Source/`; avoid `Content/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, and binary assets unless asset paths are required.

## Dos and Don'ts
- Do: inspect `.uproject`, `.Build.cs`, `.Target.cs`, and `.editorconfig` before changing build/module behavior.
- Do: keep Blueprint-facing C++ API changes backward-compatible unless the current task explicitly migrates assets.
- Do: update `Public/` headers and matching `Private/` implementations together.
- Do: add focused Unreal Automation tests for shared subsystems, libraries, and data resolution logic.
- Do: preserve Git LFS tracking for `.uasset`, `.umap`, media, font, image, mesh, and audio files.
- Don't: add plugins, modules, or new dependencies without approval.
- Don't: hand-edit `.uasset`, `.umap`, `.uexp`, or generated Visual Studio files.
- Don't: move assets in `Content/` without considering Blueprint/map references.
- Don't: commit local/editor-generated state from `Saved/`, `Intermediate/`, `DerivedDataCache/`, `Binaries/`, `.vs/`, `.vscode/`, `.idea/`, or `Build/`.
- Don't: assume web/package tooling exists; no `package.json`, `pyproject.toml`, `go.mod`, or `Cargo.toml` was found.

## Safety & Guardrails
- Off-limits without explicit request: production save data, secrets, binary assets, engine installation files, generated caches, and large architecture rewrites.
- Never edit: `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.git/`, `.vs/`, `.vscode/`, `.idea/`.
- Safe to automate: text-only C++/C# config/doc edits, `git status`, `git diff`, `git lfs version`, and scoped `rg` searches.
- Avoid running full Unreal builds, editor sessions, automation suites, asset imports, migrations, packaging, or cooking unless explicitly requested.
- Store debug logs as `.cursor/debug-*.log`, never at repo root.
- Do not include credentials, tokens, license keys, or machine-local private paths in committed docs beyond verified tool paths already required for local UE commands.

## Git & PR Rules
- Branch: current work was observed on `camera-revamp`; confirm target branch before branching or merging.
- Commit: follow existing concise sentence-style messages such as `Added delay for tile double-click` or `Refactor so it makes more sense`.
- PR: describe gameplay/editor impact, list touched systems/assets, and include build/test commands run or explicitly mark them not run.
- PR: call out any Blueprint, map, asset, config, or Git LFS changes.
- Do not commit unless explicitly asked.
