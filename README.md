> [한국어](README_KR.md) | [English](README.md)
>
> This project was implemented as a hands-on experiment in agentic coding.  
> For more context, see [this post](https://www.facebook.com/share/p/1itHsq4pjY/).

# HyperDOS

HyperDOS is a C11 IBM PC compatible emulator project focused on real-mode DOS and
Windows 3.0 behavior. The current target is to make English Windows 3.0 real mode
run correctly on a configurable 16-bit x86 PC model while keeping the emulator
core portable and separate from platform-specific frontends.

The codebase is intentionally split into a pure core layer and host platform
layers. The core code should depend only on the C runtime, while Win32 windowing,
input, menus, and rendering live under `platforms/win32`.

## Current Scope

- 16-bit x86 real-mode CPU core with 8086 and 80186-oriented execution paths.
- IBM PC XT and AT style machine modeling.
- BIOS services for boot, disk, keyboard, timer, video, and PC data areas.
- Core device models including PIC, PIT, keyboard controller, floppy, storage,
  and VGA/CGA-facing video state.
- A Win32 PC monitor frontend for interactive booting, disk insertion, keyboard
  input, rendering, and debug dumps.

This is still an emulator under active development, not a finished compatibility
product. Accuracy and standard behavior take priority over quick application
specific hacks.

## Repository Layout

```text
include/hyperdos/      Public core headers
src/                   Emulator core, BIOS, devices, firmware, PC machine logic
platforms/win32/       Win32 monitor frontend
tests/                 Emulator regression tests
hyperdos.vcxproj       Visual Studio project for the Win32 monitor
CMakeLists.txt         Main CMake build
```

## Building With CMake

On Windows with Visual Studio Build Tools and Ninja:

```bat
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
ctest --test-dir cmake-build-debug --output-on-failure
```

The Win32 monitor executable is produced as:

```text
cmake-build-debug\hyperdos_win32_pc_monitor.exe
```

## Creating a Blank Hard Disk Image

HyperDOS uses raw disk image files. On Windows, you can create a 32 MiB blank hard disk image with:

```bat
fsutil file createnew images\harddisk.img 33554432
```

Then boot with:

```bat
cmake-build-debug\hyperdos_win32_pc_monitor.exe --hard-disk images\harddisk.img --boot-hard-disk
```

The image is blank. You still need to partition and format it from DOS, for example with `FDISK` and
`FORMAT C: /S`.

## Building With Visual Studio

Open [hyperdos.vcxproj](hyperdos.vcxproj) in Visual Studio 2022 or later and build
the `Debug|x64` configuration.

The executable is produced as:

```text
vs-build\x64\Debug\hyperdos.exe
```

## Notes

Disk images, operating systems, and application software are not part of the
emulator core. Use only images that you are licensed to use.

The project is currently optimized for development, debugging, and compatibility
work. Command-line tracing and dump options are available in the Win32 monitor
for investigating CPU, disk, memory, text screen, and video state behavior.
