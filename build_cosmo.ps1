# UEX — Script de build cosmocc (PowerShell)
# Lance la compilation multi-plateforme avec cosmocc

$COSMOCC_DIR = "C:\Users\djibi\.gemini\antigravity\scratch\cosmocc\bin"
$UEX_DIR     = "C:\Users\djibi\.gemini\antigravity\scratch\uex"
$env:PATH    = "$COSMOCC_DIR;$env:PATH"

$CC     = "cosmocc"
$CFLAGS = @("-g", "-Os", "-Wall", "-Iinclude")

function Compile-Demo {
    param($name, $srcs)
    $outExe = "examples\$name.com"
    Write-Host "  Compiling $name..."
    $args = $CFLAGS + $srcs + @("-o", $outExe)
    & $CC @args 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  [OK] $outExe"
    } else {
        Write-Host "  [FAIL] $name"
    }
}

Set-Location $UEX_DIR

Write-Host "=== Building UEX demos with cosmocc ==="

Compile-Demo "demo_manifest" @("examples/demo_manifest.c", "src/header/uex_manifest.c")
Compile-Demo "demo_adaptive_boot" @("examples/demo_adaptive_boot.c")
Compile-Demo "demo_ai_streaming" @(
    "examples/demo_ai_streaming.c",
    "src/loader/uex_vfs_loader.c",
    "src/loader/uex_ai_loader.c",
    "src/loader/uex_plugin.c"
)

Write-Host ""
Write-Host "=== Files generated ==="
Get-ChildItem examples\*.com | ForEach-Object {
    $size = [math]::Round($_.Length / 1KB)
    Write-Host "  $($_.Name)  [${size} KB]"
}
Write-Host ""
Write-Host "=== Running demo_manifest.com ==="
.\examples\demo_manifest.com
