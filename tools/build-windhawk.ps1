param(
    [switch]$Dll
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")
$sourcePath = Join-Path $repoRoot "explorer-expandable-folders.wh.cpp"
$compilerRoot = "C:\Program Files\Windhawk\Compiler"
$compiler = Join-Path $compilerRoot "bin\clang++.exe"
$compileFlags = Join-Path $compilerRoot "compile_flags.txt"
$includePath = Join-Path $compilerRoot "include"
$windhawkLib = "C:\Program Files\Windhawk\Engine\1.7.3\64\windhawk.lib"

& (Join-Path $PSScriptRoot "bundle-windhawk.ps1")

& $compiler "@$compileFlags" -I $includePath -fsyntax-only $sourcePath

if ($Dll) {
    $buildDir = Join-Path $repoRoot "build"
    if (!(Test-Path -LiteralPath $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }

    $outputDll = Join-Path $buildDir "explorer-expandable-folders_0.3.3_test.dll"
    & $compiler "@$compileFlags" `
        -I $includePath `
        -DWH_MOD_ID=L'"explorer-expandable-folders"' `
        -DWH_MOD_VERSION=L'"0.3.3"' `
        -shared $sourcePath `
        -x none $windhawkLib `
        -lgdi32 `
        -o $outputDll

    Write-Output "Built $outputDll"
}

Write-Output "Windhawk source verified"
