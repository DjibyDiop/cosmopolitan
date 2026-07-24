#!/bin/bash
set -e

COSMOCC=/home/djiby/cosmocc
BIN=$COSMOCC/bin
UEX=/mnt/c/Users/djibi/.gemini/antigravity/scratch/uex
OUT=$UEX/examples

# Crucial: tell gcc where to find cc1, ld, etc.
export GCC_EXEC_PREFIX=$COSMOCC/libexec/gcc/
export PATH=$BIN:$PATH

echo '========================================='
echo ' Building UEX with cosmocc (APE format)'
echo ' cosmocc: '$BIN/cosmocc
echo '========================================='
$BIN/cosmocc --version
echo ''

do_build() {
  local name=$1; shift
  printf '[%-24s] ' "$name"
  if $BIN/cosmocc -Os -Wall -I$UEX/include -o $OUT/$name.com "$@" 2>/tmp/uex_err; then
    echo "OK"
  else
    echo 'FAILED:'
    cat /tmp/uex_err
    exit 1
  fi
}

do_build demo_manifest      $UEX/examples/demo_manifest.c     $UEX/src/header/uex_manifest.c
do_build demo_adaptive_boot $UEX/examples/demo_adaptive_boot.c
do_build demo_ai_streaming  $UEX/examples/demo_ai_streaming.c \
         $UEX/src/loader/uex_vfs_loader.c \
         $UEX/src/loader/uex_ai_loader.c  \
         $UEX/src/loader/uex_plugin.c

echo ''
echo '=== APE Binaries generated ==='
ls -lh $OUT/*.com

echo ''
echo '=== TEST 1: Manifest Parsing ==='
$OUT/demo_manifest.com

echo ''
echo '=== TEST 2: Adaptive Boot (GUI) ==='
$OUT/demo_adaptive_boot.com

echo ''
echo '=== TEST 2b: Adaptive Boot (SERVER) ==='
UEX_FORCE_MODE=server $OUT/demo_adaptive_boot.com

echo ''
echo '=== TEST 3: AI Model + Plugin ==='
$OUT/demo_ai_streaming.com

echo ''
echo '=== APE binary inspection ==='
file $OUT/demo_manifest.com

echo ''
echo 'ALL UEX APE TESTS PASSED!'
