#!/bin/bash
echo -1 > /proc/sys/fs/binfmt_misc/WSLInterop || true

rm -rf /home/djiby/uex
cp -r /mnt/c/Users/djibi/.gemini/antigravity/scratch/uex /home/djiby/uex
chown -R djiby:djiby /home/djiby/uex

sudo -u djiby bash << 'EOF'
set -e
export PATH=/home/djiby/cosmocc/bin:$PATH
export GCC_EXEC_PREFIX=/home/djiby/cosmocc/libexec/gcc/
cd /home/djiby/uex

echo '========================================='
echo ' Compiling UEX v2 Packager (CLI) & Test App'
echo '========================================='
# Compile the CLI packager with Crypto
cosmocc -Os -Wall -I./include -o ./cli/uex_pack.com ./cli/uex_pack.c ./src/crypto.c

# Compile the Test App (links with the production uex API and Crypto)
cosmocc -Os -Wall -I./include -o ./examples/test_production.com ./examples/test_production.c ./src/uex.c ./src/crypto.c

echo ''
echo '========================================='
echo ' Packaging UEX v2 Binary with uex_pack'
echo '========================================='
./cli/uex_pack.com ./examples/test_production.com -o ./examples/test_production_uex.com \
    --manifest ./examples/test_manifest.json \
    --plugin ./examples/test_plugin.bin \
    --link AIMD "https://huggingface.co/models/llama-3-8b.gguf"

echo ''
echo '========================================='
echo ' Running UEX v2 Test App'
echo '========================================='
./examples/test_production_uex.com

echo ''
echo '========================================='
echo ' Copying generated binaries to Windows'
echo '========================================='
cp ./cli/*.com /mnt/c/Users/djibi/.gemini/antigravity/scratch/uex/cli/
cp ./examples/*_uex.com /mnt/c/Users/djibi/.gemini/antigravity/scratch/uex/examples/
echo '✅ UEX v2 INTEGRATION TESTS PASSED SUCCESSFULLY!'
EOF
