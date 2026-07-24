#!/bin/bash
set -e

export PATH=/home/djiby/cosmocc/bin:$PATH
export GCC_EXEC_PREFIX=/home/djiby/cosmocc/libexec/gcc/

echo '=== Compiling in Linux Ext4 ==='
cosmocc -Os -Wall -I./include -o ./examples/demo_manifest.com ./examples/demo_manifest.c ./src/header/uex_manifest.c
cosmocc -Os -Wall -I./include -o ./examples/demo_adaptive_boot.com ./examples/demo_adaptive_boot.c
cosmocc -Os -Wall -I./include -o ./examples/demo_ai_streaming.com ./examples/demo_ai_streaming.c ./src/loader/uex_vfs_loader.c ./src/loader/uex_ai_loader.c ./src/loader/uex_plugin.c

echo '=== Tests in Linux Ext4 ==='
./examples/demo_manifest.com
./examples/demo_adaptive_boot.com
UEX_FORCE_MODE=server ./examples/demo_adaptive_boot.com
./examples/demo_ai_streaming.com

echo '=== Copying back to Windows ==='
cp ./examples/*.com /mnt/c/Users/djibi/.gemini/antigravity/scratch/uex/examples/
echo '✅ Build & Tests OK'
