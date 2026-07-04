# Usage

This document describes how to use the current Explorer-hosted scaffold. It does
not implement real expandable folders yet.

## What To Expect

When installed and enabled in Windhawk, the mod should:

- inject into `explorer.exe`;
- show an `Expandable folders` checkbox inside each visible File Explorer window;
- cover the native file-list pane with an empty view when the checkbox is checked;
- show the native file-list pane again when unchecked;
- unload cleanly.

It should not display real expandable folders yet.

## Install Through Windhawk UI

1. Open Windhawk:

   ```powershell
   Start-Process 'C:\Program Files\Windhawk\windhawk.exe'
   ```

2. Create a new local mod or edit the local scaffold mod.

3. Run the bundler if the modular source changed:

   ```powershell
   powershell -ExecutionPolicy Bypass -File .\tools\bundle-windhawk.ps1
   ```

4. Paste the contents of `explorer-expandable-folders.wh.cpp`.

5. Compile the mod.

6. Enable the mod.

7. Open or restart File Explorer.

8. In File Explorer, use the `Expandable folders` checkbox.

9. Check Windhawk logs for a message similar to:

   ```text
   Explorer Expandable Folders initialized: explorer checkbox=0
   ```

## Checkbox

The visible checkbox is in File Explorer:

```text
Expandable folders
```

It controls the persisted local value:

```text
explorerViewEnabled
```

Current behavior:

- checked: a blank child window overlays the native file-list pane;
- unchecked: the overlay is hidden and the native file-list pane is visible;
- the same state is synchronized across visible Explorer windows.

## Direct Install Status

Direct installation was attempted by copying source and compiled DLL files into
Windhawk's ProgramData folders and registering the mod under HKLM.

That failed from the current non-elevated shell:

```text
Access to the path 'C:\ProgramData\Windhawk\ModsSource\explorer-expandable-folders.wh.cpp' is denied.
Access to the path 'C:\ProgramData\Windhawk\Engine\Mods\64\explorer-expandable-folders_<version>_<build>.dll' is denied.
Access to the registry key 'HKEY_LOCAL_MACHINE\Software\Windhawk\Engine\Mods\explorer-expandable-folders' is denied.
```

Use the Windhawk UI or an elevated install path for actual registration.

## Restart Explorer

If Windhawk does not inject immediately after enabling the mod, restart Windows
Explorer from Task Manager or run:

```powershell
Stop-Process -Name explorer -Force
Start-Process explorer.exe
```

Use the Task Manager method if you want the gentler normal restart path.
