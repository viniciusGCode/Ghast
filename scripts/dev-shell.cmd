@echo off
setlocal

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -NoExit -Command ^
  "Set-Location -LiteralPath '%CD%'; . '%~dp0Enter-DevShell.ps1'"
