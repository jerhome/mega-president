"""Gueules V3 (3.08) : candidats à la présidentielle (ordre CAND_* de sim/election.h).

Tous les personnages sont fictifs (casting : docs/contenu/univers.md, section 5.4).
"""
import math

from raster import ellipse, poly, rect, line, border
import traits as T
from persos import (P_, CX, INK, bust, neck, skull, hair, nose_layer, nostrils, face,
                    comb, curl_marks, stripes, checks, rect_glasses, lipstick,
                    sweat_at, vein_at, blush_at, both)

WHITE = [(5, 5, 6), (7, 7, 7)]


# --- Camille Rouvière (UGR) ---------------------------------------------------------------

def cand_gauche_radicale():
    """Camille Rouvière : coupe courte rousse à mèche en crête, visage triangulaire,
    pommettes saillantes, regard perçant, veste de cuir, toujours en plein discours."""
    P = P_({
        "bg": [(6, 4, 4)],
        "skin": [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(3, 1, 0), (5, 2, 0), (7, 4, 1)],
        "suit": [(1, 0, 1), (2, 1, 1), (4, 2, 2)],
        "white": WHITE,
        "acc": [(5, 1, 1), (6, 2, 2)],
        "iris": [(3, 1, 0)],
    })
    neck(P, w=12, top=58, bottom=76)
    s = bust(P, top=75, wide=30, tie=None, vw=6, vdepth=11, open_collar=True)
    head = skull(P, cy=36, rx=17, ry=19, ear_y=41, ear_dx=17, ear_r=(3, 5.5), sockets=(8, 2.5),
                 cheeks=(10, 45, 4.5))
    head.shape(poly([(CX - 17, 34), (CX + 17, 34), (CX + 11, 54), (CX + 3, 65), (CX - 3, 65),
                     (CX - 11, 54)]), h=11, r=9)
    hm = ellipse(CX, 24, 19, 12) - rect(0, 27, 79, 95)
    hm |= poly([(CX - 8, 18), (CX - 2, 3), (CX + 4, 1), (CX + 10, 6), (CX + 12, 16)])   # crête
    hm |= ellipse(CX - 17, 31, 2.5, 7) | ellipse(CX + 17, 31, 2.5, 7)
    hm -= poly([(CX - 14, 27), (CX - 9, 24), (CX - 3, 27)])
    hr = hair(P, hm, h=4, r=3, dome=(CX + 2, 16, 12, 12, 5))
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 39), (CX + 1, 39), (CX + 3, 49), (CX - 3, 49)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 4, 22), (CX - 1, 10), (CX + 3, 4)], [(CX + 3, 21), (CX + 6, 11), (CX + 8, 6)],
                      [(CX - 13, 24), (CX - 10, 18)], [(CX + 14, 24), (CX + 12, 18)]], ("hair", 0))
        comb(cv, hr, [[(CX - 2, 14), (CX + 1, 6)]], ("hair", 2))
        nostrils(cv, 49, 2)
        for sx in (-1, 1):
            T.fold(cv, (CX + sx * 5, 49), (CX + sx * 9, 52), (CX + sx * 8, 57))
        T.wrinkle(cv, [(CX - 1, 30), (CX - 2, 33)])
        T.wrinkle(cv, [(CX + 1, 30), (CX + 2, 33)])
        stripes(cv, s, period=9, steps=1, x0=4)   # coutures du cuir

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m)
    face(P, eye_y=38, eye_dx=8, eye_w=8, lashes=True, iris_r=1.6, lid=2, brow_y=31, brow_w=10, brow_thick=2,
         brow_arch=1.5, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.75, "tilt": 0.8}},
         brows={"neutre": (2, -1)},
         mouth_y=56,
         mouths={"neutre": {"kind": "shout", "w": 8, "open_": 2, "lip": ("acc", 0)},
                 "content": {"kind": "smile", "w": 13, "open_": 3, "curve": 2, "lip": ("acc", 0)},
                 "furieux": {"kind": "shout", "w": 12, "open_": 4.5, "tongue": ("acc", 1)},
                 "gene": {"kind": "line", "w": 8, "curve": -0.5, "lips": lips}},
         extras={"furieux": lambda cv: [T.wrinkle(cv, [(CX + sx * 4, 66), (CX + sx * 5, 74)], ("skin", 0))
                                        for sx in (-1, 1)],
                 "gene": sweat_at(CX + 17, 22)})
    return P


# --- Noémie Vasseur (Génération Climat) ---------------------------------------------------

def cand_ecolo():
    """Noémie Vasseur : casque de vélo vert, longue tresse blonde, taches de rousseur,
    grands yeux ronds, nez retroussé, gros pull de laine."""
    P = P_({
        "bg": [(5, 6, 7)],
        "skin": [(3, 1, 1), (6, 4, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(4, 3, 0), (6, 5, 1), (7, 7, 4)],
        "suit": [(0, 3, 1), (2, 5, 1), (4, 7, 2)],
        "white": WHITE,
        "helm": [(0, 3, 1), (2, 5, 1), (4, 7, 2)],
        "iris": [(1, 3, 5)],
    })
    # tresse : une corde continue, texturée de chevrons (pas de chapelet de boules)
    br = P.layer("hair", cast=(1, 1), gain=1.3, smooth=1)
    br.shape(poly([(CX + 14, 44), (CX + 23, 44), (CX + 20, 96), (CX + 12, 96)]), h=4, r=4)
    neck(P, w=12, top=58, bottom=74)
    s = bust(P, top=72, wide=30, shirt=None, tie=None, sweater=True, lapels=False)
    roll = P.layer("suit", ink=True, cast=(1, 2))
    roll.shape(ellipse(CX, 72, 12, 5), h=3, r=3)
    br2 = P.layer("hair", cast=(1, 1), gain=1.3, smooth=1)     # la tresse passe devant
    br2.shape(poly([(CX + 13, 70), (CX + 22, 70), (CX + 20, 96), (CX + 12, 96)]), h=4, r=4)
    head = skull(P, cy=40, rx=17, ry=20, jaw=(14, 11, 52), ear_y=43, ear_dx=17, sockets=(8, 2))
    hm = ellipse(CX, 32, 19, 10) - rect(0, 34, 79, 95)
    hm |= ellipse(CX + 15, 40, 5, 9)
    hr = hair(P, hm, h=3, r=2)
    helm = P.layer("helm", cast=(1, 3), gain=1.2, smooth=1)
    hmk = ellipse(CX, 26, 22, 17) - rect(0, 29, 79, 95)
    helm.shape(hmk, h=4, r=4)
    helm.ell(CX - 2, 20, 20, 13, h=6, mode="add")
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 3, 2.5, h=3)

    @P.draw
    def tex(cv, e):
        for x in (CX - 12, CX - 4, CX + 4, CX + 12):          # aérations du casque
            for y in range(14, 22):
                if cv.owner_is(x, y, helm) and cv.owner_is(x + 1, y, helm):
                    cv.put(x, y, INK)
                    cv.put(x + 1, y, ("helm", 0))
        cv.fill(line(CX - 21, 28, CX + 21, 28), ("helm", 0))
        cv.fill(line(CX - 17, 30, CX - 15, 48), INK)            # sangle
        for y in range(47, 96, 5):        # mèches croisées de la tresse
            t = (y - 44) / 52.0
            xl, xr = CX + 14 - 2 * t, CX + 23 - 3 * t
            xc = (xl + xr) / 2
            for lay in (br, br2):
                comb(cv, lay, [[(xl + 1, y - 1), (xc, y + 2)], [(xc + 1, y + 2), (xr - 1, y - 1)]], ("hair", 0))
                comb(cv, lay, [[(xl + 2, y - 3), (xc - 1, y - 1)]], ("hair", 2))
        stripes(cv, s, period=4, x0=1)
        for (x, y) in ((CX - 12, 47), (CX - 9, 49), (CX - 13, 50), (CX - 7, 46), (CX + 11, 47), (CX + 8, 49),
                       (CX + 13, 50), (CX + 6, 46), (CX - 3, 45), (CX + 3, 45), (CX - 10, 52), (CX + 10, 52)):
            cv.put(x, y, ("skin", 1))
        cv.put(CX - 1, 50, INK)
        cv.put(CX + 1, 50, INK)

    face(P, eye_y=41, eye_dx=8, eye_w=9, eye_h=5, iris_r=2.3, lashes=True, brow_y=34, brow_w=9, brow_thick=2,
         brow_arch=2.5, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 1.0}, "content": {"open_": 0.7, "raise_": 1.5}},
         brows={"neutre": (-1, 1), "furieux": (4, -1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": -0.5},
                 "content": {"kind": "smile", "w": 13, "open_": 3, "curve": 2, "tongue": ("skin", 1)},
                 "furieux": {"kind": "pout", "w": 9},
                 "gene": {"kind": "wavy", "w": 9}},
         extras={"content": blush_at(52, 11, 4), "furieux": blush_at(52, 11, 4),
                 "gene": sweat_at(CX - 22, 34)})
    return P


# --- Hélène Marchetti (MJM) ---------------------------------------------------------------

def cand_centre():
    """Hélène Marchetti : chignon brun serré, visage ovale aux joues pleines, sourire
    conciliant, sourcil en point d'interrogation, écharpe tricolore, tailleur beige."""
    P = P_({
        "bg": [(4, 5, 6)],
        "skin": [(3, 1, 1), (5, 3, 2), (7, 5, 4), (7, 6, 5)],
        "hair": [(1, 0, 1), (3, 1, 1), (5, 3, 2)],
        "suit": [(4, 3, 2), (5, 5, 3), (6, 6, 4)],
        "white": WHITE,
        "flag": [(1, 2, 6), (6, 1, 1)],
        "acc": [(5, 1, 2), (7, 6, 2)],
        "iris": [(3, 1, 1)],
    })
    bun = P.layer("hair", cast=(1, 2), gain=1.3)
    bun.ell(CX + 2, 10, 9, 7.5, h=6)
    neck(P, w=13, top=58, bottom=76)
    s = bust(P, top=75, wide=31, tie=None, vw=6, vdepth=11, collar=True)
    sash = P.layer("flag", ink="always", cast=(1, 1), flat=1)
    sash.shape(poly([(CX + 18, 72), (CX + 26, 76), (CX - 12, 97), (CX - 22, 97)]), h=2, r=2)
    head = skull(P, cy=38, rx=17, ry=21, jaw=(16, 12, 52), ear_y=43, ear_dx=17, cheeks=(10, 50, 5.5),
                 sockets=(8, 2))
    hm = ellipse(CX, 28, 19, 15) - ellipse(CX + 3, 37, 16, 13)
    hm -= rect(0, 41, 79, 95)
    hr = hair(P, hm, h=3, r=3, dome=(CX, 24, 18, 12, 6))
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 3, 2.5, h=3)
    nose.shape(poly([(CX - 1, 41), (CX + 1, 41), (CX + 1, 47), (CX - 1, 47)]), h=2, r=1)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 15, 34), (CX - 8, 20), (CX + 2, 15)], [(CX + 4, 15), (CX + 14, 21), (CX + 16, 30)]],
             ("hair", 0))
        curl_marks(cv, bun, [(CX + 2, 10, 6)], hi=("hair", 2))
        # écharpe : bande bleue, blanche, rouge
        for y in range(70, 96):
            for x in range(10, 70):
                if cv.owner_is(x, y, sash) and cv.get(x, y)[0] != "ink":
                    t = (x * 2 + y * 3) % 18
                    cv.put(x, y, ("flag", 0) if t < 6 else (("white", 1) if t < 12 else ("flag", 1)))
        for sx in (-1, 1):   # boucles d'oreille
            cv.grid(CX + sx * 17 - 1, 47, [".k.", "kyk", ".k."], {"k": INK, "y": ("acc", 1)})
        cv.put(CX - 1, 50, INK)
        cv.put(CX + 1, 50, INK)

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m)
    face(P, eye_y=40, eye_dx=8, eye_w=8, lashes=True, iris_r=1.7, brow_y=32, brow_w=10, brow_thick=2,
         brow_arch=3, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 0.85}},
         brows={"neutre": ((0, 0), (-3, -1)), "content": ((-1, -1), (-3, -2)), "furieux": ((3, 0), (-2, -2))},
         mouth_y=57,
         mouths={"neutre": {"kind": "line", "w": 11, "curve": 1.3, "lips": lips},
                 "content": {"kind": "smile", "w": 14, "open_": 2.5, "curve": 2, "lip": ("acc", 0)},
                 "furieux": {"kind": "smirk", "w": 10, "curve": -1, "lips": lips},
                 "gene": {"kind": "wavy", "w": 10}},
         extras={"gene": sweat_at(CX - 21, 30)})
    return P


# --- Édouard Valmont (UTL) ----------------------------------------------------------------

def cand_droite():
    """Édouard Valmont : visage rond comme une lune, double menton étagé, sourire de
    banquier, petits yeux plissés, cheveux blancs courts, lunettes demi-lune, chaîne de montre."""
    P = P_({
        "bg": [(5, 5, 4)],
        "skin": [(3, 1, 1), (6, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(4, 4, 5), (6, 6, 6), (7, 7, 7)],
        "suit": [(1, 1, 2), (2, 2, 3), (3, 3, 5)],
        "white": [(4, 4, 5), (7, 7, 7)],
        "tie": [(5, 4, 1), (7, 6, 2)],
        "iris": [(1, 1, 2)],
    })
    neck(P, w=24, top=58, bottom=76)
    s = bust(P, top=76, wide=38, tie="tie", vw=6, vdepth=12)
    vest = P.layer("suit", ink="always")
    vest.shape(poly([(CX - 9, 82), (CX + 9, 82), (CX + 12, 97), (CX - 12, 97)]), h=3, r=3)
    head = skull(P, cy=40, rx=22, ry=22, ear_y=40, ear_dx=21, ear_r=(3, 5), sockets=(8, 2),
                 cheeks=(12, 47, 7))
    chin = P.layer("skin", ink="shadow", cast=(1, 2), ink_dirs="d")
    chin.ell(CX, 64, 15, 6, h=5)
    chin2 = P.layer("skin", ink="shadow", cast=(1, 2), ink_dirs="d")
    chin2.ell(CX, 69, 11, 4, h=4)
    hm = ellipse(CX, 30, 23, 15) - ellipse(CX, 38, 19, 15)
    hm -= rect(0, 38, 79, 95)
    hr = hair(P, hm, h=3, r=2, dome=(CX, 22, 18, 8, 4))
    nose = nose_layer(P)
    nose.ell(CX, 47, 4, 3.5, h=4)

    @P.draw
    def tex(cv, e):
        cv.fill(line(CX - 9, 87, CX + 9, 90), ("tie", 1))       # chaîne de montre
        cv.put(CX - 9, 87, INK)
        for y in (85, 90, 95):
            cv.put(CX, y, ("tie", 0))
        nostrils(cv, 49, 2)
        comb(cv, hr, [[(CX - 18, 30), (CX - 10, 21)], [(CX + 18, 30), (CX + 10, 21)]], ("hair", 0))

    def demi_lune(cv, e):
        for sx in (-1, 1):
            x0 = CX + sx * 8 - 5
            cv.fill(line(x0, 43, x0 + 9, 43), ("tie", 0))
            cv.curve((x0, 43), (x0 + 5, 49), (x0 + 9, 43), ("tie", 0))
        cv.fill(line(CX - 4, 44, CX + 3, 44), ("tie", 0))

    face(P, eye_y=40, eye_dx=8, eye_w=7, eye_h=3, iris_r=1.2, brow_y=33, brow_w=9, brow_thick=2,
         brow_arch=2, brow_key=("hair", 0), glasses=demi_lune,
         eyes={"neutre": {"open_": 0.55, "raise_": 0.8}, "content": {"closed_smile": True},
               "furieux": {"open_": 0.5, "tilt": 1.5}},
         brows={"neutre": (-1, -1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "grin", "w": 12, "open_": 1.3, "curve": 2.2, "under": False},
                 "content": {"kind": "smile", "w": 15, "open_": 3, "curve": 2.5, "tongue": ("skin", 1)},
                 "furieux": {"kind": "grit", "w": 13, "curve": 1},
                 "gene": {"kind": "wavy", "w": 10}},
         extras={"furieux": blush_at(50, 13, 5),
                 "content": lambda cv: [T.wrinkle(cv, [(CX + sx * 5, 70), (CX + sx * 9, 69)]) for sx in (-1, 1)],
                 "gene": sweat_at(CX + 17, 24)})
    return P


# --- Gaspard Lenoir (LSO) -----------------------------------------------------------------

def cand_nationaliste():
    """Gaspard Lenoir : cheveux noirs gominés en arrière, front bas, teint rougeaud, mâchoire
    de boxeur, bouche immense aux dents carnassières, sourcils en accent circonflexe."""
    P = P_({
        "bg": [(5, 3, 3)],
        "skin": [(3, 1, 1), (6, 3, 2), (7, 5, 4), (7, 6, 5)],   # rougeaud (DA : plus de teint brun)
        "hair": [(1, 0, 1), (1, 1, 2), (3, 3, 4)],
        "suit": [(1, 0, 1), (1, 1, 1), (2, 2, 2)],
        "white": [(5, 5, 5), (7, 7, 7)],
        "acc": [(5, 0, 1), (7, 2, 2)],
        "iris": [(2, 4, 6)],                                   # regard bleu glacier
    })
    neck(P, w=24, top=56, bottom=76)
    bust(P, top=74, wide=37, tie=None, vw=7, vdepth=13, open_collar=True)
    head = skull(P, cy=40, rx=18, ry=19, ear_y=40, ear_dx=19, sockets=(8, 3))
    head.shape(poly([(CX - 19, 38), (CX + 19, 38), (CX + 21, 56), (CX + 14, 66), (CX - 14, 66),
                     (CX - 21, 56)]), h=13, r=10)
    head.ell(CX, 33, 15, 3, h=3, mode="add")
    hm = ellipse(CX, 24, 21, 14) - rect(0, 31, 79, 95)
    hm -= ellipse(CX - 9, 33, 9, 7) | ellipse(CX + 9, 33, 9, 7)   # golfes : pointe sur le front bas
    hm |= ellipse(CX - 19, 30, 2.5, 7) | ellipse(CX + 19, 30, 2.5, 7)
    hr = hair(P, hm, h=2, r=2, dome=(CX + 2, 21, 19, 10, 7))
    nose = nose_layer(P)
    nose.shape(poly([(CX - 2, 38), (CX + 2, 38), (CX + 4, 48), (CX - 4, 48)]), h=3, r=2)
    nose.ell(CX, 47, 5, 3, h=3, mode="add")

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 14 + k * 6, 30), (CX - 12 + k * 5, 20), (CX - 6 + k * 3, 13)] for k in range(5)],
             ("hair", 0))
        comb(cv, hr, [[(CX - 12, 22), (CX - 7, 15), (CX + 2, 13)], [(CX - 9, 26), (CX - 5, 20)]], ("hair", 2))
        nostrils(cv, 49, 3)
        for sx in (-1, 1):
            T.fold(cv, (CX + sx * 6, 48), (CX + sx * 12, 52), (CX + sx * 12, 60))

    face(P, eye_y=37, eye_dx=8, eye_w=8, iris_r=1.6, lid=2, brow_y=32, brow_w=12, brow_thick=3,
         brow_arch=3.5, brow_key=INK, taper=False,
         eyes={"neutre": {"open_": 0.6, "tilt": 1.2}},
         brows={"neutre": (2, 1), "content": (1, 0)},
         mouth_y=56,
         mouths={"neutre": {"kind": "grit", "w": 15, "curve": 0},
                 "content": {"kind": "grin", "w": 17, "open_": 2.5, "curve": 2},
                 "furieux": {"kind": "shout", "w": 15, "open_": 5, "tongue": ("acc", 1)},
                 "gene": {"kind": "wavy", "w": 12}},
         extras={"furieux": lambda cv: [cv.put(x, y, ("white", 1)) for (x, y) in
                                        ((CX - 12, 54), (CX + 13, 53), (CX + 15, 58), (CX - 14, 60))],
                 "gene": sweat_at(CX + 19, 24)})
    return P


ALL = [("cand_gauche_radicale", cand_gauche_radicale), ("cand_ecolo", cand_ecolo),
       ("cand_centre", cand_centre), ("cand_droite", cand_droite), ("cand_nationaliste", cand_nationaliste)]
