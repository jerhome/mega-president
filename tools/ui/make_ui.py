#!/usr/bin/env python3
"""Mega Président — graphismes de l'interface V3 (tâches 3.01 à 3.03).

Charte : docs/CHARTE_V3.md. Python 3 seul (zlib + struct), sans PIL.

Entrée  : tools/font/glyphs.txt (glyphes 8x8 éditables à la main)
Sorties : res/gfx/pal_ui_text.png       PAL0 : interface (encre claire, cadres, jauges verte et bleue)
          res/gfx/pal_ui_highlight.png  PAL1 : même structure, encre dorée/rouge, jauges or et rouge, bandeaux
          res/gfx/pal_ui_dim.png        copies neutres (compatibilité : ui_art_close les recharge
          res/gfx/pal_ui_alert.png      dans PAL2/PAL3, lignes réservées aux illustrations en V3)
          res/gfx/font.png              police : 2 jeux d'encre (A : indices 2/3, B : indices 4/5)
          res/gfx/ui_frame.png          décor du plan B : fonds, fenêtres, bandeaux, barre de sélection…
          res/gfx/ui_gauge.png          jauges à reflet : 2 jeux x (bout gauche, milieu, bout droit) x 9
          res/gfx/ui_icons.png          icônes 8x8 (chargées en DERNIER : les illustrations suivent)
          res/gfx/ui_cursor.png         4 images du curseur animé (copiées dans la tuile du curseur)
          src/ui/ui_font_map.h          table point de code -> glyphe
          src/ui/ui_tiles.h             indices des tuiles et palettes des icônes
Usage   : python3 tools/ui/make_ui.py [--preview DIR]
"""
import os
import struct
import sys
import zlib

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
GFX = os.path.join(ROOT, "res", "gfx")

# ---------------------------------------------------------------------------
# Palettes : couleurs en niveaux VDP 0-7 par composante (r, g, b).
# Indices 0-10 : même rôle dans PAL0 et PAL1 (police et jauges y puisent) ;
# indices 11-14 : identiques dans les deux lignes (fond et cadre des fenêtres) ;
# indice 15 : texture du fond d'écran (PAL0), dégradé des bandeaux (PAL1).
# ---------------------------------------------------------------------------
TR, SH, IA1, IA2, IB1, IB2, GA, GAL, GB, GBL, TROUGH, FILL, FILL2, FLIGHT, FMID, X15 = range(16)

PAL0 = [
    (0, 1, 2),  # 0  fond d'écran (couleur de fond du VDP)
    (0, 0, 0),  # 1  ombre, contours
    (7, 7, 7),  # 2  encre A haut : texte
    (6, 6, 5),  # 3  encre A bas  : texte (dégradé ivoire)
    (5, 6, 7),  # 4  encre B haut : texte atténué (gris bleuté clair, contrasté sur le bleu)
    (4, 5, 6),  # 5  encre B bas
    (1, 5, 2),  # 6  jauge A : verte
    (4, 7, 3),  # 7  jauge A : reflet
    (1, 3, 7),  # 8  jauge B : bleue
    (4, 6, 7),  # 9  jauge B : reflet
    (0, 0, 1),  # 10 creux des jauges, fond des médaillons
    (1, 2, 4),  # 11 fond des fenêtres
    (1, 2, 5),  # 12 grain du fond des fenêtres
    (5, 6, 7),  # 13 cadre clair (liseré)
    (2, 3, 6),  # 14 cadre moyen, barre de sélection
    (1, 1, 3),  # 15 grain du fond d'écran
]
PAL1 = list(PAL0)
PAL1[IA1], PAL1[IA2] = (7, 6, 2), (6, 5, 1)      # encre A : or (surbrillance, titres)
PAL1[IB1], PAL1[IB2] = (7, 3, 2), (6, 2, 1)      # encre B : rouge (alerte)
PAL1[GA], PAL1[GAL] = (6, 5, 1), (7, 7, 4)       # jauge A : or
PAL1[GB], PAL1[GBL] = (6, 1, 1), (7, 4, 3)       # jauge B : rouge
PAL1[X15] = (1, 1, 4)                            # 15 bas des bandeaux (bleu nuit)
PAL1[0] = PAL0[0]


def rgb(c):
    r, g, b = c
    f = lambda v: (v << 5) | (v << 2) | (v >> 1)
    return (f(r) << 16) | (f(g) << 8) | f(b)


# ---------------------------------------------------------------------------
# PNG indexé 4 bits
# ---------------------------------------------------------------------------
def write_png(path, pixels, palette, bits=4):
    h, w = len(pixels), len(pixels[0])
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        if bits == 4:
            for x in range(0, w, 2):
                raw.append((row[x] << 4) | (row[x + 1] if x + 1 < w else 0))
        else:
            raw.extend(bytes(row))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    plte = b"".join(struct.pack("BBB", (c >> 16) & 255, (c >> 8) & 255, c & 255) for c in palette)
    data = (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, bits, 3, 0, 0, 0))
            + chunk(b"PLTE", plte)
            + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
            + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def write_rgb_png(path, pixels):
    h, w = len(pixels), len(pixels[0])
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for c in row:
            raw += bytes(((c >> 16) & 255, (c >> 8) & 255, c & 255))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    data = (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(bytes(raw), 9)) + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def blank_tile(c=0):
    return [[c] * 8 for _ in range(8)]


def tiles_to_image(tiles, per_row=16):
    rows = (len(tiles) + per_row - 1) // per_row
    w = min(len(tiles), per_row) * 8
    img = [[0] * w for _ in range(rows * 8)]
    for n, t in enumerate(tiles):
        ox, oy = (n % per_row) * 8, (n // per_row) * 8
        for y in range(8):
            for x in range(8):
                img[oy + y][ox + x] = t[y][x]
    return img


def from_ascii(rows, cmap):
    return [[cmap[ch] for ch in r] for r in rows]


# ---------------------------------------------------------------------------
# Police
# ---------------------------------------------------------------------------
def read_glyphs(path):
    glyphs = []
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
    assert len(glyphs) <= 128, "128 glyphes au plus par jeu d'encre"
    return glyphs


def glyph_tile(rows, ink_top, ink_bottom):
    """Encre en dégradé (lignes 0-3 / 4-7) et ombre portée d'un pixel en bas à droite."""
    ink = [[rows[y][x] == "#" for x in range(8)] for y in range(8)]
    t = blank_tile()
    for y in range(8):
        for x in range(8):
            if ink[y][x]:
                t[y][x] = ink_top if y < 4 else ink_bottom
            elif x > 0 and y > 0 and ink[y - 1][x - 1]:
                t[y][x] = SH
    return t


def make_font(glyphs):
    tiles = []
    for top, bottom in ((IA1, IA2), (IB1, IB2)):
        set_tiles = [glyph_tile(g, top, bottom) for _, g in glyphs]
        set_tiles += [blank_tile()] * (128 - len(set_tiles))
        tiles += set_tiles
    return tiles


def make_font_map(glyphs):
    extra = sorted((cp, n) for n, (cp, _) in enumerate(glyphs) if cp > 126)
    out = ["// Généré par tools/ui/make_ui.py à partir de tools/font/glyphs.txt : ne pas éditer.",
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


# ---------------------------------------------------------------------------
# Décor du plan B (ui_frame.png) : chaque tuile est nommée ; l'ordre est exporté
# dans src/ui/ui_tiles.h (enum UiFrameTile) avec la ligne de palette à utiliser.
# ---------------------------------------------------------------------------
def bg_pixel(x, y, band):
    """Grain du fond d'écran (motif de période 8, aligné sur les tuiles)."""
    # Trame diagonale clairsemée ; la densité augmente vers le bas (bandes).
    d = (x + 2 * y) % 8
    if band == 0:
        return X15 if (d == 0 and y % 4 == 0) else TR
    if band == 1:
        return X15 if (d == 0 and y % 2 == 0) else TR
    if band == 2:
        return X15 if d == 0 else TR
    return X15 if d in (0, 4) else TR


def fill_pixel(x, y):
    """Fond des fenêtres : grain discret (lignes fines tous les 4 pixels, en tirets)."""
    if y % 4 == 3 and (x + y) % 8 < 3:
        return FILL2
    return FILL


R = 3  # rayon des coins arrondis


def outside_round(px, py, w, h, r=R):
    """Vrai si (px, py) est hors du rectangle [0,w[ x [0,h[ aux coins arrondis."""
    if px < 0 or py < 0 or px >= w or py >= h:
        return True
    cx = r if px < r else (w - 1 - r if px > w - 1 - r else px)
    cy = r if py < r else (h - 1 - r if py > h - 1 - r else py)
    dx, dy = px - cx, py - cy
    return dx * dx + dy * dy > r * r + r  # disque légèrement adouci


def ring_dist(px, py, w, h, r=R):
    """Distance (en pixels) au bord du rectangle arrondi."""
    for d in range(0, 8):
        if outside_round(px - d, py - d, w - 2 * d, h - 2 * d, max(r - d, 0)) if False else \
           outside_round(px - d, py - d, w - 2 * d, h - 2 * d, max(r - d, 0)):
            return d - 1
    return 8


def window_pixel(px, py, w, h, inner):
    """Fenêtre de w x h pixels (ombre portée comprise : 1 pixel à droite et en bas)."""
    bw, bh = w - 1, h - 1
    if outside_round(px, py, bw, bh):
        if not outside_round(px - 1, py - 1, bw, bh):
            return SH                 # ombre portée
        return None                   # transparent (fond d'écran)
    d = ring_dist(px, py, bw, bh)
    if d == 0:
        return SH                     # contour
    if d == 1:
        # liseré : clair en haut à gauche, moyen en bas à droite (relief)
        return FLIGHT if (px + py) < (bw + bh) // 2 or (py < bh - 4 and px < bw - 4) else FMID
    if d == 2:
        return FMID
    if d == 3:
        return SH if (py >= bh - 4 or px >= bw - 4) else FMID if False else SH
    return inner(px, py)


def render_box(w, h, pixel):
    return [[pixel(x, y) for x in range(w)] for y in range(h)]


def cut(img, tx, ty, bg_band=None):
    t = blank_tile()
    for y in range(8):
        for x in range(8):
            v = img[ty * 8 + y][tx * 8 + x]
            if v is None:
                v = TR
            t[y][x] = v
    return t


def make_window_tiles():
    n = 24
    img = render_box(n, n, lambda x, y: window_pixel(x, y, n, n, fill_pixel))
    names = ["WIN_TL", "WIN_T", "WIN_TR", "WIN_L", "WIN_C", "WIN_R", "WIN_BL", "WIN_B", "WIN_BR"]
    return [(names[ty * 3 + tx], 0, cut(img, tx, ty)) for ty in range(3) for tx in range(3)]


def banner_band(py):
    """Dégradé vertical du bandeau (py = pixel depuis le haut du bandeau)."""
    # lignes : 0 contour, 1 liseré clair, puis moyen -> bleu nuit avec tramage
    if py <= 6:
        return FMID
    if py <= 9:
        return None  # tramage décidé par l'appelant
    return X15


def header_pixel(px, py, w, total_h, standalone):
    """Bandeau de titre : 3 rangées de tuiles (haut, texte, bas)."""
    # Rectangle « corps » : bandeau seul => coins arrondis en bas aussi.
    bw = w - 1
    bh = total_h - 1 if standalone else total_h + 16
    if outside_round(px, py, bw, bh):
        if not outside_round(px - 1, py - 1, bw, bh) and (standalone or py < total_h):
            return SH
        return None
    d = ring_dist(px, py, bw, bh)
    if d == 0:
        return SH
    if d == 1:
        return FLIGHT if py < 20 else FMID
    # intérieur
    if py < 17:
        # dégradé : moyen (haut) -> nuit (bas), tramage en damier entre les deux
        if py <= 7:
            c = FMID
        elif py <= 10:
            c = FMID if (px + py) % 2 == 0 else X15
        else:
            c = X15
        if py == 2:
            c = FLIGHT if d >= 2 else c     # reflet en haut du bandeau
        return c
    if py == 17:
        return IA2      # filet doré (PAL1 : or sombre)
    if py == 18:
        return IA1      # filet doré clair
    if py == 19:
        return SH       # ombre du filet
    if standalone:
        return X15
    if d == 2:
        return FMID
    if d == 3:
        return SH
    return fill_pixel(px, py)


def make_header_tiles():
    tiles = []
    n = 24
    # Titre dans une fenêtre : rangées 0-2 du haut d'une fenêtre (les bords de la
    # rangée 2 rejoignent le cadre de la fenêtre).
    img = render_box(n, 24, lambda x, y: header_pixel(x, y, n, 24, False))
    names = ["HD_TL", "HD_T", "HD_TR", "HD_L", "HD_M", "HD_R", "HD_SL", "HD_S", "HD_SR"]
    for ty in range(3):
        for tx in range(3):
            tiles.append((names[ty * 3 + tx], 1, cut(img, tx, ty)))
    # Bandeau seul (3 rangées) : bas arrondi avec ombre.
    img = render_box(n, 24, lambda x, y: header_pixel(x, y, n, 24, True))
    for tx, nm in enumerate(["BN_BL", "BN_B", "BN_BR"]):
        tiles.append((nm, 1, cut(img, tx, 2)))
    return tiles


def make_misc_tiles():
    tiles = []
    # Fonds d'écran : 4 bandes de densité croissante.
    for band in range(4):
        t = [[bg_pixel(x, y, band) for x in range(8)] for y in range(8)]
        tiles.append(("BG%d" % band, 0, t))

    # Barre de sélection (sur le fond d'une fenêtre) : bout gauche, milieu, bout droit.
    def sel(px, py, w):
        if outside_round(px, py, w, 8, 3):
            return fill_pixel(px, py)
        if py == 0:
            return FLIGHT if 2 <= px < w - 2 else FMID
        if py == 7:
            return SH
        return FMID
    W = 24
    img = render_box(W, 8, lambda x, y: sel(x, y, W))
    for tx, nm in enumerate(["SEL_L", "SEL_M", "SEL_R"]):
        tiles.append((nm, 0, cut(img, tx, 0)))

    # Filet de séparation gravé (milieu de la tuile), bouts estompés.
    def sep(px, py, w):
        if px < 2 or px >= w - 2:
            return fill_pixel(px, py)
        if py == 3:
            return SH
        if py == 4:
            return FMID if 4 <= px < w - 4 else fill_pixel(px, py)
        return fill_pixel(px, py)
    img = render_box(W, 8, lambda x, y: sep(x, y, W))
    for tx, nm in enumerate(["SEP_L", "SEP_M", "SEP_R"]):
        tiles.append((nm, 0, cut(img, tx, 0)))

    # Cadre de portrait (plan A, PAL1) posé SUR le bord du portrait : coins arrondis
    # (pixels hors de l'arrondi peints du fond de fenêtre), contour noir, filet doré
    # clair en haut à gauche, sombre en bas à droite, filet noir intérieur. L'intérieur
    # des tuiles reste transparent : le portrait (plan B) s'y voit.
    def ring(px, py, w, h):
        if outside_round(px, py, w, h, 4):
            return fill_pixel(px, py)
        e = min(px, py, w - 1 - px, h - 1 - py)
        d = ring_dist(px, py, w, h, 4)
        if d == 0:
            return SH
        if d == 1:
            return IA1 if (px + py) < (w + h) // 2 - 4 or (px < w - 6 and py < h - 6) else IA2
        if d == 2:
            return IA2
        if d == 3:
            return SH
        return TR
    img = render_box(24, 24, lambda x, y: ring(x, y, 24, 24))
    names = ["PF_TL", "PF_T", "PF_TR", "PF_L", None, "PF_R", "PF_BL", "PF_B", "PF_BR"]
    for ty in range(3):
        for tx in range(3):
            if names[ty * 3 + tx]:
                tiles.append((names[ty * 3 + tx], 1, cut(img, tx, ty)))
    # Fond d'un emplacement de portrait vide (plan B) : creux sombre tramé.
    t = [[TROUGH if (x + y) % 4 else X15 for x in range(8)] for y in range(8)]
    tiles.append(("PF_C", 0, t))
    return tiles


# ---------------------------------------------------------------------------
# Jauges à reflet (plan A) : 2 jeux (A : encre 6/7, B : 8/9) x (gauche, milieu, droite) x 9
# ---------------------------------------------------------------------------
def gauge_tile(k, part, fill, light):
    t = blank_tile()
    for y in range(8):
        for x in range(8):
            edge_l = part == 0 and x == 0
            edge_r = part == 2 and x == 7
            if y in (1, 6):
                c = TR if (edge_l or edge_r) else SH
            elif 2 <= y <= 5:
                if edge_l or edge_r:
                    c = SH
                elif x < k:
                    c = light if y == 2 else fill
                    if y == 5 and x % 2 == 1:
                        c = SH if False else fill
                else:
                    c = SH if y == 2 else TROUGH
                # bout de remplissage lumineux
                if x < k and x == k - 1 and 3 <= y <= 4 and k < 8:
                    c = light
            else:
                c = TR
            t[y][x] = c
    return t


def make_gauges():
    tiles = []
    for fill, light in ((GA, GAL), (GB, GBL)):
        for part in range(3):
            for k in range(9):
                tiles.append(gauge_tile(k, part, fill, light))
    return tiles


# ---------------------------------------------------------------------------
# Icônes 8x8 (plan A). Lettres : couleurs ; '.' transparent. Une palette par icône.
# ---------------------------------------------------------------------------
# Lettres communes : k ombre, w blanc/or clair (2), i ivoire/or (3), d gris/rouge clair (4),
# e gris foncé/rouge (5), g vert/or (6), G reflet (7), b bleu/rouge (8), B reflet (9),
# t creux (10), f fond fenêtre (11), L liseré (13), m cadre moyen (14), n nuit (15)
CM = {".": TR, "k": SH, "w": IA1, "i": IA2, "d": IB1, "e": IB2, "g": GA, "G": GAL,
      "b": GB, "B": GBL, "t": TROUGH, "f": FILL, "L": FLIGHT, "m": FMID, "n": X15}

ICONS = [
    # nom, palette, dessin
    ("CURSOR", 1, ["........", ".ww.....", ".iww....", ".iiww...", ".iiiwk..", ".iiik...", ".iik....", ".ik....."]),
    ("ARROW_UP", 1, ["........", "...w....", "..wwi...", ".wwwii..", "kkkkkkk.", "........", "........", "........"]),
    ("ARROW_DOWN", 1, ["........", "........", "........", ".wwwii..", "..wwik..", "...ik...", "....k...", "........"]),
    ("BULLET", 1, ["........", "...w....", "..wwi...", ".wwwii..", "..wiik..", "...ik...", "....k...", "........"]),
    ("POPULARITY", 1, ["........", ".dd.dd..", "dwddddd.", "dddddde.", ".ddddek.", "..dde.k.", "...e.k..", "........"]),
    ("GROWTH", 0, ["......G.", ".....GGk", "..g.Gg.k", ".gGgg.k.", "Ggkgk...", "gk..k...", "wwwwwww.", "kkkkkkk."]),
    ("UNEMPLOYMENT", 0, ["........", "..eeee..", "..e..e..", "dddddddk", "dbbBbbdk", "dbbbbbdk", "ddddddd.", ".kkkkkk."]),
    ("DEBT", 1, ["..iiii..", ".iwwwwi.", "iwwiiiwk", "iwiwwwik", "iwwiiiwk", "iiwwwwik", ".iiiiik.", "..kkkk.."]),
    ("SOCIAL", 0, ["..w..w..", ".www.ww.", "..w..w..", ".bbb.bbk", "bBbbbBbk", "bbbbbbbk", "bbb.bbbk", ".kk..kk."]),
    ("ECONOMY", 0, ["......d.", "......d.", ".e..e.d.", ".ee.eed.", "dddddddk", "dwdwdwdk", "dddddddk", ".kkkkkkk"]),
    ("TECHNOLOGY", 0, [".d.d.d..", "dddddddk", "dbBBBbdk", "dbBbbbdk", "dbbbbbdk", "dddddddk", ".d.d.d.k", "..k.k.k."]),
    ("ECOLOGY", 0, ["......gG", "....gGGG", "..gGGgGg", ".gGggGgk", ".ggGggk.", ".gGggk..", "g.kkk...", "........"]),
    ("SECURITY", 0, ["bBBBBBb.", "bBwwwBbk", "bBwBBbbk", "bBBwBbbk", ".bBBbbk.", ".bbbbbk.", "..bbbk..", "...kk..."]),
    ("INTERNATIONAL", 0, ["..bbbb..", ".bgGbBb.", "bggGgbbk", "bBgggbBk", "bbBbggbk", "bbbbgGbk", ".bBbbbk.", "..kkkk.."]),
    ("TENSION", 1, ["....ww..", "...wi...", "..wi....", ".wwwwi..", "...wik..", "..wik...", ".wik....", ".k......"]),
    ("SCANDAL", 1, ["...dd...", "..dwwd..", "..dwwd..", ".dwwwwd.", ".dwddwdk", "ddwwwwddk"[:8], "dddwwdddk"[:8], ".kkkkkkk"]),
    ("DEFIANCE", 1, ["..dddd..", ".dwddwd.", "dwddddwk", "ddwddwdk", "dddwwddk", "ddwddwdk", ".dddddk.", "..kkkk.."]),
    ("CP", 1, ["...w....", "...wi...", "wwwwiii.", ".wwiiik.", "..wiik..", ".wik.ik.", ".ik...ik", ".k.....k"]),
    ("ASSEMBLY", 0, ["..LLLL..", ".LmmmmL.", "LmLLLLmk", "LLmmmmLk", "LmLLLLmk", "L.L..L.k", "L.L..L.k", "kkkkkkkk"]),
    ("CALENDAR", 0, [".L..L...", "dddddddk", "dddddddk", "wwwwwwwk", "wkwkwkwk", "wwwwwwwk", "wkwkwkwk", ".kkkkkkk"]),
    ("BTN_A", 0, ["..mmmm..", ".mLwwLm.", "mLwmmwLk", "mLwwwwLk", "mLwmmwLk", ".mLmmLmk", "..mmmmk.", "...kkk.."]),
    ("BTN_B", 0, ["..mmmm..", ".mwwwLm.", "mLwmmwLk", "mLwwwLmk", "mLwmmwLk", ".mwwwLmk", "..mmmmk.", "...kkk.."]),
    ("BTN_C", 0, ["..mmmm..", ".mLwwwm.", "mLwmmmLk", "mLwmmmLk", "mLwmmmLk", ".mLwwwmk", "..mmmmk.", "...kkk.."]),
    ("BTN_START", 0, ["........", ".mmmmmm.", "mLLwLLLm", "mLLwwLLk", "mLLwwwLk", "mLLwwLLk", ".mLwmmmk", "..kkkkk."]),
    ("CHECK", 0, ["........", "......Gg", ".....Ggk", "g...Ggk.", "Gg.Ggk..", ".GgGgk..", "..Ggk...", "...k...."]),
    ("CROSS", 1, ["........", ".d....d.", ".dd..ddk", "..dddde.", "...dde..", "..dddde.", ".ddk.dde", ".dk...dk"]),
    ("PERSON", 0, ["...ii...", "..iiii..", "..iiiik.", "...iik..", ".bbBBbb.", "bbbBBbbk", "bbbbbbbk", ".kkkkkk."]),
    ("NEWS", 0, [".wwwwww.", "wiiiiiiw", "wkkkwiiw", "wkkkwkkw", "wiiiwiiw", "wkkkwkkw", "wiiiiiiw", ".kkkkkk."]),
    ("UP_GOOD", 0, ["...G....", "..GGg...", ".GGggg..", "GGgggggk", "..ggk...", "..ggk...", "..ggk...", "...k...."]),
    ("DOWN_BAD", 1, ["...dd...", "...dek..", "...dek..", "ddddeeek", ".ddeeek.", "..ddek..", "...ek...", "....k..."]),
    ("UP_BAD", 1, ["...d....", "..dde...", ".dddee..", "ddddeeek", "..dek...", "..dek...", "..dek...", "...k...."]),
    ("DOWN_GOOD", 0, ["...Gg...", "...Ggk..", "...Ggk..", "GGGgggk.", ".GGggk..", "..Ggk...", "...gk...", "....k..."]),
    # Écran Options (3.19) : flèches de réglage (or), croche (son), œil (affichage),
    # cartouche Megadrive (données).
    ("ARROW_LEFT", 1, ["........", ".....w..", "....wi..", "...wwi..", "..wwii..", "...wiik.", "....iik.", ".....ik."]),
    ("ARROW_RIGHT", 1, ["........", "..w.....", "..wi....", "..wwi...", "..wwii..", "..wiik..", "..iik...", "..ik...."]),
    ("NOTE", 0, ["....ww..", "....wiw.", "....w.iw", "....w..k", "....w...", ".wwiw...", "wwiiik..", ".iikk..."]),
    ("EYE", 0, ["........", "..iiii..", ".iwbbwi.", "iwbkkbwi", "iwbkkbwi", ".iwbbwik", "..iiiik.", "...kkk.."]),
    ("CART", 0, [".eeeeee.", "eddddddk", "edwwwwdk", "edwbbwdk", "edwwwwdk", "eddddddk", "ekekekek", ".kkkkkk."]),
]


def make_icons():
    tiles = []
    for name, pal, rows in ICONS:
        assert len(rows) == 8 and all(len(r) == 8 for r in rows), name
        tiles.append(from_ascii(rows, CM))
    return tiles


def make_cursor_frames():
    base = ICONS[0][2]
    frames = []
    for off in (0, 1, 2, 1):
        rows = ["." * off + r[:8 - off] for r in base]
        frames.append(from_ascii(rows, CM))
    return frames


# ---------------------------------------------------------------------------
# En-tête C
# ---------------------------------------------------------------------------
def write_header(frame_tiles):
    out = ["// Généré par tools/ui/make_ui.py : ne pas éditer (voir docs/CHARTE_V3.md).",
           "#ifndef UI_TILES_H", "#define UI_TILES_H", "",
           "// Tuiles de décor (ui_frame.png, plan B), dans l'ordre du jeu de tuiles.",
           "enum {"]
    for i, (name, pal, _) in enumerate(frame_tiles):
        out.append("    UI_T_%s = %d," % (name, i))
    out += ["    UI_T_COUNT = %d" % len(frame_tiles), "};", "",
            "// Ligne de palette de chaque tuile de décor (0 : PAL0, 1 : PAL1).",
            "static const u8 ui_frame_pal[UI_T_COUNT] = {"]
    out.append("    " + ", ".join(str(p) for _, p, _ in frame_tiles))
    out += ["};", "",
            "// Jauges (ui_gauge.png) : jeu (0 : A, 1 : B) x partie (0 gauche, 1 milieu, 2 droite) x 9.",
            "#define UI_GAUGE_TILE(set, part, k) ((set) * 27 + (part) * 9 + (k))", "",
            "// Icônes (ui_icons.png) : ligne de palette de chaque icône.",
            "#define UI_ICON_COUNT %d" % len(ICONS),
            "static const u8 ui_icon_pal[UI_ICON_COUNT] = {"]
    out.append("    " + ", ".join(str(p) for _, p, _ in ICONS))
    out += ["};", "", "#endif", ""]
    with open(os.path.join(ROOT, "src", "ui", "ui_tiles.h"), "w", encoding="utf-8") as f:
        f.write("\n".join(out))


def icon_enum():
    return "\n".join("    UI_ICON_%s = %d," % (n, i) for i, (n, _, _) in enumerate(ICONS))


# ---------------------------------------------------------------------------
# Aperçu (maquette) : composition de tuiles, rendu RVB agrandi
# ---------------------------------------------------------------------------
def preview(dirpath, font, frame_tiles, gauges, icons):
    os.makedirs(dirpath, exist_ok=True)
    pals = [[rgb(c) for c in PAL0], [rgb(c) for c in PAL1]]
    W, H = 40, 28
    planeB = [[None] * W for _ in range(H)]
    planeA = [[None] * W for _ in range(H)]
    fi = {n: (p, t) for n, p, t in frame_tiles}
    for y in range(H):
        for x in range(W):
            planeB[y][x] = fi["BG%d" % min(3, y * 4 // H)]

    def put(x, y, name):
        planeB[y][x] = fi[name]

    def window(x, y, w, h):
        for j in range(h):
            for i in range(w):
                col = 0 if i == 0 else (2 if i == w - 1 else 1)
                row = 0 if j == 0 else (2 if j == h - 1 else 1)
                put(x + i, y + j, ["WIN_TL", "WIN_T", "WIN_TR", "WIN_L", "WIN_C", "WIN_R",
                                   "WIN_BL", "WIN_B", "WIN_BR"][row * 3 + col])

    def header(x, y, w, standalone):
        for i in range(w):
            col = 0 if i == 0 else (2 if i == w - 1 else 1)
            put(x + i, y, ["HD_TL", "HD_T", "HD_TR"][col])
            put(x + i, y + 1, ["HD_L", "HD_M", "HD_R"][col])
            put(x + i, y + 2, (["BN_BL", "BN_B", "BN_BR"] if standalone else ["HD_SL", "HD_S", "HD_SR"])[col])

    glyph_index = {}
    gl = read_glyphs(os.path.join(ROOT, "tools", "font", "glyphs.txt"))
    for n, (cp, _) in enumerate(gl):
        glyph_index[chr(cp)] = n

    def text(x, y, s, style=0):
        pal = style & 1
        base = (style >> 1) * 128
        for i, ch in enumerate(s):
            planeA[y][x + i] = (pal, font[base + glyph_index.get(ch, 31)])

    def icon(x, y, name):
        for i, (n, p, _) in enumerate(ICONS):
            if n == name:
                planeA[y][x] = (p, icons[i])

    def gauge(x, y, w, v, mx, style):
        fill = v * w * 8 // mx
        for i in range(w):
            k = max(0, min(8, fill - i * 8))
            part = 0 if i == 0 else (2 if i == w - 1 else 1)
            planeA[y][x + i] = (style & 1, gauges[(style >> 1) * 27 + part * 9 + k])

    def sel(x, y, w):
        for i in range(w):
            put(x + i, y, "SEL_L" if i == 0 else ("SEL_R" if i == w - 1 else "SEL_M"))

    # Maquette du tableau de bord
    header(0, 0, 40, True)
    text(2, 1, "Mandat 1 - Trimestre 1 / 20", 1)
    icon(32, 1, "CP"); text(34, 1, "4/15", 0)
    window(0, 3, 40, 17)
    rows = [("POPULARITY", "Popularité", 517, 1000, 1, "51,7 %"),
            (None, None, 0, 0, 0, None),
            ("GROWTH", "Croissance", 60, 100, 0, "1,29 %"),
            ("UNEMPLOYMENT", "Chômage", 30, 100, 2, "7,70 %"),
            ("DEBT", "Dette", 40, 100, 1, "110,0 %"),
            (None, None, 0, 0, 0, None),
            ("SOCIAL", "Social", 49, 100, 0, "49,5"),
            ("ECONOMY", "Économie", 51, 100, 0, "51,0"),
            ("TECHNOLOGY", "Technologie", 49, 100, 0, "49,2"),
            ("ECOLOGY", "Écologie", 51, 100, 0, "51,3"),
            ("SECURITY", "Sécurité", 48, 100, 0, "48,7"),
            ("INTERNATIONAL", "International", 48, 100, 0, "48,8"),
            ("TENSION", "Tension", 20, 100, 3, "20 %")]
    y = 4
    for ic, lab, v, mx, st, val in rows:
        if ic:
            icon(1, y, ic)
            text(2, y, lab, 2)
            gauge(15, y, 7, v, mx, st)
            text(29 - len(val), y, val, 0)
        else:
            for i in range(1, 29):
                put(i, y, "SEP_M")
        y += 1
    text(2, 5, "Pop 51,4  Moy 52,8  Ais 50,1", 2)
    # emplacement de portrait (fond seul, le cadre est sur le plan A)
    for j in range(12):
        for i in range(10):
            put(29 + i, 4 + j, "PF_C")
    text(30, 16, "Delaunay", 1)
    text(30, 17, "Maj. abs.", 2)
    window(0, 20, 40, 8)
    sel(1, 21, 37)
    icon(1, 21, "CURSOR")
    text(2, 21, "Proposer une loi", 1)
    text(2, 22, "Forcer une loi (49.3)", 2)
    text(2, 23, "Intervention télévisée (2 CP, 15 %)", 0)
    text(2, 24, "Voir l'Assemblée", 0)
    text(2, 25, "Carte des régions", 0)
    text(2, 26, "Référendum (6 CP)", 3)
    icon(38, 26, "ARROW_DOWN")

    img = [[0] * 320 for _ in range(224)]
    for ty in range(H):
        for tx in range(W):
            p, t = planeB[ty][tx]
            a = planeA[ty][tx]
            for y in range(8):
                for x in range(8):
                    c = pals[p][t[y][x]] if t[y][x] else pals[0][0]
                    if a and a[1][y][x]:
                        c = pals[a[0]][a[1][y][x]]
                    img[ty * 8 + y][tx * 8 + x] = c
    big = [[c for c in row for _ in range(2)] for row in img for _ in range(2)]
    write_rgb_png(os.path.join(dirpath, "maquette_tableau.png"), big)

    # Planche des tuiles
    allt = [(p, t) for _, p, t in frame_tiles] + [(i // 27 % 2 * 0, g) for i, g in enumerate(gauges)] \
        + [(ICONS[i][1], t) for i, t in enumerate(icons)]
    per = 16
    rows_n = (len(allt) + per - 1) // per
    sheet = [[0x404040] * (per * 9 * 4) for _ in range(rows_n * 9 * 4)]
    for n, (p, t) in enumerate(allt):
        ox, oy = (n % per) * 9 * 4, (n // per) * 9 * 4
        for y in range(8):
            for x in range(8):
                c = pals[p][t[y][x]] if t[y][x] else 0xFF00FF
                for dy in range(4):
                    for dx in range(4):
                        sheet[oy + y * 4 + dy][ox + x * 4 + dx] = c
    write_rgb_png(os.path.join(dirpath, "planche_tuiles.png"), sheet)


def main():
    os.makedirs(GFX, exist_ok=True)
    glyphs = read_glyphs(os.path.join(ROOT, "tools", "font", "glyphs.txt"))
    font = make_font(glyphs)
    make_font_map(glyphs)
    p0 = [rgb(c) for c in PAL0]
    p1 = [rgb(c) for c in PAL1]
    write_png(os.path.join(GFX, "font.png"), tiles_to_image(font), p0)

    frame_tiles = make_misc_tiles()[:4] + make_window_tiles() + make_header_tiles() + make_misc_tiles()[4:]
    write_png(os.path.join(GFX, "ui_frame.png"), tiles_to_image([t for _, _, t in frame_tiles], 64), p0)
    gauges = make_gauges()
    write_png(os.path.join(GFX, "ui_gauge.png"), tiles_to_image(gauges, 64), p0)
    icons = make_icons()
    write_png(os.path.join(GFX, "ui_icons.png"), tiles_to_image(icons, 64), p0)
    write_png(os.path.join(GFX, "ui_cursor.png"), tiles_to_image(make_cursor_frames(), 64), p1)
    for name, pal in (("pal_ui_text", p0), ("pal_ui_highlight", p1), ("pal_ui_dim", p0), ("pal_ui_alert", p0)):
        img = [[i // 8 for i in range(16 * 8)] for _ in range(8)]
        write_png(os.path.join(GFX, name + ".png"), img, pal)
    write_header(frame_tiles)
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import make_hemi
    make_hemi.main(sys.argv[sys.argv.index("--preview") + 1] if "--preview" in sys.argv else None)
    if "--preview" in sys.argv:
        preview(sys.argv[sys.argv.index("--preview") + 1], font, frame_tiles, gauges, icons)
    print("%d glyphes x 2 encres, %d tuiles de décor, %d de jauge, %d icônes"
          % (len(glyphs), len(frame_tiles), len(gauges), len(icons)))


if __name__ == "__main__":
    main()
