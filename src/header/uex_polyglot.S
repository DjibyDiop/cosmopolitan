/*
 * uex_polyglot.S
 *
 * The Universal Executable Extension Polyglot Header
 * This acts as the bridge between standard APE and UEX.
 *
 * It contains the shell variables for POSIX fallback and DOS/PE stub.
 */

.text
.globl _start

_start:
    /* 
     * UEX Polyglot Magic 
     * Valid x86 instructions, valid PE stub, valid Shell script.
     */
    .ascii "MZqFpD='"
    .byte '\n'
    
    /* 
     * Shell script payload block for WASM / ELF fallback extraction.
     * This section is ignored by Windows PE loader.
     */
    .ascii "UEX_MAGIC=\"UEX\\1\"\n"
    .ascii "exec uex_loader \"$0\" \"$@\"\n"
    .ascii "exit 1\n"
    .ascii "'\n"

    /* Alignment and PE Stub transition */
    .align 16
uex_header_start:
    /* "UEX\1" */
    .byte 0x55, 0x45, 0x58, 0x01
    
    /* version (1) */
    .long 1
    
    /* total_size (placeholder, linker populated) */
    .quad 0x0
    
    /* chunk_count (placeholder, linker populated) */
    .long 0x0
    
    /* flags */
    .long 0x0
    
uex_manifest_chunk:
    /* "MANI" chunk header placeholder */
    .byte 0x4d, 0x41, 0x4e, 0x49
    .long 0
    .quad 0
    .quad 0
    .long 0
