CMake wrapper for legacy linux-0.11 build
======================================

This repository contains a lightweight `CMakeLists.txt` that wraps the
historic top-level `Makefile`. It is intended to let you use CMake as a
convenient front-end while migrating the project incrementally to
native CMake targets.

Quick usage
-----------

- Configure (out-of-source recommended):

```powershell
mkdir build
cd build
cmake ..
```

- Build the default legacy target (delegates to the original Makefile):

```powershell
cmake --build . --target legacy-all
```

- Build the disk image (equivalent to `make Image`):

```powershell
cmake --build . --target legacy-image
```

Generate compile_commands.json
------------------------------
To run clang-tidy or use language servers you may want `compile_commands.json`.
If you run CMake configure step as above, CMake will produce this file when
using a native build (not all legacy Makefile invocations will populate it).

If you rely on the legacy Makefile for compilation, use `bear` or `intercept-cc`
to capture compiler commands while running `cmake --build . --target legacy-all`.

Next steps
----------
- Option A: Incrementally convert subdirectories (`init`, `kernel`, `fs`, `mm`, `lib`, `tools`) to native CMake targets.
- Option B: Use the wrapper for tooling (clang-tidy, scan-build) while keeping the Makefile as the canonical build system.

If you want, I can:
- Start converting one subdirectory (pick `init` or `lib`) into native CMake targets and tests.
- Or add a GitHub Action to run clang-tidy using the generated `compile_commands.json`.
