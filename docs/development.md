# Development

## Environment

Development has been done on Windows 11 with PowerShell and the local Windhawk
installation.

Observed Windhawk version:

```text
1.7.3
```

Relevant local paths:

```text
C:\Program Files\Windhawk\Compiler\bin\clang++.exe
C:\Program Files\Windhawk\Compiler\compile_flags.txt
C:\Program Files\Windhawk\Compiler\include
C:\Program Files\Windhawk\Engine\1.7.3\64\windhawk.lib
```

## Source Layout

Modular source of truth:

```text
src/explorer_expandable_folders/
```

Generated Windhawk artifact:

```text
explorer-expandable-folders.wh.cpp
```

The generated artifact is intended to be copied directly into Windhawk's local
mod editor. Do not edit it directly; edit `src/` and bundle.

## Bundle Command

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\bundle-windhawk.ps1
```

The bundler starts at:

```text
src/explorer_expandable_folders/mod.wh.cpp
```

It recursively expands local `#include "..."` lines and writes:

```text
explorer-expandable-folders.wh.cpp
```

## Syntax Check

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build-windhawk.ps1
```

Expected result: the bundler runs and Windhawk's compiler exits with code `0`.

## DLL Build Check

This builds a temporary local DLL for validation. It does not register or inject
the mod.

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build-windhawk.ps1 -Dll
```

The `-x none` before `windhawk.lib` is important. Windhawk's compile flags force
`-x c++`; without resetting it, Clang tries to parse the import library as C++
source.

Delete generated DLLs after testing:

```powershell
Remove-Item -LiteralPath '.\build\explorer-expandable-folders_0.3.0_test.dll' -Force
```

## Tests

There is no automated test harness yet.

Current verification is:

- Windhawk Clang syntax check;
- optional x64 DLL build check;
- manual Windhawk compile and enable test;
- Windhawk log check.

## Versioning

The Windhawk metadata version is the source of truth:

```cpp
// @version         0.3.0
```

When changing mod behavior, bump this version and include it in the commit
message.
