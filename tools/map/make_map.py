#!/usr/bin/env python3
"""Génère la carte de France des 13 régions (tâche 2.01, partie graphique).

Entrée  : tools/map/regions.txt, grille de caractères éditable à la main.
          Une lettre = une case de 4x4 pixels ; '.' = mer. 40 x 40 cases.
Sorties : res/gfx/map_france.png   carte 160x160 px (20x20 tuiles), indices 1-13 = régions,
                                   14 = frontière, 0 = mer (transparent)
          res/gfx/pal_map.png      palette de la carte (une ligne de palette complète)

Convention d'indices de pal_map (stable pour le pixel art de la V2) :
    0 mer (transparent), 1-13 régions (ordre de REGION_* dans ui_map.h),
    14 frontière, 15 libre.
Colorer une région = changer UNE couleur de la palette (aucun redessin).

Python 3 seul. Usage : python3 tools/map/make_map.py
"""
import os
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "font"))
from make_gfx import write_png  # PNG indexé sans PIL

CELL = 4
# Lettre -> (indice, nom, couleur neutre par défaut)
REGIONS = [
    ("H", "Hauts-de-France", 0x6688CC),
    ("N", "Normandie", 0x44AA99),
    ("I", "Île-de-France", 0xCC6666),
    ("E", "Grand Est", 0xBB8844),
    ("B", "Bretagne", 0x8866BB),
    ("L", "Pays de la Loire", 0xCC88AA),
    ("C", "Centre-Val de Loire", 0x99AA44),
    ("U", "Bourgogne-Franche-Comté", 0x4488AA),
    ("A", "Nouvelle-Aquitaine", 0xAA6644),
    ("R", "Auvergne-Rhône-Alpes", 0x66AA66),
    ("O", "Occitanie", 0xCCAA44),
    ("P", "Provence-Alpes-Côte d'Azur", 0xBB66BB),
    ("K", "Corse", 0x44AACC),
]
BORDER = 0x000022


def load():
    with open(os.path.join(ROOT, "tools", "map", "regions.txt"), encoding="utf-8") as f:
        rows = [l.rstrip("\n") for l in f if l.strip() and not l.startswith("#")]
    w = len(rows[0])
    assert all(len(r) == w for r in rows), "lignes de longueurs différentes"
    assert w % 2 == 0 and len(rows) % 2 == 0, "dimensions multiples de 2 cases (tuiles de 8 px)"
    letters = {r[0]: i + 1 for i, r in enumerate(REGIONS)}
    grid = []
    for y, r in enumerate(rows):
        row = []
        for x, ch in enumerate(r):
            if ch == ".":
                row.append(0)
            elif ch in letters:
                row.append(letters[ch])
            else:
                sys.exit("caractère inconnu %r en (%d,%d)" % (ch, x, y))
        grid.append(row)
    missing = [r[1] for i, r in enumerate(REGIONS) if not any(i + 1 in row for row in grid)]
    assert not missing, "régions absentes : %s" % missing
    return grid


def render(grid):
    gh, gw = len(grid), len(grid[0])

    def at(x, y):
        return grid[y][x] if 0 <= x < gw and 0 <= y < gh else 0

    img = [[0] * (gw * CELL) for _ in range(gh * CELL)]
    for y in range(gh):
        for x in range(gw):
            v = grid[y][x]
            if not v:
                continue
            for py in range(CELL):
                for px in range(CELL):
                    c = v
                    # Terre/mer : trait côté terre ; région/région : trait côté droite/bas.
                    if px == 0 and at(x - 1, y) == 0: c = 14
                    if py == 0 and at(x, y - 1) == 0: c = 14
                    if px == CELL - 1 and at(x + 1, y) != v: c = 14
                    if py == CELL - 1 and at(x, y + 1) != v: c = 14
                    img[y * CELL + py][x * CELL + px] = c
    return img


def main():
    grid = load()
    pal = [0x001133] + [r[2] for r in REGIONS] + [BORDER, 0xFFFFFF]
    gfx = os.path.join(ROOT, "res", "gfx")
    os.makedirs(gfx, exist_ok=True)
    img = render(grid)
    write_png(os.path.join(gfx, "map_france.png"), img, pal)
    write_png(os.path.join(gfx, "pal_map.png"), [[i // 8 for i in range(16 * 8)] for _ in range(8)], pal)
    tw, th = len(img[0]) // 8, len(img) // 8
    print("carte %dx%d tuiles écrite dans res/gfx/" % (tw, th))


if __name__ == "__main__":
    main()
