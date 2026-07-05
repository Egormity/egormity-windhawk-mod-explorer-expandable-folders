param(
    [string]$Entry = "src/explorer_expandable_folders/mod.wh.cpp",
    [string]$Output = "explorer-expandable-folders.wh.cpp"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")
$entryPath = Resolve-Path -LiteralPath (Join-Path $repoRoot $Entry)
$outputPath = Join-Path $repoRoot $Output
$included = New-Object "System.Collections.Generic.HashSet[string]"
$lines = New-Object "System.Collections.Generic.List[string]"

function Resolve-LocalInclude {
    param(
        [string]$Include,
        [string]$CurrentDirectory
    )

    $candidate = Join-Path $CurrentDirectory $Include
    if (Test-Path -LiteralPath $candidate) {
        return (Resolve-Path -LiteralPath $candidate).Path
    }

    $sourceCandidate = Join-Path (Join-Path $repoRoot "src/explorer_expandable_folders") $Include
    if (Test-Path -LiteralPath $sourceCandidate) {
        return (Resolve-Path -LiteralPath $sourceCandidate).Path
    }

    throw "Unable to resolve local include '$Include' from '$CurrentDirectory'."
}

function Expand-SourceFile {
    param([string]$Path)

    $resolvedPath = (Resolve-Path -LiteralPath $Path).Path
    if ($included.Contains($resolvedPath)) {
        return
    }

    [void]$included.Add($resolvedPath)
    $currentDirectory = Split-Path -Parent $resolvedPath

    foreach ($line in Get-Content -LiteralPath $resolvedPath) {
        if ($line.Trim() -eq "#pragma once") {
            continue
        }

        if ($line -match '^\s*#include\s+"([^"]+)"\s*$') {
            $includePath = Resolve-LocalInclude -Include $Matches[1] -CurrentDirectory $currentDirectory
            Expand-SourceFile -Path $includePath
            continue
        }

        [void]$lines.Add($line)
    }

    [void]$lines.Add("")
}

Expand-SourceFile -Path $entryPath

$parent = Split-Path -Parent $outputPath
if ($parent -and !(Test-Path -LiteralPath $parent)) {
    New-Item -ItemType Directory -Path $parent | Out-Null
}

$content = ($lines -join [Environment]::NewLine).TrimEnd() + [Environment]::NewLine
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText($outputPath, $content, $utf8NoBom)
Write-Output "Bundled $Entry -> $Output"
