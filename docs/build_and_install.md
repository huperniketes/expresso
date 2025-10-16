# Building, Installing and Packaging Expresso

This document describes how to build Expresso from source, install the headers and binaries, and produce distributable packages (.tar.gz, .zip, and macOS .pkg) using CMake and CPack.

Prerequisites
- CMake >= 3.31
- A C/C++ toolchain (clang or gcc)
- make or ninja
- On macOS: Xcode command line tools (for productbuild)

If you don't have CMake 3.31+ installed, use your package manager or download from https://cmake.org.

Quick build (out-of-source)

```bash
# from project root
mkdir -p build && cd build
cmake -DBUILD_TESTS=ON ..
cmake --build . -- -j$(nproc || sysctl -n hw.ncpu)
```

Notes:
- The project creates three main targets: `expresso_core` (static lib), `expresso_parser` (static lib wrapping ANTLR parser), and `expresso` (CLI executable).
- The ANTLR runtime is vendored in `vendor/antlr4`; CMake configures and builds it automatically.

Run tests

```bash
ctest --output-on-failure
```

Install to system prefix (local test install)

By default, CMake's install prefix is `/usr/local`. For local installs or packaging, you can set `CMAKE_INSTALL_PREFIX` when configuring.

```bash
# Install to /usr/local (may require sudo)
cd build
cmake --install . --prefix /usr/local

# Or install to a local directory (no sudo)
cmake --install . --prefix $HOME/.local
```

What gets installed
- `bin/expresso` — CLI executable
- `lib/libexpresso_core.a`, `lib/libexpresso_parser.a` — static libraries
- `include/` — public headers (project headers and parser headers)
- `lib/cmake/Expresso` — CMake package config files (ExpressoConfig.cmake, ExpressoTargets.cmake)

Using the installed package from another CMake project

In the consumer project's `CMakeLists.txt`:

```cmake
find_package(Expresso CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE Expresso::expresso_parser Expresso::expresso_core)
target_include_directories(myapp PRIVATE ${Expresso_INCLUDE_DIR})
```

Packaging with CPack

From the `build/` directory you can create packages using CPack. We configured CPack in the top-level CMakeLists to produce TGZ, ZIP and macOS productbuild packages.

- Create tar.gz (TGZ):

```bash
cd build
cpack -G TGZ
# produces expresso-<version>.tar.gz
```

- Create zip:

```bash
cpack -G ZIP
# produces expresso-<version>.zip
```

- Create macOS installer (.pkg):

```bash
cpack -G productbuild
# produces expresso-<version>.pkg
```

Notes about packaging
- The ZIP/TGZ include the installed `bin/`, `lib/`, and `include/` directories.
- The macOS `.pkg` is a simple ProductBuild package created by CPack and may be un-signed. For distribution via the App Store or Gatekeeper you should sign the package with your Apple Developer credentials.
- If you want additional files in the package (LICENSE, README, man pages), add them via `install(FILES ...)` in the CMakeLists or adjust the CPACK settings.

Advanced options

- Change the install prefix used by the package by setting `CMAKE_INSTALL_PREFIX` at configure time. Example for packaging into `/opt/expresso`:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/expresso -DBUILD_TESTS=ON ..
cpack -G TGZ
```

- Create a distribution that includes a CMake package config so consumers can `find_package(Expresso)` as described earlier — we already generate `ExpressoConfig.cmake` and `ExpressoTargets.cmake` during install.

Troubleshooting

- If CPack reports missing symbols from third-party static libs (ranlib warnings), this is usually harmless for packaging, but you may want to ensure your toolchain's ranlib/strip configuration is correct.
- On macOS, creating `.pkg` installers requires `productbuild` which is installed with Xcode command-line tools. If `productbuild` is missing, install Xcode CLI tools via `xcode-select --install`.

Summary

This repository includes CPack configuration and install rules. Use the steps above to build, test, install locally, and create distributable packages for users.
