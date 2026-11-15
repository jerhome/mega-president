# Mega Président — commandes de développement (à lancer depuis WSL2)
#
#   make            compile la ROM (out/rom.bin) via Docker + SGDK
#   make run        compile puis lance BlastEm en PAL (Europe, 50 Hz)
#   make run-ntsc   compile puis lance BlastEm en NTSC (USA, 60 Hz)
#   make debug      ROM de debug (symboles, -O1)
#   make rom-annee4 ROM de test : rapport de fin d'année 3 (Sénat)
#   make test       tests unitaires du cœur de simulation, sur PC (gcc natif)
#   make content    régénère le code C depuis docs/contenu/*.md
#   make gfx        régénère la police et les images d'interface (res/gfx/)
#   make art        régénère les illustrations (pixel art, tools/art/)
#   make music      régénère la musique FM (tools/music/ -> res/music/)
#   make map        régénère la carte des régions (tools/map/)
#   make check      tests + ROM + budgets mémoire + contrôle des 38 colonnes (avertissements)
#   make shots      captures des ROM de démo (SHOTS="-t '2 4' 11 12"), à comparer avec tools/shots_compare.py
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
SIM_SRC     := $(wildcard src/sim/*.c) $(wildcard src/content/*.c) src/save/scores.c src/save/gamesave.c src/save/settings.c $(wildcard src/util/*.c)
TEST_SRC    := $(wildcard tests/*.c)
TEST_BIN    := out/host/tests

.PHONY: shots music all rom rom-demo rom-annee4 debug run run-ntsc test check clean blastem content gfx art map

all: rom

# Contenu (lois, événements, univers) : docs/contenu/*.md -> src/sim + src/content
CONTENT_MD  := $(wildcard docs/contenu/*.md)
CONTENT_GEN := src/sim/events_ids.h src/sim/events_data.c src/content/content_events.c \
               src/content/content_texts.c src/content/content_texts.h

$(CONTENT_GEN) &: $(CONTENT_MD) tools/content/gen_content.py src/sim/effects.h src/sim/laws.h src/sim/election.h src/sim/ids_v1.h
	python3 tools/content/gen_content.py

content: $(CONTENT_GEN)

gfx:
	python3 tools/font/make_gfx.py

art:
	python3 tools/art/make_art.py

map:
	python3 tools/map/make_map.py

music:
	python3 tools/music/make_music.py

rom: $(CONTENT_GEN)
	$(DOCKER_SGDK) release EXTRA_FLAGS=-fstack-protector-strong

debug:
	$(DOCKER_SGDK) debug

# ROM de test : démarre directement sur le rapport de fin d'année 3 (renouvellement
# du Sénat, plantage V2). Refaire `make clean rom` ensuite pour la ROM normale.
rom-annee4: $(CONTENT_GEN)
	rm -rf out/src out/rom.out
	echo "-fstack-protector-strong -DMP_DEBUG_SKIP_YEARS" > out/dbgflags.txt
	$(DOCKER_SGDK) release EXTRA_FLAGS=@/src/out/dbgflags.txt
	rm -rf out/src out/rom.out

# ROM de démonstration (captures, docs/CHARTE_V3.md § 9) : saut direct à un écran
# (DEMO=1..10, voir game_debug_jump) et manette simulée (SCRIPT="w30 A w20 A").
# Produit out/demo.bin ; out/rom.bin est ensuite recompilée normalement.
DEMO   ?= 1
SCRIPT ?=
rom-demo: $(CONTENT_GEN)
	rm -rf out/src out/rom.out
	printf '%s' '-fstack-protector-strong -DMP_DEBUG_SCREEN=$(DEMO)' > out/dbgflags.txt
	if [ -n "$(SCRIPT)" ]; then printf ' -DMP_DEBUG_SCRIPT="\\"%s\\""' "$(SCRIPT)" >> out/dbgflags.txt; fi
	$(DOCKER_SGDK) release EXTRA_FLAGS=@/src/out/dbgflags.txt
	cp out/rom.bin out/demo.bin
	rm -rf out/src out/rom.out out/rom.bin

# ROM de mesure de performance (docs/MESURES_V4_1.md) : comme rom-demo, avec
# MP_DEBUG_PERF (maximum d'update + draw et images perdues affichés en bas à droite).
# Produit out/perf.bin.
rom-perf: $(CONTENT_GEN)
	rm -rf out/src out/rom.out
	printf '%s' '-fstack-protector-strong -DMP_DEBUG_PERF -DMP_DEBUG_SCREEN=$(DEMO)' > out/dbgflags.txt
	if [ -n "$(SCRIPT)" ]; then printf ' -DMP_DEBUG_SCRIPT="\\"%s\\""' "$(SCRIPT)" >> out/dbgflags.txt; fi
	$(DOCKER_SGDK) release EXTRA_FLAGS=@/src/out/dbgflags.txt
	cp out/rom.bin out/perf.bin
	rm -rf out/src out/rom.out out/rom.bin

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

$(TEST_BIN): $(CONTENT_GEN) $(SIM_SRC) $(TEST_SRC) $(wildcard src/sim/*.h) $(wildcard src/save/*.h) $(wildcard tests/*.h)
	mkdir -p $(dir $@)
	$(HOST_CC) $(HOST_CFLAGS) $(SIM_SRC) $(TEST_SRC) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

check: test rom
	./tools/budget.sh
	-python3 tools/check_cols.py

shots:
	./tools/shots.sh $(SHOTS)

clean:
	rm -rf out
