# Ghast

Ghast é um projeto Windows-only em C++20/CMake para experimentos de inspeção de memória de processos usando WinAPI diretamente.

O objetivo é evoluir para uma ferramenta open source brasileira para estudo e construção prática de tooling low-level no Windows.

Funcionalidades atuais:

- enumeração de processos em execução;
- filtro por nome do executável;
- impressão de PID, nome do executável e caminho quando disponível;
- leitura de bytes brutos de outro processo com `ReadProcessMemory`;
- `ghast_lab` como processo-alvo controlado com valores conhecidos em memória.

## Requisitos

- Windows 10/11 x64
- Visual Studio Build Tools com workload de C++
- CMake
- Ninja

## Build

Na raiz do repositório, abra o shell de desenvolvimento MSVC:

```powershell
.\scripts\dev-shell.cmd
```

Depois rode:

```powershell
cmake --preset msvc-debug
cmake --build --preset msvc-debug
ctest --test-dir build\msvc-debug --output-on-failure
```

## Uso

Inicie o processo de laboratório em um terminal:

```powershell
.\build\msvc-debug\apps\ghast_lab\Debug\ghast_lab.exe
```

Ele imprime o PID, um endereço de estado, offsets dos campos e espera comandos:

```text
show
set 2048
quit
```

Em outro terminal, liste processos filtrando pelo lab:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe LAB
```

Leia memória do lab usando o PID e o endereço de estado impressos pelo `ghast_lab`:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe read <pid> <hex-address> 32
```

Exemplo:

```powershell
.\build\msvc-debug\apps\ghast_app\Debug\ghast_app.exe read 13808 0000007A2DEFF748 32
```

## Estrutura

```text
apps/ghast_app   executável CLI principal
apps/ghast_lab   processo-alvo controlado para inspeção de memória
src/windows      helpers de processo e memória usando WinAPI
scripts          helpers locais de shell de desenvolvimento
```

`build/` é saída gerada e não deve ser editado.
