#!/usr/bin/env python3
"""Mega Président — géométrie de l'hémicycle dynamique (src/ui/ui_hemi.c).

Calcule la place de chaque siège (Assemblée : 577, Sénat : 348) dans un demi-cercle
de 20 x 10 tuiles (160 x 80 px), avec 4 allées rayonnantes et un perchoir, puis écrit
src/ui/ui_hemi_data.h : pour chaque tuile non vide, ses sièges (rang de gauche à
droite, position dans la tuile) et ses pixels de décor. Le jeu colore les sièges à
l'exécution selon la répartition des groupes (rang -> groupe), sans tampon en RAM.

Appelé par tools/ui/make_ui.py ; --preview DIR écrit un aperçu agrandi.
"""
import math
import os
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
TW, TH = 20, 10
W, H = TW * 8, TH * 8
CX, CY = 79.5, 77.5
AISLES = (36, 72, 108, 144)   # degrés

# Indices de couleur (PAL3 pendant l'affichage, voir ui_hemi.c)
C_SHADOW, C_FLOOR, C_DESK, C_DESK_LIGHT, C_ARC = 1, 12, 13, 14, 15


def seats_layout(n, r0, r1, rows):
    """Renvoie la liste des sièges (x, y, angle) répartis sur rows rangées."""
    radii = [r0 + (r1 - r0) * k / (rows - 1) for k in range(rows)]
    # longueur utile de chaque rangée (moins les allées, 4 px chacune)
    lens = [math.pi * r - len(AISLES) * 5 for r in radii]
    tot = sum(lens)
    counts = [int(n * l / tot) for l in lens]
    k = 0
    while sum(counts) < n:   # complète par les rangées extérieures
        counts[-1 - (k % rows)] += 1
        k += 1
    seats = []
    edges = [0] + list(AISLES) + [180]
    for r, cnt in zip(radii, counts):
        # répartit cnt sièges sur les 5 segments, au prorata de leur longueur
        segs = []
        for a0, a1 in zip(edges, edges[1:]):
            m0 = a0 + (0 if a0 == 0 else math.degrees(2.5 / r))
            m1 = a1 - (0 if a1 == 180 else math.degrees(2.5 / r))
            segs.append((m0, m1))
        seglen = [s[1] - s[0] for s in segs]
        sc = [int(cnt * l / sum(seglen)) for l in seglen]
        j = 0
        while sum(sc) < cnt:
            sc[(2 + j) % len(sc)] += 1
            j += 1
        for (m0, m1), c in zip(segs, sc):
            for i in range(c):
                a = m0 + (m1 - m0) * (i + 0.5) / c
                x = CX + r * math.cos(math.radians(a)) - 0.5
                y = CY - r * math.sin(math.radians(a)) - 0.5
                seats.append((int(round(x)), int(round(y)), a, r))
    # rang : de gauche (180°) à droite (0°), puis de l'intérieur vers l'extérieur
    seats.sort(key=lambda s: (-s[2], s[3]))
    return seats


def decor_pixels():
    """Perchoir et tribune au centre, filet de l'arc extérieur."""
    px = {}
    # tribune (bureau) : rectangle arrondi au centre bas
    for y in range(66, 78):
        for x in range(64, 96):
            if (y == 66 and (x < 66 or x > 93)) or (y == 77 and (x < 65 or x > 94)):
                continue
            px[(x, y)] = C_DESK_LIGHT if y <= 67 else C_DESK
    for x in range(65, 95):
        px[(x, 78)] = C_SHADOW
    # perchoir : petit pupitre surélevé
    for y in range(60, 66):
        for x in range(74, 86):
            px[(x, y)] = C_DESK_LIGHT if y == 60 else C_DESK
    # sol de l'hémicycle : demi-disque intérieur tramé
    for y in range(40, 80):
        for x in range(0, W):
            d = math.hypot(x + 0.5 - CX, y + 0.5 - CY)
            if d < 22 and (x, y) not in px and y < 79 and (x + y) % 2 == 0:
                px[(x, y)] = C_FLOOR
    # arc extérieur
    for a10 in range(0, 1801):
        a = math.radians(a10 / 10)
        x = int(round(CX + 77 * math.cos(a) - 0.5))
        y = int(round(CY - 77 * math.sin(a) - 0.5))
        if 0 <= x < W and 0 <= y < H:
            px[(x, y)] = C_ARC
    return px


def build(name, n, r0, r1, rows):
    seats = seats_layout(n, r0, r1, rows)
    assert len(seats) == n, (name, len(seats))
    occ = {}
    for i, (x, y, _, _) in enumerate(seats):
        for dx in (0, 1):
            for dy in (0, 1):
                assert (x + dx, y + dy) not in occ, (name, "sièges qui se chevauchent", x, y)
                occ[(x + dx, y + dy)] = i
    deco = decor_pixels()
    tiles = {}
    for i, (x, y, _, _) in enumerate(seats):
        assert 0 <= x < W - 1 and 0 <= y < H - 1, (name, x, y)
        # un siège peut chevaucher deux tuiles : on l'enregistre pixel par pixel
        for dx in (0, 1):
            for dy in (0, 1):
                t = ((x + dx) // 8, (y + dy) // 8)
                tiles.setdefault(t, {"seats": [], "deco": []})
                tiles[t]["seats"].append((i, (x + dx) % 8, (y + dy) % 8, 1 if (dx, dy) == (0, 0) else 0))
    for (x, y), c in deco.items():
        if (x, y) in occ:
            continue
        t = (x // 8, y // 8)
        tiles.setdefault(t, {"seats": [], "deco": []})
        tiles[t]["deco"].append((x % 8, y % 8, c))
    return seats, tiles


def emit(f, name, seats, tiles):
    keys = sorted(tiles, key=lambda t: (t[1], t[0]))
    pix = []
    first = []
    for k in keys:
        first.append(len(pix))
        for (i, x, y, light) in tiles[k]["seats"]:
            pix.append((i, x, y, light))
    first.append(len(pix))
    dpix, dfirst = [], []
    for k in keys:
        dfirst.append(len(dpix))
        for (x, y, c) in tiles[k]["deco"]:
            dpix.append((x, y, c))
    dfirst.append(len(dpix))
    N = name.upper()
    f.write("#define HEMI_%s_SEATS %d\n#define HEMI_%s_TILES %d\n" % (N, len(seats), N, len(keys)))
    f.write("static const u16 hemi_%s_tile_xy[HEMI_%s_TILES] = {  // (ty << 5) | tx\n" % (name, N))
    f.write(",".join(str((ty << 5) | tx) for tx, ty in keys) + "\n};\n")
    f.write("static const u16 hemi_%s_first[HEMI_%s_TILES + 1] = {\n" % (name, N))
    f.write(",".join(map(str, first)) + "\n};\n")
    f.write("// pixel de siège : rang << 7 | reflet << 6 | y << 3 | x\n")
    f.write("static const u32 hemi_%s_pix[%d] = {\n" % (name, len(pix)))
    f.write(",".join(str((i << 7) | (light << 6) | (y << 3) | x) for i, x, y, light in pix) + "\n};\n")
    f.write("static const u16 hemi_%s_dfirst[HEMI_%s_TILES + 1] = {\n" % (name, N))
    f.write(",".join(map(str, dfirst)) + "\n};\n")
    f.write("static const u8 hemi_%s_deco[%d] = {  // (couleur << 6 en 2 octets) : x | y << 3, couleur\n" % (name, 2 * max(1, len(dpix))))
    f.write(",".join("%d,%d" % ((y << 3) | x, c) for x, y, c in dpix) or "0,0")
    f.write("\n};\n\n")


def preview(dirpath, name, seats, tiles):
    import zlib, struct
    cols = [0x203060, 0xE04040, 0xF080B0, 0xF0C040, 0x4080F0, 0x6040A0]
    img = [[0x2a3a6a] * W for _ in range(H)]
    n = len(seats)
    for i, (x, y, _, _) in enumerate(seats):
        g = min(4, i * 5 // n)
        for dx in (0, 1):
            for dy in (0, 1):
                img[y + dy][x + dx] = cols[1 + g]
    for (x, y), c in decor_pixels().items():
        if img[y][x] == 0x2a3a6a:
            img[y][x] = {C_FLOOR: 0x1a2a50, C_DESK: 0x806040, C_DESK_LIGHT: 0xc0a060, C_ARC: 0x8090c0, C_SHADOW: 0}[c]
    s = 4
    raw = bytearray()
    for row in img:
        for _ in range(s):
            raw.append(0)
            for c in row:
                raw += bytes(((c >> 16) & 255, (c >> 8) & 255, c & 255)) * s

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
    os.makedirs(dirpath, exist_ok=True)
    with open(os.path.join(dirpath, "hemi_%s.png" % name), "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", W * s, H * s, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(bytes(raw), 9)) + chunk(b"IEND", b""))


def main(prev=None):
    out = os.path.join(ROOT, "src", "ui", "ui_hemi_data.h")
    with open(out, "w", encoding="utf-8") as f:
        f.write("// Généré par tools/ui/make_hemi.py : ne pas éditer.\n#ifndef UI_HEMI_DATA_H\n#define UI_HEMI_DATA_H\n\n")
        f.write("#define HEMI_TW %d\n#define HEMI_TH %d\n\n" % (TW, TH))
        for name, n, r0, r1, rows in (("an", 577, 26, 73, 14), ("senat", 348, 26, 73, 10)):
            seats, tiles = build(name, n, r0, r1, rows)
            emit(f, name, seats, tiles)
            if prev:
                preview(prev, name, seats, tiles)
        f.write("#endif\n")


if __name__ == "__main__":
    main(sys.argv[sys.argv.index("--preview") + 1] if "--preview" in sys.argv else None)
