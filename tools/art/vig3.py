"""Vignettes d'événements V3 (tâche 3.17) : 64 x 48 px (8 x 6 tuiles), affichées en PAL3.

Style des gueules V3 : volumes éclairés, contour sombre, une « gueule » au premier
plan (traits de traits.py) et un décor qui raconte l'événement. Humour bon enfant,
avec une pointe de gauche. Tout est fictif. Ordre = enum ART_EVT_* (src/ui/ui_art.h).
"""
import math
import random

from raster import ellipse, rect, poly, line, polyline, border
from scene import Scene, chibi, face, big_head, big_face, sweat, stroke, burst, INK
from vignettes import text8, text3, text8_width, FONT3
import traits as T

FONT3.setdefault("Z", ["###", "..#", ".#.", "#..", "###"])

VW, VH = 64, 48
SKIN = [(3, 1, 1), (5, 3, 2), (7, 5, 4), (7, 6, 5)]
WHITE = [(5, 5, 6), (7, 7, 7)]


def S_(ramps):
    return Scene(VW, VH, ramps)


def bust(S, mat, cx, top, w, ink=True):
    l = S.layer(mat, ink=ink)
    l.shape(poly([(cx - w - 3, 50), (cx - w, top + 6), (cx - w + 4, top + 1), (cx - 4, top), (cx + 4, top),
                  (cx + w - 4, top + 1), (cx + w, top + 6), (cx + w + 3, 50)]), h=6, r=5)
    return l


def collar(S, cx, top, mat="white"):
    c = S.layer(mat, ink="always", cast=(1, 1))
    c.shape(poly([(cx - 5, top - 2), (cx, top + 3), (cx - 3, top + 6), (cx - 6, top + 2)]) |
            poly([(cx + 5, top - 2), (cx, top + 3), (cx + 3, top + 6), (cx + 6, top + 2)]), h=1, r=1)
    return c


# --- 1. Grève ----------------------------------------------------------------------

def v_greve():
    S = S_({"bg": [(3, 4, 5), (5, 6, 7)], "skin": [(2, 1, 0), (4, 2, 1), (6, 4, 2), (7, 5, 3)],
            "red": [(3, 0, 0), (5, 0, 0), (7, 1, 1)], "white": WHITE, "hair": [(1, 0, 1), (3, 2, 2)],
            "grey": [(4, 4, 5)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))
        for (x, y, r) in ((6, 40, 6), (16, 44, 6), (56, 42, 7)):       # la foule derrière
            g.fill(ellipse(x, y, r, r), ("grey", 0))
        for x in (4, 58):
            g.rect(x, 10, x, 47, ("hair", 1))

    ban = S.layer("red", cast=(1, 2))
    ban.shape(poly([(2, 3), (61, 2), (61, 14), (2, 15)]), h=2, r=3)
    bust(S, "red", 30, 36, 15)
    collar(S, 30, 36)
    tail = S.layer("hair", cast=(1, 1), smooth=1)
    tail.shape(poly([(30, 14), (40, 13), (45, 22), (43, 32), (38, 24)]), h=3, r=3)
    big_head(S, 28, 27, 10, style="short")
    meg = S.layer("white", cast=(1, 1))
    meg.shape(poly([(36, 33), (50, 26), (52, 40), (37, 37)]), h=3, r=3)
    hand = S.layer("skin")
    hand.ell(38, 38, 3, 3, h=2)

    @S.draw
    def d(cv):
        text8(cv, 31 - text8_width("GRÈVE !") // 2, 5, "GRÈVE !", ("white", 1), shadow=("red", 0))
        big_face(cv, 27, 27, 10, "shout", brow_key=("hair", 0), lashes=True, tongue=("red", 2))
        for sx in (-1, 1):
            m = ellipse(28 + sx * 10.5, 31, 1.6, 2.2)
            pass
        cv.fill(ellipse(51, 33, 1.5, 6), ("grey", 0))
        for (x0, y0, x1, y1) in ((54, 27, 58, 24), (55, 33, 60, 33), (54, 39, 58, 42)):   # le cri
            cv.fill(line(x0, y0, x1, y1), ("white", 1))
        cv.grid(20, 40, ["kkkkk", "kwrwk", "kkkkk"], {"k": INK, "w": ("white", 1), "r": ("red", 2)})   # badge
    return S.render()


# --- 2. Canicule -------------------------------------------------------------------

def v_canicule():
    S = S_({"bg": [(7, 4, 1), (7, 6, 3)], "sun": [(6, 3, 0), (7, 5, 1), (7, 7, 4)], "skin": SKIN[1:],
            "red": [(5, 0, 0), (7, 1, 1)], "white": WHITE, "pink": [(7, 4, 5)], "blue": [(1, 2, 5), (2, 4, 7)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    sun = S.layer("sun", gain=1.2)
    sun.ell(13, 12, 10, 10, h=7)
    th = S.layer("white", cast=(1, 1))
    th.shape(rect(53, 3, 57, 36), h=2, r=2)
    th.ell(55, 38, 5, 5, h=3)
    merc = S.layer("red")
    merc.shape(rect(54, 6, 56, 35), h=1, r=1)
    merc.ell(55, 38, 3.2, 3.2, h=2)
    bust(S, "blue", 33, 38, 14)
    big_head(S, 33, 28, 11, style="bald", nose=1.4)
    ice = S.layer("pink", cast=(1, 1))
    ice.ell(19, 30, 3.5, 3.5, h=2)
    cone = S.layer("sun")
    cone.shape(poly([(15, 31), (23, 31), (19, 41)]), h=1, r=1)
    hand = S.layer("skin")
    hand.ell(19, 41, 3, 2.5, h=2)

    @S.draw
    def d(cv):
        for k in range(8):
            a = math.radians(k * 45 + 10)
            cv.fill(line(int(13 + 12 * math.cos(a)), int(12 + 12 * math.sin(a)),
                         int(13 + 15 * math.cos(a)), int(12 + 15 * math.sin(a))), ("sun", 0))
        cv.fill(rect(6, 9, 11, 12) | rect(15, 9, 20, 12) | line(11, 9, 15, 9), INK)   # lunettes de soleil
        cv.put(7, 9, ("white", 1))
        cv.put(16, 9, ("white", 1))
        cv.fill(line(8, 16, 10, 17) | line(10, 17, 16, 17) | line(16, 17, 18, 15), INK)   # le soleil ricane
        big_face(cv, 33, 28, 11, "worried")
        for (x, y) in ((44, 18), (22, 20), (43, 28)):
            sweat(cv, x, y)
        for (x, y) in ((17, 33), (17, 34), (21, 34), (21, 35), (21, 36)):   # la glace fond
            cv.put(x, y, ("pink", 0))
        text3(cv, 42, 1, "45", ("red", 1))
        cv.put(50, 1, ("red", 1))
    return S.render()


# --- 3. Inondation -----------------------------------------------------------------

def v_inondation():
    S = S_({"bg": [(1, 1, 2), (3, 3, 5)], "water": [(1, 2, 4), (2, 4, 6), (5, 6, 7)], "wall": [(5, 4, 3), (7, 6, 5)],
            "roof": [(4, 0, 0), (6, 1, 1)], "skin": SKIN[1:], "yellow": [(5, 4, 0), (7, 6, 1)], "white": WHITE,
            "hair": [(1, 0, 1)]})

    @S.background
    def bg(g):
        g.gradient(0, 30, ("bg", 0), ("bg", 1))
        for k in range(30):
            x, y = (k * 23) % 64, (k * 13) % 30
            g.fill(line(x, y, x - 1, y + 3), ("water", 1))

    house = S.layer("wall", cast=(1, 1))
    house.shape(rect(3, 16, 20, 33), h=3, r=3)
    roof = S.layer("roof", cast=(1, 2))
    roof.shape(poly([(0, 17), (11, 7), (23, 17)]), h=3, r=3)
    big_head(S, 41, 24, 9, style="short", nose=1.2)
    tub = S.layer("white", cast=(1, 2))
    tub.shape(poly([(25, 30), (60, 30), (56, 41), (29, 41)]), h=4, r=3)
    water = S.layer("water", ink=False, gain=1.5)
    water.shape(rect(0, 36, 63, 47), h=2, r=1)
    for k in range(9):
        water.ell(4 + k * 8, 37, 4, 1.6, h=1.2, mode="add")
    duck = S.layer("yellow", cast=(1, 1))
    duck.ell(12, 40, 4, 2.5, h=2)
    duck.ell(14, 37, 2.2, 2.2, h=1.5)

    @S.draw
    def d(cv):
        cv.fill(rect(6, 21, 9, 25) | rect(13, 21, 16, 25), ("water", 0))
        cv.put(16, 37, INK)
        cv.put(17, 38, ("roof", 1))
        big_face(cv, 41, 24, 9, "worried")
        stroke(cv, [(53, 10), (53, 31)])          # le parapluie, au cas où
        m = ellipse(53, 12, 9, 5.5) - rect(0, 13, 99, 99)
        cv.fill(m, ("roof", 1))
        cv.fill(border(m), INK)
        cv.fill(line(48, 9, 51, 8), ("white", 1))
        burst(cv, 30, 4, 3, ("yellow", 1), ("white", 1))
        for x in (30, 35, 46, 51):
            cv.put(x, 31, ("water", 2))
    return S.render()


# --- 4. Scandale -------------------------------------------------------------------

def v_scandale():
    S = S_({"bg": [(1, 1, 2), (3, 1, 4)], "paper": [(5, 5, 4), (7, 7, 6)], "skin": SKIN[1:],
            "suit": [(1, 1, 2), (2, 2, 4)], "red": [(5, 0, 0), (7, 1, 1)], "green": [(1, 3, 1), (3, 6, 2)],
            "white": WHITE, "hair": [(3, 3, 4), (5, 5, 6)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    paper = S.layer("paper", cast=(1, 2))
    paper.shape(poly([(2, 3), (36, 2), (37, 25), (3, 26)]), h=2, r=2)
    bust(S, "suit", 47, 36, 13)
    big_head(S, 47, 24, 10, hair="hair", style="short", nose=1.3)
    case = S.layer("suit", cast=(1, 2))
    case.shape(rect(34, 27, 60, 39), h=3, r=2)
    for (x, y) in ((5, 31), (14, 36), (23, 30), (26, 40)):
        bill = S.layer("green", ink=True)
        bill.shape(poly([(x, y), (x + 9, y - 2), (x + 10, y + 3), (x + 1, y + 5)]), h=1, r=1)

    @S.draw
    def d(cv):
        text3(cv, 5, 5, "SCANDALE", ("red", 1))
        for y in (12, 15, 18, 21):
            cv.fill(line(5, y, 22, y), ("paper", 0))
        cv.fill(rect(25, 11, 34, 22), ("suit", 1))     # photo floue
        cv.fill(ellipse(29.5, 15, 2.5, 2.5) | rect(26, 18, 33, 22), ("hair", 0))
        cv.fill(rect(36, 28, 58, 28), ("suit", 1))
        cv.fill(rect(44, 25, 49, 26), INK)              # poignée
        for sx in (-1, 1):                               # les yeux dépassent
            T.eye(cv, 47 + sx * 4, 22, w=4, h=2, side=sx, open_=1.0, look=2, iris=INK, iris_r=1.0)
        sweat(cv, 58, 15)
        burst(cv, 58, 5, 5, ("white", 1), ("white", 1))
        burst(cv, 40, 4, 3, ("white", 1), ("white", 1))
        for (x, y) in ((5, 31), (14, 36), (23, 30), (26, 40)):
            cv.put(x + 4, y + 1, ("green", 0))
            cv.put(x + 5, y + 2, ("green", 0))
    return S.render()


# --- 5. Sommet international -------------------------------------------------------

def v_sommet():
    S = S_({"bg": [(2, 3, 5), (3, 4, 6)], "skin": SKIN, "suit": [(1, 1, 2), (2, 2, 3)], "white": WHITE,
            "red": [(6, 1, 1)], "blue": [(1, 2, 6)], "yellow": [(7, 6, 1)], "hair": [(1, 0, 1), (5, 5, 5)]})

    @S.background
    def bg(g):
        g.gradient(0, 30, ("bg", 1), ("bg", 0))
        for k, col in enumerate((("red", 0), ("blue", 0), ("yellow", 0), ("white", 1))):
            x = 8 + k * 14       # drapeaux génériques
            g.rect(x, 2, x, 12, ("hair", 1))
            g.rect(x + 1, 2, x + 7, 6, col)

    bust(S, "suit", 15, 36, 12)
    bust(S, "suit", 49, 36, 12)
    big_head(S, 15, 26, 9, style="short")
    big_head(S, 49, 26, 9, hair="hair", style="bald", jaw=1.1)
    arms = S.layer("suit", cast=(1, 1))
    arms.shape(poly([(22, 38), (30, 36), (30, 41), (23, 43)]) | poly([(42, 38), (34, 36), (34, 41), (41, 43)]),
               h=2, r=2)
    hands = S.layer("skin", cast=(1, 1))
    hands.shape(ellipse(32, 38.5, 4.5, 3), h=2, r=2)
    table = S.layer("hair", cast=(1, 1))
    table.shape(rect(0, 43, 63, 47), h=2, r=1)

    @S.draw
    def d(cv):
        big_face(cv, 15, 26, 9, "grin")
        big_face(cv, 49, 26, 9, "grin", brow_key=("hair", 1))
        sweat(cv, 7, 18)
        sweat(cv, 58, 18)
        # derrière le dos : les doigts croisés
        cv.grid(58, 39, [".k.k", "kokok", "kooo", ".kk."], {"k": INK, "o": ("skin", 2)})
        burst(cv, 32, 30, 3, ("yellow", 0), ("white", 1))   # flash des photographes
    return S.render()


# --- 6. Bourse, crise financière -----------------------------------------------------

def v_bourse():
    S = S_({"bg": [(0, 1, 2), (1, 2, 3)], "grid": [(1, 3, 4)], "red": [(4, 0, 0), (7, 1, 1)], "skin": SKIN,
            "suit": [(1, 1, 2), (2, 2, 4)], "white": WHITE, "hair": [(3, 1, 1), (5, 3, 2)], "gold": [(7, 6, 2)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))
        for x in range(4, 64, 8):
            g.rect(x, 2, x, 46, ("grid", 0))
        for y in range(4, 47, 6):
            g.rect(2, y, 61, y, ("grid", 0))
        pts = [(2, 6), (9, 4), (15, 10), (22, 7), (29, 14), (36, 11), (44, 22), (50, 19), (58, 32)]
        for i in range(len(pts) - 1):
            m = line(*pts[i], *pts[i + 1])
            g.fill(m | {(x, y + 1) for (x, y) in m}, ("red", 1))
        g.fill(poly([(54, 31), (62, 36), (60, 27)]), ("red", 1))

    bust(S, "suit", 26, 38, 14)
    collar(S, 26, 38)
    tie = S.layer("red", cast=(1, 1))
    tie.shape(poly([(25, 39), (28, 39), (29, 48), (24, 48)]), h=1, r=1)
    big_head(S, 26, 27, 11, style="spiky", nose=1.2)
    hands = S.layer("skin", cast=(1, 1))
    hands.ell(13, 20, 3.5, 3.5, h=2)
    hands.ell(39, 20, 3.5, 3.5, h=2)

    @S.draw
    def d(cv):
        big_face(cv, 26, 27, 11, "shout", brow_key=("hair", 0), tongue=("red", 1))
        sweat(cv, 41, 27)
        sweat(cv, 9, 28)
        text3(cv, 44, 41, "-12%", ("red", 1))
        text3(cv, 45, 2, "CAC", ("gold", 0))
    return S.render()


# --- 7. Santé ----------------------------------------------------------------------

def v_sante():
    S = S_({"bg": [(5, 6, 6), (6, 7, 7)], "white": [(4, 5, 5), (7, 7, 7)], "skin": SKIN,
            "hair": [(3, 3, 2), (6, 6, 4)], "blue": [(1, 3, 5), (2, 5, 6)], "red": [(6, 1, 1)],
            "bag": [(4, 2, 4)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))
        g.rect(3, 3, 15, 15, ("blue", 0))
        g.rect(5, 5, 6, 13, ("white", 1))
        g.rect(12, 5, 13, 13, ("white", 1))
        g.rect(7, 8, 11, 9, ("white", 1))

    bust(S, "white", 32, 37, 15)
    scrub = S.layer("blue", ink="always")
    scrub.shape(poly([(26, 36), (38, 36), (32, 44)]), h=1, r=1)
    big_head(S, 32, 26, 11, hair="hair", style="bun")
    cup = S.layer("white", cast=(1, 1))
    cup.shape(rect(47, 36, 54, 44), h=2, r=1)
    hand = S.layer("skin")
    hand.ell(46, 41, 3, 3, h=2)

    @S.draw
    def d(cv):
        big_face(cv, 32, 26, 11, "sleep", brow_key=("hair", 0))
        for sx in (-1, 1):   # cernes violets
            for x in range(32 + sx * 5 - 2, 32 + sx * 5 + 3):
                cv.put(x, 29, ("bag", 0))
        cv.fill(rect(48, 37, 53, 37), ("hair", 0))
        for (x, y) in ((50, 34), (51, 32), (50, 30)):
            cv.put(x, y, ("white", 0))
        cv.curve((22, 37), (20, 44), (28, 47), INK)     # stéthoscope
        cv.curve((42, 37), (44, 44), (36, 47), INK)
        text3(cv, 45, 3, "Z", ("blue", 0))
        text3(cv, 50, 8, "Z", ("blue", 0))
        text3(cv, 2, 20, "36H", ("red", 0))
        text3(cv, 2, 26, "DE", ("red", 0))
        text3(cv, 2, 32, "GARDE", ("red", 0))
    return S.render()


# --- 8. Pollution, climat ----------------------------------------------------------

def v_pollution():
    S = S_({"bg": [(3, 3, 2), (5, 5, 3)], "smoke": [(2, 2, 2), (4, 4, 3)], "brick": [(3, 1, 1), (5, 2, 1)],
            "water": [(1, 2, 3), (2, 3, 4)], "sludge": [(1, 2, 0), (3, 4, 1)], "ice": [(4, 5, 6), (6, 7, 7), (7, 7, 7)]})

    @S.background
    def bg(g):
        g.gradient(0, 34, ("bg", 0), ("bg", 1))

    smoke = S.layer("smoke", ink=False, gain=1.2)
    for (x, y, r) in ((8, 5, 5), (15, 4, 6), (23, 6, 5)):
        smoke.ell(x, y, r, r * 0.8, h=3)
    fac = S.layer("brick", cast=(1, 2))
    fac.shape(poly([(0, 32), (0, 22), (8, 17), (8, 22), (16, 17), (16, 32)]), h=3, r=2)
    fac.shape(rect(5, 8, 9, 32), h=2, r=2)
    water = S.layer("water", ink=False)
    water.shape(rect(0, 33, 63, 47), h=1, r=1)
    sludge = S.layer("sludge")
    sludge.shape(poly([(0, 33), (18, 33), (26, 38), (0, 42)]), h=1, r=1)
    floe = S.layer("ice", cast=(1, 2))
    floe.shape(poly([(28, 34), (58, 32), (62, 39), (31, 40)]), h=3, r=2)
    bear = S.layer("ice", cast=(1, 2), gain=1.1)
    bear.ell(42, 26, 9, 7, h=6)                 # corps
    bear.ell(52, 18, 6.5, 6, h=5)               # tête
    bear.ell(56, 21, 3, 2.5, h=2)               # museau
    bear.ell(48, 13, 1.8, 1.8, h=1)
    bear.ell(55, 13, 1.8, 1.8, h=1)
    bear.ell(37, 32, 3, 2.5, h=2)
    bear.ell(47, 32, 3, 2.5, h=2)

    @S.draw
    def d(cv):
        cv.put(51, 17, INK)                      # l'ours, perplexe sur son glaçon qui fond
        cv.put(51, 16, INK)
        cv.put(55, 17, INK)
        cv.put(55, 16, INK)
        cv.fill(line(49, 14, 51, 15) | line(55, 15, 57, 14), INK)
        cv.fill(rect(58, 20, 59, 21), INK)
        cv.fill(line(54, 24, 57, 23), INK)
        sweat(cv, 60, 9, ("ice", 2))
        for (x, y) in ((33, 41), (40, 42), (50, 41), (58, 42), (29, 44), (45, 45)):
            cv.fill(line(x, y, x + 3, y), ("ice", 0))   # gouttes du glaçon
        for x in range(2, 22, 5):
            cv.put(x, 37, ("sludge", 1))
        text3(cv, 33, 3, "+2", ("brick", 1))
        cv.put(41, 3, ("brick", 1))
    return S.render()


# --- 9. Sécurité -------------------------------------------------------------------

def v_securite():
    S = S_({"bg": [(0, 0, 2), (1, 1, 3)], "build": [(1, 1, 2), (2, 2, 3)], "light": [(6, 6, 2)],
            "navy": [(1, 0, 1), (1, 1, 3), (2, 2, 5)], "skin": SKIN, "white": WHITE, "blue": [(1, 3, 7)],
            "red": [(6, 1, 1)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))
        for (x0, x1, top) in ((0, 12, 10), (13, 22, 18), (42, 52, 14), (53, 63, 8)):
            g.rect(x0, top, x1, 47, ("build", 0))
            for y in range(top + 3, 46, 5):
                for x in range(x0 + 2, x1 - 1, 4):
                    g.rect(x, y, x + 1, y + 1, ("light", 0) if (x * 3 + y) % 4 else ("build", 1))

    bust(S, "navy", 32, 37, 16)
    big_head(S, 32, 25, 11, hair="navy", style="crew", jaw=1.3, nose=1.3)
    shield = S.layer("white", cast=(1, 1))
    shield.shape(rect(2, 30, 15, 47), h=2, r=2)

    @S.draw
    def d(cv):
        big_face(cv, 32, 25, 11, "angry", brow_key=("navy", 1), bags=1)
        for x in range(23, 42, 2):
            cv.put(x, 14, ("navy", 2))
        cv.fill(rect(3, 34, 14, 35), ("navy", 1))
        cv.grid(40, 40, ["kkk", "bwr", "kkk"], {"k": INK, "b": ("blue", 0), "w": ("white", 1), "r": ("red", 0)})
        burst(cv, 58, 4, 4, ("blue", 0), ("white", 1))     # gyrophares
        burst(cv, 6, 4, 4, ("red", 0), ("white", 1))
    return S.render()


# --- 10. Numérique ------------------------------------------------------------------

def v_numerique():
    S = S_({"bg": [(1, 1, 2), (2, 2, 3)], "screen": [(0, 2, 1), (1, 4, 2), (3, 7, 3)], "case": [(3, 3, 4), (5, 5, 6)],
            "hood": [(1, 0, 1), (2, 2, 2), (3, 3, 4)], "cat": [(4, 2, 0), (6, 4, 1), (7, 6, 3)], "red": [(6, 1, 1)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    mon = S.layer("case", cast=(1, 2))
    mon.shape(rect(20, 2, 61, 29), h=3, r=2)
    mon.shape(rect(37, 29, 44, 34), h=2, r=1)
    scr = S.layer("screen", ink=True, gain=0.4)
    scr.shape(rect(23, 5, 58, 26), h=1, r=6)
    desk = S.layer("case", cast=(1, 1))
    desk.shape(rect(0, 38, 63, 47), h=2, r=1)
    kb = S.layer("hood", cast=(1, 1))
    kb.shape(rect(24, 35, 56, 40), h=1, r=1)
    cat = S.layer("cat", cast=(1, 1), gain=1.2)
    cat.ell(40, 35, 10, 5, h=4)
    cat.ell(51, 29, 5.5, 5, h=4)
    cat.shape(poly([(47, 26), (48, 21), (51, 25)]) | poly([(52, 25), (55, 21), (56, 26)]), h=1, r=1)
    cat.shape(poly([(30, 35), (24, 30), (26, 29), (31, 33)]), h=1, r=1)
    hood = S.layer("hood", cast=(1, 2))
    hood.shape(poly([(0, 47), (0, 24), (5, 13), (13, 13), (17, 24), (18, 47)]), h=4, r=4)

    @S.draw
    def d(cv):
        text3(cv, 26, 8, "404", ("screen", 2))
        text3(cv, 26, 15, "ERREUR", ("screen", 2))
        cv.fill(rect(26, 22, 54, 22), ("screen", 0))
        for x in (49, 53):     # le chat fait la sieste sur le clavier : tout plante
            cv.fill(line(x - 1, 29, x + 1, 29), INK)
        cv.put(51, 31, ("red", 0))
        for k in range(3):
            cv.fill(line(36 + k * 4, 33, 37 + k * 4, 37), ("cat", 0))
        cv.put(6, 21, ("screen", 2))     # les yeux dans la capuche
        cv.put(11, 21, ("screen", 2))
    return S.render()


# --- 11. Éducation -----------------------------------------------------------------

def v_education():
    S = S_({"bg": [(5, 4, 3), (6, 5, 4)], "board": [(1, 2, 1), (2, 3, 2)], "wood": [(3, 1, 0), (5, 3, 1)],
            "skin": SKIN, "hair": [(4, 4, 4), (7, 7, 7)], "suit": [(2, 2, 1), (3, 3, 2)], "red": [(6, 1, 1)]})
    S.P.ramps["white"] = [(4, 4, 4), (7, 7, 7)]

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    board = S.layer("board", cast=(1, 2), gain=0.3)
    board.shape(rect(2, 2, 37, 30), h=1, r=4)
    frame = S.layer("wood", ink=True)
    frame.shape(rect(1, 30, 38, 33), h=1, r=1)
    bust(S, "suit", 49, 38, 13)
    hb = S.layer("hair", cast=(1, 2), smooth=1)
    for k in range(9):
        a = math.radians(170 + 200 * k / 8)
        x, y = 49 + 12 * math.cos(a), 24 + 11 * math.sin(a)
        hb.shape(poly([(x - 2, y), (x + 2, y), (x + 5 * math.cos(a), y + 5 * math.sin(a))]), h=2, r=2)
    big_head(S, 49, 24, 10, hair="hair", style="bald", nose=1.2)
    beard = S.layer("hair", smooth=1)
    beard.shape(poly([(41, 29), (57, 29), (54, 38), (49, 44), (44, 38)]), h=2, r=2)
    beard.cut(ellipse(49, 31, 3, 1.3))
    pen = S.layer("red")
    pen.shape(poly([(31, 15), (33, 14), (39, 29), (37, 30)]), h=1, r=1)
    hand = S.layer("skin")
    hand.ell(38, 31, 2.5, 2.5, h=2)

    @S.draw
    def d(cv):
        text3(cv, 5, 5, "2+2=5", ("hair", 1))
        stroke(cv, [(4, 4), (24, 10)], ("red", 0))
        stroke(cv, [(4, 10), (24, 4)], ("red", 0))
        text3(cv, 5, 15, "ZÉRO", ("red", 0))
        text3(cv, 5, 22, "VU !", ("hair", 1))
        big_face(cv, 49, 24, 10, "angry", brow_key=("hair", 1))
        for sx in (-1, 1):     # petites lunettes rondes
            m = ellipse(49 + sx * 4.2, 24, 3, 2.6)
            cv.fill(border(m), INK)
        cv.fill(line(48, 23, 50, 23), INK)
        cv.grid(8, 40, ["kkkkk", "kwwwk", "kkkkk"], {"k": INK, "w": ("hair", 1)})
    return S.render()


# --- 12. Énergie -------------------------------------------------------------------

def v_energie():
    S = S_({"bg": [(0, 0, 1), (1, 1, 2), (2, 2, 3)], "glow": [(5, 3, 0), (7, 6, 2)], "skin": SKIN,
            "hair": [(1, 0, 1), (3, 1, 1)], "pyj": [(2, 2, 4), (3, 4, 6)], "paper": [(5, 5, 4), (7, 7, 6)],
            "red": [(6, 1, 1)]})
    S.P.ramps["white"] = [(5, 5, 4), (7, 7, 6)]

    @S.background
    def bg(g):
        g.rect(0, 0, 63, 47, ("bg", 0))
        g.fill(ellipse(14, 30, 24, 22), ("bg", 1))
        g.fill(ellipse(14, 30, 14, 13), ("bg", 2))

    bust(S, "pyj", 30, 37, 14)
    big_head(S, 30, 26, 10, style="short", nose=1.2)
    bill = S.layer("paper", cast=(1, 2))
    bill.shape(poly([(40, 16), (61, 14), (62, 42), (41, 44)]), h=2, r=2)
    hand = S.layer("skin")
    hand.ell(41, 38, 3, 3, h=2)
    candle = S.layer("paper", cast=(1, 1))
    candle.shape(rect(8, 33, 12, 47), h=1, r=1)
    flame = S.layer("glow", ink=False)
    flame.ell(10, 29, 2.2, 3.5, h=2)

    @S.draw
    def d(cv):
        big_face(cv, 30, 26, 10, "o", look=2)
        sweat(cv, 20, 18)
        text3(cv, 44, 18, "PRIX", INK)
        text3(cv, 44, 27, "999€", ("red", 0))
        cv.fill(line(44, 35, 58, 34), ("paper", 0))
        cv.fill(line(44, 38, 56, 37), ("paper", 0))
        cv.put(10, 27, ("glow", 1))
    return S.render()


# --- 13. Agriculture ---------------------------------------------------------------

def v_agriculture():
    S = S_({"bg": [(4, 5, 7), (6, 7, 7)], "grass": [(1, 3, 1), (2, 5, 1), (4, 6, 2)], "cow": [(4, 4, 5), (6, 6, 6), (7, 7, 7)],
            "pink": [(6, 3, 4), (7, 5, 5)], "skin": SKIN[1:], "cap": [(5, 1, 1)], "blue": [(1, 2, 4)]})
    S.P.ramps["white"] = [(6, 6, 6), (7, 7, 7)]

    @S.background
    def bg(g):
        g.gradient(0, 30, ("bg", 1), ("bg", 0))
        g.rect(0, 31, 63, 47, ("grass", 1))
        for x in range(0, 64, 5):
            g.put(x, 33 + x % 3, ("grass", 2))

    cowb = S.layer("cow", cast=(1, 2))
    cowb.ell(16, 34, 15, 9, h=7)
    cowb.shape(rect(5, 37, 9, 47) | rect(23, 37, 27, 47), h=2, r=2)
    head = S.layer("cow", cast=(1, 2), gain=1.1)
    head.ell(36, 24, 9, 9, h=6)
    head.ell(28, 17, 3, 1.8, h=1)
    head.ell(44, 17, 3, 1.8, h=1)
    muz = S.layer("pink", cast=(1, 1))
    muz.ell(36, 31, 6.5, 4, h=3)
    bust(S, "blue", 55, 38, 9)
    chibi(S, 55, 29, 7, hair="cap", style="cap")

    @S.draw
    def d(cv):
        for (x, y, r) in ((10, 31, 4), (20, 36, 3), (37, 21, 3)):
            for (px, py) in ellipse(x, y, r, r * 0.8):
                k = cv.get(px, py)
                if k and k[0] == "cow":
                    cv.put(px, py, INK)
        for sx in (-1, 1):   # la vache boude
            T.eye(cv, 36 + sx * 4, 25, w=4, h=2.5, side=sx, open_=0.6, tilt=1.5, iris=INK, iris_r=1.0)
            cv.fill(line(36 + sx * 2, 21, 36 + sx * 6, 20), INK)
        cv.put(34, 31, INK)
        cv.put(38, 31, INK)
        cv.fill(line(34, 34, 38, 34), ("pink", 0))
        face(cv, 55, 29, 7, "worried")
        text3(cv, 2, 2, "LAIT", ("blue", 0))
        text3(cv, 2, 8, "-30%", ("cap", 0))
    return S.render()


# --- 14. Industrie -----------------------------------------------------------------

def v_industrie():
    S = S_({"bg": [(3, 3, 4), (4, 4, 5)], "brick": [(3, 1, 1), (5, 2, 1), (6, 3, 2)], "skin": SKIN,
            "white": WHITE, "red": [(6, 1, 1)], "blue": [(1, 2, 4), (2, 3, 6)], "gold": [(5, 4, 0), (7, 6, 1)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    fac = S.layer("brick", cast=(1, 2))
    fac.shape(rect(1, 6, 36, 40), h=3, r=2)
    gate = S.layer("bg", ink=True)
    gate.shape(rect(6, 19, 31, 40), h=1, r=1)
    sign = S.layer("white", cast=(1, 1))
    sign.shape(rect(4, 9, 33, 16), h=1, r=1)
    bust(S, "blue", 50, 38, 11)
    big_head(S, 50, 27, 9, hair="blue", style="short")
    box = S.layer("brick", cast=(1, 1))
    box.shape(rect(40, 39, 60, 47), h=2, r=1)
    bag = S.layer("gold", cast=(1, 1))
    bag.ell(18, 42, 5, 4.5, h=3)
    bag.ell(18, 37, 2, 1.5, h=1)

    @S.draw
    def d(cv):
        text3(cv, 8, 11, "FERMÉ", ("red", 0))
        for x in range(8, 31, 3):
            cv.fill(rect(x, 20, x, 39), ("bg", 0))
        cv.grid(17, 26, [".kk.", "k..k", "kyyk", "kyyk"], {"k": INK, "y": ("gold", 1)})   # cadenas
        big_face(cv, 50, 27, 9, "sad")
        text3(cv, 16, 40, "€", INK)
        for (x0, y0) in ((25, 42), (27, 38), (25, 46)):   # les dividendes, eux, filent
            cv.fill(line(x0, y0, x0 + 4, y0), ("white", 1))
        cv.fill(line(42, 41, 58, 41), ("brick", 0))
    return S.render()


# --- 15. Fête, bonne nouvelle -------------------------------------------------------

def v_fete():
    S = S_({"bg": [(1, 0, 3), (2, 1, 4)], "gold": [(4, 3, 0), (7, 5, 0), (7, 7, 4)], "skin": SKIN[1:],
            "red": [(6, 1, 1)], "blue": [(2, 3, 7)], "green": [(2, 6, 2)], "white": WHITE, "hair": [(1, 0, 1)]})

    @S.background
    def bg(g):
        g.gradient(0, 47, ("bg", 1), ("bg", 0))

    cup = S.layer("gold", cast=(1, 2), gain=1.3)
    cup.shape(poly([(21, 4), (43, 4), (40, 18), (35, 22), (29, 22), (24, 18)]), h=5, r=5)
    cup.shape(rect(30, 22, 34, 27), h=2, r=2)
    cup.shape(rect(25, 27, 39, 30), h=2, r=2)
    handles = S.layer("gold", cast=(1, 1))
    handles.shape((ellipse(20, 10, 4, 4.5) - ellipse(20, 10, 2, 2.5)) | (ellipse(44, 10, 4, 4.5) - ellipse(44, 10, 2, 2.5)),
                  h=1, r=1)
    for k, (x, col) in enumerate(((9, "red"), (25, "blue"), (39, "green"), (55, "red"))):
        bust(S, col, x, 44, 7)
        chibi(S, x, 37, 6.5, hair="hair", style="short" if k % 2 else "curly")

    @S.draw
    def d(cv):
        for x in (9, 25, 39, 55):
            face(cv, x, 37, 6.5, "happy")
        rnd = random.Random(7)
        for _ in range(45):
            x, y = rnd.randrange(2, 62), rnd.randrange(2, 30)
            k = rnd.choice((("red", 0), ("blue", 0), ("green", 0), ("gold", 2), ("white", 1)))
            if cv.get(x, y) and cv.get(x, y)[0] == "bg":
                cv.put(x, y, k)
        burst(cv, 8, 8, 5, ("gold", 2), ("white", 1))
        burst(cv, 56, 6, 5, ("blue", 0), ("white", 1))
    return S.render()


_ORDER = [("greve", v_greve), ("canicule", v_canicule), ("inondation", v_inondation),
          ("scandale", v_scandale), ("sommet", v_sommet), ("bourse", v_bourse), ("sante", v_sante),
          ("pollution", v_pollution), ("securite", v_securite), ("numerique", v_numerique),
          ("education", v_education), ("energie", v_energie), ("agriculture", v_agriculture),
          ("industrie", v_industrie), ("fete", v_fete)]


def _entry(name, fn):
    img, pal = fn()
    return ("art_evt_" + name, (lambda c=img: c), pal)


def vignettes():
    return [_entry(n, f) for n, f in _ORDER]
