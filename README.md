# Ghast

Ghast is an early Windows-only C++20/CMake project for process memory inspection experiments using WinAPI directly.

Current capabilities:

- enumerate running processes;
- filter processes by executable name;
- print PID, executable name, and executable path when available;
- read raw bytes from another process with `ReadProcessMemory`;
- run `ghast_lab` as a controlled target process with known values in memory.

## Requirements

- Windows 10/11 x64
- Visual Studio Build Tools with the C++ workload
- CMake
- Ninja

## Build

From the repository root, open the MSVC developer shell:

```powershell
.\scripts\dev-shell.cmd
```

Then build:

```powershell
cmake --preset msvc-debug
cmake --build --preset msvc-debug
ctest --test-dir build\msvc-debug --output-on-failure
```

## Usage

Start the lab process in one terminal:

```powershell
.\build\msvc-debug\apps\ghast_lab\Debug\ghast_lab.exe
```

It prints its PID, a state address, field offsets, and then waits for commands:

```text
show
set 2048
quit
```

In another terminal, list processes matching the lab:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe LAB
```

Read memory from the lab using the PID and state address printed by `ghast_lab`:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe read <pid> <hex-address> 32
```

Example:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe read 13808 0000007A2DEFF748 32
```

## Layout

```text
apps/ghast_app   main CLI executable
apps/ghast_lab   controlled target process for memory inspection
src/windows      WinAPI-backed process and memory helpers
scripts          local developer shell helpers
```

`build/` is generated output and should not be edited.
