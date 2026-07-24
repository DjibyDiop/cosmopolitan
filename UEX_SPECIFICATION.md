# UEX - Universal Evolutive eXecutable
**Specification Version**: 1.0
**Author**: Djiby Diop & AI Assistant
**Status**: RFC (Request for Comments)

## 1. Abstract
The Universal Evolutive eXecutable (UEX) is a modern software distribution format built as a semantic and secure superset of the Actually Portable Executable (APE) format (pioneered by Cosmopolitan Libc).
While APE achieves absolute multi-platform portability (Write Once, Run Anywhere) at the machine level, UEX transforms the binary into an "Intelligent Container". It allows executables to carry self-validating metadata, cryptographic security, WebAssembly plugins, and lazy-loaded network assets, while maintaining Zero-Copy memory alignment.

## 2. Architecture & Symbiosis
UEX leverages Cosmopolitan's internal PKZIP Virtual File System (`/zip/`).
Instead of designing a custom appended binary blob, a UEX container is a valid Cosmopolitan APE executable with a valid PKZIP structure appended to it.

All UEX-specific chunks are stored inside the PKZIP directory under the `.uex/` virtual folder.
This enables immediate symbiosis with existing PKZIP tooling and Cosmopolitan's internal `open()` and `mmap()` syscall hooks.

## 3. Chunk Format
A "Chunk" in UEX is a distinct capability or payload.
Every file located in the `/zip/.uex/` directory is considered a Chunk.

### 3.1 Naming Convention
A chunk filename consists of a 4-character type identifier and an extension:
- `TYPE.bin`: Represents a direct binary payload embedded in the executable.
- `TYPE.lnk`: Represents a Lazy-Loaded Link. The content of this file is a URL or URI where the payload can be downloaded by the application at runtime.

### 3.2 Security (Zero-Trust)
Every chunk (both `.bin` and `.lnk`) must be cryptographically signed.
The first **32 bytes** of the file content inside the ZIP represent the HMAC-SHA256 signature of the remaining payload.

**Structure of a UEX File inside the ZIP:**
```
[ 32 Bytes: HMAC-SHA256 Signature ] [ N Bytes: Raw Payload Data ]
```
The UEX runtime MUST validate this signature using a pre-shared key or an Ed25519 Public Key before exposing the payload to the application.

## 4. Standard Chunk Types

### 4.1 `MANI` (Manifest)
- **Format**: JSON
- **Purpose**: Defines the semantic requirements of the executable (CPU features, required RAM, AI capabilities, GUI vs Server mode).
- **Example**: `.uex/MANI.bin`

### 4.2 `PLUG` (Plugin)
- **Format**: WebAssembly (WASM)
- **Purpose**: Extends the functionality of the host binary. Plugins MUST be compiled to `.wasm` to maintain the universal portability of the host APE binary. The host executes the plugin inside a WASM sandbox.
- **Example**: `.uex/PLUG.bin`

### 4.3 `AIMD` (AI Model)
- **Format**: GGUF / SafeTensors
- **Purpose**: Large Language Models or specialized neural networks. Since these models can exceed several gigabytes, they are typically lazy-loaded.
- **Example**: `.uex/AIMD.lnk` (Contains `https://huggingface.co/...`)

## 5. Runtime API (C/C++)
The UEX runtime library (`uex.c`) abstracts the underlying VFS and cryptography.
Applications only need to invoke:
- `uex_init()`: Mounts the UEX subsystem.
- `uex_get_chunk("TYPE", &chunk)`: Returns a zero-copy pointer to the decrypted and verified memory-mapped payload.

## 6. Philosophy
UEX is not a replacement for APE. It is a new category: the **Intelligent Executable**.
It answers the question: *"How do we distribute an entire adaptive ecosystem (AI models, dynamic plugins, configuration) in a single portable file, securely?"*
