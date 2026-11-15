"""Écran titre V3 (tâche 3.16) : 320 x 128 px (40 x 16 tuiles), palette title.PALETTE (PAL2).

Le palais (title.py) est conservé ; la scène est repeuplée dans le style des gueules
V3 (figures sculptées et éclairées par le moteur gueules.py, en version « grosse
tête ») : Antoine Verdier salue sur le perron, des pigeons squattent le drapeau, un
CRS bâille, la secrétaire de l'UST brandit sa pancarte, un retraité râle, le patron
de l'UPE compte ses billets et Chaîne Info 24 tend son micro.
Les 3 premières lignes de tuiles (y < 24) restent un ciel uni : le titre « MEGA
PRÉSIDENT » est écrit par-dessus par l'interface. Tout est fictif.
"""
import math

import title as T0
from title import (W, H, GROUND, PALETTE, INK, BLUE, SKY2, SKY3, WHITE, STONE, STONE_SH, SLATE,
                   NAVY, RED, SKIN, BROWN, GRAVEL, GREEN, GOLD)
from raster import Canvas, md, ellipse, rect, poly, line, border
from gueules import Portrait, bez

# Couleurs MD (niveaux 0-7) de la palette du titre, par indice
_RGB = {i: c for i, c in enumerate(PALETTE)}
LV = {INK: (1, 0, 1), BLUE: (1, 2, 6), SKY2: (3, 4, 7), SKY3: (5, 6, 7), WHITE: (7, 7, 7), STONE: (7, 7, 5),
      STONE_SH: (5, 5, 4), SLATE: (2, 3, 4), NAVY: (1, 1, 3), RED: (6, 1, 1), SKIN: (7, 5, 4),
      BROWN: (4, 2, 1), GRAVEL: (6, 5, 3), GREEN: (2, 4, 1), GOLD: (7, 6, 2)}
for i, t in LV.items():
    assert md(*t) == PALETTE[i], i
TO_IDX = {t: i for i, t in LV.items()}

RAMPS = {
    "bg": [(0, 0, 0)],
    "ink": [LV[INK]],
    "skin": [LV[BROWN], LV[SKIN], LV[STONE]],
    "tan": [LV[BROWN], LV[GRAVEL], LV[SKIN]],
    "orange": [LV[BROWN], LV[GRAVEL], LV[GOLD]],
    "hair": [LV[INK], LV[BROWN]],
    "grey": [LV[SLATE], LV[STONE_SH], LV[WHITE]],
    "navy": [LV[INK], LV[NAVY], LV[SLATE]],
    "red": [LV[BROWN], LV[RED], LV[GOLD]],
    "white": [LV[STONE_SH], LV[WHITE]],
    "cord": [LV[INK], LV[BROWN], LV[GRAVEL]],
    "gold": [LV[BROWN], LV[GOLD], LV[STONE]],
    "blue": [LV[NAVY], LV[BLUE], LV[SKY2]],
    "green": [LV[NAVY], LV[GREEN], LV[GRAVEL]],
    "sky": [LV[SKY2], LV[SKY3], LV[WHITE]],
}


def fig(w, h):
    return Portrait(dict(RAMPS), w=w, h=h)


def blit(c, P, ox, oy, extra=None):
    """Rend la figure (éclairage + traits) et la colle sur le canevas en (ox, oy)."""
    P.render_base()
    key = P.render("neutre")
    for y in range(P.h):
        for x in range(P.w):
            k = key[y][x]
            if k[0] == "bg":
                continue
            c.put(ox + x, oy + y, TO_IDX[P.ramps[k[0]][k[1]]])


# --- petite police 3 x 5 (pancartes, bulles) ---------------------------------------
GLYPHS = {
    "U": ["k.k", "k.k", "k.k", "k.k", "kkk"], "S": ["kkk", "k..", "kkk", "..k", "kkk"],
    "T": ["kkk", ".k.", ".k.", ".k.", ".k."], "G": ["kkk", "k..", "k.k", "k.k", "kkk"],
    "R": ["kk.", "k.k", "kk.", "k.k", "k.k"], "!": ["k", "k", "k", ".", "k"],
    "M": ["k...k", "kk.kk", "k.k.k", "k...k", "k...k"], "I": ["k", "k", "k", "k", "k"],
    "C": ["kkk", "k..", "k..", "k..", "kkk"], "+": ["...", ".k.", "kkk", ".k.", "..."],
    "2": ["kkk", "..k", "kkk", "k..", "kkk"], "4": ["k.k", "k.k", "kkk", "..k", "..k"],
    "E": ["kkk", "k..", "kk.", "k..", "kkk"], "€": ["..kkk", ".k...", "kkkk.", ".k...", "..kkk"],
    "Z": ["kkk", "..k", ".k.", "k..", "kkk"], "z": ["...", "kkk", ".k.", "k..", "kkk"],
    " ": ["..", "..", "..", "..", ".."], "A": [".k.", "k.k", "kkk", "k.k", "k.k"],
    "H": ["k.k", "k.k", "kkk", "k.k", "k.k"], "1": [".k", "kk", ".k", ".k", ".k"],
    "N": ["k..k", "kk.k", "k.kk", "k..k", "k..k"], "O": ["kkk", "k.k", "k.k", "k.k", "kkk"],
    "D": ["kk.", "k.k", "k.k", "k.k", "kk."], "L": ["k..", "k..", "k..", "k..", "kkk"],
    ".": [".", ".", ".", ".", "k"], "?": ["kkk", "..k", ".kk", "...", ".k."],
}


def text(c, x, y, s, col, scale=1):
    for ch in s:
        g = GLYPHS[ch]
        for j, row in enumerate(g):
            for i, v in enumerate(row):
                if v == "k":
                    for sy in range(scale):
                        for sx in range(scale):
                            c.put(x + i * scale + sx, y + j * scale + sy, col)
        x += (len(g[0]) + 1) * scale
    return x


def text_w(s, scale=1):
    return sum((len(GLYPHS[ch][0]) + 1) * scale for ch in s) - scale


def bubble(c, x, y, w, h, tail_x, tail_dir=1):
    m = ellipse(x + w / 2, y + h / 2, w / 2, h / 2)
    tail = poly([(tail_x - 2, y + h - 2), (tail_x + 2, y + h - 2), (tail_x + 5 * tail_dir, y + h + 5)])
    c.paint(m | tail, WHITE, ink=INK)


# --- figures (style gueules, grosse tête) --------------------------------------------

def eye(cv, x, y, key=("ink", 0), shine=True):
    """Œil 2 x 3 avec reflet."""
    cv.fill(rect(x, y, x + 1, y + 2), key)
    if shine:
        cv.put(x, y, ("white", 1))


def brow(cv, x0, y0, x1, y1, key=("ink", 0)):
    cv.fill(line(x0, y0, x1, y1) | line(x0, y0 + 1, x1, y1 + 1), key)


def body_layer(P, mat, cx, top, wide, bottom):
    l = P.layer(mat)
    l.shape(poly([(cx - wide - 2, bottom), (cx - wide, top + 8), (cx - wide + 6, top + 2), (cx - 6, top),
                  (cx + 6, top), (cx + wide - 6, top + 2), (cx + wide, top + 8), (cx + wide + 2, bottom)]),
            h=7, r=6)
    return l


def verdier():
    """Antoine Verdier qui salue : nuage de boucles, barbe, lunettes rondes, velours."""
    P = fig(48, 58)
    cx = 22
    arm = P.layer("cord", cast=(1, 1))
    arm.shape(poly([(31, 33), (36, 32), (44, 11), (40, 9)]), h=3, r=3)
    hand = P.layer("skin")
    hand.ell(42.5, 8, 4, 4, h=3)
    legs = P.layer("navy")
    legs.shape(rect(14, 48, 20, 57) | rect(24, 48, 30, 57), h=3, r=3)
    body = body_layer(P, "cord", cx, 29, 14, 52)
    shirt = P.layer("white", ink=True)
    shirt.shape(poly([(cx - 5, 28), (cx + 5, 28), (cx, 37)]), h=1, r=1)
    hb = P.layer("hair", cast=(1, 1), gain=1.4, smooth=1)
    clumps = []
    for k in range(13):
        a = math.radians(165 + 210 * k / 12)
        x, y = cx + 14 * math.cos(a), 17 + 12 * math.sin(a)
        if y < 22:
            hb.ell(x, y, 5.5, 5, h=4)
            clumps.append((x, y))
    hb.ell(cx, 13, 13, 9, h=7)
    head = P.layer("skin", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 18, 10, 11, h=4.5)
    head.ell(cx, 25, 9, 6, h=3.5)
    beard = P.layer("hair", smooth=1, gain=1.2)
    beard.shape((ellipse(cx, 23, 10.5, 9) - ellipse(cx, 17, 8.5, 7.5)) & head.mask(), h=2, r=2)
    beard.cut(ellipse(cx, 26.5, 4, 1.8))
    hf = P.layer("hair", gain=1.4, smooth=1)
    fcl = ((cx - 7, 9), (cx, 7.5), (cx + 7, 9))
    for (x, y) in fcl:
        hf.ell(x, y, 5, 4, h=4)
    nose = P.layer("skin", ink="shadow", ink_dirs="rdl")
    nose.ell(cx + 0.5, 21, 2.5, 2, h=2)

    @P.draw
    def face(cv, e):
        for (x, y) in list(clumps) + list(fcl):
            for a in range(0, 120, 25):
                px = int(x + 3.5 * math.cos(math.radians(a)))
                py = int(y + 3 * math.sin(math.radians(a)))
                if cv.get(px, py) and cv.get(px, py)[0] == "hair":
                    cv.put(px, py, ("hair", 0))
        for s in (-1, 1):
            eye(cv, cx + s * 4 - (1 if s < 0 else 0), 16)
            m = ellipse(cx + s * 4.5, 17.5, 3.6, 3.2)
            cv.fill(border(m), ("ink", 0))
            brow(cv, cx + s * 2, 12, cx + s * 7, 11, ("hair", 0))
        cv.fill(line(cx - 1, 16, cx + 1, 16), ("ink", 0))
        # grand sourire
        m = ellipse(cx, 26, 4.5, 2.6) - rect(0, 0, 47, 25)
        cv.fill(m, ("ink", 0))
        cv.fill(rect(cx - 3, 26, cx + 3, 26), ("white", 1))
        cv.put(cx + 9, 36, ("green", 1))     # badge vert
        cv.put(cx + 9, 37, ("green", 0))
    return P


def crs():
    """CRS qui bâille : casque à visière relevée, bouche grande ouverte, bouclier."""
    P = fig(44, 62)
    cx = 24
    body = body_layer(P, "navy", cx, 34, 15, 62)
    head = P.layer("skin", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 23, 10, 11, h=4.5)
    helm = P.layer("navy", cast=(1, 3), gain=1.2)
    helm.shape(ellipse(cx, 17, 13, 12) - rect(0, 19, 43, 61), h=5, r=4)
    helm.ell(cx, 13, 11, 8, h=5, mode="add")
    visor = P.layer("sky", ink=True)
    visor.shape(rect(cx - 12, 5, cx + 12, 9), h=1, r=1)
    shield = P.layer("sky", ink=True, cast=(1, 1))
    shield.shape(rect(0, 36, 17, 62), h=2, r=2)
    nose = P.layer("skin", ink="shadow", ink_dirs="rdl")
    nose.ell(cx + 0.5, 25, 2.5, 2, h=2)

    @P.draw
    def face(cv, e):
        for s in (-1, 1):   # yeux fermés de fatigue
            cv.fill(line(cx + s * 5 - 2, 21, cx + s * 5 + 2, 22), ("ink", 0))
            brow(cv, cx + s * 2, 17, cx + s * 7, 18, ("navy", 0))
        m = ellipse(cx, 30.5, 4, 5)
        cv.fill(m, ("ink", 0))
        cv.fill(ellipse(cx, 33.5, 2.5, 1.5) & m, ("red", 1))
        cv.fill(rect(cx - 2, 26, cx + 2, 26) & m, ("white", 1))
        for (x, y) in ((cx + 9, 23), (cx + 9, 24), (cx + 10, 25)):   # larme de bâillement
            cv.put(x, y, ("sky", 2))
        cv.fill(line(3, 41, 14, 41), ("white", 1))
        text(PseudoCanvas(cv), 3, 46, "CRS", ("navy", 1))
    return P


class PseudoCanvas:
    """Adapte text() (indices de palette) au dessin par clés des figures."""

    def __init__(self, cv):
        self.cv = cv

    def put(self, x, y, k):
        self.cv.put(x, y, k)


def syndicaliste():
    """Nadia Benali (UST) qui brandit sa pancarte : queue de cheval, gilet rouge."""
    P = fig(44, 52)
    cx = 22
    tail = P.layer("hair", smooth=1, gain=1.3)
    tail.shape(poly([(cx + 3, 3), (cx + 12, 2), (cx + 17, 12), (cx + 15, 26), (cx + 10, 16)]), h=3, r=3)
    arm = P.layer("red")
    arm.shape(poly([(cx + 8, 32), (cx + 13, 31), (cx + 16, 6), (cx + 12, 6)]), h=3, r=3)
    body = body_layer(P, "red", cx, 29, 15, 52)
    head = P.layer("tan", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 17, 10, 11, h=4.5)
    hr = P.layer("hair", smooth=1)
    hr.shape(ellipse(cx, 12, 11, 8) - rect(0, 13, 43, 51), h=3, r=3)
    hand = P.layer("tan")
    hand.ell(cx + 14, 5, 3.5, 3.5, h=3)
    nose = P.layer("tan", ink="shadow", ink_dirs="rdl")
    nose.ell(cx + 0.5, 20, 2, 2, h=2)

    @P.draw
    def face(cv, e):
        for s in (-1, 1):
            eye(cv, cx + s * 4 - (1 if s < 0 else 0), 15)
            brow(cv, cx + s * 2, 12, cx + s * 7, 12)
            m = ellipse(cx + s * 10.5, 23, 2, 2.5)
            cv.fill(border(m), ("gold", 1))   # créoles
        m = ellipse(cx, 25, 4.5, 3.2)          # elle scande
        cv.fill(m, ("ink", 0))
        cv.fill(rect(cx - 3, 23, cx + 3, 23) & m, ("white", 1))
        cv.fill(ellipse(cx, 27, 2, 1) & m, ("red", 1))
        cv.fill(rect(cx - 9, 36, cx - 4, 39), ("white", 1))   # badge
        cv.fill(rect(cx - 8, 37, cx - 5, 38), ("red", 1))
    return P


def retraite():
    """Le retraité qui râle : béret, moustache blanche, sourcils froncés, canne."""
    P = fig(40, 50)
    cx = 19
    body = body_layer(P, "green", cx, 30, 14, 50)
    scarf = P.layer("red")
    scarf.shape(rect(cx - 7, 28, cx + 7, 32), h=1, r=1)
    head = P.layer("skin", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 18, 10, 11, h=4.5)
    beret = P.layer("navy", cast=(1, 2))
    beret.shape(ellipse(cx, 9, 13, 5), h=3, r=3)
    beret.ell(cx + 1, 5, 1.5, 2, h=1)
    nose = P.layer("skin", ink="shadow", ink_dirs="rdl", cast=(1, 1))
    nose.ell(cx + 0.5, 21, 3.5, 3, h=3)
    mu = P.layer("grey")
    mu.shape(ellipse(cx - 3.5, 24.5, 4.5, 2) | ellipse(cx + 3.5, 24.5, 4.5, 2), h=1, r=1)
    cane = P.layer("cord")
    cane.shape(rect(cx + 15, 30, cx + 16, 50) | rect(cx + 11, 29, cx + 16, 30), h=1, r=1)

    @P.draw
    def face(cv, e):
        for s in (-1, 1):
            eye(cv, cx + s * 4 - (1 if s < 0 else 0), 16, shine=False)
            brow(cv, cx + s * 1, 15, cx + s * 7, 12, ("grey", 0))   # sourcils froncés
        cv.fill(line(cx - 3, 28, cx + 3, 28), ("ink", 0))
        cv.put(cx - 4, 29, ("ink", 0))
        cv.put(cx + 4, 29, ("ink", 0))
        for x in (cx - 6, cx + 6):
            cv.put(x, 19, ("skin", 0))
    return P


def patron():
    """Le patron de l'UPE qui compte ses billets : teint orangé, cheveux argent gominés."""
    P = fig(44, 52)
    cx = 22
    body = body_layer(P, "white", cx, 29, 15, 52)
    vest = P.layer("navy", ink=True)
    vest.shape(poly([(cx - 15, 52), (cx - 14, 36), (cx - 7, 30), (cx - 3, 52)]) |
               poly([(cx + 15, 52), (cx + 14, 36), (cx + 7, 30), (cx + 3, 52)]), h=3, r=3)
    head = P.layer("orange", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 17, 10, 11, h=4.5)
    hr = P.layer("grey", smooth=1, gain=1.3)
    hr.shape(ellipse(cx, 11, 11, 7) - rect(0, 12, 43, 51), h=3, r=3)
    hr.ell(cx - 1, 8, 10, 4, h=4, mode="add")
    bills = P.layer("green", ink="always", cast=(1, 1))
    for k in range(5):
        bills.shape(poly([(cx - 11 + k * 4, 34), (cx - 4 + k * 4, 32), (cx - 2 + k * 4, 43), (cx - 9 + k * 4, 45)]),
                    h=1, r=1)
    hands = P.layer("orange")
    hands.ell(cx - 9, 44, 3.5, 3.5, h=3)
    hands.ell(cx + 10, 42, 3.5, 3.5, h=3)
    nose = P.layer("orange", ink="shadow", ink_dirs="rdl")
    nose.ell(cx + 0.5, 20, 2, 2, h=2)

    @P.draw
    def face(cv, e):
        for s in (-1, 1):   # yeux rivés sur les billets
            cv.fill(rect(cx + s * 4 - 1, 17, cx + s * 4, 18), ("ink", 0))
            brow(cv, cx + s * 2, 13, cx + s * 7, 12, ("grey", 0))
        m = ellipse(cx, 24, 7, 3) - rect(0, 0, 43, 23)   # sourire trop blanc
        cv.fill(m, ("white", 1))
        cv.fill(border(m), ("ink", 0))
        for x in range(cx - 4, cx + 5, 2):
            cv.put(x, 25, ("white", 0))
        for k in range(5):
            cv.put(cx - 7 + k * 4, 38, ("green", 2))
            cv.put(cx - 6 + k * 4, 38, ("green", 2))
    return P


def journaliste():
    """Karine Lopez (Chaîne Info 24) : brushing gonflé, sourire figé, micro tendu."""
    P = fig(46, 56)
    cx = 26
    hb = P.layer("gold", cast=(1, 1), gain=1.2)
    hb.shape(ellipse(cx, 18, 17, 16), h=7, r=7)
    body = body_layer(P, "white", cx, 33, 14, 56)
    arm = P.layer("white")
    arm.shape(poly([(cx - 10, 38), (cx - 18, 30), (cx - 21, 33), (cx - 13, 44)]), h=3, r=3)
    head = P.layer("skin", cast=(1, 2))
    head.bias = 0.15
    head.ell(cx, 21, 10, 11, h=4.5)
    hf = P.layer("gold", gain=1.3)
    hf.shape(ellipse(cx - 4, 12, 13, 7) - rect(0, 15, 45, 55), h=4, r=3)
    mic = P.layer("navy")
    mic.shape(rect(1, 21, 3, 31), h=1, r=1)
    mh = P.layer("navy")
    mh.ell(2.5, 18, 3.5, 3.5, h=3)
    cube = P.layer("red", ink=True)
    cube.shape(rect(0, 22, 7, 28), h=1, r=1)
    nose = P.layer("skin", ink="shadow", ink_dirs="rdl")
    nose.ell(cx + 0.5, 24, 2, 2, h=2)

    @P.draw
    def face(cv, e):
        for s in (-1, 1):
            eye(cv, cx + s * 4 - (1 if s < 0 else 0), 19)
            cv.put(cx + s * 6, 18, ("ink", 0))   # cils
            brow(cv, cx + s * 2, 15, cx + s * 6, 14, ("gold", 0))
        m = ellipse(cx, 27, 6.5, 3) - rect(0, 0, 45, 26)
        cv.fill(m, ("white", 1))
        cv.fill(border(m), ("red", 1))
        cv.fill(line(cx - 6, 27, cx + 6, 27), ("ink", 0))
        text(PseudoCanvas(cv), 1, 23, "24", ("white", 1))
    return P


def pigeon(c, x, y, flip=False):
    """Pigeon (10 x 8) : il squatte."""
    rows = ["..kkk.....",
            ".kgggk....",
            "kygwgk....",
            ".kkgggkkk.",
            "..kgggsggk",
            "..kggsssk.",
            "...kkkkk..",
            "....y.y..."]
    c.grid(x, y, rows, {"k": INK, "g": STONE_SH, "w": WHITE, "y": GOLD, "s": SLATE}, flip)


def make_title():
    c = Canvas(W, H, BLUE)
    T0.sky(c)
    T0.building(c)
    T0.courtyard(c)
    T0.topiary(c, 216, GROUND)
    # des pigeons squattent le drapeau et la toiture
    pigeon(c, 150, 24)
    pigeon(c, 34, 46, flip=True)
    pigeon(c, 286, 46)
    # le président salue sur le perron
    blit(c, verdier(), 138, GROUND - 4 - 58 + 2)
    # premier plan : le CRS bâille au pied du perron
    blit(c, crs(), 96, H - 56)
    # pancarte de l'UST
    sx, sy = 16, 48
    c.paint(rect(sx, sy, sx + 46, sy + 22), WHITE)
    c.fill(rect(sx + 1, sy + 1, sx + 45, sy + 2), RED)
    text(c, sx + 24 - text_w("UST", 2) // 2, sy + 5, "UST", RED, 2)
    text(c, sx + 24 - text_w("SMIC +") // 2, sy + 16, "SMIC +", INK)
    c.paint(rect(sx + 26, sy + 23, sx + 27, 88), BROWN)
    blit(c, syndicaliste(), 8, H - 46)
    blit(c, retraite(), 62, H - 44)
    bubble(c, 70, 62, 30, 15, 82, -1)
    text(c, 85 - text_w("GRR!") // 2, 67, "GRR!", INK)
    blit(c, patron(), 212, H - 46)
    bubble(c, 196, 60, 30, 15, 214, 1)
    text(c, 211 - text_w("1 M€") // 2, 65, "1 M€", GREEN)
    blit(c, journaliste(), 268, H - 52)
    return c


TITLE = [("art_title", make_title)]
