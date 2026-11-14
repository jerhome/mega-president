# Mega Président — commandes de développement (à lancer depuis WSL2)
#
#   make            compile la ROM (out/rom.bin) via Docker + SGDK
#   make run        compile puis lance BlastEm en PAL (Europe, 50 Hz)
#   make run-ntsc   compile puis lance BlastEm en NTSC (USA, 60 Hz)
#   make debug      ROM de debug (symboles, -O1)
#   make test       tests unitaires du cœur de simulation, sur PC (gcc natif)
#   make check      tests + ROM + budgets mémoire
#   make clean      supprime out/

SGDK_IMAGE ?= ghcr.io/stephane-d/sgdk:latest
BLASTEM_VER := 0.6.2
BLASTEM_DIR := tools/blastem64-$(BLASTEM_VER)
BLASTEM     := $(BLASTEM_DIR)/blastem
ROM         := out/rom.bin

DOCKER_SGDK = docker run --rm -u $(shell id -u):$(shell id -g) -e HOME=/tmp \
              -v "$(CURDIR)":/src $(SGDK_IMAGE)

HOST_CC     ?= gcc
HOST_CFLAGS := -std=c99 -Wall -Wextra -Werror -O1 -Isrc
SIM_SRC     := $(wildcard src/sim/*.c)
TEST_SRC    := $(wildcard tests/*.c)
TEST_BIN    := out/host/tests

.PHONY: all rom debug run run-ntsc test check clean blastem

all: rom

rom:
	$(DOCKER_SGDK) release

debug:
	$(DOCKER_SGDK) debug

run: rom blastem
	pkill -x blastem || true
	SDL_VIDEODRIVER=x11 $(BLASTEM) -r E $(ROM) >/dev/null 2>&1 &

run-ntsc: rom blastem
	pkill -x blastem || true
	SDL_VIDEODRIVER=x11 $(BLASTEM) -r U $(ROM) >/dev/null 2>&1 &

blastem: $(BLASTEM)

$(BLASTEM):
	mkdir -p tools
	curl -sSfL https://www.retrodev.com/blastem/blastem64-$(BLASTEM_VER).tar.gz | tar xz -C tools
	# WSLg ne gère pas le réglage vsync d'OpenGL : rendu SDL sans OpenGL
	sed -i 's/^\tgl on/\tgl off/' $(BLASTEM_DIR)/default.cfg

$(TEST_BIN): $(SIM_SRC) $(TEST_SRC) $(wildcard src/sim/*.h) $(wildcard tests/*.h)
	mkdir -p $(dir $@)
	$(HOST_CC) $(HOST_CFLAGS) $(SIM_SRC) $(TEST_SRC) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

check: test rom
	./tools/budget.sh

clean:
	rm -rf out
