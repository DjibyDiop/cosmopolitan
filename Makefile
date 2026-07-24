# UEX Build System
# Supports: cosmocc (primary), gcc (fallback for testing)

# Auto-detect compiler: prefer cosmocc, fallback to gcc
COSMOCC := $(shell where cosmocc 2>NUL)
ifdef COSMOCC
  CC = cosmocc
  EXT = .com
else
  CC = gcc
  EXT = .exe
endif

CFLAGS  = -g -Os -Wall -Wextra -Iinclude
LDFLAGS =

# ──────────────────────────────────────────────
# CLI tool
# ──────────────────────────────────────────────
TARGET   = uex
SRCS_CLI = src/cli/main.c src/cli/inspector.c src/cli/packer.c \
           src/header/uex_manifest.c \
           src/loader/uex_vfs_loader.c \
           src/loader/uex_ai_loader.c \
           src/loader/uex_plugin.c
OBJS_CLI = $(SRCS_CLI:.c=.o)

# ──────────────────────────────────────────────
# Demo examples
# ──────────────────────────────────────────────
DEMO_SRCS = examples/demo_manifest.c \
            examples/demo_adaptive_boot.c \
            examples/demo_ai_streaming.c

DEMO_BINS = $(DEMO_SRCS:.c=$(EXT))

DEMO_LIBS_MANIFEST  = src/header/uex_manifest.c
DEMO_LIBS_ADAPTIVE  =
DEMO_LIBS_AI        = src/loader/uex_vfs_loader.c \
                      src/loader/uex_ai_loader.c \
                      src/loader/uex_plugin.c

# ──────────────────────────────────────────────
# Targets
# ──────────────────────────────────────────────
all: $(TARGET)$(EXT) demos

$(TARGET)$(EXT): $(OBJS_CLI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build each demo with its dependencies
examples/demo_manifest$(EXT): examples/demo_manifest.c $(DEMO_LIBS_MANIFEST)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

examples/demo_adaptive_boot$(EXT): examples/demo_adaptive_boot.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

examples/demo_ai_streaming$(EXT): examples/demo_ai_streaming.c $(DEMO_LIBS_AI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

demos: examples/demo_manifest$(EXT) examples/demo_adaptive_boot$(EXT) examples/demo_ai_streaming$(EXT)

# Run all demos sequentially
test: demos
	@echo "========================================="
	@echo " TEST 1: Manifest Parsing"
	@echo "========================================="
	examples/demo_manifest$(EXT)
	@echo ""
	@echo "========================================="
	@echo " TEST 2: Adaptive Boot (GUI mode)"
	@echo "========================================="
	examples/demo_adaptive_boot$(EXT)
	@echo ""
	@echo "========================================="
	@echo " TEST 2b: Adaptive Boot (SERVER mode)"
	@echo "========================================="
	UEX_FORCE_MODE=server examples/demo_adaptive_boot$(EXT)
	@echo ""
	@echo "========================================="
	@echo " TEST 3: AI Model + Plugin Streaming"
	@echo "========================================="
	examples/demo_ai_streaming$(EXT)
	@echo ""
	@echo "[OK] All UEX tests passed!"

clean:
	@if exist $(TARGET)$(EXT) del /Q $(TARGET)$(EXT)
	@for %%f in ($(OBJS_CLI)) do @if exist %%f del /Q %%f
	@if exist examples\demo_manifest$(EXT) del /Q examples\demo_manifest$(EXT)
	@if exist examples\demo_adaptive_boot$(EXT) del /Q examples\demo_adaptive_boot$(EXT)
	@if exist examples\demo_ai_streaming$(EXT) del /Q examples\demo_ai_streaming$(EXT)

.PHONY: all demos test clean
