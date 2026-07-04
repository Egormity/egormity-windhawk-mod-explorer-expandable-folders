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

## Source File

Main source:

```text
explorer-expandable-folders.wh.cpp
```

The source is intended to be copied directly into Windhawk's local mod editor.

## Syntax Check

```powershell
& 'C:\Program Files\Windhawk\Compiler\bin\clang++.exe' '@C:\Program Files\Windhawk\Compiler\compile_flags.txt' -I 'C:\Program Files\Windhawk\Compiler\include' -fsyntax-only '.\explorer-expandable-folders.wh.cpp'
```

Expected result: no output, exit code `0`.

## DLL Build Check

This builds a temporary local DLL for validation. It does not register or inject
the mod.

```powershell
& 'C:\Program Files\Windhawk\Compiler\bin\clang++.exe' '@C:\Program Files\Windhawk\Compiler\compile_flags.txt' -I 'C:\Program Files\Windhawk\Compiler\include' -DWH_MOD_ID=L'"explorer-expandable-folders"' -DWH_MOD_VERSION=L'"0.1.2"' -shared '.\explorer-expandable-folders.wh.cpp' -x none 'C:\Program Files\Windhawk\Engine\1.7.3\64\windhawk.lib' -o '.\explorer-expandable-folders_0.1.2_test.dll'
```

The `-x none` before `windhawk.lib` is important. Windhawk's compile flags force
`-x c++`; without resetting it, Clang tries to parse the import library as C++
source.

Delete generated DLLs after testing:

```powershell
Remove-Item -LiteralPath '.\explorer-expandable-folders_0.1.2_test.dll' -Force
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
// @version         0.1.2
```

When changing mod behavior, bump this version and include it in the commit
message.
