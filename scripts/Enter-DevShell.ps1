[CmdletBinding()]
param()

if ($env:VSCMD_VER -and (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    Write-Host "MSVC developer environment is already active ($env:VSCMD_VER)."
    return
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) {
    throw 'vswhere.exe was not found. Install Visual Studio Build Tools with the C++ workload.'
}

$installPath = & $vswhere `
    -latest `
    -products '*' `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath

if (-not $installPath) {
    throw 'No Visual Studio installation with the MSVC x64 toolchain was found.'
}

$devShellModule = Join-Path $installPath 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll'
if (-not (Test-Path -LiteralPath $devShellModule)) {
    throw "Developer Shell module was not found at: $devShellModule"
}

Import-Module $devShellModule -ErrorAction Stop
Enter-VsDevShell `
    -VsInstallPath $installPath `
    -SkipAutomaticLocation `
    -DevCmdArguments '-arch=x64 -host_arch=x64' `
    -ErrorAction Stop

Write-Host "MSVC x64 developer environment ready ($env:VSCMD_VER)."
Write-Host 'Available: cl, cmake, ninja, msbuild.'
