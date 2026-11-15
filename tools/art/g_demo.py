"""Gueules de démonstration V3 : le président de gauche, un ministre, un chef d'opposition.

Tous les personnages sont fictifs.
"""
import math

from raster import ellipse, poly, rect, line
import traits as T
from persos import (P_, CX, INK, bust, neck, ears, curl_marks, stripes, comb,
                    round_glasses, EXP_EYE, EXP_BROW)


# --- Le président de gauche ---------------------------------------------------------------

def pres_gauche():
    """Antoine Verdier, président (gauche, RSV) : tignasse frisée en nuage, barbe de prof de fac,
    lunettes rondes sur un nez en pomme de terre, veste de velours côtelé."""
    P = P_({
        "bg": [(5, 6, 5)],
        "skin": [(3, 1, 1), (5, 3, 2), (6, 4, 3), (7, 6, 5)],
        "hair": [(1, 1, 1), (2, 1, 1), (4, 3, 2)],
        "suit": [(3, 1, 1), (4, 2, 1), (5, 3, 2)],
        "white": [(5, 5, 5), (7, 7, 7)],
        "acc": [(6, 1, 1)],
        "iris": [(2, 1, 1)],
        "pin": [(2, 5, 2)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.4, smooth=1)
    clumps = []
    for k in range(19):
        a = math.radians(165 + 210 * k / 18)
        x = CX + 27 * math.cos(a)
        y = 31 + 23 * math.sin(a)
        if y < 50:
            r = 7.5 if k % 2 else 6.5
            clumps.append((x, y, r))
    for (x, y, r) in clumps:
        hb.ell(x, y, r, r * 0.92, h=6)
    hb.ell(CX, 26, 24, 18, h=12)
    neck(P, w=18, top=54, bottom=76)
    suit = bust(P, top=74, wide=33, tie=None, vw=6, vdepth=11, open_collar=True)
    head = P.layer("skin", cast=(1, 3))
    head.ell(CX, 38, 18, 21, h=16)
    head.ell(CX, 50, 17, 13, h=14)
    ears(head, y=41, dx=18)
    head.ell(CX - 10, 47, 6, 5, h=2.5, mode="add")
    head.ell(CX + 10, 47, 6, 5, h=2.5, mode="add")
    head.ell(CX - 8, 36, 6, 3, h=2.5, mode="sub")
    head.ell(CX + 8, 36, 6, 3, h=2.5, mode="sub")
    beard = P.layer("hair", gain=1.3, smooth=1)
    bm = (ellipse(CX, 49, 18.5, 17) - ellipse(CX, 41, 16, 15.5)) | ellipse(CX, 61, 9, 5.5)
    bm &= head.mask() | ellipse(CX, 62, 10, 6)
    beard.shape(bm, h=3, r=2)
    bclumps = [(CX - 15, 50, 3.5), (CX + 15, 50, 3.5), (CX - 12, 57, 4), (CX + 12, 57, 4),
               (CX - 5, 62, 4.5), (CX + 5, 62, 4.5)]
    for (x, y, r) in bclumps:
        beard.ell(x, y, r, r * 0.8, h=2, mode="add")
    beard.cut(ellipse(CX, 57, 7.5, 3))
    mu = P.layer("hair", ink=True, smooth=1)
    mu.ell(CX - 4.5, 53, 6.5, 2.8, h=3)
    mu.ell(CX + 4.5, 53, 6.5, 2.8, h=3)
    hf = P.layer("hair", cast=(1, 2), gain=1.4, smooth=1)
    fclumps = [(CX - 14, 21, 6), (CX - 5, 18, 6.5), (CX + 4, 18, 6.5), (CX + 13, 20, 6),
               (CX + 19, 26, 4.5), (CX - 20, 27, 4.5), (CX - 9, 24, 4), (CX + 9, 24, 4)]
    for (x, y, r) in fclumps:
        hf.ell(x, y, r, r * 0.9, h=5)
    nose = P.layer("skin", ink="shadow", cast=(1, 1), ink_dirs="rdl")
    nose.ell(CX + 1, 46, 6, 5, h=6)
    head.ell(CX + 0.5, 40, 2.5, 5, h=3, mode="add")

    @P.draw
    def textures(cv, e):
        curl_marks(cv, hb, clumps, hi=("hair", 2))
        curl_marks(cv, hf, fclumps, hi=("hair", 2))
        curl_marks(cv, beard, bclumps)
        stripes(cv, suit, period=3)
        cv.put(CX - 3, 49, INK)
        cv.put(CX + 4, 49, INK)
        cv.grid(CX + 16, 80, [".kk.", "kppk", "kppk", ".kk."], {"k": INK, "p": ("pin", 0)})

    @P.draw
    def face(cv, e):
        o, tilt, rs = EXP_EYE[e]
        idy, ody = EXP_BROW[e]
        for s in (-1, 1):
            T.eye(cv, CX + s * 8, 37, w=8, h=4, side=s, open_=o, raise_=rs, tilt=tilt,
                  look=2 if e == "gene" else 0, iris_r=1.8, bags=1)
            T.brow(cv, CX + s * 9, 29, w=12, side=s, inner_dy=idy, outer_dy=ody, arch=2.5, thick=3,
                   key=("hair", 0))
        round_glasses(cv, 37.5)
        if e == "neutre":
            T.mouth(cv, CX, 57, w=9, kind="line", curve=1, under=False)
        elif e == "content":
            T.mouth(cv, CX, 56, w=13, kind="smile", open_=3.5, tongue=("acc", 0), under=False)
            for s in (-1, 1):
                T.wrinkle(cv, [(CX + s * 15, 39), (CX + s * 17, 41)])
        elif e == "furieux":
            T.mouth(cv, CX, 56, w=11, kind="shout", open_=3.5, tongue=("acc", 0), under=False)
            T.wrinkle(cv, [(CX - 1, 30), (CX - 1, 33)])
            T.wrinkle(cv, [(CX + 2, 30), (CX + 2, 33)])
        else:
            T.mouth(cv, CX + 1, 57, w=11, kind="wavy", under=False)
            T.sweat(cv, CX + 17, 30)
            for s in (-1, 1):
                T.blush(cv, CX + s * 11 - 2, 45, w=4)
    return P


# --- Ministre de l'Intérieur ----------------------------------------------------------------

def min_interieur():
    """Régis Fermont, ministre de l'Intérieur : tête de bouledogue en parpaing, brosse
    rase, mâchoire plus large que le crâne, petits yeux soupçonneux, cou de taureau."""
    P = P_({
        "bg": [(3, 4, 5)],
        "skin": [(3, 1, 1), (5, 2, 2), (6, 4, 3), (7, 5, 4)],
        "hair": [(2, 2, 2), (3, 3, 3), (5, 5, 5)],
        "suit": [(1, 0, 1), (1, 1, 2), (2, 2, 4)],
        "white": [(5, 5, 5), (7, 7, 7)],
        "tie": [(1, 1, 2), (3, 3, 6)],
        "iris": [(2, 2, 4)],
        "acc": [(6, 1, 1)],
    })
    neck(P, w=32, top=50, bottom=76)
    bust(P, top=73, wide=38, tie="tie", vw=6, vdepth=12)
    head = P.layer("skin", cast=(1, 3), gain=1.1)
    head.shape(poly([(CX - 15, 15), (CX + 15, 15), (CX + 19, 34), (CX + 24, 52), (CX + 20, 64),
                     (CX - 20, 64), (CX - 24, 52), (CX - 19, 34)]), h=14, r=12)
    head.ell(CX - 13, 56, 11, 9, h=8)           # bajoues
    head.ell(CX + 13, 56, 11, 9, h=8)
    head.ell(CX, 63, 12, 5, h=7)                # menton en galoche
    ears(head, y=37, dx=19, rx=3.5, ry=5)
    head.ell(CX - 8, 34, 6, 3, h=3, mode="sub")
    head.ell(CX + 8, 34, 6, 3, h=3, mode="sub")
    head.ell(CX, 29, 15, 3.5, h=3, mode="add")  # arcade
    hr = P.layer("hair", cast=(1, 2), smooth=1)
    hr.shape(poly([(CX - 16, 10), (CX + 16, 10), (CX + 17, 13), (CX + 19, 24), (CX + 16, 20),
                   (CX - 16, 20), (CX - 19, 24), (CX - 17, 13)]), h=4, r=3)
    nose = P.layer("skin", ink="shadow", cast=(1, 1))
    nose.ell(CX, 45, 6.5, 3.5, h=4)            # nez écrasé de boxeur
    nose.ell(CX - 1, 40, 3, 4, h=3)

    @P.draw
    def textures(cv, e):
        for x in range(CX - 16, CX + 17, 2):   # brosse : poils en dents de scie
            cv.put(x, 10, ("bg", 0))
            if cv.owner_is(x, 11, hr):
                cv.put(x, 11, INK)
        comb(cv, hr, [[(CX - 14 + k * 4, 13), (CX - 14 + k * 4, 15)] for k in range(8)], ("hair", 2))
        cv.put(CX - 3, 47, INK)
        cv.put(CX + 2, 47, INK)
        T.wrinkle(cv, [(CX + 5, 61), (CX + 7, 64)], T.sk(0))     # cicatrice
        cv.grid(CX + 18, 80, ["kkk", "bwr", "kkk"], {"k": INK, "b": ("tie", 1), "w": ("white", 1), "r": ("acc", 0)})

    @P.draw
    def face(cv, e):
        o = {"neutre": 0.55, "content": 0.4, "furieux": 0.5, "gene": 0.85}[e]
        tilt = {"furieux": 2.5, "neutre": 1.2, "gene": -1.5}.get(e, 0)
        for s in (-1, 1):
            T.eye(cv, CX + s * 8, 35, w=7, h=4, side=s, open_=o, tilt=tilt,
                  look={"gene": -2, "neutre": 1}.get(e, 0),
                  iris_r=1.5, raise_={"content": 1}.get(e, 0), bags=2, lidfold=False)
            idy = {"furieux": 4, "gene": -3, "neutre": 2, "content": 0}[e]
            T.brow(cv, CX + s * 9, 29, w=13, side=s, inner_dy=idy, outer_dy=-1, arch=0.5, thick=3,
                   key=("hair", 0), taper=False)
        for s in (-1, 1):
            T.fold(cv, (CX + s * 7, 44), (CX + s * 12, 50), (CX + s * 11, 57))
        if e == "neutre":
            T.mouth(cv, CX, 55, w=13, kind="line", curve=-1.2)
        elif e == "content":
            T.mouth(cv, CX + 1, 55, w=13, kind="smirk", curve=2)
        elif e == "furieux":
            T.mouth(cv, CX, 54, w=15, kind="grit", curve=-1)
            T.vein(cv, CX + 8, 18, ("acc", 0))
        else:
            T.mouth(cv, CX, 56, w=9, kind="wavy")
            T.sweat(cv, CX - 23, 22)
            for s in (-1, 1):
                T.blush(cv, CX + s * 13 - 2, 46)
    return P


# --- Chef de l'opposition de droite ---------------------------------------------------------

def chef_utl():
    """Aymeric Dubreuil-Castagnac, président du groupe UTL : visage en lame de couteau,
    banane argentée laquée vers l'arrière, long nez tombant, menton à fossette en
    avant, paupières lourdes et sourcil hautain, pochette."""
    P = P_({
        "bg": [(5, 4, 3)],
        "skin": [(3, 1, 1), (5, 3, 2), (7, 5, 4), (7, 6, 6)],
        "hair": [(3, 3, 4), (5, 5, 6), (7, 7, 7)],
        "suit": [(1, 1, 2), (2, 2, 3), (3, 3, 4)],
        "white": [(5, 5, 6), (7, 7, 7)],
        "tie": [(4, 0, 1), (6, 1, 2)],
        "iris": [(1, 3, 4)],
    })
    hb = P.layer("hair", cast=(1, 2), gain=1.2)
    hb.ell(CX + 8, 28, 15, 16, h=8)            # volume derrière
    neck(P, w=12, top=60, bottom=78)
    bust(P, top=76, wide=28, tie="tie", vw=5, vdepth=11)
    head = P.layer("skin", cast=(1, 3))
    head.ell(CX, 36, 14, 20, h=12)
    head.ell(CX, 54, 10, 15, h=10)
    head.ell(CX + 1, 67, 6.5, 5, h=6)          # menton en avant
    ears(head, y=42, dx=14.5, rx=3.5, ry=6.5)
    head.ell(CX - 6, 40, 5, 3, h=2.5, mode="sub")
    head.ell(CX + 6, 40, 5, 3, h=2.5, mode="sub")
    head.ell(CX, 33, 11, 2.5, h=1.5, mode="add")
    # banane laquée : crête haute qui part vers l'arrière
    hf = P.layer("hair", cast=(1, 2), gain=1.3, smooth=1)
    crest = poly([(CX - 14, 28), (CX - 15, 18), (CX - 10, 8), (CX - 2, 3), (CX + 10, 3), (CX + 19, 8),
                  (CX + 22, 16), (CX + 21, 30), (CX + 16, 36), (CX + 14, 24), (CX + 6, 21), (CX - 4, 21),
                  (CX - 11, 24)])
    hf.shape(crest, h=6, r=5)
    hf.ell(CX - 3, 11, 11, 7, h=4, mode="add")
    hf.ell(CX + 10, 10, 9, 6, h=3, mode="add")
    nose = P.layer("skin", ink="shadow", cast=(1, 1))
    nose.shape(poly([(CX - 1.5, 39), (CX + 1.5, 39), (CX + 4, 50), (CX + 5, 56), (CX + 2, 59), (CX - 1, 59),
                     (CX - 3, 56), (CX - 2, 49)]), h=4, r=3)
    nose.ell(CX + 1, 56, 4, 3, h=3, mode="add")

    @P.draw
    def textures(cv, e):
        comb(cv, hf, [[(CX - 12, 22), (CX - 9, 12), (CX, 6), (CX + 12, 6), (CX + 19, 12)],
                      [(CX - 7, 21), (CX - 3, 13), (CX + 6, 9), (CX + 16, 13), (CX + 19, 24)],
                      [(CX + 5, 20), (CX + 12, 15), (CX + 17, 27)]], ("hair", 0))
        comb(cv, hf, [[(CX - 10, 14), (CX - 5, 8), (CX + 2, 6)]], ("hair", 2))
        cv.put(CX - 2, 58, INK)
        cv.put(CX + 3, 58, INK)
        T.wrinkle(cv, [(CX + 1, 68), (CX + 1, 70)])        # fossette du menton
        cv.grid(CX + 16, 84, ["..k.k.", ".kwkwk", "kwwwwk", "kkkkkk"], {"k": INK, "w": ("white", 1)})

    @P.draw
    def face(cv, e):
        o = {"neutre": 0.5, "content": 0.4, "furieux": 0.8, "gene": 0.95}[e]
        droop = {"neutre": 1.2, "content": 1.2}.get(e, 0)
        tilt = {"furieux": 2.0, "gene": -1.5}.get(e, 0)
        for s in (-1, 1):
            T.eye(cv, CX + s * 6.5, 41, w=8, h=4, side=s, open_=o, droop=droop, tilt=tilt,
                  look={"gene": 2}.get(e, 0), iris_r=1.6, lid=2)
            idy = {"furieux": 3, "gene": -3, "neutre": -1, "content": -2}[e]
            ody = {"neutre": -1, "content": -2}.get(e, 0)
            if s == 1 and e in ("neutre", "content"):   # sourcil hautain
                idy, ody = idy - 3, ody - 4
            T.brow(cv, CX + s * 7.5, 34, w=10, side=s, inner_dy=idy, outer_dy=ody, arch=2, thick=2,
                   key=("hair", 0))
        for s in (-1, 1):
            T.fold(cv, (CX + s * 5, 55), (CX + s * 8, 59), (CX + s * 8, 64))
        T.wrinkle(cv, [(CX - 7, 27), (CX + 7, 27)])
        T.wrinkle(cv, [(CX - 5, 30), (CX + 5, 30)])
        if e == "neutre":
            T.mouth(cv, CX, 62, w=9, kind="smirk", curve=1)
        elif e == "content":
            T.mouth(cv, CX, 62, w=11, kind="grin", open_=2.5, curve=2)
        elif e == "furieux":
            T.mouth(cv, CX, 61, w=10, kind="shout", open_=3, tongue=("tie", 1))
        else:
            T.mouth(cv, CX, 62, w=8, kind="pout")
            T.sweat(cv, CX - 21, 32)
    return P


ALL = [("pres_gauche", pres_gauche), ("min_interieur", min_interieur), ("chef_utl", chef_utl)]
