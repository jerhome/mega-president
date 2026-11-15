"""Gueules V3 (3.09) : les ministres (Régis Fermont, à l'Intérieur, est dans g_demo.py).

Tous les personnages sont fictifs (casting : docs/contenu/univers.md, section 5.5).
"""
import math

from raster import ellipse, poly, rect, line, border
import traits as T
from persos import (P_, CX, INK, bust, neck, skull, hair, nose_layer, nostrils, face,
                    comb, curl_marks, stripes, round_glasses, rect_glasses, lipstick,
                    sweat_at, vein_at, blush_at, both)

WHITE = [(5, 5, 6), (7, 7, 7)]


# --- Gilles Fontaine, Économie ------------------------------------------------------------

def min_economie():
    """Gilles Fontaine : visage de hibou derrière d'épaisses lunettes rondes, yeux énormes,
    cheveux roux clairsemés, oreilles décollées, menton fuyant, trois stylos en poche."""
    P = P_({
        "bg": [(4, 6, 5)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(4, 1, 0), (6, 3, 1), (7, 5, 2)],
        "suit": [(1, 0, 1), (2, 2, 2), (4, 4, 3)],
        "white": WHITE,
        "tie": [(1, 1, 3), (5, 1, 1)],
        "iris": [(3, 2, 0)],
    })
    neck(P, w=11, top=58, bottom=76)
    s = bust(P, top=76, wide=26, tie="tie", vw=5, vdepth=11)
    head = skull(P, cy=38, rx=17, ry=20, jaw=(11, 10, 54), ear_y=42, ear_dx=19, ear_r=(5, 7),
                 sockets=(8, 2))
    hm = ellipse(CX, 24, 17, 9) - ellipse(CX, 30, 12, 9)
    hm |= ellipse(CX - 15, 30, 3, 6) | ellipse(CX + 15, 30, 3, 6)
    hr = hair(P, hm, h=2, r=2)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 43), (CX + 1, 43), (CX + 2, 51), (CX, 53), (CX - 2, 51)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        for k in range(3):   # mèches rabattues, rares
            cv.curve((CX - 8 + k * 6, 22), (CX - 4 + k * 6, 15), (CX + k * 6, 20), ("hair", 1))
        stripes(cv, [l for l in P.layers if l.mat == "tie"][0], period=3)
        for k, c in enumerate((("tie", 1), ("tie", 0), ("white", 1))):   # trois stylos
            x = CX + 14 + k * 2
            cv.fill(line(x, 80, x, 84), c)
            cv.put(x, 79, INK)
        cv.fill(line(CX + 12, 84, CX + 21, 84), INK)
        nostrils(cv, 52, 2)
        for sx in (-1, 1):
            cv.fill(line(CX + sx * 18, 39, CX + sx * 19, 45), ("skin", 1))   # pavillon

    def lunettes(cv, e):
        if e == "furieux":   # de travers
            for sx, dy in ((-1, -1), (1, 2)):
                m = ellipse(CX + sx * 8, 40 + dy, 7.5, 7)
                cv.fill(border(m) | border(m - border(m)), INK)
            cv.fill(line(CX - 1, 39, CX + 1, 41), INK)
            return
        for sx in (-1, 1):
            m = ellipse(CX + sx * 8, 40, 7.5, 7)
            cv.fill(border(m) | border(m - border(m)), INK)
            cv.put(CX + sx * 8 - 4, 36, ("white", 1))
            cv.put(CX + sx * 8 - 3, 35, ("white", 1))
        cv.fill(line(CX - 1, 38, CX + 1, 38), INK)

    face(P, eye_y=40, eye_dx=8, eye_w=10, eye_h=5.5, iris_r=2.6, brow_y=31, brow_w=9, brow_thick=1,
         brow_arch=2, brow_key=("hair", 0), glasses=lunettes,
         eyes={"neutre": {"open_": 1.0}, "content": {"open_": 1.0, "raise_": 0.5},
               "furieux": {"open_": 1.0, "tilt": 0, "right": {"open_": 0.4, "tilt": 1.5}},
               "gene": {"open_": 0.4}},
         brows={"neutre": (-1, -1), "content": (-3, -3), "furieux": ((2, -1), (4, 0))},
         mouth_y=58,
         mouths={"neutre": {"kind": "line", "w": 7, "curve": -0.3},
                 "content": {"kind": "smile", "w": 10, "open_": 2.5, "curve": 1.5},
                 "furieux": {"kind": "grit", "w": 9, "curve": -1},
                 "gene": {"kind": "wavy", "w": 7}},
         extras={"content": lambda cv: [cv.grid(CX + sx * 8 - 1, 37, [".y.", "yyy", ".y."], {"y": ("white", 1)})
                                        for sx in (-1, 1)],
                 "gene": sweat_at(CX + 16, 24)})
    return P


# --- Odile Varenne, Santé -----------------------------------------------------------------

def min_sante():
    """Odile Varenne : chignon bas gris-blond piqué d'un stylo, cernes violets, rides de
    rire, lunettes relevées sur le front, blouse blanche sous le blazer, stéthoscope."""
    P = P_({
        "bg": [(5, 6, 6)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(3, 3, 3), (5, 5, 4), (7, 6, 5)],   # gris-blond (DA : plus de kaki « casque »)
        "suit": [(1, 2, 3), (2, 3, 4), (3, 4, 5)],
        "white": [(5, 5, 6), (7, 7, 7)],
        "bag": [(4, 2, 4)],
        "iris": [(1, 2, 3)],
        "acc": [(4, 2, 4)],
    })
    bun = P.layer("hair", cast=(1, 2), gain=1.3)
    bun.ell(CX + 14, 50, 7, 6, h=5)
    neck(P, w=12, top=58, bottom=76)
    s = bust(P, top=76, wide=29, suit="suit", tie=None, vw=8, vdepth=16, collar=False)
    coat = P.layer("white", ink="always")
    coat.shape(poly([(CX - 10, 73), (CX - 2, 96), (CX - 15, 96), (CX - 17, 80)]), h=3, r=3)
    coat.shape(poly([(CX + 10, 73), (CX + 2, 96), (CX + 15, 96), (CX + 17, 80)]), h=3, r=3)
    head = skull(P, cy=38, rx=16, ry=20, jaw=(13, 11, 53), ear_y=43, ear_dx=16, sockets=(7, 3))
    hm = ellipse(CX, 30, 18, 15) - ellipse(CX - 3, 39, 15, 13)
    hm -= rect(0, 46, 79, 95)
    hr = hair(P, hm, h=3, r=3, dome=(CX, 24, 16, 10, 5))
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 41), (CX + 1, 41), (CX + 3, 50), (CX - 2, 50)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 14, 30), (CX - 6, 20), (CX + 6, 18), (CX + 15, 28)]], ("hair", 0))
        curl_marks(cv, bun, [(CX + 14, 50, 5)], hi=("hair", 2))
        cv.fill(line(CX + 17, 47, CX + 25, 39), ("acc", 0))         # stylo planté dans le chignon
        cv.fill(line(CX + 18, 47, CX + 26, 39), INK)
        cv.put(CX + 25, 38, INK)
        # lunettes relevées sur le front
        for sx in (-1, 1):
            m = ellipse(CX + sx * 7, 23, 5, 3.5)
            cv.fill(border(m), INK)
            cv.put(CX + sx * 7 - 2, 22, ("white", 1))
        cv.fill(line(CX - 2, 22, CX + 2, 22), INK)
        # stéthoscope
        cv.curve((CX - 9, 72), (CX - 13, 84), (CX - 4, 90), INK)
        cv.curve((CX + 9, 72), (CX + 13, 84), (CX + 4, 90), INK)
        cv.grid(CX - 5, 89, [".kkk.", "kwwwk", ".kkk."], {"k": INK, "w": ("white", 0)})
        nostrils(cv, 50, 2)
        for sx in (-1, 1):   # cernes violets, rides de rire
            for x in range(CX + sx * 7 - 3, CX + sx * 7 + 4):
                cv.put(x, 44, ("bag", 0))
            T.wrinkle(cv, [(CX + sx * 13, 39), (CX + sx * 15, 41)])
            T.wrinkle(cv, [(CX + sx * 13, 42), (CX + sx * 15, 43)])

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m, ("skin", 1), ("skin", 1))
    face(P, eye_y=40, eye_dx=7, eye_w=8, lashes=True, iris_r=1.6, droop=0.8, brow_y=33, brow_w=9,
         brow_thick=2, brow_arch=1.5, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.55}, "content": {"closed_smile": True},
               "furieux": {"open_": 0.6, "tilt": 2}},
         brows={"neutre": (-1, 1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "line", "w": 9, "curve": -0.3, "lips": lips},
                 "content": {"kind": "smile", "w": 14, "open_": 3.5, "curve": 2, "tongue": ("acc", 0)},
                 "furieux": {"kind": "line", "w": 9, "curve": -1.3, "lips": lips},
                 "gene": {"kind": "wavy", "w": 9}},
         extras={"gene": sweat_at(CX - 20, 30)})
    return P


# --- Étienne Marchal, Éducation -----------------------------------------------------------

def min_education():
    """Étienne Marchal : cheveux blancs hirsutes en pointes, sourcils en broussaille,
    longue barbe poivre et sel en pointe, petites lunettes rondes, stylo rouge à l'oreille."""
    P = P_({
        "bg": [(6, 5, 4)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 5)],
        "hair": [(3, 3, 3), (5, 5, 5), (7, 7, 7)],
        "suit": [(2, 1, 0), (3, 2, 1), (5, 3, 1)],   # velours côtelé marron (DA)
        "white": [(5, 5, 5), (7, 7, 7)],
        "scarf": [(4, 0, 1), (6, 1, 1)],
        "iris": [(1, 2, 3)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.4, smooth=1)
    tufts = []
    lens = (6, 9, 5, 10, 7, 11, 6, 9, 12, 5, 8, 10, 6, 9)
    for k in range(14):
        a = math.radians(165 + 210 * k / 13)
        x = CX + 18 * math.cos(a)
        y = 28 + 15 * math.sin(a)
        if y < 40:
            tufts.append((x, y, a, lens[k]))
    spikes = ellipse(CX, 26, 19, 15) - rect(0, 32, 79, 95)
    for (x, y, a, L) in tufts:
        tx, ty = x + L * math.cos(a + 0.25 * math.sin(L)), y + L * math.sin(a + 0.25 * math.sin(L))
        spikes |= poly([(x - 3.5 * math.sin(a), y + 3.5 * math.cos(a)),
                        (x + 3.5 * math.sin(a), y - 3.5 * math.cos(a)), (tx, ty)])
    hb.shape(spikes, h=4, r=3)
    hb.ell(CX, 24, 16, 11, h=5, mode="add")
    tufts = [(x, y) for (x, y, a, L) in tufts]
    neck(P, w=11, top=60, bottom=76)
    s = bust(P, top=76, wide=27, tie=None, shirt="white", vw=5, vdepth=10)
    sc = P.layer("scarf", cast=(1, 2))
    sc.shape(ellipse(CX, 75, 13, 4) | poly([(CX + 6, 75), (CX + 11, 75), (CX + 12, 96), (CX + 6, 96)]), h=3, r=3)
    head = skull(P, cy=37, rx=15, ry=21, jaw=(12, 13, 52), ear_y=41, ear_dx=15, sockets=(7, 2.5))
    fm = ellipse(CX, 20, 15, 8)
    for k, (x, L) in enumerate(((CX - 10, 6), (CX - 4, 8), (CX + 3, 5), (CX + 9, 7))):
        fm |= poly([(x - 3, 22), (x + 3, 22), (x + (k - 1.5) * 1.5, 22 + L)])
    hf = hair(P, fm, h=3, r=3, dome=(CX, 18, 14, 7, 4))
    beard =P.layer("hair", cast=(1, 2), gain=1.3, smooth=1)
    bm = poly([(CX - 13, 48), (CX - 9, 58), (CX - 3, 74), (CX, 80), (CX + 3, 74), (CX + 9, 58), (CX + 13, 48),
               (CX + 8, 55), (CX - 8, 55)])
    beard.shape(bm, h=4, r=4)
    beard.cut(ellipse(CX, 59, 5, 2.5))
    mu = P.layer("hair", ink=True)
    mu.ell(CX - 4, 55.5, 5.5, 2.3, h=2)
    mu.ell(CX + 4, 55.5, 5.5, 2.3, h=2)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 40), (CX + 1, 40), (CX + 3, 51), (CX - 3, 51)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, beard, [[(CX - 6 + k * 3, 60), (CX - 3 + k * 1.5, 74)] for k in range(5)], ("hair", 0))
        comb(cv, hb, [[(x, y), (x + (x - CX) * 0.3, y - 5)] for (x, y) in tufts], ("hair", 0))
        comb(cv, hf, [[(CX - 10, 18), (CX - 9, 25)], [(CX - 3, 17), (CX - 4, 27)], [(CX + 4, 17), (CX + 3, 24)],
                      [(CX + 9, 18), (CX + 10, 26)]], ("hair", 0))
        cv.fill(line(CX + 14, 30, CX + 21, 38), ("scarf", 1))    # stylo rouge à l'oreille
        cv.put(CX + 21, 38, INK)
        stripes(cv, s, period=3)
        nostrils(cv, 51, 2)

    face(P, eye_y=39, eye_dx=7, eye_w=7, iris_r=1.4, brow_y=32, brow_w=11, brow_thick=3, brow_arch=2.5,
         brow_key=("hair", 1), glasses=lambda cv, e: round_glasses(cv, 39.5, dx=7, rx=4.5, ry=4),
         eyes={"neutre": {"open_": 0.8}},
         brows={"neutre": ((0, 0), (-3, -2)), "content": (-2, -2), "furieux": (4, -2)},
         mouth_y=59,
         mouths={"neutre": {"kind": "line", "w": 7, "curve": 0, "under": False},
                 "content": {"kind": "smile", "w": 9, "open_": 2.5, "curve": 1.5, "under": False},
                 "furieux": {"kind": "pout", "w": 7, "under": False},
                 "gene": {"kind": "wavy", "w": 7, "under": False}},
         extras={"furieux": vein_at(CX - 8, 22, ("scarf", 1)), "gene": sweat_at(CX - 19, 30)})
    return P


# --- Maëlle Kervella, Écologie ------------------------------------------------------------

def min_ecologie():
    """Maëlle Kervella : coupe asymétrique au côté rasé, mèche verte, visage rond aux
    joues rosies, grand sourire, anneau au nez, pull trop large."""
    P = P_({
        "bg": [(6, 6, 5)],
        "skin": [(3, 1, 1), (6, 3, 2), (7, 5, 4), (7, 6, 5)],
        "hair": [(1, 0, 1), (2, 1, 1), (4, 2, 2)],
        "suit": [(4, 2, 1), (6, 3, 2), (7, 5, 2)],
        "white": WHITE,
        "green": [(1, 4, 2), (3, 6, 2)],
        "acc": [(6, 3, 2)],
        "iris": [(1, 4, 2)],
    })
    neck(P, w=12, top=58, bottom=76)
    s = bust(P, top=73, wide=33, shirt=None, tie=None, sweater=True, lapels=False)
    roll = P.layer("suit", ink=True, cast=(1, 2))
    roll.shape(ellipse(CX, 73, 13, 5), h=3, r=3)
    head = skull(P, cy=41, rx=18, ry=20, jaw=(17, 13, 51), ear_y=44, ear_dx=18, cheeks=(10, 51, 5.5),
                 sockets=(8, 2))
    head.bias = 0.1
    hm = poly([(CX - 12, 22), (CX - 4, 18), (CX + 10, 18), (CX + 20, 24), (CX + 22, 40), (CX + 19, 56),
               (CX + 15, 52), (CX + 14, 36), (CX + 4, 30), (CX - 6, 34), (CX - 14, 30)])
    hr = hair(P, hm, h=4, r=3, dome=(CX + 4, 24, 16, 9, 5))
    shaved = P.layer("hair", ink=False)
    shaved.shape(ellipse(CX - 14, 30, 6, 9) - ellipse(CX - 4, 42, 13, 16), h=1, r=1)
    green = P.layer("green", cast=(1, 1), gain=1.2)
    green.shape(poly([(CX - 2, 19), (CX + 6, 18), (CX - 5, 38), (CX - 10, 37)]), h=3, r=2)
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 50, 3, 2.5, h=3)

    @P.draw
    def tex(cv, e):
        for y in range(20, 40):   # côté rasé : trame
            for x in range(18, 34):
                if cv.owner_is(x, y, shaved) and (x + y) % 2:
                    cv.put(x, y, ("skin", 1))
        comb(cv, hr, [[(CX + 4, 22), (CX + 14, 30), (CX + 18, 48)], [(CX + 8, 21), (CX + 18, 28)]], ("hair", 2))
        comb(cv, green, [[(CX + 2, 21), (CX - 6, 35)]], ("green", 0))
        cv.grid(CX + 1, 51, [".k.", "k.k", ".k."], {"k": ("white", 0)})   # anneau
        cv.put(CX - 1, 51, INK)
        stripes(cv, s, period=5, x0=2)
        T.blush(cv, CX - 15, 53, w=4, key=("acc", 0))
        T.blush(cv, CX + 11, 53, w=4, key=("acc", 0))

    face(P, eye_y=42, eye_dx=8, eye_w=9, eye_h=4.5, iris_r=2.0, lashes=True, brow_y=35, brow_w=9,
         brow_thick=2, brow_arch=2.5, brow_key=("hair", 0),
         eyes={"content": {"closed_smile": True}, "furieux": {"open_": 1.0, "tilt": 1.5}},
         brows={"neutre": (-1, -1), "furieux": (3, -2)},
         mouth_y=58,
         mouths={"neutre": {"kind": "smile", "w": 12, "open_": 2, "curve": 1.5},
                 "content": {"kind": "smile", "w": 16, "open_": 4, "curve": 2.5, "tongue": ("acc", 0)},
                 "furieux": {"kind": "shout", "w": 7, "open_": 3.5, "tongue": ("acc", 0)},
                 "gene": {"kind": "wavy", "w": 10}},
         extras={"furieux": lambda cv: (cv.fill(line(CX - 3, 20, CX - 1, 12), ("green", 1)),
                                        cv.fill(line(CX - 4, 20, CX - 2, 12), INK)),
                 "gene": sweat_at(CX + 18, 30)})
    return P


# --- Louis de Valcourt, Affaires étrangères -----------------------------------------------

def min_affaires_etr():
    """Louis de Valcourt : visage allongé et pâle, cheveux argentés ondulés tombant sur
    le col, long nez fin, pommettes creuses, foulard de soie, œillet à la boutonnière."""
    P = P_({
        "bg": [(3, 4, 6)],
        "skin": [(3, 2, 2), (5, 4, 4), (7, 6, 5), (7, 7, 6)],
        "hair": [(3, 3, 4), (5, 5, 6), (7, 7, 7)],
        "suit": [(0, 0, 1), (1, 1, 2), (2, 2, 4)],
        "white": [(5, 5, 6), (7, 7, 7)],
        "silk": [(4, 0, 2), (6, 2, 3)],
        "iris": [(1, 2, 4)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.3, smooth=1)
    for sx in (-1, 1):   # ondulations qui tombent sur le col
        pts = [(CX + sx * 10, 26)]
        for k in range(7):
            y = 28 + k * 5
            pts.append((CX + sx * (19 + (2 if k % 2 else 0) - k * 0.4), y))
        pts += [(CX + sx * 13, 62), (CX + sx * 11, 40)]
        hb.shape(poly(pts), h=4, r=3)
    neck(P, w=10, top=60, bottom=78)
    s = bust(P, top=77, wide=27, tie=None, vw=5, vdepth=11)
    silk = P.layer("silk", cast=(1, 2), gain=1.2)
    silk.shape(poly([(CX - 5, 72), (CX + 5, 72), (CX + 3, 80), (CX + 5, 88), (CX - 2, 88), (CX - 3, 80)]), h=3, r=2)
    head = skull(P, cy=35, rx=14, ry=22, jaw=(10, 14, 53), chin=(6, 5, 65), ear_y=None, sockets=(6, 3))
    head.ell(CX - 9, 51, 3.5, 6, h=2, mode="sub")
    head.ell(CX + 9, 51, 3.5, 6, h=2, mode="sub")
    hm = ellipse(CX, 22, 17, 12) - poly([(CX, 18), (CX - 13, 26), (CX - 14, 40), (CX + 14, 40), (CX + 13, 26)])
    hm |= ellipse(CX - 14, 32, 4, 9) | ellipse(CX + 14, 32, 4, 9)
    hr = hair(P, hm, h=3, r=3)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 37), (CX + 1, 37), (CX + 2, 50), (CX + 1, 53), (CX - 2, 53), (CX - 2, 50)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 2, 16), (CX - 8, 18), (CX - 13, 25)], [(CX + 2, 16), (CX + 8, 18), (CX + 13, 25)]],
             ("hair", 0))
        for sx in (-1, 1):
            comb(cv, hb, [[(CX + sx * (13 + k), 30 + k * 3), (CX + sx * (17 + k * 0.5), 38 + k * 4),
                           (CX + sx * (14 + k), 48 + k * 4)] for k in range(3)], ("hair", 0))
            comb(cv, hb, [[(CX + sx * 15, 31), (CX + sx * 17, 36)]], ("hair", 2))
        cv.grid(CX + 13, 80, [".r.", "rwr", ".r."], {"r": ("silk", 1), "w": ("white", 1)})   # œillet
        dotsilk = [(CX - 2, 82), (CX + 2, 84), (CX, 86), (CX - 1, 76)]
        for p in dotsilk:
            cv.put(*p, ("silk", 0))
        nostrils(cv, 52, 2)

    face(P, eye_y=38, eye_dx=6, eye_w=7, iris_r=1.4, lid=2, droop=1.0, brow_y=30, brow_w=9, brow_thick=1,
         brow_arch=2.5, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.5}, "content": {"open_": 0.5, "right": {"open_": 0.2}},
               "furieux": {"open_": 0.45, "tilt": 1}},
         brows={"neutre": (-2, -2), "content": ((-2, -2), (1, 1)), "furieux": (1, 0)},
         mouth_y=58,
         mouths={"neutre": {"kind": "line", "w": 9, "curve": 0.8},
                 "content": {"kind": "smirk", "w": 10, "curve": 2},
                 "furieux": {"kind": "line", "w": 6, "curve": -0.5},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"furieux": lambda cv: [cv.put(CX + sx * 6 - 2, 38, ("white", 1)) for sx in (-1, 1)],
                 "gene": sweat_at(CX - 21, 26)})
    return P


ALL = [("min_economie", min_economie), ("min_sante", min_sante), ("min_education", min_education),
       ("min_ecologie", min_ecologie), ("min_affaires_etr", min_affaires_etr)]
