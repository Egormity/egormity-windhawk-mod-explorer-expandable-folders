# Explorer Expandable Folders

Windhawk mod experiment for adding macOS Finder-style expandable folders to Windows File Explorer.

Current status: Explorer-hosted UI scaffold. The mod targets `explorer.exe`, adds an `Expandable folders` checkbox into each visible File Explorer window, and covers the native file-list area with an empty view when the checkbox is enabled.

## Files

- `src/explorer_expandable_folders/` - modular source of truth.
- `explorer-expandable-folders.wh.cpp` - generated single-file Windhawk artifact.
- `tools/bundle-windhawk.ps1` - joins the modular source into the Windhawk artifact.
- `tools/build-windhawk.ps1` - bundles and verifies the generated artifact.
- `agent.md` - handoff notes for future agents working on this repo.
- `docs/usage.md` - how to install, compile, enable, and verify the Explorer-hosted scaffold.
- `docs/development.md` - local build and verification commands.
- `docs/architecture.md` - intended architecture and implementation rules.
- `docs/research-notes.md` - Finder and Windows Shell research summary.
- `docs/roadmap.md` - phased implementation plan and MVP scope.

## Current Behavior

The mod defines:

- Windhawk metadata for `explorer.exe`.
- An Explorer-hosted checkbox named `Expandable folders`.
- A blank child window that overlays the file-list area when the checkbox is checked.
- `Wh_ModInit` and `Wh_ModUninit` callbacks.
- A manager thread that attaches controls to visible Explorer windows.

Expected Explorer behavior today: the checkbox is visible; checking it replaces the file-list pane with an empty dark view. The native view remains underneath and is shown again when unchecked.

## Quick Verification

Bundle the Windhawk artifact from modular source:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bundle-windhawk.ps1
```

Run Windhawk's bundled compiler syntax check:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build-windhawk.ps1
```

A successful run prints `Windhawk source verified` and exits with code `0`.

## Source Structure

The repo uses a unity-bundled source layout:

- edit files under `src/explorer_expandable_folders/`;
- run `tools/build-windhawk.ps1`;
- paste or compile the generated `explorer-expandable-folders.wh.cpp` in Windhawk.

Do not edit the generated root `.wh.cpp` directly unless you also port the
change back into `src/`.

## Notes

Direct installation into `C:\ProgramData\Windhawk` and `HKLM:\Software\Windhawk` may require elevated permissions. If non-elevated file or registry writes fail, install the mod through the Windhawk UI.
