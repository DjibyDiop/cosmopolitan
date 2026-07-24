# UEX Binary Format Specification v1

The Universal Executable Extension (UEX) is a binary container format that encapsulates a standard Actually Portable Executable (APE) header, followed by a series of modular chunks.

## High-Level Structure

```
+-----------------------------------+
| APE Compatibility Stub (Header)   | 0x00000000
+-----------------------------------+
| UEX Magic Signature ('UEX\1')     |
| UEX Container Header              |
+-----------------------------------+
| Chunk 1: Manifest                 |
+-----------------------------------+
| Chunk 2: x86_64 Payload           |
+-----------------------------------+
| Chunk 3: aarch64 Payload          |
+-----------------------------------+
| Chunk N: Modules / Resources      |
+-----------------------------------+
```

## 1. UEX Container Header
Immediately following the APE shell script/DOS stub, the UEX format is identified by the magic signature `0x55 0x45 0x58 0x01` (`UEX\1`).

```c
struct uex_header {
    uint32_t magic;         // "UEX\1"
    uint32_t version;       // 1
    uint64_t total_size;    // Total size of the UEX file
    uint32_t chunk_count;   // Number of chunks
    uint32_t flags;         // Global flags
};
```

## 2. Chunk Architecture
A UEX container consists of sequential chunks. Each chunk has a standard header.

```c
struct uex_chunk_header {
    uint32_t type;          // e.g., 'MANI', 'X86_', 'AAR6', 'WASM', 'VFS_'
    uint32_t flags;         // Compression, encryption, etc.
    uint64_t size;          // Size of the chunk payload
    uint64_t decomp_size;   // Size of payload when decompressed
    uint32_t crc32;         // Integrity check
    uint8_t  payload[];     // The data
};
```

### Chunk Types
- `MANI`: The capability manifest (JSON or binary encoded).
- `X86_`: AMD64 (x86_64) native executable payload (ELF/PE/Mach-O).
- `AAR6`: ARM64 (aarch64) native executable payload.
- `RISC`: RISC-V 64-bit native executable payload.
- `WASM`: WebAssembly fallback payload.
- `VFS_`: Virtual File System (e.g., zip) containing assets, shared libraries, or models.
