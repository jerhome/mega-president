"""Gueules V3 (3.09) : chefs de groupe, presse, Sénat, syndicat, patronat.
(Aymeric Dubreuil-Castagnac, chef du groupe UTL, est dans g_demo.py.)

Tous les personnages, partis, médias et organisations sont fictifs
(casting : docs/contenu/univers.md, sections 5.6 et 5.7).
"""
import math

from raster import ellipse, poly, rect, line, border
import traits as T
from persos import (P_, CX, INK, bust, neck, skull, hair, nose_layer, nostrils, face,
                    comb, curl_marks, stripes, checks, round_glasses, rect_glasses, lipstick,
                    sweat_at, vein_at, blush_at, both, H, W)

WHITE = [(5, 5, 6), (7, 7, 7)]


def layer_of(P, mat):
    return [l for l in P.layers if l.mat == mat]


# --- Yannick Brossard, chef du groupe UGR ---------------------------------------------------

def chef_ugr():
    """Yannick Brossard : tête ronde et chauve, barbe grise de bûcheron, sourcils en
    broussaille, bouche large, chemise à carreaux rouges, bretelles, gros ventre."""
    P = P_({
        "bg": [(6, 5, 4)],
        "skin": [(3, 1, 1), (5, 3, 2), (7, 5, 4), (7, 6, 6)],
        "hair": [(3, 3, 3), (5, 5, 5), (7, 7, 7)],
        "suit": [(3, 0, 0), (5, 1, 1), (7, 2, 2)],
        "white": [(5, 5, 5), (7, 7, 7)],
        "strap": [(1, 1, 2), (2, 2, 4)],
        "iris": [(1, 1, 2)],
    })
    neck(P, w=22, top=58, bottom=76)
    shirt = bust(P, top=73, wide=38, shirt=None, tie=None, lapels=False, sweater=True)
    straps = P.layer("strap", ink=True, cast=(1, 1))
    straps.shape(rect(CX - 16, 74, CX - 12, 96) | rect(CX + 12, 74, CX + 16, 96), h=2, r=2)
    head = skull(P, cy=36, rx=20, ry=21, ear_y=42, ear_dx=20, ear_r=(3.5, 5.5), sockets=(8, 2.5),
                 cheeks=(11, 46, 5))
    head.ell(CX - 5, 22, 10, 8, h=3, mode="add")
    beard = P.layer("hair", cast=(1, 2), gain=1.4, smooth=1)
    bm = (ellipse(CX, 52, 21, 22) - ellipse(CX, 40, 18, 14)) - rect(0, 0, 79, 42)
    beard.shape(bm | ellipse(CX, 68, 14, 9), h=4, r=3)
    clumps = [(CX - 16, 52, 4), (CX + 16, 52, 4), (CX - 12, 62, 5), (CX + 12, 62, 5), (CX - 4, 70, 5),
              (CX + 5, 70, 5), (CX, 62, 4)]
    for (x, y, r) in clumps:
        beard.ell(x, y, r, r * 0.85, h=2, mode="add")
    beard.cut(ellipse(CX, 56, 9, 3.5))
    mu = P.layer("hair", ink=True, smooth=1)
    mu.ell(CX - 5, 52, 7, 3, h=3)
    mu.ell(CX + 5, 52, 7, 3, h=3)
    nose = nose_layer(P)
    nose.ell(CX, 46, 5, 4, h=4)

    @P.draw
    def tex(cv, e):
        checks(cv, shirt, period=6)
        curl_marks(cv, beard, clumps, hi=("hair", 2))
        comb(cv, mu, [[(CX - 9 + k * 3, 51), (CX - 10 + k * 3, 54)] for k in range(7)], ("hair", 0))
        cv.put(CX - 7, 18, ("skin", 3))
        cv.put(CX - 6, 18, ("skin", 3))
        cv.put(CX - 8, 19, ("skin", 3))
        nostrils(cv, 48, 2)

    face(P, eye_y=38, eye_dx=8, eye_w=8, iris_r=1.6, brow_y=31, brow_w=12, brow_thick=4, brow_arch=2.5,
         brow_key=("hair", 1),
         eyes={"neutre": {"open_": 0.75}},
         brows={"neutre": (1, -1)},
         mouth_y=56,
         mouths={"neutre": {"kind": "smirk", "w": 13, "curve": 1.5, "under": False},
                 "content": {"kind": "smile", "w": 17, "open_": 4, "curve": 2, "tongue": ("suit", 2), "under": False},
                 "furieux": {"kind": "shout", "w": 14, "open_": 4.5, "tongue": ("suit", 2), "under": False},
                 "gene": {"kind": "wavy", "w": 12, "under": False}},
         extras={"furieux": both(vein_at(CX + 8, 20, ("suit", 1)), vein_at(CX - 12, 22, ("suit", 1))),
                 "gene": sweat_at(CX + 17, 20)})
    return P


# --- Claire Aubert, cheffe du groupe RSV ----------------------------------------------------

def chef_rsv():
    """Claire Aubert : cheveux courts bruns à frange droite, larges lunettes rouges,
    visage rond à fossettes, sourire en biais, col roulé vert, écharpe tricotée."""
    P = P_({
        "bg": [(5, 5, 6)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(1, 0, 1), (3, 1, 1), (5, 3, 2)],
        "suit": [(0, 3, 1), (1, 4, 2), (3, 6, 3)],
        "white": WHITE,
        "red": [(5, 0, 1), (7, 2, 2)],
        "iris": [(3, 1, 1)],
        "wool": [(5, 4, 1)],
    })
    hb = P.layer("hair", cast=(1, 2))
    hb.shape(ellipse(CX, 40, 22, 20) - rect(0, 52, 79, 95), h=6, r=5)
    neck(P, w=13, top=58, bottom=74)
    s = bust(P, top=72, wide=32, shirt=None, tie=None, sweater=True, lapels=False)
    roll = P.layer("suit", ink=True, cast=(1, 2))
    roll.shape(rect(CX - 9, 64, CX + 8, 74), h=3, r=3)
    sc = P.layer("wool", ink=True, flat=0)
    sc.shape(poly([(CX - 14, 72), (CX + 14, 72), (CX + 16, 77), (CX + 8, 78), (CX + 10, 96), (CX + 4, 96),
                   (CX + 2, 78), (CX - 15, 77)]), h=2, r=2)
    head = skull(P, cy=41, rx=18, ry=20, jaw=(16, 11, 52), ear_y=None, cheeks=(10, 51, 5), sockets=(8, 2))
    head.bias = 0.05
    fr = ellipse(CX, 28, 20, 14) - rect(0, 34, 79, 95)
    fr |= rect(CX - 19, 26, CX + 18, 33)
    fr |= poly([(CX - 21, 30), (CX - 16, 30), (CX - 17, 50), (CX - 22, 50)])
    fr |= poly([(CX + 16, 30), (CX + 21, 30), (CX + 22, 50), (CX + 17, 50)])
    hr = hair(P, fr, h=4, r=3, dome=(CX, 24, 18, 10, 5))
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 3, 2.5, h=3)

    @P.draw
    def tex(cv, e):
        for x in range(CX - 18, CX + 19, 3):     # frange droite
            cv.put(x, 33, ("hair", 0))
            cv.put(x, 32, ("hair", 0))
        comb(cv, hr, [[(CX - 10, 18), (CX + 4, 15), (CX + 14, 20)]], ("hair", 2))
        stripes(cv, s, period=4)
        stripes(cv, roll, period=2)
        cv.put(CX - 1, 50, INK)
        cv.put(CX + 1, 50, INK)
        for x in (CX - 12, CX + 12):             # fossettes
            cv.put(x, 57, ("skin", 1))

    def lunettes(cv, e):
        dy = 2 if e == "furieux" else 0          # lunettes baissées
        rect_glasses(cv, 37 + dy, dx=8, w=12, h=7, key=("red", 0), thick=2)

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m, ("red", 0), ("red", 1))
    face(P, eye_y=41, eye_dx=8, eye_w=8, lashes=True, iris_r=1.7, brow_y=None, brow_w=0, brow_thick=0,
         glasses=lunettes,
         eyes={"furieux": {"open_": 0.7, "tilt": 1.5, "look_y": -1}},
         mouth_y=57,
         mouths={"neutre": {"kind": "smirk", "w": 10, "curve": 1, "lips": lips},
                 "content": {"kind": "smile", "w": 13, "open_": 2.5, "curve": 2, "lip": ("red", 0)},
                 "furieux": {"kind": "line", "w": 9, "curve": -1, "lips": lips},
                 "gene": {"kind": "wavy", "w": 9}},
         extras={"content": lambda cv: [cv.put(x, 56, ("skin", 0)) for x in (CX - 9, CX + 9)],
                 "gene": sweat_at(CX + 18, 36)})
    return P


# --- Gérard Poulain, chef du groupe MJM -----------------------------------------------------

def chef_mjm():
    """Gérard Poulain : visage lunaire, calvitie, favoris blonds en côtelettes, petit nez
    rond et rouge, joues rebondies, regard fuyant, costume beige, cravate pâle."""
    P = P_({
        "bg": [(4, 5, 6)],
        "skin": [(3, 1, 1), (6, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(3, 2, 0), (5, 4, 1), (7, 6, 3)],
        "suit": [(4, 3, 2), (5, 5, 3), (6, 6, 4)],
        "white": WHITE,
        "tie": [(5, 5, 6), (7, 7, 7)],
        "red": [(6, 1, 1)],
        "iris": [(1, 0, 1)],
    })
    neck(P, w=22, top=58, bottom=76)
    bust(P, top=75, wide=37, tie="tie", vw=6, vdepth=12)
    head = skull(P, cy=40, rx=21, ry=22, ear_y=42, ear_dx=21, ear_r=(3, 5), sockets=(7, 2),
                 cheeks=(12, 50, 6.5))
    head.ell(CX - 6, 26, 10, 8, h=3, mode="add")
    side = (poly([(CX - 20, 30), (CX - 16, 30), (CX - 11, 52), (CX - 14, 58), (CX - 19, 46)]) |
            poly([(CX + 20, 30), (CX + 16, 30), (CX + 11, 52), (CX + 14, 58), (CX + 19, 46)]))
    hr = hair(P, side & head.mask(), h=3, r=2)
    nose = P.layer("red", ink=True, cast=(1, 1))
    nose.ell(CX + 0.5, 47, 3.5, 3, h=3)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 18, 37), (CX - 16, 52)], [(CX + 18, 37), (CX + 16, 52)]], ("hair", 0))
        cv.put(CX - 1, 46, ("white", 1))
        cv.put(CX - 8, 21, ("skin", 3))
        cv.put(CX - 7, 21, ("skin", 3))
        cv.put(CX - 9, 22, ("skin", 3))

    face(P, eye_y=40, eye_dx=8, eye_w=7, iris_r=1.5, brow_y=34, brow_w=8, brow_thick=1, brow_arch=2,
         brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.75, "look": -2, "look_y": -1}, "content": {"open_": 0.6, "look": 2, "raise_": 1},
               "furieux": {"open_": 0.8, "tilt": -1, "look": -2}, "gene": {"open_": 0.9, "look": 2}},
         brows={"neutre": (-1, 0), "content": (-2, 0), "furieux": (-3, 1), "gene": (-3, 1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "wavy", "w": 8},
                 "content": {"kind": "grin", "w": 11, "open_": 1.5, "curve": 1},
                 "furieux": {"kind": "wavy", "w": 10, "skew": 1},
                 "gene": {"kind": "smirk", "w": 9, "curve": -0.5}},
         extras={"furieux": both(sweat_at(CX + 10, 18), sweat_at(CX - 14, 22), sweat_at(CX + 17, 30)),
                 "gene": blush_at(51, 12, 5)})
    return P


# --- Maxime Corbeau, chef du groupe LSO -----------------------------------------------------

def chef_lso():
    """Maxime Corbeau : visage taillé à la serpe, joues creuses, nez en bec, cheveux noirs
    en brosse aux tempes rasées, petite barbiche, bomber noir sur chemise blanche."""
    P = P_({
        "bg": [(5, 5, 5)],
        "skin": [(3, 2, 1), (5, 3, 3), (6, 5, 4), (7, 6, 5)],
        "hair": [(1, 0, 1), (2, 1, 2), (3, 3, 4)],
        "suit": [(1, 0, 1), (2, 2, 2), (3, 3, 3)],
        "white": WHITE,
        "iris": [(1, 1, 1)],
        "pin": [(6, 5, 1)],
    })
    neck(P, w=11, top=60, bottom=76)
    s = bust(P, top=76, wide=31, tie=None, vw=6, vdepth=12, lapels=False)
    coll = P.layer("suit", ink="always", cast=(1, 1))
    coll.shape(ellipse(CX - 11, 75, 6, 4) | ellipse(CX + 11, 75, 6, 4), h=2, r=2)
    head = skull(P, cy=37, rx=15, ry=21, jaw=(11, 14, 51), chin=(5, 5, 64), ear_y=41, ear_dx=15,
                 sockets=(7, 3))
    head.ell(CX - 9, 50, 3.5, 6, h=1.5, mode="sub")
    head.ell(CX + 9, 50, 3.5, 6, h=1.5, mode="sub")
    hm = poly([(CX - 12, 14), (CX + 12, 14), (CX + 14, 26), (CX - 14, 26)])
    hr = hair(P, hm, h=3, r=2)
    shaved = P.layer("hair", ink=False)
    shaved.shape(rect(CX - 15, 22, CX - 12, 34) | rect(CX + 12, 22, CX + 15, 34), h=1, r=1)
    goat = P.layer("hair", ink=True)
    goat.shape(poly([(CX - 3, 64), (CX + 3, 64), (CX + 1, 71), (CX - 1, 71)]), h=2, r=1)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 38), (CX + 2, 38), (CX + 6, 48), (CX + 4, 52), (CX, 54), (CX - 3, 51)]), h=4, r=3)

    @P.draw
    def tex(cv, e):
        for x in range(CX - 12, CX + 13, 2):
            cv.put(x, 14, ("bg", 0))
        comb(cv, hr, [[(CX - 10 + k * 4, 16), (CX - 10 + k * 4, 19)] for k in range(6)], ("hair", 2))
        for y in range(22, 35):
            for x in range(20, 60):
                if cv.owner_is(x, y, shaved) and (x + y) % 2:
                    cv.put(x, y, ("skin", 1))
        stripes(cv, s, period=8, x0=3)
        cv.grid(CX + 15, 82, [".k.", "kyk", ".k."], {"k": INK, "y": ("pin", 0)})
        nostrils(cv, 53, 2)

    face(P, eye_y=38, eye_dx=7, eye_w=8, iris_r=1.4, lid=2, brow_y=32, brow_w=9, brow_thick=2, brow_arch=1,
         brow_key=INK, taper=True,
         eyes={"neutre": {"open_": 0.6, "look": 2}, "content": {"open_": 0.5, "raise_": 1}},
         brows={"neutre": (2, 0), "content": ((1, -1), (-1, -3))},
         mouth_y=59,
         mouths={"neutre": {"kind": "smirk", "w": 8, "curve": 0.5},
                 "content": {"kind": "grin", "w": 13, "open_": 2, "curve": 3},
                 "furieux": {"kind": "grit", "w": 11, "curve": 0},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"gene": sweat_at(CX - 20, 24)})
    return P


# --- Karine Lopez, présentatrice de Chaîne Info 24 ------------------------------------------

def jour_chaine_info():
    """Karine Lopez : brushing blond roux gonflé et immobile, maquillage appuyé, grands
    yeux, sourire large et figé, micro à la main, bandeau rouge « urgent » en bas."""
    P = P_({
        "bg": [(2, 3, 5)],
        "skin": [(3, 1, 1), (6, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(5, 2, 0), (7, 4, 1), (7, 6, 3)],
        "suit": [(5, 5, 6), (6, 6, 6), (7, 7, 7)],
        "white": [(5, 5, 6), (7, 7, 7)],
        "red": [(5, 0, 1), (7, 1, 1)],
        "lid": [(3, 2, 5)],
        "iris": [(3, 2, 5)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.3)
    hb.shape(ellipse(CX, 36, 28, 28) - rect(0, 62, 79, 95), h=8, r=8)
    for (x, y) in ((CX - 24, 54), (CX + 24, 54), (CX - 26, 40), (CX + 26, 40)):
        hb.ell(x, y, 6, 7, h=4, mode="add")
    neck(P, w=12, top=58, bottom=76)
    bust(P, top=76, wide=30, shirt="white", tie=None, vw=6, vdepth=10, collar=False)
    head = skull(P, cy=40, rx=16, ry=20, jaw=(13, 12, 52), ear_y=None, cheeks=(9, 50, 4.5), sockets=(7, 2))
    head.bias = 0.05
    fr = ellipse(CX, 22, 22, 12) | ellipse(CX - 12, 28, 10, 8)
    fr -= ellipse(CX + 8, 36, 14, 9)
    hf = hair(P, fr, h=5, r=4, dome=(CX - 4, 18, 18, 9, 5))
    # micro de plateau : manche épais, tête grillagée, cube « 24 » rouge, main qui le tient
    mic = P.layer("lid", ink=True, cast=(1, 1))
    mic.shape(poly([(CX - 31, 96), (CX - 25, 96), (CX - 18, 70), (CX - 23, 69)]), h=2, r=2)
    mh = P.layer("suit", ink=True, cast=(1, 1), gain=1.2)
    mh.ell(CX - 19.5, 64, 6, 5.5, h=4)
    cube = P.layer("red", ink=True, cast=(1, 1))
    cube.shape(rect(CX - 27, 70, CX - 15, 78), h=2, r=1)
    hand = P.layer("skin", cast=(1, 1))
    hand.ell(CX - 26, 86, 4.5, 4, h=3)
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 2.5, 2.5, h=3)

    @P.draw
    def tex(cv, e):
        comb(cv, hf, [[(CX - 18, 30), (CX - 12, 18), (CX, 12), (CX + 14, 14), (CX + 20, 22)],
                      [(CX - 12, 32), (CX - 6, 22), (CX + 6, 18)]], ("hair", 0))
        comb(cv, hb, [[(CX - 24, 34), (CX - 26, 50), (CX - 22, 60)], [(CX + 24, 34), (CX + 26, 50), (CX + 22, 60)]],
             ("hair", 0))
        comb(cv, hf, [[(CX - 14, 20), (CX - 6, 14)]], ("hair", 2))
        for y in range(60, 69):       # grille du micro
            for x in range(CX - 25, CX - 13):
                if cv.owner_is(x, y, mh) and (x + y) % 2 == 0 and cv.get(x, y)[0] != "ink":
                    cv.put(x, y, ("suit", 0))
        from vignettes import text3
        text3(cv, CX - 25, 72, "24", ("white", 1))
        for y in range(88, 96):      # bandeau rouge « urgent »
            for x in range(0, W):
                cv.put(x, y, ("red", 1) if y > 88 else INK)
        for x in range(4, 40, 2):
            if x % 8 != 6:
                cv.put(x, 91, ("white", 1))
                cv.put(x, 92, ("white", 1))
        cv.fill(rect(44, 90, 76, 93), ("red", 0))
        for sx in (-1, 1):   # fard à paupières
            for x in range(CX + sx * 7 - 3, CX + sx * 7 + 4):
                cv.put(x, 36, ("lid", 0))
        cv.put(CX - 1, 50, INK)
        cv.put(CX + 1, 50, INK)

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m, ("red", 0), ("red", 1))
    face(P, eye_y=40, eye_dx=7, eye_w=9, eye_h=5, iris_r=2.0, lashes=True, lid=2, brow_y=32, brow_w=9,
         brow_thick=1, brow_arch=4, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.9}, "content": {"open_": 1.2}, "furieux": {"open_": 1.0, "tilt": 0.8}},
         brows={"neutre": (-1, -1), "content": (-3, -3), "furieux": (2, -1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "grin", "w": 13, "open_": 1.6, "curve": 1.5, "lip": ("red", 1)},
                 "content": {"kind": "smile", "w": 15, "open_": 3.5, "curve": 2, "tongue": ("red", 0),
                             "lip": ("red", 1)},
                 "furieux": {"kind": "grin", "w": 14, "open_": 1.6, "curve": 1.0, "lip": ("red", 1)},
                 "gene": {"kind": "line", "w": 9, "curve": -0.5, "lips": lips}},
         extras={"furieux": vein_at(CX + 9, 16, ("red", 1)), "gene": sweat_at(CX + 15, 28)})
    return P


# --- Hubert Lenglet, éditorialiste du Quotidien du Matin ------------------------------------

def jour_editorialiste():
    """Hubert Lenglet : crâne dégarni, longues mèches blanches sur les côtés, gros nez,
    monture d'écaille, bajoues, nœud papillon bordeaux, veste en tweed, sourcil levé."""
    P = P_({
        "bg": [(6, 6, 5)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(4, 4, 4), (6, 6, 6), (7, 7, 7)],
        "suit": [(2, 2, 1), (4, 3, 2), (5, 4, 3)],
        "white": [(6, 6, 6), (7, 7, 7)],
        "tie": [(3, 0, 1), (5, 1, 2)],
        "shell": [(3, 1, 0), (5, 3, 1)],
    })
    P.ramps["iris"] = [(3, 1, 0)]
    hb = P.layer("hair", cast=(1, 2), gain=1.3, smooth=1)
    for sx in (-1, 1):
        hb.shape(poly([(CX + sx * 14, 30), (CX + sx * 22, 34), (CX + sx * 25, 50), (CX + sx * 22, 62),
                       (CX + sx * 17, 58), (CX + sx * 16, 42)]), h=4, r=3)
    neck(P, w=22, top=60, bottom=76)
    s = bust(P, top=76, wide=33, tie="tie", bow=True, vw=6, vdepth=12)
    head = skull(P, cy=38, rx=18, ry=21, jaw=(19, 12, 54), ear_y=44, ear_dx=18, sockets=(8, 2))
    head.ell(CX - 14, 58, 7, 7, h=4)   # bajoues
    head.ell(CX + 14, 58, 7, 7, h=4)
    head.ell(CX - 5, 22, 10, 8, h=3, mode="add")
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 6, 5, h=5)
    nose.shape(poly([(CX - 1.5, 40), (CX + 1.5, 40), (CX + 3, 46), (CX - 3, 46)]), h=2, r=1)

    @P.draw
    def tex(cv, e):
        for sx in (-1, 1):
            comb(cv, hb, [[(CX + sx * 17, 34), (CX + sx * 22, 46), (CX + sx * 20, 58)],
                          [(CX + sx * 15, 38), (CX + sx * 19, 50)]], ("hair", 0))
        for k in range(3):   # trois mèches rabattues
            cv.curve((CX - 12 + k * 4, 26), (CX - 6 + k * 5, 17), (CX + 2 + k * 5, 22), ("hair", 1))
        cv.put(CX - 8, 19, ("skin", 3))
        cv.put(CX - 7, 19, ("skin", 3))
        # tweed : chevrons
        for y in range(76, 96):
            for x in range(0, W):
                if cv.owner_is(x, y, s) and (x + (y // 2) * 2) % 4 == 0 and cv.get(x, y)[0] == "suit":
                    k = cv.get(x, y)
                    cv.put(x, y, ("suit", max(0, k[1] - 1)))
        cv.put(CX - 3, 52, INK)
        cv.put(CX + 3, 52, INK)

    def ecaille(cv, e):
        for sx in (-1, 1):
            x0 = CX + sx * 8 - 5
            m = rect(x0, 38, x0 + 10, 44)
            cv.fill(border(m), ("shell", 0))
            cv.fill({(x, 38) for x in range(x0, x0 + 11) if x % 2}, ("shell", 1))
        cv.fill(line(CX - 2, 39, CX + 2, 39), ("shell", 0))

    face(P, eye_y=42, eye_dx=8, eye_w=7, iris_r=1.4, lid=2, droop=1.0, brow_y=33, brow_w=10, brow_thick=2,
         brow_arch=2, brow_key=("hair", 1), glasses=ecaille,
         eyes={"neutre": {"open_": 0.55, "look_y": -1}, "content": {"open_": 0.4, "raise_": 0.6}},
         brows={"neutre": ((0, 0), (-4, -3)), "content": ((0, 0), (-3, -3)), "furieux": (3, -2)},
         mouth_y=60,
         mouths={"neutre": {"kind": "line", "w": 9, "curve": -0.8},
                 "content": {"kind": "smirk", "w": 9, "curve": 1},
                 "furieux": {"kind": "shout", "w": 11, "open_": 3.5, "tongue": ("tie", 1)},
                 "gene": {"kind": "wavy", "w": 9}},
         extras={"furieux": lambda cv: [cv.fill(line(x, y, x + dx, y - 5), INK)
                                        for (x, y, dx) in ((CX - 22, 34, -3), (CX + 22, 34, 3), (CX - 24, 44, -4),
                                                           (CX + 24, 44, 4))],
                 "gene": sweat_at(CX + 8, 14)})
    return P


# --- Gaston Mirebeau, président du Sénat ----------------------------------------------------

def senat_president():
    """Gaston Mirebeau : très âgé, visage ridé comme une carte, oreilles immenses, couronne
    de cheveux blancs, grosses lunettes, nez bourgeonnant, écharpe tricolore, voûté."""
    P = P_({
        "bg": [(5, 3, 3)],
        "skin": [(3, 1, 1), (5, 3, 3), (6, 5, 4), (7, 6, 5)],
        "hair": [(4, 4, 4), (6, 6, 6), (7, 7, 7)],
        "suit": [(2, 2, 2), (3, 3, 3), (4, 4, 4)],
        "white": [(6, 6, 6), (7, 7, 7)],
        "flag": [(1, 2, 6), (6, 1, 1)],
        "iris": [(2, 2, 2)],
        "nose": [(6, 3, 3)],
    })
    neck(P, w=14, top=60, bottom=72)
    s = bust(P, top=70, wide=32, tie="flag", vw=5, vdepth=11)    # dos voûté : épaules hautes
    sash = P.layer("flag", ink="always", flat=1)
    sash.shape(poly([(CX - 20, 72), (CX - 12, 72), (CX + 26, 97), (CX + 16, 97)]), h=2, r=2)
    head = skull(P, cy=40, rx=16, ry=20, jaw=(13, 12, 54), ear_y=44, ear_dx=19, ear_r=(6, 10),
                 sockets=(7, 3))
    hm = (ellipse(CX - 15, 36, 5, 8) | ellipse(CX + 15, 36, 5, 8) | ellipse(CX, 22, 10, 3)) - ellipse(CX, 42, 13, 17)
    hr = hair(P, hm, h=3, r=2)
    nose = P.layer("nose", ink="shadow", cast=(1, 1), ink_dirs="rdl")
    nose.ell(CX + 0.5, 51, 5, 4.5, h=5)
    nose.ell(CX - 3, 50, 2.5, 2.5, h=2)

    @P.draw
    def tex(cv, e):
        for y in range(70, 96):
            for x in range(0, W):
                if cv.owner_is(x, y, sash) and cv.get(x, y)[0] != "ink":
                    t = (x * 2 - y * 3) % 18
                    cv.put(x, y, ("flag", 0) if t < 6 else (("white", 1) if t < 12 else ("flag", 1)))
        for y in (25, 28, 31):          # carte de rides
            cv.curve((CX - 10, y + 1), (CX, y - 1), (CX + 10, y + 1), ("skin", 1))
        for sx in (-1, 1):
            T.fold(cv, (CX + sx * 6, 54), (CX + sx * 10, 58), (CX + sx * 10, 64))
            T.wrinkle(cv, [(CX + sx * 13, 46), (CX + sx * 14, 52)])
            T.wrinkle(cv, [(CX + sx * 12, 40), (CX + sx * 15, 40)])
            T.wrinkle(cv, [(CX + sx * 18, 38), (CX + sx * 19, 48)], ("skin", 0))   # pavillon
        cv.put(CX - 2, 53, INK)
        cv.put(CX + 3, 53, INK)

    def lunettes(cv, e):
        for sx in (-1, 1):
            m = ellipse(CX + sx * 7, 42, 6.5, 5.5)
            cv.fill(border(m) | border(m - border(m)), INK)
            cv.put(CX + sx * 7 - 3, 39, ("white", 1))
        cv.fill(line(CX - 1, 41, CX + 1, 41), INK)

    def gum(cv, mx, my, m):   # sourire édenté
        cv.fill(rect(int(mx) - 3, my + 1, int(mx) + 2, my + 1), ("nose", 0))

    face(P, eye_y=42, eye_dx=7, eye_w=7, iris_r=1.3, lid=2, droop=1.5, bags=2, brow_y=35, brow_w=9,
         brow_thick=2, brow_arch=1, brow_key=("hair", 1),
         glasses=lunettes,
         eyes={"neutre": {"open_": 0.3}, "content": {"closed_smile": True},
               "furieux": {"open_": 0.7, "tilt": 1.5}, "gene": {"open_": 0.6}},
         brows={"neutre": (-1, 1)},
         mouth_y=60,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": -0.3},
                 "content": {"kind": "smile", "w": 12, "open_": 2.5, "curve": 2, "teeth": False,
                             "inner": ("nose", 0)},
                 "furieux": {"kind": "shout", "w": 9, "open_": 3, "teeth": False, "tongue": ("nose", 0)},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"furieux": vein_at(CX + 6, 22, ("flag", 1)), "gene": sweat_at(CX - 6, 18)})
    return P


# --- Nadia Benali, secrétaire générale de l'UST ---------------------------------------------

def synd_ust():
    """Nadia Benali : queue de cheval haute noire aux mèches grises, visage en cœur, regard
    vif, sourire large, grosses créoles, gilet syndical rouge, mégaphone."""
    P = P_({
        "bg": [(6, 6, 4)],
        "skin": [(2, 1, 0), (4, 2, 1), (6, 4, 2), (7, 5, 3)],
        "hair": [(1, 0, 1), (2, 1, 1), (5, 5, 5)],
        "suit": [(4, 0, 0), (6, 1, 1), (7, 3, 2)],
        "white": WHITE,
        "gold": [(5, 4, 0), (7, 6, 2)],
        "iris": [(2, 1, 0)],
        "grey": [(3, 3, 4)],
    })
    tail = P.layer("hair", cast=(1, 2), gain=1.3, smooth=1)
    tail.shape(poly([(CX + 6, 8), (CX + 14, 4), (CX + 24, 10), (CX + 28, 26), (CX + 26, 44), (CX + 21, 50),
                     (CX + 22, 30), (CX + 16, 16)]), h=5, r=4)
    neck(P, w=13, top=58, bottom=76)
    s = bust(P, top=75, wide=33, suit="suit", shirt="white", tie=None, vw=6, vdepth=11, lapels=False)
    head = skull(P, cy=38, rx=18, ry=19, jaw=(13, 13, 51), chin=(6, 5, 63), ear_y=42, ear_dx=17,
                 cheeks=(10, 47, 5), sockets=(8, 2))
    hm = ellipse(CX, 26, 19, 13) - ellipse(CX, 34, 16, 10)
    hm -= rect(0, 36, 79, 95)
    hm |= ellipse(CX + 6, 12, 5, 4)
    hr = hair(P, hm, h=3, r=3, dome=(CX, 22, 17, 10, 6))
    meg = P.layer("white", ink=True, cast=(1, 1))
    meg.shape(poly([(CX - 30, 78), (CX - 22, 82), (CX - 22, 92), (CX - 34, 96), (CX - 36, 76)]), h=3, r=2)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 41), (CX + 1, 41), (CX + 3, 49), (CX - 3, 49)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 14, 30), (CX - 8, 20), (CX + 4, 14)], [(CX + 6, 16), (CX + 14, 22), (CX + 16, 30)]],
             ("hair", 0))
        comb(cv, hr, [[(CX - 10, 24), (CX - 2, 17)]], ("grey", 0))
        comb(cv, tail, [[(CX + 18, 10), (CX + 25, 24), (CX + 23, 44)]], ("grey", 0))
        comb(cv, tail, [[(CX + 15, 8), (CX + 21, 18)]], ("hair", 2))
        for sx in (-1, 1):   # créoles
            m = ellipse(CX + sx * 18, 51, 3, 4)
            cv.fill(border(m), ("gold", 1))
        cv.fill(rect(CX - 33, 84, CX - 26, 86), ("suit", 1))    # bande du mégaphone
        cv.grid(CX + 12, 80, ["kkkkk", "kwrwk", "kkkkk"], {"k": INK, "w": ("white", 1), "r": ("suit", 1)})   # badge
        nostrils(cv, 49, 2)

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m, ("suit", 0), ("suit", 1))
    face(P, eye_y=39, eye_dx=8, eye_w=9, eye_h=4.5, iris_r=1.9, lashes=True, lid=2, brow_y=32, brow_w=10,
         brow_thick=2, brow_arch=2.5, brow_key=INK,
         eyes={"content": {"closed_smile": True}},
         mouth_y=56,
         mouths={"neutre": {"kind": "smile", "w": 12, "open_": 2, "curve": 1, "lip": ("suit", 1)},
                 "content": {"kind": "smile", "w": 15, "open_": 4.5, "curve": 2, "tongue": ("suit", 2),
                             "lip": ("suit", 1)},
                 "furieux": {"kind": "shout", "w": 13, "open_": 5, "tongue": ("suit", 2)},
                 "gene": {"kind": "line", "w": 9, "curve": -0.5, "lips": lips}},
         extras={"gene": sweat_at(CX - 21, 28)})
    return P


# --- Charles-Henri Dumoulin, président de l'UPE ---------------------------------------------

def patron_upe():
    """Charles-Henri Dumoulin : teint orange, dents trop blanches, sourire d'agent
    immobilier, cheveux argent gominés, favoris, chemise rose, gilet sans manches, montre."""
    P = P_({
        "bg": [(3, 5, 4)],
        "skin": [(3, 1, 0), (6, 3, 1), (7, 4, 2), (7, 6, 3)],
        "hair": [(3, 3, 4), (5, 5, 6), (7, 7, 7)],
        "suit": [(1, 1, 2), (2, 2, 3), (3, 3, 5)],
        "white": [(5, 5, 6), (7, 7, 7)],
        "pink": [(6, 3, 4), (7, 5, 6)],
        "iris": [(1, 1, 2)],
        "gold": [(7, 6, 2)],
    })
    neck(P, w=20, top=58, bottom=76)
    s = bust(P, top=75, wide=38, suit="pink", shirt=None, tie=None, lapels=False, sweater=True)
    coll = P.layer("pink", ink="always", cast=(1, 2))
    coll.shape(poly([(CX - 10, 70), (CX - 1, 77), (CX - 4, 81), (CX - 12, 76)]) |
               poly([(CX + 10, 70), (CX + 1, 77), (CX + 4, 81), (CX + 12, 76)]), h=2, r=2)
    vest = P.layer("suit", ink=True, cast=(1, 1))
    vest.shape(poly([(CX - 28, 82), (CX - 12, 78), (CX - 4, 96), (CX - 26, 96)]) |
               poly([(CX + 28, 82), (CX + 12, 78), (CX + 4, 96), (CX + 26, 96)]), h=4, r=3)
    head = skull(P, cy=38, rx=18, ry=21, jaw=(17, 13, 52), ear_y=42, ear_dx=18, cheeks=(10, 49, 5),
                 sockets=(8, 2))
    hm = ellipse(CX, 24, 20, 13) - ellipse(CX + 2, 33, 15, 10)
    hm |= poly([(CX - 18, 28), (CX - 15, 28), (CX - 15, 46), (CX - 18, 44)])
    hm |= poly([(CX + 18, 28), (CX + 15, 28), (CX + 15, 46), (CX + 18, 44)])
    hr = hair(P, hm, h=3, r=2, dome=(CX - 2, 19, 18, 9, 7))
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 40), (CX + 1, 40), (CX + 3, 49), (CX - 3, 49)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 16, 26), (CX - 8, 15), (CX + 6, 12), (CX + 18, 20)],
                      [(CX - 12, 28), (CX - 2, 19), (CX + 10, 17), (CX + 17, 25)]], ("hair", 0))
        comb(cv, hr, [[(CX - 12, 20), (CX - 4, 14)]], ("hair", 2))
        cv.grid(CX + 26, 86, [".kkk.", "kgggk", "kgwgk", "kgggk", ".kkk."],
                {"k": INK, "g": ("gold", 0), "w": ("white", 1)})   # montre énorme
        for y in (84, 89, 94):
            cv.put(CX - 7, y, ("gold", 0))
        nostrils(cv, 49, 2)
        for sx in (-1, 1):
            T.fold(cv, (CX + sx * 6, 49), (CX + sx * 10, 53), (CX + sx * 10, 58))

    face(P, eye_y=38, eye_dx=8, eye_w=8, iris_r=1.5, brow_y=32, brow_w=8, brow_thick=1, brow_arch=3,
         brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.65, "raise_": 0.8}, "content": {"closed_smile": True}},
         brows={"neutre": (-1, -2), "content": (-2, -3)},
         mouth_y=56,
         mouths={"neutre": {"kind": "grin", "w": 15, "open_": 1.7, "curve": 2, "under": False},
                 "content": {"kind": "smile", "w": 17, "open_": 3, "curve": 3},
                 "furieux": {"kind": "grin", "w": 13, "open_": 1.3, "curve": 0.5, "under": False},
                 "gene": {"kind": "grit", "w": 13, "curve": 0}},
         extras={"furieux": both(vein_at(CX + 8, 18, ("pink", 0)),
                                 lambda cv: [cv.put(x, y, ("pink", 0)) for x in range(CX - 8, CX + 9)
                                             for y in (68, 69) if cv.get(x, y) and cv.get(x, y)[0] == "skin"]),
                 "gene": sweat_at(CX - 21, 26)})
    return P


# --- Firmin Tisserand, huissier de l'Élysée (écran Options, 3.19) ---------------------------

def huissier():
    """Firmin Tisserand : sexagénaire impassible, crâne dégarni et couronne de cheveux gris
    plaqués, grosse moustache blanche en guidon, sourcils broussailleux, nez fort, joues
    couperosées ; habit noir, plastron et nœud papillon blancs, chaîne d'argent à médaille sur
    les épaules, et un petit tournevis dans la poche de poitrine (il « tient les boutons »)."""
    P = P_({
        "bg": [(6, 5, 3)],
        "skin": [(3, 1, 1), (5, 3, 2), (6, 5, 4), (7, 6, 5)],
        "rose": [(7, 3, 3)],
        "suit": [(0, 0, 1), (1, 1, 2), (2, 2, 3)],
        "white": [(6, 6, 5), (7, 7, 7)],
        "silver": [(4, 4, 5), (6, 6, 7)],
        "steel": [(3, 4, 5)],
    })
    neck(P, w=17, top=58, bottom=74)
    s = bust(P, top=72, wide=37, suit="suit", shirt="white", tie="white", bow=True, vw=7, vdepth=16)
    med = P.layer("silver", ink=True, cast=(1, 1))
    med.ell(CX, 90, 4.5, 4.5, h=3)
    head = skull(P, cy=38, rx=18, ry=21, jaw=(15, 12, 52), ear_y=43, ear_dx=18, ear_r=(4, 6),
                 sockets=(8, 2))
    head.ell(CX - 5, 22, 9, 7, h=3, mode="add")          # crâne qui brille
    # couronne plaquée : deux touffes au-dessus et en arrière des oreilles (pas des écouteurs)
    hm = (ellipse(CX - 17, 32, 5, 5) | ellipse(CX + 17, 32, 5, 5)
          | ellipse(CX - 20, 36, 3, 3.5) | ellipse(CX + 20, 36, 3, 3.5)) - ellipse(CX, 37, 15.5, 21)
    hm -= rect(0, 39, 79, 95)
    hr = hair(P, hm, h=2, r=2, mat="silver")
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 39), (CX + 1, 39), (CX + 4, 50), (CX - 4, 50)]), h=4, r=2)
    nose.ell(CX, 49, 4.5, 3.5, h=3)
    mu = P.layer("white", ink=True, cast=(1, 1), smooth=1)  # moustache en guidon
    mu.ell(CX - 5, 54.5, 6, 3.2, h=3)
    mu.ell(CX + 5, 54.5, 6, 3.2, h=3)
    for sx in (-1, 1):                                    # branches qui tombent puis remontent
        mu.ell(CX + sx * 11, 56, 2.8, 2.2, h=2)
        mu.ell(CX + sx * 14, 54.5, 1.8, 2, h=2)
        mu.ell(CX + sx * 15.5, 51.5, 1.3, 1.8, h=2)
        mu.ell(CX + sx * 16, 49.5, 1, 1.3, h=1)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 14, 31), (CX - 21, 34)], [(CX - 15, 34), (CX - 22, 37)],
                      [(CX + 14, 31), (CX + 21, 34)], [(CX + 15, 34), (CX + 22, 37)]], ("silver", 0))
        comb(cv, mu, [[(CX - 9, 55), (CX - 2, 57)], [(CX + 2, 57), (CX + 9, 55)], [(CX - 7, 53), (CX - 3, 54)],
                     [(CX + 3, 54), (CX + 7, 53)]], ("white", 0))
        # chaîne d'argent posée sur les épaules, maillons alternés
        for i in range(0, 29):
            t = i / 28.0
            x = (1 - t) ** 2 * (CX - 27) + 2 * (1 - t) * t * CX + t * t * (CX + 27)
            y = (1 - t) ** 2 * 76 + 2 * (1 - t) * t * 104 + t * t * 76
            x, y = int(round(x)), int(round(y))
            if abs(x - CX) > 4:   # maillons de 2 px : une chaîne, pas une couture
                cv.put(x, y, ("silver", 1) if i % 2 else ("silver", 0))
                cv.put(x, y + 1, ("silver", 0) if i % 2 else ("silver", 1))
                cv.put(x, y + 2, INK)
        cv.put(CX - 1, 89, ("white", 1))                 # reflet de la médaille
        # tournevis : manche rouge dans la poche de poitrine, tige d'acier qui dépasse
        cv.fill(rect(CX + 15, 84, CX + 23, 84), ("suit", 2))
        cv.fill(rect(CX + 18, 79, CX + 19, 83), ("rose", 0))
        cv.fill(rect(CX + 18, 75, CX + 18, 78), ("steel", 0))
        cv.put(CX + 19, 78, INK)
        cv.put(CX + 20, 79, INK)
        # joues couperosées : une rougeur nette sous chaque pommette, pas des mouchetures
        for sx in (-1, 1):
            for (dx, dy) in ((0, 0), (2, 0), (1, 1), (3, 1), (4, 0)):
                x, y = CX + sx * (9 + dx), 46 + dy
                k = cv.get(x, y)
                if k and k[0] == "skin":
                    cv.put(x, y, ("rose", 0))
        # raie centrale de la moustache (sous le nez) : le guidon se lit en deux ailes
        cv.put(CX, 52, INK)
        cv.put(CX, 57, INK)
        nostrils(cv, 50, 3)

    def tip_up(cv):   # un coin de moustache qui se relève
        cv.fill(ellipse(CX + 16.5, 47.5, 1.2, 1.6), ("white", 1))
        cv.put(CX + 17, 45, INK)
        cv.put(CX + 18, 47, INK)
        cv.put(CX + 16, 45, INK)

    face(P, eye_y=41, eye_dx=8, eye_w=8, iris_r=1.5, lid=2, bags=1, brow_y=33, brow_w=12,
         brow_thick=3, brow_arch=1, brow_key=("silver", 1), iris=INK,
         eyes={"neutre": {"open_": 0.7}, "content": {"open_": 0.7, "right": {"closed_smile": True}}},
         brows={"neutre": (0, 1), "content": ((0, 1), (-1, -2))},
         mouth_y=61,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": 0},
                 "content": {"kind": "line", "w": 8, "curve": 1, "skew": 1},
                 "furieux": {"kind": "grit", "w": 10, "curve": -0.5},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"content": tip_up, "furieux": vein_at(CX + 7, 20, ("rose", 0)),
                 "gene": sweat_at(CX - 16, 24)})
    return P


ALL = [("chef_ugr", chef_ugr), ("chef_rsv", chef_rsv), ("chef_mjm", chef_mjm), ("chef_lso", chef_lso),
       ("jour_chaine_info", jour_chaine_info), ("jour_editorialiste", jour_editorialiste),
       ("senat_president", senat_president), ("synd_ust", synd_ust), ("patron_upe", patron_upe),
       ("huissier", huissier)]
