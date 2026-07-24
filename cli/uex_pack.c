#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../include/crypto.h"

// Hardcoded secret key for PoC. In production, this would be an Ed25519 Public Key validation.
static const uint8_t UEX_SECRET_KEY[] = "SuperSecretUEXKey2026";

// PKZIP Signatures
#define ZIP_LOCAL_HEADER_SIG 0x04034b50
#define ZIP_CENTRAL_HEADER_SIG 0x02014b50
#define ZIP_EOCD_SIG 0x06054b50

#pragma pack(push, 1)
typedef struct {
    uint32_t signature;
    uint16_t version_needed;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_len;
} zip_local_header_t;

typedef struct {
    uint32_t signature;
    uint16_t version_made_by;
    uint16_t version_needed;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_len;
    uint16_t comment_len;
    uint16_t disk_start;
    uint16_t internal_attr;
    uint32_t external_attr;
    uint32_t local_header_offset;
} zip_central_header_t;

typedef struct {
    uint32_t signature;
    uint16_t disk_num;
    uint16_t disk_cd_start;
    uint16_t cd_records_disk;
    uint16_t cd_records_total;
    uint32_t cd_size;
    uint32_t cd_offset;
    uint16_t comment_len;
} zip_eocd_t;
#pragma pack(pop)

// Standard CRC32 for ZIP (not for security, just for format compliance)
static uint32_t crc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}

typedef struct {
    char type[5]; // 4 chars + null
    const char* filepath;
    bool is_link;
} cli_chunk_t;

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: uex_pack <input.com> -o <output.com> [chunks...]\n");
        printf("Chunks:\n");
        printf("  --manifest <file.json>\n");
        printf("  --plugin <file.wasm>\n");
        printf("  --ai <model.gguf>\n");
        printf("  --link <type> <url>\n");
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = NULL;
    
    cli_chunk_t chunks[64];
    int chunk_count = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "--manifest") == 0 && i + 1 < argc) {
            strcpy(chunks[chunk_count].type, "MANI");
            chunks[chunk_count].filepath = argv[++i];
            chunks[chunk_count].is_link = false;
            chunk_count++;
        } else if (strcmp(argv[i], "--plugin") == 0 && i + 1 < argc) {
            strcpy(chunks[chunk_count].type, "PLUG");
            chunks[chunk_count].filepath = argv[++i];
            chunks[chunk_count].is_link = false;
            chunk_count++;
        } else if (strcmp(argv[i], "--ai") == 0 && i + 1 < argc) {
            strcpy(chunks[chunk_count].type, "AIMD");
            chunks[chunk_count].filepath = argv[++i];
            chunks[chunk_count].is_link = false;
            chunk_count++;
        } else if (strcmp(argv[i], "--link") == 0 && i + 2 < argc) {
            // --link AIMD https://...
            strncpy(chunks[chunk_count].type, argv[++i], 4);
            chunks[chunk_count].type[4] = '\0';
            chunks[chunk_count].filepath = argv[++i];
            chunks[chunk_count].is_link = true;
            chunk_count++;
        }
    }

    if (!output_file) {
        printf("Error: Output file (-o) is required.\n");
        return 1;
    }

    FILE* fin = fopen(input_file, "rb");
    if (!fin) {
        printf("Error: Cannot open input %s\n", input_file);
        return 1;
    }

    FILE* fout = fopen(output_file, "wb");
    if (!fout) {
        printf("Error: Cannot open output %s\n", output_file);
        fclose(fin);
        return 1;
    }

    // 1. Copy APE executable
    char buffer[8192];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        fwrite(buffer, 1, n, fout);
    }
    fclose(fin);

    // ZIP state tracking
    uint32_t zip_start_offset = ftell(fout);
    
    struct cd_record {
        zip_central_header_t hdr;
        char filename[256];
    } cd_records[64];

    // 2. Write Local Files
    for (int i = 0; i < chunk_count; i++) {
        uint8_t* raw_data = NULL;
        uint64_t raw_length = 0;
        
        if (chunks[i].is_link) {
            // The data is just the URL string for LINK
            raw_length = strlen(chunks[i].filepath);
            raw_data = malloc(raw_length);
            memcpy(raw_data, chunks[i].filepath, raw_length);
        } else {
            FILE* fchunk = fopen(chunks[i].filepath, "rb");
            if (!fchunk) {
                printf("Error: Cannot open chunk file %s\n", chunks[i].filepath);
                return 1;
            }
            fseek(fchunk, 0, SEEK_END);
            raw_length = ftell(fchunk);
            fseek(fchunk, 0, SEEK_SET);
            raw_data = malloc(raw_length);
            if (raw_length > 0) fread(raw_data, 1, raw_length, fchunk);
            fclose(fchunk);
        }

        // Generate HMAC-SHA256 signature
        uint8_t signature[32];
        hmac_sha256(UEX_SECRET_KEY, strlen((const char*)UEX_SECRET_KEY), raw_data, raw_length, signature);

        // The file in ZIP will contain: [32-byte HMAC] + [Raw Data]
        uint32_t file_total_size = 32 + raw_length;
        uint8_t* zip_file_data = malloc(file_total_size);
        memcpy(zip_file_data, signature, 32);
        memcpy(zip_file_data + 32, raw_data, raw_length);
        
        uint32_t zip_crc = crc32(zip_file_data, file_total_size);

        char filename[256];
        if (chunks[i].is_link) {
            snprintf(filename, sizeof(filename), ".uex/%s.lnk", chunks[i].type);
        } else {
            snprintf(filename, sizeof(filename), ".uex/%s.bin", chunks[i].type);
        }

        uint32_t current_offset = ftell(fout); // Use absolute offset for Cosmopolitan

        // Write Local Header
        zip_local_header_t lh = {0};
        lh.signature = ZIP_LOCAL_HEADER_SIG;
        lh.version_needed = 10;
        lh.filename_len = strlen(filename);
        lh.compressed_size = file_total_size;
        lh.uncompressed_size = file_total_size;
        lh.crc32 = zip_crc;

        fwrite(&lh, sizeof(lh), 1, fout);
        fwrite(filename, 1, lh.filename_len, fout);
        fwrite(zip_file_data, 1, file_total_size, fout);

        // Save Central Directory Info
        cd_records[i].hdr.signature = ZIP_CENTRAL_HEADER_SIG;
        cd_records[i].hdr.version_made_by = 20;
        cd_records[i].hdr.version_needed = 10;
        cd_records[i].hdr.filename_len = lh.filename_len;
        cd_records[i].hdr.compressed_size = lh.compressed_size;
        cd_records[i].hdr.uncompressed_size = lh.uncompressed_size;
        cd_records[i].hdr.crc32 = lh.crc32;
        cd_records[i].hdr.local_header_offset = current_offset;
        strcpy(cd_records[i].filename, filename);

        free(raw_data);
        free(zip_file_data);

        printf("Packed UEX chunk %s (%u bytes + 32b HMAC) via ZIP\n", filename, (uint32_t)raw_length);
    }

    // 3. Write Central Directory
    uint32_t cd_offset = ftell(fout); // Use absolute offset
    for (int i = 0; i < chunk_count; i++) {
        fwrite(&cd_records[i].hdr, sizeof(zip_central_header_t), 1, fout);
        fwrite(cd_records[i].filename, 1, cd_records[i].hdr.filename_len, fout);
    }
    uint32_t cd_size = ftell(fout) - cd_offset;

    // 4. Write EOCD
    zip_eocd_t eocd = {0};
    eocd.signature = ZIP_EOCD_SIG;
    eocd.cd_records_disk = chunk_count;
    eocd.cd_records_total = chunk_count;
    eocd.cd_size = cd_size;
    eocd.cd_offset = cd_offset;
    
    // UEX Magic as a ZIP comment! This makes the ZIP valid but still marks it as UEX
    const char* uex_comment = "UEX\x01";
    eocd.comment_len = 4;

    fwrite(&eocd, sizeof(eocd), 1, fout);
    fwrite(uex_comment, 1, 4, fout);

    fclose(fout);
    chmod(output_file, 0755);

    printf("Successfully packed UEX v2 Container (ZIP Symbiosis)!\n");
    return 0;
}
