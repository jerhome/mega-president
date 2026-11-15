"""Outils communs aux gueules V3 (buste, cou, oreilles, textures) et registre.

Tous les personnages sont fictifs. Repères (80 x 96) : axe x = 40, tête vers
y 10-66, cou 54-76, épaules à partir de y 72.
Les personnages sont définis dans g_*.py ; ALL donne l'ordre (= enum PortraitId).
"""
import math

from gueules import Portrait, W, H
from raster import ellipse, poly, rect, line, polyline, border
import traits as T

INK = ("ink", 0)
CX = 40


def P_(ramps):
    r = {"ink": [(1, 0, 1)]}
    r.update(ramps)
    return Portrait(r)


# --- buste -------------------------------------------------------------------------

def bust(P, top=72, wide=34, suit="suit", shirt="white", tie=None, collar=True,
         lapels=True, vdepth=14, vw=7, sweater=False, bow=False, open_collar=False):
    """À appeler APRÈS neck() : la veste et le col recouvrent le bas du cou."""
    """Épaules, veste, chemise, cravate. Retourne la couche de la veste."""
    s = P.layer(suit)
    pts = [(CX - wide - 6, H + 2), (CX - wide, top + 10), (CX - wide + 6, top + 4),
           (CX - 12, top - 1), (CX + 12, top - 1), (CX + wide - 6, top + 4),
           (CX + wide, top + 10), (CX + wide + 6, H + 2)]
    s.shape(poly(pts), h=10, r=9)
    if shirt:
        v = poly([(CX - vw - 1, top - 3), (CX + vw + 1, top - 3), (CX, top + vdepth)])
        if open_collar:   # col ouvert : la peau du cou continue dans l'échancrure
            sh = P.layer("skin", ink=True)
            sh.shape(v, h=3, r=3)
            sh.cut(ellipse(CX, top - 4, vw + 2, 3))
        else:
            sh = P.layer(shirt, ink=True)
            sh.shape(v, h=3, r=3)
        if collar and not sweater:
            c = P.layer(shirt, ink="always", cast=(1, 2))
            c.shape(poly([(CX - vw - 2, top - 5), (CX - 1, top + 2), (CX - 4, top + 7),
                           (CX - vw - 3, top + 1)]), h=2, r=2)
            c.shape(poly([(CX + vw + 2, top - 5), (CX + 1, top + 2), (CX + 4, top + 7),
                           (CX + vw + 3, top + 1)]), h=2, r=2)
    if tie and not bow:
        t = P.layer(tie, ink=True, cast=(1, 1))
        t.shape(poly([(CX - 2.5, top + 1), (CX + 2.5, top + 1), (CX + 4, top + 16),
                      (CX, top + 22), (CX - 4, top + 16)]), h=2, r=2)
        t.shape(poly([(CX - 3, top - 3), (CX + 3, top - 3), (CX + 2, top + 2), (CX - 2, top + 2)]), h=3, r=2)
    if lapels and not sweater:
        lp = P.layer(suit, ink="always", cast=(1, 1))
        lp.shape(poly([(CX - vw - 3, top - 3), (CX - 1, top + vdepth + 1), (CX - 6, top + vdepth + 6),
                       (CX - 13, top + 4)]), h=3, r=3)
        lp.shape(poly([(CX + vw + 3, top - 3), (CX + 1, top + vdepth + 1), (CX + 6, top + vdepth + 6),
                       (CX + 13, top + 4)]), h=3, r=3)
    if tie and bow:
        t = P.layer(tie, ink=True, cast=(1, 1))
        t.shape(poly([(CX - 7, top - 2), (CX, top + 1), (CX - 7, top + 4)]), h=2, r=2)
        t.shape(poly([(CX + 7, top - 2), (CX, top + 1), (CX + 7, top + 4)]), h=2, r=2)
        t.ell(CX, top + 1, 2, 2, h=2)
    return s


def neck(P, w=16, top=54, bottom=76, skin="skin"):
    n = P.layer(skin)
    n.shape(rect(CX - w // 2, top, CX + w // 2 - 1, bottom), h=6, r=7)
    return n


def ears(head, y=40, dx=21, rx=3.5, ry=6):
    head.ell(CX - dx, y, rx, ry, h=3)
    head.ell(CX + dx, y, rx, ry, h=3)


def ear_detail(cv, y=40, dx=21):
    for s in (-1, 1):
        x = CX + s * dx
        T.wrinkle(cv, [(x, y - 3), (x + s, y - 1), (x + s, y + 2), (x, y + 3)])


# --- textures ---------------------------------------------------------------------

def curl_marks(cv, layer, centers, key=("hair", 0), hi=None):
    """Boucles : arc sombre en bas à droite de chaque boucle, reflet en haut à gauche."""
    for (x, y, r) in centers:
        for a in range(-20, 140, 12):
            px = int(math.floor(x + r * 0.8 * math.cos(math.radians(a))))
            py = int(math.floor(y + r * 0.75 * math.sin(math.radians(a))))
            if cv.owner_is(px, py, layer) and cv.get(px, py)[0] != "ink":
                cv.put(px, py, key)
        if hi:
            hx, hy = int(x - r * 0.35), int(y - r * 0.4)
            for dx in (0, 1):
                if cv.owner_is(hx + dx, hy, layer):
                    cv.put(hx + dx, hy, hi)


def stripes(cv, layer, period=3, steps=1, x0=0):
    """Velours côtelé / rayures : colonnes assombries."""
    for y in range(H):
        for x in range(W):
            if (x - x0) % period == 0 and cv.owner_is(x, y, layer):
                k = cv.get(x, y)
                if k[0] != "ink":
                    cv.put(x, y, (k[0], max(0, k[1] - steps)))


def comb(cv, layer, lines, key):
    """Mèches : traits (polylignes) dessinés seulement sur la couche."""
    for pts in lines:
        m = polyline([(int(round(a)), int(round(b))) for a, b in pts])
        for (x, y) in m:
            if cv.owner_is(x, y, layer) and cv.get(x, y)[0] != "ink":
                cv.put(x, y, key)


def round_glasses(cv, y, dx=8, rx=6, ry=5.2, cx=CX, key=INK):
    for s in (-1, 1):
        m = ellipse(cx + s * dx, y, rx, ry)
        cv.fill(border(m), key)
        hx, hy = int(cx + s * dx - rx * 0.5), int(y - ry * 0.45)
        cv.put(hx, hy, ("white", 1))
        cv.put(hx + 1, hy - 1, ("white", 1))
    cv.fill(line(int(cx - dx + rx) - 1, int(y) - 1, int(cx + dx - rx), int(y) - 1), key)
    cv.fill(line(int(cx - dx - rx), int(y) - 1, int(cx - dx - rx) - 4, int(y)), key)
    cv.fill(line(int(cx + dx + rx) - 1, int(y) - 1, int(cx + dx + rx) + 3, int(y)), key)


def rect_glasses(cv, y, dx=8, w=11, h=6, cx=CX, key=INK, thick=1):
    for s in (-1, 1):
        x0 = int(round(cx + s * dx - w / 2))
        m = rect(x0, y, x0 + w - 1, y + h - 1)
        cv.fill(border(m), key)
        if thick > 1:
            cv.fill(line(x0, y + 1, x0 + w - 1, y + 1), key)
        cv.put(x0 + 1, y + 1 + (thick > 1), ("white", 1))
        cv.put(x0 + 2, y + 1 + (thick > 1), ("white", 1))
    xl = int(round(cx - dx - w / 2))
    xr = int(round(cx + dx + w / 2)) - 1
    cv.fill(line(xl + w, y + 1, int(round(cx + dx - w / 2)) - 1, y + 1), key)
    cv.fill(line(xl - 1, y + 1, xl - 3, y + 2), key)
    cv.fill(line(xr + 1, y + 1, xr + 3, y + 2), key)


EXP_EYE = {   # (ouverture, inclinaison, paupière inf. remontée)
    "neutre": (0.9, 0.0, 0.0),
    "content": (0.55, 0.0, 1.6),
    "furieux": (0.6, 2.2, 0.0),
    "gene": (0.95, -1.2, 0.0),
}
EXP_BROW = {  # (intérieur, extérieur)
    "neutre": (0, 0),
    "content": (-1, -2),
    "furieux": (4, -1),
    "gene": (-3, 1),
}


def registry():
    import g_demo
    return list(g_demo.ALL)


def __getattr__(name):
    if name == "ALL":
        return registry()
    raise AttributeError(name)
