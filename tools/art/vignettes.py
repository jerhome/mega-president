"""Vignettes d'événements (64 x 48 px, 8 x 6 tuiles), une palette de 15 couleurs chacune.

Une vignette par grand thème (enum ART_EVT_* de src/ui/ui_art.h). Style BD :
cadre et contours d'encre, aplats, une ombre, quelques reflets. Tout est fictif
(pas de logo, de drapeau national ni de personne réelle, hormis le drapeau
tricolore générique).
"""
import math
import os

from raster import (Canvas, md, ellipse, rect, poly, line, polyline, border, shift, arc, thick)

VW, VH = 64, 48
FRAME = rect(0, 0, VW - 1, VH - 1)
HERE = os.path.dirname(os.path.abspath(__file__))


class Pal:
    """Palette nommée : Pal(('ink', (1,0,1)), ...) ; p.ink -> indice ; p.list -> 16 couleurs."""

    def __init__(self, *entries):
        assert len(entries) <= 15
        self.list = [md(0, 0, 0)] + [md(*lv) for _, lv in entries]
        self.list += [md(0, 0, 0)] * (16 - len(self.list))
        for i, (name, _) in enumerate(entries):
            setattr(self, name, i + 1)


# --- Textes ------------------------------------------------------------------------

def _load_font():
    glyphs = {}
    path = os.path.join(HERE, "..", "font", "glyphs.txt")
    with open(path, encoding="utf-8") as f:
        lines = [l.rstrip("\n") for l in f]
    i = 0
    while i < len(lines):
        if lines[i].startswith("glyph U+"):
            cp = int(lines[i].split()[1][2:], 16)
            glyphs[chr(cp)] = lines[i + 1:i + 9]
            i += 9
        else:
            i += 1
    return glyphs


FONT8 = _load_font()

# Petite police 3 x 5 (capitales et chiffres utiles)
FONT3 = {
    'A': ["###", "#.#", "###", "#.#", "#.#"], 'B': ["##.", "#.#", "##.", "#.#", "##."],
    'C': ["###", "#..", "#..", "#..", "###"], 'D': ["##.", "#.#", "#.#", "#.#", "##."],
    'E': ["###", "#..", "##.", "#..", "###"], 'F': ["###", "#..", "##.", "#..", "#.."],
    'G': ["###", "#..", "#.#", "#.#", "###"], 'H': ["#.#", "#.#", "###", "#.#", "#.#"],
    'I': ["###", ".#.", ".#.", ".#.", "###"], 'L': ["#..", "#..", "#..", "#..", "###"],
    'M': ["#.#", "###", "###", "#.#", "#.#"], 'N': ["##.", "#.#", "#.#", "#.#", "#.#"],
    'O': ["###", "#.#", "#.#", "#.#", "###"], 'P': ["###", "#.#", "###", "#..", "#.."],
    'R': ["##.", "#.#", "##.", "#.#", "#.#"], 'S': ["###", "#..", "###", "..#", "###"],
    'T': ["###", ".#.", ".#.", ".#.", ".#."], 'U': ["#.#", "#.#", "#.#", "#.#", "###"],
    'V': ["#.#", "#.#", "#.#", "#.#", ".#."], 'X': ["#.#", "#.#", ".#.", "#.#", "#.#"],
    'Q': ["###", "#.#", "#.#", "###", "..#"], 'K': ["#.#", "##.", "#..", "##.", "#.#"],
    '0': ["###", "#.#", "#.#", "#.#", "###"], '1': [".#.", "##.", ".#.", ".#.", "###"],
    '2': ["##.", "..#", ".#.", "#..", "###"], '3': ["##.", "..#", ".#.", "..#", "##."],
    '4': ["#.#", "#.#", "###", "..#", "..#"], '5': ["###", "#..", "##.", "..#", "##."],
    '6': ["###", "#..", "###", "#.#", "###"], '7': ["###", "..#", ".#.", ".#.", ".#."],
    '8': ["###", "#.#", "###", "#.#", "###"], '9': ["###", "#.#", "###", "..#", "###"],
    '+': ["...", ".#.", "###", ".#.", "..."], '=': ["...", "###", "...", "###", "..."],
    '!': [".#.", ".#.", ".#.", "...", ".#."], '%': ["#.#", "..#", ".#.", "#..", "#.#"],
    '-': ["...", "...", "###", "...", "..."], ' ': ["...", "...", "...", "...", "..."],
    '€': [".##", "#..", "##.", "#..", ".##"], 'É': ["###", "#..", "##.", "#..", "###"],
    'È': ["###", "#..", "##.", "#..", "###"], '?': ["##.", "..#", ".#.", "...", ".#."],
}


def text8(c, x, y, s, col, shadow=None, bold=False):
    """Texte avec la police du jeu (8 x 8, glyphe utile 7 px)."""
    for ch in s:
        g = FONT8.get(ch, FONT8['?'])
        for j, row in enumerate(g):
            for i, v in enumerate(row):
                if v == '#':
                    if shadow is not None:
                        c.put(x + i + 1, y + j + 1, shadow)
                    c.put(x + i, y + j, col)
                    if bold:
                        c.put(x + i + 1, y + j, col)
        x += 7 if not bold else 8


def text3(c, x, y, s, col):
    """Petite police 3 x 5 ; renvoie la largeur écrite."""
    x0 = x
    for ch in s:
        g = FONT3[ch]
        for j, row in enumerate(g):
            for i, v in enumerate(row):
                if v == '#':
                    c.put(x + i, y + j, col)
        x += 4
    return x - x0 - 1


def text8_width(s, bold=False):
    return len(s) * (8 if bold else 7)


def frame(c, ink):
    c.fill(border(FRAME), ink)


def outline_color(c, inside_colors, ink):
    """Contour d'encre autour des pixels d'une couleur (vers l'extérieur)."""
    m = c.mask_of(inside_colors)
    c.fill(border(m, outer=True) & FRAME, ink)


def sky_gradient(c, top, bottom, y0, y1, x0=0, x1=VW - 1):
    for y in range(y0, y1 + 1):
        lvl = (y - y0) * 17 // max(1, (y1 - y0 + 1))
        c.dither(rect(x0, y, x1, y), top, bottom, lvl)


def star_burst(c, cx, cy, r, col, core):
    m = set()
    for k in range(8):
        a = math.radians(k * 45 + 22)
        rr = r if k % 2 == 0 else r * 0.6
        m |= line(cx, cy, int(round(cx + rr * math.cos(a))), int(round(cy + rr * math.sin(a))))
    c.fill(thick(m, 1), col)
    c.fill(m | ellipse(cx + 0.5, cy + 0.5, 1.5, 1.5), core)


def person(c, p, x, y, skin, body, hair, mouth='o', arm=None, h=1.0):
    """Petit personnage de foule (tête de 7 px), pieds hors cadre. arm : 'up' ou None."""
    head = ellipse(x + 0.5, y + 0.5, 3.5, 4)
    bod = (ellipse(x + 0.5, y + 13, 6, 8) & rect(x - 7, y + 5, x + 7, VH)) & FRAME
    c.paint(bod, body, ink=p.ink)
    c.paint(head, skin, ink=p.ink)
    hm = ellipse(x + 0.5, y - 1, 4, 3) - rect(x - 5, y, x + 5, y + 5)
    if hair is not None:
        c.paint(hm, hair, ink=p.ink)
    c.put(x - 1, y, p.ink); c.put(x + 2, y, p.ink)
    if mouth == 'o':
        c.put(x, y + 2, p.ink); c.put(x + 1, y + 2, p.ink)
        c.put(x, y + 3, p.ink); c.put(x + 1, y + 3, p.ink)
    else:
        c.put(x, y + 2, p.ink); c.put(x + 1, y + 2, p.ink)
    if arm == 'up':
        c.fill(line(x + 5, y + 7, x + 7, y - 2) | line(x + 6, y + 7, x + 8, y - 2), body)
        c.paint(ellipse(x + 8, y - 3, 1.8, 1.8), skin, ink=p.ink)


# --- 1. Grève, manifestation ---------------------------------------------------------

def v_greve():
    p = Pal(('ink', (1, 0, 1)), ('sky', (4, 5, 7)), ('sky2', (6, 6, 7)), ('wall', (6, 5, 4)),
            ('wall2', (4, 4, 4)), ('win', (2, 2, 4)), ('red', (7, 1, 1)), ('red2', (4, 0, 1)),
            ('white', (7, 7, 7)), ('skin', (7, 5, 4)), ('skin2', (5, 3, 2)), ('yel', (7, 6, 1)),
            ('blue', (2, 3, 6)), ('green', (2, 5, 2)), ('brown', (3, 1, 1)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky, p.sky2, 0, 20)
    # immeubles haussmanniens au fond
    for (x0, x1, top) in ((-2, 14, 6), (15, 34, 3), (35, 50, 8), (51, 66, 4)):
        b = rect(x0, top, x1, 40)
        c.paint(b, p.wall if (x0 // 15) % 2 == 0 else p.wall2, ink=p.ink)
        c.fill(rect(x0 + 1, top + 1, x1 - 1, top + 2), p.win)
        for wx in range(x0 + 3, x1 - 2, 5):
            for wy in range(top + 5, 30, 6):
                c.fill(rect(wx, wy, wx + 2, wy + 3), p.win)
                c.put(wx, wy, p.sky2)
    # banderole tenue par deux manifestants
    person(c, p, 6, 22, p.skin, p.blue, p.brown, arm=None)
    person(c, p, 57, 22, p.skin, p.green, p.yel, arm=None)
    ban = poly([(4, 14), (60, 14), (60, 26), (32, 27), (4, 26)])
    c.paint(ban, p.white, ink=p.ink)
    c.fill(rect(4, 14, 4, 34) | rect(60, 14, 60, 34), p.brown)
    tx = 32 - text8_width("GRÈVE !") // 2
    text8(c, tx, 17, "GRÈVE !", p.red, shadow=p.red2)
    # foule au premier plan, poings levés et pancartes
    crowd = [(14, 33, p.skin, p.red, p.brown, 'up'), (25, 31, p.skin2, p.yel, p.ink, None),
             (35, 34, p.skin, p.blue, p.yel, 'up'), (46, 32, p.skin, p.red2, p.wall2, None),
             (5, 37, p.skin, p.green, p.ink, None), (56, 37, p.skin2, p.blue, p.brown, 'up')]
    # pancartes derrière les têtes
    for (x, y, w) in ((20, 26, 9), (42, 27, 8)):
        c.fill(rect(x + w // 2, y + 6, x + w // 2, y + 12), p.brown)
        c.paint(rect(x, y, x + w, y + 6), p.yel, ink=p.ink)
        c.fill(rect(x + 2, y + 2, x + w - 2, y + 2) | rect(x + 2, y + 4, x + w - 3, y + 4), p.red2)
    for (x, y, sk, bo, ha, arm) in crowd:
        person(c, p, x, y, sk, bo, ha, arm=arm)
    # drapeau syndical rouge
    c.fill(rect(30, 6, 30, 30), p.brown)
    flag = poly([(31, 6), (40, 5), (44, 8), (40, 12), (31, 12)])
    c.paint(flag, p.red, ink=p.ink)
    c.put(34, 8, p.white); c.put(35, 8, p.white); c.put(36, 9, p.white)
    frame(c, p.ink)
    return c, p.list


# --- 2. Canicule, sécheresse -----------------------------------------------------------

def v_canicule():
    p = Pal(('ink', (1, 0, 1)), ('sky', (7, 6, 3)), ('sky2', (7, 7, 5)), ('sun', (7, 6, 1)),
            ('sun2', (7, 4, 0)), ('red', (6, 1, 0)), ('ground', (5, 3, 1)), ('ground2', (3, 2, 1)),
            ('white', (7, 7, 7)), ('glass', (2, 2, 3)), ('grey', (4, 4, 5)), ('green', (3, 4, 1)),
            ('dry', (6, 5, 2)), ('blue', (3, 5, 7)), ('pink', (7, 3, 3)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky2, p.sky, 0, 34)
    # vagues de chaleur
    for y in (8, 16, 24):
        for x in range(2, 62, 12):
            c.fill(arc(x + 3, y, 3, 1.5, 180, 360) | arc(x + 9, y, 3, 1.5, 0, 180), p.sky2)
    # sol craquelé
    gnd = rect(0, 34, VW - 1, VH - 1)
    c.fill(gnd, p.ground)
    c.fill(rect(0, 34, VW - 1, 34), p.ink)
    for pts in ([(4, 38), (10, 41), (9, 46)], [(10, 41), (18, 39)], [(26, 36), (30, 42), (38, 44), (40, 47)],
                [(30, 42), (24, 46)], [(46, 37), (50, 41), (58, 40)], [(50, 41), (52, 47)]):
        c.fill(polyline(pts), p.ground2)
    # soleil à lunettes noires qui transpire
    rays = set()
    for k in range(12):
        a = math.radians(k * 30 + 8)
        rays |= poly([(22 + 7 * math.cos(a - 0.25), 16 + 7 * math.sin(a - 0.25)),
                      (22 + 15 * math.cos(a), 16 + 15 * math.sin(a)),
                      (22 + 7 * math.cos(a + 0.25), 16 + 7 * math.sin(a + 0.25))])
    c.paint(rays, p.sun2, ink=p.ink)
    sun = ellipse(22, 16, 10, 10)
    c.paint(sun, p.sun, ink=p.ink)
    c.shade(sun - border(sun), p.sun2, -2, -2)
    c.fill(arc(21, 15, 7, 7, 200, 250), p.white)
    c.grid(14, 12, ["kkkkkk..kkkkkk",
                    "kggwggkkkggwgk",
                    ".kgggk..kgggk.",
                    "..kkk....kkk.."], {'k': p.ink, 'g': p.glass, 'w': p.white})
    c.grid(17, 19, ["k......k",
                    ".kkkkkk.",
                    "..kppk.."], {'k': p.ink, 'p': p.pink})
    c.grid(31, 8, [".k.", "kbk", "kbk", ".k."], {'k': p.ink, 'b': p.blue})   # goutte
    # thermomètre
    c.paint(rect(47, 4, 53, 30), p.white, ink=p.ink)
    c.paint(ellipse(50, 31, 4.5, 4.5), p.red, ink=p.ink)
    c.fill(rect(49, 8, 50, 29), p.red)
    c.fill(rect(49, 8, 49, 29), p.pink)
    for y in range(7, 28, 4):
        c.fill(rect(52, y, 52, y), p.ink)
    c.put(48, 30, p.pink)
    star_burst(c, 50, 5, 4, p.sun2, p.white)
    # fleur fanée
    c.fill(polyline([(37, 34), (37, 29), (39, 26), (42, 26)]), p.green)
    c.paint(ellipse(42.5, 28, 2.5, 2), p.dry, ink=p.ink)
    c.put(40, 31, p.green); c.put(36, 31, p.green)
    frame(c, p.ink)
    return c, p.list


# --- 3. Inondation, tempête --------------------------------------------------------------

def v_inondation():
    p = Pal(('ink', (1, 0, 1)), ('cloud', (3, 3, 4)), ('cloud2', (5, 5, 6)), ('sky', (4, 5, 6)),
            ('rain', (6, 7, 7)), ('water', (1, 3, 5)), ('water2', (2, 4, 6)), ('foam', (7, 7, 7)),
            ('wall', (7, 6, 4)), ('wall2', (5, 4, 3)), ('roof', (5, 1, 1)), ('win', (2, 2, 3)),
            ('boat', (7, 4, 0)), ('skin', (7, 5, 4)), ('yel', (7, 7, 1)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.cloud, p.sky, 0, 22)
    # nuages d'orage
    clouds = set()
    for (x, y, r) in ((6, 3, 8), (18, 1, 9), (32, 3, 9), (46, 0, 10), (60, 3, 8)):
        clouds |= ellipse(x, y, r, r * 0.7)
    c.paint(clouds & FRAME, p.cloud, ink=p.ink, edge=False)
    for (x, y, r) in ((10, 3, 4), (24, 2, 5), (40, 3, 4), (54, 2, 5)):
        c.fill(arc(x, y, r, r * 0.6, 200, 320), p.cloud2)
    # éclair
    bolt = poly([(48, 7), (54, 7), (51, 13), (55, 13), (46, 24), (49, 15), (45, 15)])
    c.paint(bolt, p.yel, ink=p.ink)
    # maison à moitié dans l'eau
    house = rect(8, 18, 30, 40)
    c.paint(house, p.wall, ink=p.ink)
    c.fill(rect(26, 19, 29, 39), p.wall2)
    roof = poly([(5, 19), (19, 8), (33, 19)])
    c.paint(roof, p.roof, ink=p.ink)
    c.fill(line(10, 17, 19, 10), p.wall2)
    c.paint(rect(24, 9, 27, 15), p.wall2, ink=p.ink)       # cheminée
    for (wx, wy) in ((11, 22), (21, 22)):
        c.paint(rect(wx, wy, wx + 5, wy + 5), p.win, ink=p.ink)
        c.fill(rect(wx + 2, wy, wx + 2, wy + 5) | rect(wx, wy + 2, wx + 5, wy + 2), p.ink)
        c.put(wx + 1, wy + 1, p.rain)
    # personne réfugiée sur le toit... à la fenêtre : bras qui s'agite
    c.paint(ellipse(14, 24.5, 1.8, 2), p.skin, ink=None)
    # eau
    water = rect(0, 31, VW - 1, VH - 1)
    c.fill(water, p.water)
    for y in range(31, VH, 4):
        for x in range(-4, VW, 10):
            off = 5 if (y // 4) % 2 else 0
            c.fill(arc(x + off + 3, y + 1, 3, 1.5, 180, 360), p.water2)
    c.fill(arc(19, 32, 14, 2, 0, 180), p.foam)
    for x in range(0, VW, 6):
        c.put(x, 31, p.foam); c.put(x + 1, 31, p.foam); c.put(x + 2, 30, p.foam)
    # canot pneumatique avec un sauveteur
    boat = ellipse(47, 37, 11, 3.5) - rect(30, 0, 64, 34)
    boat |= ellipse(47, 35, 11, 3)
    c.paint(ellipse(47, 31.5, 3, 3.5), p.skin, ink=p.ink)      # tête
    c.paint(ellipse(47, 29, 3.5, 2) - rect(40, 30, 54, 34), p.yel, ink=p.ink)   # casque
    c.paint(rect(44, 33, 50, 36), p.boat, ink=p.ink)
    c.paint(boat, p.boat, ink=p.ink)
    c.fill(rect(38, 35, 56, 35), p.foam)
    c.fill(line(52, 30, 58, 39), p.ink)                         # rame
    # pluie
    for k in range(40):
        x = (k * 23) % 64
        y = 8 + (k * 37) % 26
        if c.get(x, y) in (p.sky, p.cloud) and c.get(x - 1, y + 2) in (p.sky, p.cloud):
            c.put(x, y, p.rain); c.put(x - 1, y + 2, p.rain)
    frame(c, p.ink)
    return c, p.list


# --- 4. Scandale, affaire, presse ---------------------------------------------------------

def v_scandale():
    p = Pal(('ink', (1, 0, 1)), ('bg', (2, 1, 3)), ('bg2', (3, 2, 5)), ('paper', (7, 7, 6)),
            ('paper2', (5, 5, 4)), ('red', (7, 1, 1)), ('red2', (4, 0, 0)), ('grey', (3, 3, 3)),
            ('yel', (7, 6, 1)), ('white', (7, 7, 7)), ('skin', (7, 5, 4)), ('gold', (6, 5, 1)),
            ('green', (3, 5, 2)), ('blue', (2, 3, 6)), ('brown', (4, 2, 1)))
    c = Canvas(VW, VH, p.bg)
    # rayons « explosion » derrière
    for k in range(16):
        if k % 2 == 0:
            a0 = math.radians(k * 22.5)
            a1 = math.radians((k + 1) * 22.5)
            c.fill(poly([(32, 24), (32 + 60 * math.cos(a0), 24 + 60 * math.sin(a0)),
                         (32 + 60 * math.cos(a1), 24 + 60 * math.sin(a1))]) & FRAME, p.bg2)
    # journal incliné
    paper = poly([(6, 9), (55, 4), (59, 41), (9, 45)])
    c.paint(shift(paper, 2, 2), p.ink, ink=None)
    c.paint(paper, p.paper, ink=p.ink)
    # bandeau titre rouge
    band = poly([(8, 11), (54, 6.5), (55.5, 18), (9.5, 22.5)])
    c.paint(band, p.red, ink=p.ink)
    # texte « SCANDALE » légèrement incliné : on écrit à plat puis on décale par colonnes
    tmp = Canvas(VW, 12, 0)
    text8(tmp, 1, 1, "SCANDALE", p.white, shadow=p.red2)
    for y in range(tmp.h):
        for x in range(tmp.w):
            v = tmp.px[y][x]
            if v:
                c.put(x + 2 + 0, y + 11 - (x * 10) // 100, v)
    # photo floue et lignes de texte
    ph = poly([(12, 25), (28, 23.5), (29.5, 38), (13.5, 39.5)])
    c.paint(ph, p.grey, ink=p.ink)
    c.paint(ellipse(21, 30, 3.5, 4), p.skin, ink=p.ink)
    c.fill(rect(18, 28, 24, 29), p.ink)                         # lunettes noires
    c.paint(poly([(15, 39), (27, 37.5), (25, 34), (17, 35)]), p.ink, ink=None)
    for k in range(5):
        y = 25 + k * 3
        c.fill(line(32, y - 1, 54, y - 3) - rect(0, 0, 31, 47), p.paper2)
    # mallette de billets
    case = rect(38, 33, 58, 44)
    c.paint(case, p.brown, ink=p.ink)
    c.fill(rect(39, 34, 57, 34), p.gold)
    c.paint(rect(45, 30, 51, 33) - rect(47, 31, 49, 33), p.grey, ink=p.ink)
    for (bx, by, a) in ((40, 27, 0), (50, 25, 1), (56, 29, 0)):
        note = rect(bx, by, bx + 7, by + 3)
        c.paint(note, p.green, ink=p.ink)
        c.put(bx + 3, by + 1, p.white); c.put(bx + 4, by + 2, p.white)
    # flashs
    star_burst(c, 6, 5, 4, p.yel, p.white)
    star_burst(c, 59, 44, 4, p.yel, p.white)
    frame(c, p.ink)
    return c, p.list


# --- 5. Sommet international ------------------------------------------------------------------

def v_sommet():
    p = Pal(('ink', (1, 0, 1)), ('sky', (3, 4, 6)), ('sky2', (5, 6, 7)), ('sea', (1, 3, 6)),
            ('land', (3, 5, 2)), ('land2', (2, 3, 1)), ('suit', (1, 1, 3)), ('suit2', (3, 3, 4)),
            ('white', (7, 7, 7)), ('skin', (7, 5, 4)), ('skin2', (5, 3, 2)), ('red', (6, 1, 1)),
            ('yel', (7, 6, 1)), ('orange', (7, 4, 1)), ('green', (1, 5, 3)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky, p.sky2, 0, VH - 1)
    # globe
    globe = ellipse(32, 26, 21, 21)
    c.paint(globe, p.sea, ink=p.ink)
    for m in (poly([(16, 14), (25, 10), (30, 15), (27, 22), (20, 24), (14, 20)]),
              poly([(35, 9), (45, 11), (49, 18), (44, 22), (38, 18)]),
              poly([(24, 28), (31, 27), (32, 36), (26, 42), (22, 35)]),
              poly([(41, 30), (49, 28), (51, 35), (44, 38)])):
        c.paint(m & globe - border(globe), p.land, ink=None)
        c.shade(m & globe - border(globe), p.land2, -1, -1)
    c.fill(arc(27, 21, 13, 13, 200, 250), p.white)
    # guirlande de drapeaux fictifs
    c.fill(arc(32, -10, 40, 18, 15, 165), p.ink)
    cols = [(p.red, p.white), (p.yel, p.green), (p.sea, p.yel), (p.orange, p.white), (p.green, p.red), (p.white, p.sea)]
    for k in range(6):
        a = math.radians(25 + k * 26)
        fx = int(32 + 40 * math.cos(a)); fy = int(-10 + 18 * math.sin(a))
        f = poly([(fx - 3, fy), (fx + 3, fy), (fx + 3, fy + 5), (fx - 3, fy + 5)])
        c.paint(f, cols[k][0], ink=p.ink)
        c.fill(rect(fx - 2, fy + 2, fx + 2, fy + 2), cols[k][1])
    # poignée de main au premier plan (grande, façon BD)
    left = poly([(-2, 33), (17, 31), (19, 42), (-2, 46)])
    right = poly([(66, 31), (47, 30), (45, 41), (66, 46)])
    c.paint(left & FRAME, p.suit, ink=p.ink, edge=False)
    c.paint(right & FRAME, p.suit2, ink=p.ink, edge=False)
    c.fill(line(0, 39, 16, 37), p.ink); c.fill(line(64, 38, 47, 36), p.ink)   # plis
    c.paint(rect(15, 30, 19, 42), p.white, ink=p.ink)             # manchettes
    c.paint(rect(45, 29, 49, 41), p.white, ink=p.ink)
    c.put(17, 35, p.yel); c.put(47, 34, p.yel)
    # main droite (venant de la droite), en dessous
    rh = ellipse(37, 37, 10, 6)
    c.paint(rh, p.skin, ink=p.ink)
    c.shade(rh - border(rh), p.skin2, 0, -2)
    # main gauche (venant de la gauche), par-dessus : dos de la main + doigts repliés
    lh = ellipse(27, 34, 9, 5.5)
    c.paint(lh, p.skin, ink=p.ink)
    for k in range(4):                                          # doigts qui enserrent
        fx = 32 + k * 3
        f = rect(fx, 33 - (1 if k in (1, 2) else 0), fx + 2, 41 - (1 if k == 3 else 0))
        c.paint(f, p.skin, ink=p.ink)
        c.put(fx + 1, 40 - (1 if k == 3 else 0), p.skin2)
    c.fill(arc(27, 33, 6, 3, 200, 330), p.white)                # reflet
    # pouce de la main droite posé dessus
    thumb = poly([(44, 31), (30, 29), (28, 31.5), (31, 33), (44, 34)])
    c.paint(thumb, p.skin, ink=p.ink)
    # étincelles d'accord
    star_burst(c, 33, 22, 4, p.yel, p.white)
    star_burst(c, 13, 24, 2, p.yel, p.white)
    star_burst(c, 52, 22, 2, p.yel, p.white)
    frame(c, p.ink)
    return c, p.list


# Les vignettes suivantes sont dans vignettes2.py (même style).
from vignettes2 import (v_bourse, v_sante, v_pollution, v_securite, v_numerique,   # noqa: E402
                        v_education, v_energie, v_agriculture, v_industrie, v_fete)

# Ordre = enum ART_EVT_* (src/ui/ui_art.h)
_ORDER = [("greve", v_greve), ("canicule", v_canicule), ("inondation", v_inondation),
          ("scandale", v_scandale), ("sommet", v_sommet), ("bourse", v_bourse), ("sante", v_sante),
          ("pollution", v_pollution), ("securite", v_securite), ("numerique", v_numerique),
          ("education", v_education), ("energie", v_energie), ("agriculture", v_agriculture),
          ("industrie", v_industrie), ("fete", v_fete)]


def _entry(name, fn):
    c, pal = fn()
    return ("art_evt_" + name, (lambda c=c: c), pal)


VIGNETTES = [_entry(n, f) for n, f in _ORDER]
