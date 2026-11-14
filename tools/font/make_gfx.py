#!/usr/bin/env python3
"""Génère les graphismes provisoires de l'interface (tâches 1.3 et 1.5).

Entrée  : tools/font/glyphs.txt (glyphes 8x8 éditables à la main)
Sorties : res/gfx/font.png        police (16 glyphes par ligne, ordre de glyphs.txt)
          res/gfx/ui_frame.png    cadre de fenêtre 3x3 tuiles
          res/gfx/ui_gauge.png    jauge : 9 tuiles, 0 à 8 pixels remplis
          res/gfx/ui_icons.png    curseur, flèches haut/bas, puce
          res/gfx/pal_*.png       les 4 palettes nommées (bandeau 16 couleurs)
          src/ui/ui_font_map.h    table point de code Unicode -> indice de glyphe

Python 3 seul (zlib + struct), sans PIL. Usage : python3 tools/font/make_gfx.py
Les PNG sont indexés 4 bits : un pixel artiste peut les redessiner en gardant
les dimensions et la convention d'indices de couleur (voir src/ui/README.md).
"""
import os
import struct
import zlib

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
GFX = os.path.join(ROOT, "res", "gfx")

# Convention d'indices de couleur (identique dans les 4 palettes)
C_TRANSP, C_FILL, C_FRAME_LIGHT, C_FRAME_MID, C_FRAME_DARK = 0, 1, 2, 3, 4
C_INK, C_SHADOW, C_GAUGE, C_GAUGE_LIGHT, C_GAUGE_EMPTY, C_GAUGE_EDGE, C_CURSOR = 5, 6, 7, 8, 9, 10, 11

BASE = [
    0x001133,  # 0  fond de l'écran (couleur de fond du VDP pour la ligne 0)
    0x002266,  # 1  intérieur des fenêtres
    0x99BBEE,  # 2  cadre clair
    0x4466AA,  # 3  cadre moyen
    0x000022,  # 4  cadre sombre
    0xEEEEEE,  # 5  encre du texte
    0x000000,  # 6  ombre du texte
    0x22AA44,  # 7  jauge remplie
    0x66EE88,  # 8  jauge remplie, reflet
    0x223355,  # 9  jauge vide
    0x000011,  # 10 bord de jauge
    0xEEDD22,  # 11 curseur
    0x000000, 0x000000, 0x000000, 0x000000,
]


def variant(ink, gauge, gauge_light):
    p = list(BASE)
    p[C_INK], p[C_GAUGE], p[C_GAUGE_LIGHT] = ink, gauge, gauge_light
    return p


PALETTES = {
    "pal_ui_text": variant(0xEEEEEE, 0x22AA44, 0x66EE88),       # ligne 0 : texte, cadres, jauge positive
    "pal_ui_highlight": variant(0xEEDD22, 0xCC9922, 0xEECC66),  # ligne 1 : sélection
    "pal_ui_dim": variant(0x8888AA, 0x4477CC, 0x88AAEE),        # ligne 2 : atténué, jauge neutre
    "pal_ui_alert": variant(0xEE5544, 0xCC2222, 0xEE6655),      # ligne 3 : alerte, jauge négative
}


def write_png(path, pixels, palette):
    """PNG indexé 4 bits. pixels : liste de lignes d'indices 0-15."""
    h, w = len(pixels), len(pixels[0])
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for x in range(0, w, 2):
            raw.append((row[x] << 4) | (row[x + 1] if x + 1 < w else 0))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    plte = b"".join(struct.pack("BBB", (c >> 16) & 255, (c >> 8) & 255, c & 255) for c in palette)
    data = (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 4, 3, 0, 0, 0))
            + chunk(b"PLTE", plte)
            + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
            + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def blank(w, h):
    return [[0] * w for _ in range(h)]


def read_glyphs(path):
    glyphs = []  # (codepoint, 8 lignes)
    with open(path, encoding="utf-8") as f:
        lines = [l.rstrip("\n") for l in f]
    i = 0
    while i < len(lines):
        l = lines[i]
        if l.startswith("glyph U+"):
            cp = int(l.split()[1][2:], 16)
            rows = lines[i + 1:i + 9]
            assert len(rows) == 8 and all(len(r) == 8 and set(r) <= set("#.") for r in rows), l
            glyphs.append((cp, rows))
            i += 9
        else:
            i += 1
    cps = [g[0] for g in glyphs]
    assert cps[:95] == list(range(32, 127)), "glyphs.txt doit commencer par ASCII 32-126 dans l'ordre"
    assert len(set(cps)) == len(cps), "point de code en double"
    return glyphs


def make_font(glyphs):
    per_row = 16
    rows = (len(glyphs) + per_row - 1) // per_row
    img = blank(per_row * 8, rows * 8)
    for n, (_, g) in enumerate(glyphs):
        ox, oy = (n % per_row) * 8, (n // per_row) * 8
        ink = [[g[y][x] == "#" for x in range(8)] for y in range(8)]
        for y in range(8):
            for x in range(8):
                if ink[y][x]:
                    img[oy + y][ox + x] = C_INK
                elif x > 0 and y > 0 and ink[y - 1][x - 1]:
                    img[oy + y][ox + x] = C_SHADOW  # ombre portée en bas à droite
    write_png(os.path.join(GFX, "font.png"), img, PALETTES["pal_ui_text"])


def make_font_map(glyphs):
    extra = sorted((cp, n) for n, (cp, _) in enumerate(glyphs) if cp > 126)
    out = ["// Généré par tools/font/make_gfx.py à partir de tools/font/glyphs.txt : ne pas éditer.",
           "#ifndef UI_FONT_MAP_H", "#define UI_FONT_MAP_H", "",
           "#define UI_FONT_GLYPHS %d" % len(glyphs), "#define UI_FONT_EXTRA %d" % len(extra), "",
           "// Points de code hors ASCII, triés, et indice du glyphe correspondant.",
           "static const u16 ui_font_extra_cp[UI_FONT_EXTRA] = {"]
    out += ["    0x%04X,  // %s" % (cp, chr(cp)) for cp, _ in extra]
    out += ["};", "static const u8 ui_font_extra_glyph[UI_FONT_EXTRA] = {"]
    out += ["    %d,  // %s" % (n, chr(cp)) for cp, n in extra]
    out += ["};", "", "#endif", ""]
    with open(os.path.join(ROOT, "src", "ui", "ui_font_map.h"), "w", encoding="utf-8") as f:
        f.write("\n".join(out))


def make_frame():
    # Fenêtre 24x24 découpée en 3x3 tuiles : coins, bords, intérieur.
    n = 24
    img = blank(n, n)
    for y in range(n):
        for x in range(n):
            d = min(x, y, n - 1 - x, n - 1 - y)
            corner = min(x, n - 1 - x) + min(y, n - 1 - y)
            if corner < 1:
                c = C_TRANSP
            elif d == 0:
                c = C_FRAME_MID
            elif d == 1:
                c = C_FRAME_LIGHT
            elif d == 2:
                c = C_FRAME_DARK
            else:
                c = C_FILL
            img[y][x] = c
    write_png(os.path.join(GFX, "ui_frame.png"), img, PALETTES["pal_ui_text"])


def make_gauge():
    # 9 tuiles : k = 0..8 pixels remplis (de gauche à droite).
    img = blank(9 * 8, 8)
    for k in range(9):
        for y in range(8):
            for x in range(8):
                if y in (1, 6):
                    c = C_GAUGE_EDGE
                elif 2 <= y <= 5:
                    if x < k:
                        c = C_GAUGE_LIGHT if y == 2 else C_GAUGE
                    else:
                        c = C_GAUGE_EMPTY
                else:
                    c = C_TRANSP
                img[y][k * 8 + x] = c
    write_png(os.path.join(GFX, "ui_gauge.png"), img, PALETTES["pal_ui_text"])


ICONS = [
    # 0 curseur (flèche vers la droite)
    ["........", ".##.....", ".###....", ".####...", ".#####..", ".####...", ".###....", ".##....."],
    # 1 flèche haut (liste défilante)
    ["........", "...##...", "..####..", ".######.", "........", "........", "........", "........"],
    # 2 flèche bas
    ["........", "........", "........", "........", ".######.", "..####..", "...##...", "........"],
    # 3 puce
    ["........", "........", "...##...", "..####..", "..####..", "...##...", "........", "........"],
]


def make_icons():
    img = blank(len(ICONS) * 8, 8)
    for n, g in enumerate(ICONS):
        for y in range(8):
            for x in range(8):
                if g[y][x] == "#":
                    img[y][n * 8 + x] = C_CURSOR
                elif x > 0 and y > 0 and g[y - 1][x - 1] == "#":
                    img[y][n * 8 + x] = C_SHADOW
    write_png(os.path.join(GFX, "ui_icons.png"), img, PALETTES["pal_ui_text"])


def make_palettes():
    for name, pal in PALETTES.items():
        img = [[i // 8 for i in range(16 * 8)] for _ in range(8)]
        write_png(os.path.join(GFX, name + ".png"), img, pal)


def main():
    os.makedirs(GFX, exist_ok=True)
    glyphs = read_glyphs(os.path.join(ROOT, "tools", "font", "glyphs.txt"))
    make_font(glyphs)
    make_font_map(glyphs)
    make_frame()
    make_gauge()
    make_icons()
    make_palettes()
    print("%d glyphes, graphismes écrits dans res/gfx/" % len(glyphs))


if __name__ == "__main__":
    main()
