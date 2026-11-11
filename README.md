# tr04

A matching decompilation of Tomb Raider IV: The Last Revelation, the
Steam Windows release, built and verified on Linux through Wine.

The goal is C and C++ source that compiles into the original machine code.
During development, a DLL replaces recovered functions in a copy of the
original executable, so they can be exercised in the game.

![Progress](docs/progress.svg)

## Sources

This is not a clean-room project. The references are:

- [TRX](https://github.com/LostArtefacts/TRX): the target's function and
  global names, addresses and types in `docs/symbols/tr4.txt`.
- `refs/tomb4-legacy`: the hand decompilation, for structures and logic.
- `refs/tr02` and `refs/tr03`: reusable code and module layout.

IDA and Ghidra provide disassembly and pseudocode for the original binary.

## Target

`tomb4.exe`, linked 1999-12-17, 32-bit PE with image base `0x400000`.
SHA-256:

```text
1ae1e16a111598f087c661323b5e920ef8e23d922b6abf5168682e9a6ad986d1
```

Game files are not distributed. Supply a copy at `orig/tomb4`; see
[`orig/README.md`](orig/README.md).

## Toolchain

Visual C++ 6.0 SP3, run under Wine. Component versions matter:

| Component | Version |
| --- | --- |
| Compiler driver (`cl.exe`) | 12.00.8168 |
| C frontend (`c1.dll`) | 12.00.8472 |
| C++ frontend (`c1xx.dll`) | 12.00.8472 |
| Optimizer (`c2.dll`) | 12.00.8168 |
| Resource converter (`cvtres.exe`) | 5.00.1736.1 |
| Original assembly (Rich record) | MASM 6.13.7299 |
| Linker (`link.exe`) | 6.00.8447 |

The original has linker version 6.00. Its Rich header contains compiler
build 8447 (VC6 SP3: 93 C and 26 C++ objects), plus build 8168 (VC6 RTM:
117 C and 2 C++ objects). This suggests SP3 game modules linked with older
library objects; it does not identify the compiler of each function.

The Microsoft SP3 disc updates the frontends and linker while retaining
the RTM optimizer. The Rich compiler build is not the optimizer file version.

The header evidence does not establish an exact original linker build.
The versions above identify the selected build tools; their file hashes
are recorded in `config/toolchain.sha256`. The Rich IDs are decoded using
[richprint's compiler table](https://github.com/dishather/richprint/blob/master/comp_id.txt).

The DLL build uses the Windows headers and import libraries from VC6.
The exact SDK revision used for the original executable is not established.

The compiler installation comes from Microsoft Visual C++ 6.0
Professional (X04-58170) with the English updates from the Visual Studio
6.0 SP3 disc (X04-73545).

The release flags in `project.toml` use `/G5 /Zp1 /W3 /O2`: Pentium
scheduling, byte-packed structures and speed optimizations. Matching
additional functions may require adjustments to these flags.

## Setup

Clone with submodules, or run `git submodule update --init`.
Requirements: Python 3.11+, `just`, Wine, Ghidra and Java 21. Running the
game headlessly also needs Xvfb, xdotool and ImageMagick.

Place the reference checkouts at the `refs/` paths above. Directories or
local symlinks work; game files, references, `.wine/` and `build/` are
ignored by Git. Initialize the prefix with
`WINEPREFIX="$PWD/.wine" wineboot -i`, and supply the compiler's `bin`,
`include` and `lib` directories under `.wine/drive_c/msvc6sp3`.
Set `GHIDRA` if the installation is outside the search paths in `project.toml`.

## Workflow

| Command | Purpose |
| --- | --- |
| `just analyze` | Analyze the original and record function sizes |
| `just manifest` | Apply names and types from TRX's symbol manifest |
| `just decompile ADDRESS` | Export Ghidra pseudocode |
| `just fn NAME` | Look up a function in the reference source |
| `just compare [NAME]` | Build and check byte parity; optionally show a function diff |
| `just progress` | Rebuild the progress chart |
| `just game` | Prepare the game with the replacement DLL |
| `just run wait:10 shot:title` | Launch under Wine and capture a screenshot |

Write recovered functions in `src/`, following the original module layout.
Record function and global addresses in `config/symbols.csv`. The DLL's
hook table is generated from the functions defined in the build; other
symbols resolve to the original executable's addresses.

Use one newly recovered function per commit, with a subject such as
`Decompile NAME (0xADDRESS)`. The hooks keep byte parity and the
progress chart current on every commit; intentional differences are
listed in `hooks.allow_diff` in `project.toml`.

The build tools are a submodule of
[decomp-tools](https://github.com/LostArtefacts/decomp-tools), shared with
other Tomb Raider decompilations. `just` lists all available commands.
