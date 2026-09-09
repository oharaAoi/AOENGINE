[CmdletBinding()]
param(
    [string]$ConfigPath = ""
)

$ErrorActionPreference = "Stop"

$scriptFile = $MyInvocation.MyCommand.Path
if ([string]::IsNullOrWhiteSpace($scriptFile)) {
    throw "The script file path could not be resolved."
}
$projectDir = [System.IO.Path]::GetFullPath((Split-Path -Parent $scriptFile))
if ([string]::IsNullOrWhiteSpace($ConfigPath)) {
    $ConfigPath = Join-Path $projectDir "CreateExeFile.local.psd1"
}
$repositoryDir = [System.IO.Path]::GetFullPath((Join-Path $projectDir ".."))
$repositoryParentDir = Split-Path -Parent $repositoryDir

# Default settings. Output is placed next to the AOENGINE repository.
$settings = @{
    BuildRoot = Join-Path $repositoryDir "Generated\Outputs\Release"
    ResourceDir = Join-Path $projectDir "Assets"
    OutputDir = Join-Path $repositoryParentDir "AOENGINE-Exe"
    SourceExeName = "AOENGINE.exe"
    NewExeName = "Game.exe"
}

# Local settings are optional and are excluded from Git.
if (Test-Path -LiteralPath $ConfigPath -PathType Leaf) {
    $localSettings = Import-PowerShellDataFile -LiteralPath $ConfigPath
    foreach ($key in @($settings.Keys)) {
        if ($localSettings.ContainsKey($key) -and
            -not [string]::IsNullOrWhiteSpace([string]$localSettings[$key])) {
            $settings[$key] = [string]$localSettings[$key]
        }
    }
}

# Resolve relative paths from the Project directory.
function Resolve-ConfiguredPath([string]$Path) {
    # IsPathFullyQualified is unavailable on the .NET Framework used by
    # Windows PowerShell 5.1. IsPathRooted supports the required Windows paths.
    if ([System.IO.Path]::IsPathRooted($Path)) {
        return [System.IO.Path]::GetFullPath($Path)
    }
    return [System.IO.Path]::GetFullPath((Join-Path $projectDir $Path))
}

# Build output path
$buildRoot = Resolve-ConfiguredPath $settings.BuildRoot
# Resource path
$resourceDir = Resolve-ConfiguredPath $settings.ResourceDir
# Package output path
$outputDir = Resolve-ConfiguredPath $settings.OutputDir
$newExeName = $settings.NewExeName

if (-not (Test-Path -LiteralPath $buildRoot -PathType Container)) {
    throw "Release build output was not found: $buildRoot"
}
if (-not (Test-Path -LiteralPath $resourceDir -PathType Container)) {
    throw "Resource directory was not found: $resourceDir"
}
if ([System.IO.Path]::GetExtension($newExeName) -ne ".exe") {
    throw "NewExeName must have an .exe extension: $newExeName"
}

# Never place or clear package output inside the Git repository.
$repositoryPrefix = $repositoryDir.TrimEnd('\') + '\'
if ($outputDir -eq $repositoryDir -or
    $outputDir.StartsWith($repositoryPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDir must be outside the AOENGINE repository: $outputDir"
}

$sourceExeName = $settings.SourceExeName
if ([System.IO.Path]::GetExtension($sourceExeName) -ne ".exe") {
    throw "SourceExeName must have an .exe extension: $sourceExeName"
}

$exeFiles = @(Get-ChildItem -LiteralPath $buildRoot -Recurse -Filter $sourceExeName -File)
if ($exeFiles.Count -eq 0) {
    throw "The configured executable was not found: $sourceExeName (BuildRoot: $buildRoot)"
}
if ($exeFiles.Count -gt 1) {
    throw "Multiple matching executables were found. Set BuildRoot to a more specific directory."
}

# Clear output only after all source paths have been validated.
if (Test-Path -LiteralPath $outputDir) {
    Write-Host "=== Clearing existing output directory... ==="
    Get-ChildItem -LiteralPath $outputDir -Force | Remove-Item -Recurse -Force
} else {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
}

Write-Host "=== Output: $outputDir ==="
Write-Host "=== Copying executable and DLL files... ==="

$sourceExe = $exeFiles[0]
$sourceBinaryDir = $sourceExe.Directory.FullName
Copy-Item -LiteralPath $sourceExe.FullName -Destination (Join-Path $outputDir $newExeName) -Force
Get-ChildItem -LiteralPath $sourceBinaryDir -Filter "*.dll" -File |
    ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $outputDir $_.Name) -Force
    }

Write-Host "=== Copying resource files... ==="
$destinationProjectDir = Join-Path $outputDir "Project"
New-Item -ItemType Directory -Path $destinationProjectDir -Force | Out-Null
Copy-Item -LiteralPath $resourceDir -Destination (Join-Path $destinationProjectDir "Assets") -Recurse -Force

Write-Host "Package creation completed: $outputDir"
Start-Process -FilePath $outputDir
