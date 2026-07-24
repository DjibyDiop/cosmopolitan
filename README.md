# UEX (Universal Executable) Format

UEX is an evolutionary step beyond the Actually Portable Executable (APE). It defines a next-generation adaptive binary container format built on the foundation of Cosmopolitan Libc.

## The Vision

Where APE answers the question: *"How to make a portable executable?"*
UEX answers the question: *"How to make an executable that can evolve, adapt, and transport rich modular capabilities?"*

```text
ELF ──► PE ──► Mach-O ──► APE ──► UEX
```

UEX is designed to contain:
- Multiple CPU Architectures (x86_64, aarch64, riscv64)
- Multiple execution environments (Desktop, Server, Bare-metal, WASM)
- Embedded AI models and VFS (Virtual File System)
- A capabilities manifest for intelligent runtime loading

## Features

- **Polyglot Header:** Backwards-compatible with APE, providing execution on Windows, Linux, macOS, FreeBSD, NetBSD, OpenBSD, and BIOS.
- **Selective Memory Loader:** The loader selectively decompresses and maps only the target architecture and necessary assets directly into memory (zero-disk footprint).
- **Capability Manifest:** A JSON/binary descriptor that defines CPU bounds, memory limits, and permissions (network, GPU, AI, sandbox).
- **Extensible Sections:** A container structure for assets, AI models, and dynamic extensions.

## Architecture

1. **UEX Polyglot Header**: The universal entry point.
2. **UEX Capability Manifest**: Metadata guiding the loader.
3. **Universal Adaptive Loader**: Maps the correct payload into execution memory.
4. **Native Payloads**: The actual compiled binaries.
5. **Modular Extensions**: In-memory VFS containing `.so`/`.dll`, assets, and models.

## Usage

(To be implemented via `uex` CLI tool)

```bash
uex pack --manifest spec/uex_manifest_v1.json program.uex
uex inspect program.uex
```
