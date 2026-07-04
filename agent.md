# Agent Handoff

This repository contains a Windhawk mod experiment for File Explorer expandable
folders. The project is currently in Explorer-hosted scaffold mode: injectable,
visibly controllable from Explorer, and intentionally not doing real file-tree
logic yet.

## Current Branch

Work is on:

```text
feature/codex/init-windhawk-mod/2026-07-04
```

Follow the user's global branch rule before starting a new feature. If a new
feature is requested from another branch, ask whether to continue or switch.

## Project Files

- `explorer-expandable-folders.wh.cpp` is the only source file.
- `README.md` is the project entry point.
- `docs/` contains detailed usage, development, architecture, research, and
  roadmap notes.

## Current Mod State

Version: `0.2.0`

The mod:

- targets `explorer.exe`;
- declares `@architecture x86-64`;
- removes the Windhawk settings checkbox;
- adds an Explorer-hosted checkbox named `Expandable folders`;
- stores the checkbox state in Windhawk local storage as
  `explorerViewEnabled`;
- overlays the file-list pane with a blank child window when checked;
- restores the native view by hiding the overlay when unchecked;
- intentionally does not implement real expandable folder logic yet.

## Local Windhawk Context

Observed local paths:

```text
C:\Program Files\Windhawk
C:\Program Files\Windhawk\Compiler\bin\clang++.exe
C:\Program Files\Windhawk\Compiler\compile_flags.txt
C:\Program Files\Windhawk\Compiler\include\windhawk_api.h
C:\Program Files\Windhawk\Engine\1.7.3\64\windhawk.lib
C:\ProgramData\Windhawk\ModsSource
C:\ProgramData\Windhawk\Engine\Mods\64
HKLM:\Software\Windhawk\Engine\Mods
```

Direct writes to `C:\ProgramData\Windhawk` and
`HKLM:\Software\Windhawk\Engine\Mods` failed from the non-elevated shell with
`Access is denied`. Use the Windhawk UI or an elevated install flow.

## Verification Commands

Syntax check:

```powershell
& 'C:\Program Files\Windhawk\Compiler\bin\clang++.exe' '@C:\Program Files\Windhawk\Compiler\compile_flags.txt' -I 'C:\Program Files\Windhawk\Compiler\include' -fsyntax-only '.\explorer-expandable-folders.wh.cpp'
```

Temporary x64 DLL build check:

```powershell
& 'C:\Program Files\Windhawk\Compiler\bin\clang++.exe' '@C:\Program Files\Windhawk\Compiler\compile_flags.txt' -I 'C:\Program Files\Windhawk\Compiler\include' -DWH_MOD_ID=L'"explorer-expandable-folders"' -DWH_MOD_VERSION=L'"0.2.0"' -shared '.\explorer-expandable-folders.wh.cpp' -x none 'C:\Program Files\Windhawk\Engine\1.7.3\64\windhawk.lib' -lgdi32 -o '.\explorer-expandable-folders_0.2.0_test.dll'
```

Do not commit generated DLL files.

## Safety Rules

- Keep Explorer behavior behind the in-window checkbox.
- Prefer small, reversible probes before touching Explorer internals.
- Do not create fake native Explorer rows in the existing list view.
- Do not implement real file operations with raw paths.
- Delegate Shell behavior to Shell APIs once the project moves beyond the
  scaffold.
- Keep unsupported Explorer locations on native behavior until explicitly
  supported.
