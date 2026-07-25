param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [ValidateSet("x64", "Win32", "AnyCPU")]
    [string]$Platform = "x64"
)

$ErrorActionPreference = "Stop"

$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

if (-not (Test-Path $msbuild)) {
    throw "MSBuild.exe was not found: $msbuild"
}

$repositoryRoot = [System.IO.Path]::GetFullPath(
    (Join-Path $PSScriptRoot "..")
)

$solution = Join-Path $repositoryRoot "Project\AOENGINE.slnx"

if (-not (Test-Path $solution)) {
    throw "Solution file was not found: $solution"
}

Write-Host "MSBuild      : $msbuild"
Write-Host "Solution     : $solution"
Write-Host "Configuration: $Configuration"
Write-Host "Platform     : $Platform"

& $msbuild `
    $solution `
    "/m" `
    "/p:Configuration=$Configuration" `
    "/p:Platform=$Platform"

$exitCode = $LASTEXITCODE

if ($exitCode -ne 0) {
    Write-Host "Build failed. Exit code: $exitCode"
}

exit $exitCode