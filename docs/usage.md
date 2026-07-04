# Usage

This document describes how to use the current scaffold. It does not implement
expandable folders yet.

## What To Expect

When installed and enabled in Windhawk, the mod should:

- inject into `explorer.exe`;
- show a settings checkbox named `Enable expandable folders scaffold`;
- log whether that checkbox is enabled;
- unload cleanly.

It should not change File Explorer visually yet.

## Install Through Windhawk UI

1. Open Windhawk:

   ```powershell
   Start-Process 'C:\Program Files\Windhawk\windhawk.exe'
   ```

2. Create a new local mod or edit the local scaffold mod.

3. Paste the contents of `explorer-expandable-folders.wh.cpp`.

4. Compile the mod.

5. Enable the mod.

6. Open or restart File Explorer.

7. Check Windhawk logs for a message similar to:

   ```text
   Explorer Expandable Folders initialized: enabled=1
   ```

## Checkbox

The visible Windhawk checkbox is:

```text
Enable expandable folders scaffold
```

It controls the setting:

```text
enabled
```

Current behavior:

- checked: logs `enabled=1`;
- unchecked: logs `enabled=0`;
- future behavior: all Explorer logic should be skipped when unchecked.

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
