"""Gueules V3 (3.08) : présidents, Premiers ministres, PM de cohabitation.

Tous les personnages sont fictifs (casting : docs/contenu/univers.md, section 5).
"""
import math

from raster import ellipse, poly, rect, line
import traits as T
from persos import (P_, CX, INK, bust, neck, skull, hair, nose_layer, nostrils, face,
                    comb, curl_marks, stripes, rect_glasses, round_glasses, lipstick,
                    sweat_at, vein_at, blush_at, both, dots)

SKIN_PALE = [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 6)]
SKIN_STD = [(3, 1, 1), (5, 3, 2), (6, 4, 3), (7, 6, 5)]
SKIN_ROSE = [(3, 1, 1), (5, 3, 3), (7, 5, 4), (7, 6, 5)]
WHITE = [(5, 5, 6), (7, 7, 7)]


# --- Laurent Delmas, président MJM ---------------------------------------------------------

def pres_centre():
    """Laurent Delmas : visage en rectangle étroit, front haut, raie nette, sourire
    de publicité aux dents parfaites, sourcils fins, costume ajusté, cravate bleue."""
    P = P_({
        "bg": [(6, 5, 3)],
        "skin": SKIN_ROSE,
        "hair": [(1, 0, 1), (3, 2, 1), (5, 3, 2)],
        "suit": [(1, 1, 2), (2, 2, 4), (3, 3, 5)],
        "white": WHITE,
        "tie": [(1, 2, 5), (3, 5, 7)],
        "iris": [(1, 2, 5)],
    })
    neck(P, w=14, top=58, bottom=76)
    bust(P, top=75, wide=27, tie="tie", vw=5, vdepth=11)
    head = skull(P, cy=36, rx=15, ry=23, jaw=(14, 12, 54), chin=(9, 5, 64), ear_y=42, ear_dx=15,
                 cheeks=(9, 51, 4))
    head.shape(poly([(CX - 15, 30), (CX + 15, 30), (CX + 14, 60), (CX - 14, 60)]), h=12, r=10)
    hm = ellipse(CX, 22, 17, 13) - poly([(CX - 6, 25), (CX + 20, 21), (CX + 18, 40), (CX - 14, 40)])
    hm -= rect(0, 26, 79, 95)
    hm |= ellipse(CX - 15, 26, 3, 6) | ellipse(CX + 15, 28, 2.5, 5)
    hr = hair(P, hm, h=4, r=3)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 40), (CX + 1, 40), (CX + 3, 50), (CX - 3, 50)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 6, 13), (CX - 5, 20), (CX - 6, 24)]], INK)          # raie
        comb(cv, hr, [[(CX - 3, 13), (CX + 8, 14), (CX + 14, 20)],
                      [(CX - 3, 17), (CX + 6, 18), (CX + 12, 23)]], ("hair", 0))
        comb(cv, hr, [[(CX - 12, 15), (CX - 9, 12)]], ("hair", 2))
        nostrils(cv, 50, 2)
        for s in (-1, 1):
            T.fold(cv, (CX + s * 5, 51), (CX + s * 9, 55), (CX + s * 9, 59))

    face(P, eye_y=40, eye_dx=7, eye_w=8, brow_y=33, brow_w=10, brow_thick=1, brow_arch=1.5, taper=False,
         eyes={"furieux": {"open_": 0.95, "tilt": 0, "right": {"open_": 0.35, "tilt": 1.5}},
               "neutre": {"open_": 0.85}},
         brows={"furieux": ((0, 0), (2, -1)), "content": (-1, -1)},
         mouth_y=58,
         mouths={"neutre": {"kind": "grin", "w": 11, "open_": 1.6, "curve": 1.5, "under": False},
                 "content": {"kind": "smile", "w": 15, "open_": 3, "curve": 2.5},
                 "furieux": {"kind": "grin", "w": 13, "open_": 1.6, "curve": 1.0, "under": False},
                 "gene": {"kind": "grit", "w": 11, "curve": 0}},
         extras={"furieux": lambda cv: (T.wrinkle(cv, [(CX + 13, 37), (CX + 15, 36)], INK),
                                        T.wrinkle(cv, [(CX + 13, 43), (CX + 15, 44)], INK)),
                 "gene": sweat_at(CX + 13, 20)})
    return P


# --- Henri de Saint-Aubin, président UTL ---------------------------------------------------

def pres_droite():
    """Henri de Saint-Aubin : mâchoire carrée, menton fendu, cou de taureau, cheveux gris
    plaqués aux tempes argentées, sourcils horizontaux, regard de commandant, cravate rouge."""
    P = P_({
        "bg": [(4, 4, 6)],
        "skin": SKIN_ROSE,
        "hair": [(2, 2, 3), (4, 4, 5), (5, 5, 6)],
        "suit": [(1, 0, 1), (1, 1, 2), (2, 2, 4)],
        "white": WHITE,
        "tie": [(4, 0, 0), (7, 1, 1)],
        "iris": [(2, 2, 3)],
    })
    neck(P, w=26, top=54, bottom=76)
    bust(P, top=74, wide=36, tie="tie", vw=6, vdepth=12)
    head = skull(P, cy=34, rx=17, ry=20, jaw=None, ear_y=40, ear_dx=18, sockets=(8, 3))
    head.shape(poly([(CX - 18, 32), (CX + 18, 32), (CX + 19, 56), (CX + 13, 66), (CX - 13, 66),
                     (CX - 19, 56)]), h=14, r=10)
    head.ell(CX, 64, 9, 4, h=5)
    hm = ellipse(CX, 24, 19, 15) - ellipse(CX, 34, 15, 13)
    hm |= ellipse(CX - 17, 30, 3, 8) | ellipse(CX + 17, 30, 3, 8)
    hm -= rect(0, 38, 79, 95)
    hr = hair(P, hm, h=2, r=2, dome=(CX - 2, 22, 20, 14, 10))
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1.5, 38), (CX + 1.5, 38), (CX + 3.5, 49), (CX - 3.5, 49)]), h=3, r=2)
    nose.ell(CX, 48, 4, 2.5, h=2, mode="add")

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 12 + k * 6, 24), (CX - 10 + k * 5, 15), (CX - 6 + k * 3, 10)] for k in range(5)],
             ("hair", 0))
        comb(cv, hr, [[(CX - 17, 26), (CX - 17, 34)], [(CX + 17, 26), (CX + 17, 34)]], ("hair", 2))
        nostrils(cv, 49, 3)
        T.wrinkle(cv, [(CX, 64), (CX, 66)])            # menton fendu
        for s in (-1, 1):
            T.fold(cv, (CX + s * 6, 49), (CX + s * 11, 54), (CX + s * 11, 60))
        T.wrinkle(cv, [(CX - 8, 25), (CX + 8, 25)])

    face(P, eye_y=37, eye_dx=8, eye_w=8, iris_r=1.6, lid=2, brow_y=31, brow_w=12, brow_thick=3,
         brow_arch=0.3, taper=False, brow_key=("hair", 1),
         eyes={"neutre": {"open_": 0.6}, "content": {"open_": 0.55, "raise_": 0.8}},
         brows={"neutre": (1, 0), "content": (0, -1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "line", "w": 12, "curve": -0.5},
                 "content": {"kind": "smirk", "w": 12, "curve": 2},
                 "furieux": {"kind": "grit", "w": 14, "curve": -1},
                 "gene": {"kind": "wavy", "w": 10}},
         extras={"furieux": vein_at(CX + 7, 21, ("tie", 1)), "gene": sweat_at(CX - 21, 24)})
    return P


# --- Pierre Delaunay, PM technocrate -------------------------------------------------------

def pm_technocrate():
    """Pierre Delaunay : crâne dégarni et luisant, trois mèches rabattues, lunettes
    rectangulaires fines, nez long et pointu, bouche en trait, costume gris souris, voûté."""
    P = P_({
        "bg": [(5, 6, 6)],
        "skin": SKIN_PALE,
        "hair": [(2, 2, 2), (4, 4, 4), (5, 5, 6)],
        "suit": [(2, 2, 2), (3, 3, 3), (4, 4, 4)],
        "white": WHITE,
        "tie": [(1, 1, 3), (2, 2, 5)],
        "iris": [(2, 2, 2)],
        "fog": [(5, 5, 6)],
    })
    neck(P, w=12, top=60, bottom=74)
    s = bust(P, top=70, wide=30, tie="tie", vw=5, vdepth=11)   # épaules hautes : dos voûté
    head = skull(P, cy=38, rx=17, ry=22, jaw=(12, 12, 54), chin=(6, 5, 64), ear_y=44, ear_dx=17,
                 ear_r=(4, 6.5), sockets=(7, 2.5))
    head.ell(CX - 4, 22, 9, 7, h=3, mode="add")     # crâne bombé, luisant
    hm = (ellipse(CX - 16, 41, 4, 7) | ellipse(CX + 16, 41, 4, 7)) - ellipse(CX, 46, 14, 14)
    hr = hair(P, hm, h=2, r=2)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 41), (CX + 1, 41), (CX + 3, 52), (CX + 1, 55), (CX - 2, 53)]), h=4, r=2)

    @P.draw
    def tex(cv, e):
        for (y, x0, x1) in ((24, CX - 9, CX + 11), (27, CX - 11, CX + 12), (21, CX - 6, CX + 9)):
            cv.curve((x0, y + 2), ((x0 + x1) / 2, y - 2), (x1, y + 1), ("hair", 0))   # mèches rabattues
        cv.put(CX - 7, 17, ("white", 1))
        cv.put(CX - 6, 17, ("white", 1))
        cv.put(CX - 8, 18, ("white", 1))
        nostrils(cv, 54, 2)
        for x in range(CX + 14, CX + 30):                    # dossier sous le bras
            for y in range(80, 90):
                if x in (CX + 14, CX + 29) or y in (80, 89):
                    cv.put(x, y, INK)
                else:
                    cv.put(x, y, ("fog", 0) if y < 82 else ("white", 1))
        for y in (84, 86):
            cv.fill(line(CX + 17, y, CX + 26, y), ("suit", 1))

    def glasses(cv, e):
        rect_glasses(cv, 37, dx=7, w=10, h=5)
        if e == "furieux":   # lunettes embuées
            for sx in (-1, 1):
                x0 = int(round(CX + sx * 7 - 5))
                for x in range(x0 + 1, x0 + 9):
                    for y in range(38, 41):
                        cv.put(x, y, ("fog", 0) if (x + y) % 3 else ("white", 1))

    face(P, eye_y=39, eye_dx=7, eye_w=7, eye_h=3.5, iris_r=1.3, brow_y=33, brow_w=9, brow_thick=1,
         brow_arch=1.0, brow_key=("hair", 0), glasses=glasses,
         eyes={"neutre": {"open_": 0.7}, "content": {"open_": 0.6, "raise_": 0.6}},
         brows={"neutre": (0, 0), "content": (0, 0), "furieux": ((3, -1), (1, -3))},
         mouth_y=59,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": 0},
                 "content": {"kind": "line", "w": 8, "curve": 0.6},
                 "furieux": {"kind": "line", "w": 9, "curve": -1.2},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"gene": sweat_at(CX + 4, 12), "furieux": vein_at(CX - 12, 18, ("skin", 0))})
    return P


# --- Marianne Deschamps, PM rassembleuse ---------------------------------------------------

def pm_rassembleur():
    """Marianne Deschamps : carré blond en casque, visage rond, joues roses, large
    sourire maternel, veste rouge vif, gros collier de perles de bois."""
    P = P_({
        "bg": [(5, 6, 7)],
        "skin": [(3, 1, 1), (6, 3, 3), (7, 5, 4), (7, 6, 6)],
        "hair": [(4, 2, 0), (6, 5, 1), (7, 7, 4)],
        "suit": [(3, 0, 0), (5, 1, 1), (7, 2, 2)],
        "white": WHITE,
        "acc": [(5, 1, 1), (7, 2, 2)],
        "wood": [(4, 2, 0), (6, 5, 1)],
        "iris": [(1, 3, 3)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.2)
    hb.shape(ellipse(CX, 38, 26, 27) - rect(0, 60, 79, 95), h=8, r=7)
    neck(P, w=14, top=58, bottom=76)
    bust(P, top=76, wide=34, tie=None, vw=7, vdepth=10, collar=False, lapels=True, open_collar=True)
    head = skull(P, cy=42, rx=19, ry=20, jaw=(17, 12, 52), ear_y=None, cheeks=(10, 51, 5), sockets=(8, 2))
    hf = P.layer("hair", cast=(1, 2), gain=1.3)
    fr = ellipse(CX, 22, 23, 14) - rect(0, 30, 79, 95)
    fr |= ellipse(CX - 8, 26, 14, 7) | ellipse(CX + 10, 25, 12, 6)
    hf.shape(fr, h=5, r=4)
    hf.ell(CX - 10, 27, 8, 4, h=2, mode="add")
    hf.ell(CX + 9, 26, 8, 4, h=2, mode="add")
    sides = P.layer("hair", cast=(1, 1), gain=1.2)
    sides.shape(poly([(CX - 25, 30), (CX - 18, 30), (CX - 19, 62), (CX - 26, 62)]), h=4, r=3)
    sides.shape(poly([(CX + 18, 30), (CX + 25, 30), (CX + 26, 62), (CX + 19, 62)]), h=4, r=3)
    nose = nose_layer(P)
    nose.ell(CX + 0.5, 49, 3, 2.5, h=3)

    @P.draw
    def tex(cv, e):
        comb(cv, hf, [[(CX - 18, 30), (CX - 8, 18), (CX + 2, 13)], [(CX + 4, 14), (CX + 14, 20), (CX + 19, 29)]],
             ("hair", 0))
        comb(cv, sides, [[(CX - 22, 34), (CX - 22, 58)], [(CX + 22, 34), (CX + 22, 58)]], ("hair", 0))
        comb(cv, hf, [[(CX - 12, 15), (CX - 4, 11)]], ("hair", 2))
        for k in range(11):                     # collier de perles de bois
            a = math.radians(20 + 140 * k / 10)
            x = CX + 15 * math.cos(a)
            y = 74 + 7 * math.sin(a)
            cv.grid(int(x) - 1, int(y) - 1, [".k.", "kwk", ".k."], {"k": INK, "w": ("wood", 1)})
            cv.put(int(x) - 1, int(y), ("wood", 0))
        T.blush(cv, CX - 15, 52, w=4, key=("acc", 1))
        T.blush(cv, CX + 11, 52, w=4, key=("acc", 1))

    lips = lambda cv, mx, my, m: lipstick(cv, mx, my, m)
    face(P, eye_y=41, eye_dx=8, eye_w=8, lashes=True, iris_r=1.8, brow_y=34, brow_w=10, brow_thick=2,
         brow_arch=2.5, brow_key=("hair", 0),
         eyes={"content": {"closed_smile": True}},
         brows={"neutre": (-1, 0), "furieux": (4, -1)},
         mouth_y=57,
         mouths={"neutre": {"kind": "smile", "w": 13, "open_": 2, "curve": 1.5, "lip": ("acc", 0)},
                 "content": {"kind": "smile", "w": 15, "open_": 4, "curve": 2, "tongue": ("acc", 1),
                             "lip": ("acc", 0)},
                 "furieux": {"kind": "line", "w": 8, "curve": -1, "lips": lips},
                 "gene": {"kind": "wavy", "w": 10}},
         extras={"gene": both(sweat_at(CX + 16, 34)),
                 "furieux": lambda cv: T.wrinkle(cv, [(CX, 33), (CX, 36)])})
    return P


# --- Julien Lestrade, PM réformateur -------------------------------------------------------

def pm_reformateur():
    """Julien Lestrade : tête fine et anguleuse, banane gominée en vague haute, grands
    yeux cernés, dents de lapin, chemise blanche sans cravate, goutte de sueur."""
    P = P_({
        "bg": [(6, 6, 4)],
        "skin": SKIN_STD,
        "hair": [(1, 0, 0), (3, 1, 0), (5, 2, 1)],
        "suit": [(4, 4, 5), (6, 6, 6), (7, 7, 7)],
        "white": [(4, 4, 5), (7, 7, 7)],
        "iris": [(1, 3, 1)],
        "acc": [(6, 1, 1)],
    })
    neck(P, w=11, top=58, bottom=76)
    bust(P, top=75, wide=27, suit="suit", tie=None, vw=5, vdepth=10, lapels=False, open_collar=True)
    head = skull(P, cy=40, rx=15, ry=20, ear_y=44, ear_dx=15, sockets=(7, 2.5))
    head.shape(poly([(CX - 15, 40), (CX + 15, 40), (CX + 10, 60), (CX + 3, 67), (CX - 3, 67), (CX - 10, 60)]),
               h=10, r=8)
    hm = poly([(CX - 15, 36), (CX - 16, 24), (CX - 12, 12), (CX - 4, 5), (CX + 8, 2), (CX + 18, 5),
               (CX + 21, 12), (CX + 16, 16), (CX + 17, 34), (CX + 13, 26), (CX + 2, 22), (CX - 8, 24),
               (CX - 13, 30)])
    hr = hair(P, hm, h=6, r=4)
    hr.ell(CX + 4, 10, 12, 6, h=3, mode="add")
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1, 43), (CX + 1, 43), (CX + 2, 53), (CX - 3, 53)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 12, 22), (CX - 6, 10), (CX + 6, 5), (CX + 18, 8)],
                      [(CX - 8, 23), (CX - 2, 13), (CX + 10, 9), (CX + 17, 13)]], ("hair", 0))
        comb(cv, hr, [[(CX - 10, 14), (CX - 3, 8), (CX + 5, 5)]], ("hair", 2))
        nostrils(cv, 53, 2)
        if e != "content":
            T.sweat(cv, CX - 21, 30)
        if e == "furieux":   # la banane se défait
            for (x0, y0, x1, y1) in ((CX - 14, 12, CX - 19, 8), (CX + 16, 6, CX + 22, 2), (CX - 6, 5, CX - 8, 0)):
                cv.fill(line(x0, y0, x1, y1), INK)
                cv.fill(line(x0, y0 + 1, x1, y1 + 1), ("hair", 1))

    def teeth(cv, mx, my, m):   # dents de lapin
        cv.grid(int(mx) - 2, my + 1, ["kwwk", "kwwk", ".kk."], {"k": INK, "w": ("white", 1)})

    face(P, eye_y=41, eye_dx=7, eye_w=9, eye_h=4.5, iris_r=2.0, bags=2, brow_y=33, brow_w=10, brow_thick=2,
         brow_arch=3, brow_key=("hair", 0),
         eyes={"neutre": {"open_": 1.0, "look": 2}},
         brows={"neutre": (-1, -2), "content": (-2, -3), "furieux": (5, -2), "gene": (-4, 1)},
         mouth_y=58,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": 0, "lips": teeth},
                 "content": {"kind": "smile", "w": 13, "open_": 3, "curve": 2, "tongue": ("acc", 0)},
                 "furieux": {"kind": "shout", "w": 11, "open_": 3.5, "tongue": ("acc", 0)},
                 "gene": {"kind": "wavy", "w": 8, "lips": teeth}},
         extras={"gene": blush_at(51, 9, 3)})
    return P


# --- Sophie Carrel, PM de cohabitation de gauche -------------------------------------------

def cohab_gauche():
    """Sophie Carrel : cheveux courts poivre et sel en pétard, visage carré, rides
    profondes, regard direct, foulard rouge noué, veste en toile, tête rentrée."""
    P = P_({
        "bg": [(6, 5, 5)],
        "skin": SKIN_STD,
        "hair": [(2, 2, 2), (4, 4, 4), (6, 6, 6)],
        "suit": [(2, 2, 1), (3, 3, 2), (5, 4, 3)],
        "white": [(6, 6, 6), (7, 7, 7)],
        "scarf": [(4, 0, 0), (7, 1, 1)],
        "iris": [(2, 2, 2)],
    })
    neck(P, w=18, top=58, bottom=74)
    bust(P, top=71, wide=36, suit="suit", shirt=None, tie=None, lapels=True, vw=8, vdepth=12)
    sc = P.layer("scarf", cast=(1, 2), gain=1.2)
    sc.shape(ellipse(CX, 69, 15, 6), h=4, r=3)
    sc.shape(poly([(CX - 2, 70), (CX + 5, 70), (CX + 9, 86), (CX + 3, 88)]), h=3, r=2)
    sc.shape(poly([(CX - 4, 70), (CX + 1, 72), (CX - 3, 85), (CX - 8, 83)]), h=3, r=2)
    head = skull(P, cy=38, rx=18, ry=20, ear_y=42, ear_dx=18, sockets=(8, 3))
    head.shape(poly([(CX - 18, 36), (CX + 18, 36), (CX + 17, 56), (CX + 11, 64), (CX - 11, 64),
                     (CX - 17, 56)]), h=12, r=9)
    spikes = ellipse(CX, 25, 19, 11) - rect(0, 29, 79, 95)
    for k, top in enumerate((17, 12, 14, 10, 13, 9, 13, 11, 16)):
        x = CX - 18 + k * 4.5
        spikes |= poly([(x - 4.5, 22), (x + 4.5, 22), (x + (k - 4) * 1.1 + (1 if k % 2 else -1), top)])
    spikes |= ellipse(CX - 18, 31, 2.5, 6) | ellipse(CX + 18, 31, 2.5, 6)
    hr = hair(P, spikes, h=4, r=3)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1.5, 40), (CX + 1.5, 40), (CX + 3, 50), (CX - 3, 50)]), h=3, r=2)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 12 + k * 5, 24), (CX - 12 + k * 5 + (k - 2), 16)] for k in range(6)], ("hair", 0))
        comb(cv, hr, [[(CX - 14 + k * 7, 20), (CX - 13 + k * 7, 15)] for k in range(5)], ("hair", 2))
        nostrils(cv, 50, 3)
        for s in (-1, 1):
            T.fold(cv, (CX + s * 6, 50), (CX + s * 11, 55), (CX + s * 11, 61))
            T.wrinkle(cv, [(CX + s * 15, 40), (CX + s * 17, 42)])
        T.wrinkle(cv, [(CX - 9, 26), (CX + 9, 26)])
        T.wrinkle(cv, [(CX - 7, 29), (CX + 7, 29)])

    face(P, eye_y=38, eye_dx=8, eye_w=8, iris_r=1.6, lid=2, brow_y=32, brow_w=11, brow_thick=2,
         brow_arch=1, brow_key=("hair", 0), taper=False, bags=1,
         eyes={"neutre": {"open_": 0.75}},
         brows={"neutre": (2, 0)},
         mouth_y=57,
         mouths={"neutre": {"kind": "line", "w": 11, "curve": -0.6},
                 "content": {"kind": "smile", "w": 14, "open_": 3.5, "curve": 2, "tongue": ("scarf", 1)},
                 "furieux": {"kind": "shout", "w": 12, "open_": 4, "tongue": ("scarf", 1)},
                 "gene": {"kind": "smirk", "w": 9, "curve": -1}},
         extras={"gene": sweat_at(CX + 19, 26)})
    return P


# --- Bernard Lacombe, PM de cohabitation du centre -----------------------------------------

def cohab_centre():
    """Bernard Lacombe : crâne chauve à couronne grise, bajoues, moustache grise touffue,
    sourcils tombants et inquiets, gilet de laine sous le veston, petit corps rond."""
    P = P_({
        "bg": [(4, 5, 4)],
        "skin": SKIN_ROSE,
        "hair": [(3, 3, 3), (5, 5, 5), (7, 7, 7)],
        "suit": [(1, 1, 2), (2, 2, 3), (3, 3, 4)],
        "white": [(5, 5, 5), (7, 7, 7)],
        "wool": [(3, 2, 1), (5, 4, 2)],
        "tie": [(3, 3, 1)],
        "iris": [(2, 2, 3)],
    })
    neck(P, w=24, top=56, bottom=76)
    s = bust(P, top=76, wide=36, tie=None, vw=8, vdepth=16)
    gilet = P.layer("wool", ink=True)
    gilet.shape(poly([(CX - 9, 76), (CX + 9, 76), (CX + 13, 96), (CX - 13, 96)]) -
                poly([(CX - 6, 74), (CX + 6, 74), (CX, 88)]), h=3, r=3)
    lp = P.layer("suit", ink="always", cast=(1, 1))
    lp.shape(poly([(CX - 10, 74), (CX - 6, 96), (CX - 15, 96), (CX - 16, 80)]), h=3, r=3)
    lp.shape(poly([(CX + 10, 74), (CX + 6, 96), (CX + 15, 96), (CX + 16, 80)]), h=3, r=3)
    head = skull(P, cy=38, rx=18, ry=21, jaw=(20, 12, 54), ear_y=43, ear_dx=19, sockets=(8, 2.5))
    head.ell(CX - 13, 57, 8, 8, h=5)       # bajoues
    head.ell(CX + 13, 57, 8, 8, h=5)
    head.ell(CX - 4, 22, 9, 7, h=3, mode="add")
    hm = (ellipse(CX - 18, 36, 5, 8) | ellipse(CX + 18, 36, 5, 8)) - ellipse(CX, 42, 15, 17)
    hr = hair(P, hm, h=3, r=2)
    mu = P.layer("hair", cast=(1, 2), gain=1.3)
    mu.ell(CX - 5, 54, 7.5, 3.5, h=4)
    mu.ell(CX + 5, 54, 7.5, 3.5, h=4)
    mu.ell(CX - 10, 56, 3.5, 2.5, h=2)
    mu.ell(CX + 10, 56, 3.5, 2.5, h=2)
    nose = nose_layer(P)
    nose.ell(CX, 49, 4.5, 3.5, h=4)

    @P.draw
    def tex(cv, e):
        comb(cv, mu, [[(CX - 9 + k * 3, 53), (CX - 10 + k * 3, 57)] for k in range(7)], ("hair", 0))
        cv.put(CX - 7, 18, ("skin", 3))
        cv.put(CX - 6, 18, ("skin", 3))
        stripes(cv, gilet, period=2)
        for y in (80, 85, 90):
            cv.grid(CX - 1, y, ["kk", "kk"], {"k": ("tie", 0)})

    face(P, eye_y=40, eye_dx=8, eye_w=7, iris_r=1.5, bags=2, brow_y=33, brow_w=11, brow_thick=3,
         brow_key=("hair", 1), brow_arch=1,
         eyes={"neutre": {"open_": 0.85, "tilt": -1.0}, "content": {"open_": 0.6, "raise_": 1.2, "tilt": -0.8}},
         brows={"neutre": (-3, 3), "content": (-2, 2), "furieux": (3, -1), "gene": (-5, 3)},
         mouth_y=60,
         mouths={"neutre": {"kind": "line", "w": 9, "curve": -1, "under": False},
                 "content": {"kind": "smile", "w": 11, "open_": 2.5, "curve": 1.5, "under": False},
                 "furieux": {"kind": "grit", "w": 11, "curve": -1},
                 "gene": {"kind": "wavy", "w": 9, "under": False}},
         extras={"furieux": lambda cv: [T.wrinkle(cv, [(CX - 10, 24 + k * 3), (CX + 10, 24 + k * 3)])
                                        for k in range(3)],
                 "gene": both(sweat_at(CX + 14, 18), sweat_at(CX - 19, 26))})
    return P


# --- Philippe de Montclar, PM de cohabitation de droite ------------------------------------

def cohab_droite():
    """Philippe de Montclar : visage long au menton conquérant levé, grand nez aquilin,
    monocle, moustache fine cirée, cheveux bruns en pointe de veuve, nœud papillon à pois."""
    P = P_({
        "bg": [(6, 6, 5)],
        "skin": SKIN_PALE,
        "hair": [(1, 0, 1), (2, 1, 1), (5, 3, 3)],
        "suit": [(1, 0, 1), (1, 2, 2), (2, 3, 3)],
        "white": WHITE,
        "tie": [(4, 0, 2), (6, 1, 3)],
        "gold": [(5, 4, 1), (7, 6, 3)],
        "iris": [(1, 2, 2)],
    })
    neck(P, w=12, top=60, bottom=78)
    bust(P, top=76, wide=28, tie="tie", bow=True, vw=6, vdepth=12)
    head = skull(P, cy=34, rx=15, ry=21, jaw=(11, 15, 52), chin=(7, 5, 65), ear_y=40, ear_dx=15,
                 sockets=(7, 2.5))
    hm = ellipse(CX, 22, 17, 13) - poly([(CX - 14, 28), (CX - 9, 17), (CX, 22), (CX + 9, 17), (CX + 14, 28),
                                         (CX + 14, 40), (CX - 14, 40)])
    hm -= rect(0, 33, 79, 95)
    hm |= ellipse(CX - 15, 30, 2.5, 6) | ellipse(CX + 15, 30, 2.5, 6)
    hr = hair(P, hm, h=4, r=3)
    nose = nose_layer(P)
    nose.shape(poly([(CX - 1.5, 37), (CX + 2, 37), (CX + 6, 46), (CX + 5, 51), (CX + 1, 53), (CX - 3, 51),
                     (CX - 2, 44)]), h=5, r=3)
    nose.ell(CX + 3, 41, 2, 3, h=2, mode="add")
    mu = P.layer("hair", ink=True, smooth=0)
    mu.shape(poly([(CX - 1, 54), (CX - 6, 55), (CX - 11, 53), (CX - 13, 50), (CX - 11, 54), (CX - 6, 57),
                   (CX, 56)]), h=1, r=1)
    mu.shape(poly([(CX + 1, 54), (CX + 6, 55), (CX + 11, 53), (CX + 13, 50), (CX + 11, 54), (CX + 6, 57),
                   (CX, 56)]), h=1, r=1)

    @P.draw
    def tex(cv, e):
        comb(cv, hr, [[(CX - 13, 22), (CX - 4, 13), (CX + 8, 12), (CX + 14, 20)]], ("hair", 2))
        nostrils(cv, 52, 3)
        for s in (-1, 1):
            T.fold(cv, (CX + s * 6, 49), (CX + s * 9, 54), (CX + s * 9, 60))
        for l in P.layers:   # nœud papillon à pois
            if l.mat == "tie":
                dots(cv, l, 3)

    def monocle(cv, e):
        from raster import border as bd
        if e == "furieux":   # le monocle tombe
            m = ellipse(CX + 9, 60, 4, 4)
            cv.fill(bd(m), ("gold", 0))
            cv.fill(line(CX + 9, 56, CX + 13, 44), ("gold", 1))
            return
        m = ellipse(CX + 7, 39, 5.5, 5.5)
        cv.fill(bd(m), ("gold", 0))
        cv.put(CX + 4, 36, ("white", 1))
        cv.fill(line(CX + 12, 41, CX + 16, 60), ("gold", 1))

    face(P, eye_y=39, eye_dx=7, eye_w=7, iris_r=1.5, lid=2, brow_y=32, brow_w=9, brow_thick=2,
         brow_arch=2.5, brow_key=("hair", 0), glasses=monocle,
         eyes={"neutre": {"open_": 0.5, "droop": 1.0, "right": {"open_": 0.9}},
               "content": {"open_": 0.45, "droop": 1.0, "right": {"open_": 0.8}},
               "furieux": {"open_": 1.0, "tilt": 1.5}},
         brows={"neutre": ((0, 0), (-3, -3)), "content": ((-1, -1), (-3, -4))},
         mouth_y=59,
         mouths={"neutre": {"kind": "line", "w": 8, "curve": 0.3},
                 "content": {"kind": "smirk", "w": 9, "curve": 2},
                 "furieux": {"kind": "shout", "w": 9, "open_": 3},
                 "gene": {"kind": "wavy", "w": 8}},
         extras={"gene": sweat_at(CX - 18, 26),
                 "furieux": lambda cv: (cv.put(CX - 4, 51, INK), cv.put(CX + 4, 51, INK))})
    return P


ALL = [("pres_centre", pres_centre), ("pres_droite", pres_droite), ("pm_technocrate", pm_technocrate),
       ("pm_rassembleur", pm_rassembleur), ("pm_reformateur", pm_reformateur),
       ("cohab_gauche", cohab_gauche), ("cohab_centre", cohab_centre), ("cohab_droite", cohab_droite)]
