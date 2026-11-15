"""Scènes V3 (vignettes, décors) : le moteur des gueules appliqué à une image entière.

Une scène = un Portrait de taille quelconque (couches sculptées et éclairées, contour
d'encre) + un fond peint directement (dégradés tramés) + des traits dessinés.
Les couleurs sont des clés (rampe, ton) ; la palette (15 couleurs + transparent)
est calculée à partir des clés réellement utilisées.
"""
import math

from gueules import Portrait, bez
from raster import ellipse, rect, poly, line, polyline, border, BAYER4

INK = ("ink", 0)


class Img:
    def __init__(self, w, h, px):
        self.w, self.h, self.px = w, h, px


class Scene:
    def __init__(self, w, h, ramps, bg="bg"):
        r = {"ink": [(1, 0, 1)]}
        r.update(ramps)
        self.P = Portrait(r, w=w, h=h, bg=bg)
        self.w, self.h = w, h
        self.bgops = []

    def layer(self, mat, **kw):
        return self.P.layer(mat, **kw)

    def draw(self, fn):
        self.P.draw(lambda cv, e: fn(cv))
        return fn

    def background(self, fn):
        self.bgops.append(fn)
        return fn

    def render(self, frame=True):
        def bgd(key):
            g = Grid(key, self.w, self.h)
            for f in self.bgops:
                f(g)
        self.P.render_base(bg_draw=bgd)
        key = self.P.render("neutre")
        if frame:
            for x in range(self.w):
                key[0][x] = INK
                key[self.h - 1][x] = INK
            for y in range(self.h):
                key[y][0] = INK
                key[y][self.w - 1] = INK
        pal, index = self.P.palette([key])
        return Img(self.w, self.h, [[index[k] for k in row] for row in key]), pal


class Grid:
    """Peinture du fond directement dans la carte de clés."""

    def __init__(self, key, w, h):
        self.key, self.w, self.h = key, w, h

    def put(self, x, y, k):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.key[y][x] = k

    def fill(self, m, k):
        for (x, y) in m:
            self.put(x, y, k)

    def rect(self, x0, y0, x1, y1, k):
        self.fill(rect(x0, y0, x1, y1), k)

    def dither(self, m, k1, k2, level):
        for (x, y) in m:
            self.put(x, y, k2 if BAYER4[y & 3][x & 3] < level else k1)

    def gradient(self, y0, y1, k1, k2, x0=0, x1=None):
        """Dégradé vertical tramé de k1 (haut) à k2 (bas)."""
        x1 = self.w - 1 if x1 is None else x1
        for y in range(y0, y1 + 1):
            lvl = (y - y0) * 17 // max(1, (y1 - y0 + 1))
            self.dither(rect(x0, y, x1, y), k1, k2, lvl)


# --- petits personnages (grosse tête) -------------------------------------------------

def chibi(S, cx, cy, r=6.0, skin="skin", hair="hair", style="short", body=None, body_w=None,
          body_h=20, shoulders=None, cast=(1, 1), hair_mat=None):
    """Tête (rayon r) centrée en (cx, cy) ; buste en dessous si body (matière).
    style : short, bald, long, curly, bun, spiky, cap (casquette = hair_mat), none."""
    layers = {}
    if body:
        bw = body_w or r * 1.4
        top = cy + r * 0.8
        b = S.layer(body)
        b.shape(poly([(cx - bw - 1, top + body_h), (cx - bw, top + 4), (cx - bw + 3, top + 1), (cx - 3, top),
                      (cx + 3, top), (cx + bw - 3, top + 1), (cx + bw, top + 4), (cx + bw + 1, top + body_h)]),
                h=4, r=4)
        layers["body"] = b
    if style == "long":
        hb = S.layer(hair, cast=cast, smooth=1)
        hb.shape(ellipse(cx, cy + r * 0.2, r * 1.2, r * 1.25) - rect(0, int(cy + r * 1.1), 999, 999), h=3, r=3)
        layers["hair_back"] = hb
    if style == "curly":
        hb = S.layer(hair, cast=cast, smooth=1, gain=1.3)
        for k in range(9):
            a = math.radians(170 + 200 * k / 8)
            hb.ell(cx + r * 1.15 * math.cos(a), cy - r * 0.1 + r * 1.0 * math.sin(a), r * 0.5, r * 0.45,
                   h=r * 0.4)
        layers["hair_back"] = hb
    head = S.layer(skin, cast=cast)
    head.ell(cx, cy, r, r * 1.08, h=r * 0.5)
    if r >= 6 and style not in ("long",):
        head.ell(cx - r, cy + r * 0.15, r * 0.25, r * 0.35, h=1)   # oreilles
        head.ell(cx + r, cy + r * 0.15, r * 0.25, r * 0.35, h=1)
    head.bias = 0.08
    layers["head"] = head
    if r >= 6:   # nez (bosse, contour côté ombre)
        nose = S.layer(skin, ink="shadow", ink_dirs="rd")
        nose.ell(cx + 0.5, cy + r * 0.33, r * 0.22 + 0.4, r * 0.2 + 0.3, h=1.5)
        layers["nose"] = nose
    if style in ("short", "long", "bun", "curly"):
        hm = ellipse(cx, cy - r * 0.35, r * 1.05, r * 0.85) - rect(0, int(cy - r * 0.2), 999, 999)
        h = S.layer(hair, cast=cast, smooth=1)
        h.shape(hm, h=2, r=2)
        h.ell(cx - r * 0.2, cy - r * 0.6, r * 0.8, r * 0.5, h=1.5, mode="add")
        layers["hair"] = h
    if style == "bun":
        layers["hair"].ell(cx, cy - r * 1.1, r * 0.45, r * 0.4, h=2, mode="new")
    if style == "spiky":
        hm = ellipse(cx, cy - r * 0.4, r, r * 0.75) - rect(0, int(cy - r * 0.2), 999, 999)
        for k in range(5):
            x = cx - r * 0.8 + k * r * 0.4
            hm |= poly([(x - 1.5, cy - r * 0.6), (x + 1.5, cy - r * 0.6), (x + (k - 2) * 0.6, cy - r * 1.35)])
        h = S.layer(hair, cast=cast, smooth=1)
        h.shape(hm, h=2, r=2)
        layers["hair"] = h
    if style == "cap":
        h = S.layer(hair_mat or hair, cast=cast, smooth=1)
        h.shape(ellipse(cx, cy - r * 0.35, r * 1.05, r * 0.8) - rect(0, int(cy - r * 0.15), 999, 999), h=2, r=2)
        h.shape(rect(int(cx - r * 1.1), int(cy - r * 0.3), int(cx + r * 0.2), int(cy - r * 0.1)), h=1, r=1)
        layers["hair"] = h
    return layers


def face(cv, cx, cy, r=6.0, mood="neutral", eye=INK, skin="skin", glasses=False, white=("white", 1),
         tongue=None):
    """Yeux et bouche d'un chibi. mood : neutral, happy, angry, shout, sad, sleep, o, grin, worried."""
    ex = max(2, int(round(r * 0.42)))
    ey = int(round(cy + r * 0.05))
    my = int(round(cy + r * 0.55))
    for s in (-1, 1):
        x = int(round(cx + s * ex)) - (1 if s < 0 else 0)
        if mood in ("happy", "sleep"):
            cv.put(x - 1, ey, eye)
            cv.put(x, ey - 1, eye)
            cv.put(x + 1, ey, eye)
        elif r >= 7:
            # blanc de l'œil, pupille vers le centre, paupière d'encre
            big = mood in ("o", "shout", "worried")
            for dx in (-1, 0, 1):
                for dy in ((-1, 0, 1) if big else (-1, 0)):
                    cv.put(x + dx, ey + dy, white)
            px = x - s * (0 if big else 0)
            cv.put(px, ey, eye)
            cv.put(px, ey - 1, eye)
            if r >= 8:
                cv.put(px - s, ey, eye)
            top = ey - 2
            if mood == "angry":   # paupière basse côté nez
                cv.fill(line(x + s * 1, top, x - s * 1, top + 1), eye)
                cv.put(x - s, ey - 1, eye)
            else:
                cv.fill(line(x - 1, top, x + 1, top), eye)
        else:
            cv.put(x, ey, eye)
            cv.put(x, ey - 1, eye)
        by = ey - max(2, int(r * 0.35)) - (1 if r >= 7 else 0)
        if mood == "angry":
            cv.put(x - s, by + 1, eye)
            cv.put(x, by + 1, eye)
            cv.put(x + s, by, eye)
            if r >= 7:
                cv.put(x + s * 2, by - 1, eye)
        elif mood in ("sad", "worried"):
            cv.put(x - s, by, eye)
            cv.put(x, by, eye)
            cv.put(x + s, by + 1, eye)
        elif r >= 7:
            cv.fill(line(x - 1, by, x + 1, by), eye)
        if glasses:
            m = ellipse(x + 0.5, ey - 0.5, r * 0.32 + 1, r * 0.3 + 1)
            cv.fill(border(m), eye)
    mx = int(round(cx))
    w = max(2, int(r * 0.45))
    if mood in ("happy", "grin"):
        m = ellipse(mx + 0.5, my - 0.5, w + 0.5, w * 0.7 + 0.5) - rect(0, 0, 999, my - 1)
        cv.fill(m, eye)
        if mood == "grin" or r >= 7:
            cv.fill(rect(mx - w + 1, my, mx + w - 1, my), white)
    elif mood in ("shout", "o"):
        m = ellipse(mx + 0.5, my + 0.5, w * (0.9 if mood == "shout" else 0.5) + 0.3, w * 0.8 + 0.3)
        cv.fill(m, eye)
        if tongue:
            cv.put(mx, my + 1, tongue)
            cv.put(mx + 1, my + 1, tongue)
    elif mood in ("sad", "angry", "worried"):
        cv.fill(line(mx - w, my + 1, mx - w + 1, my) | line(mx - w + 1, my, mx + w - 1, my) |
                line(mx + w - 1, my, mx + w, my + 1), eye)
    elif mood == "sleep":
        cv.fill(line(mx - 1, my, mx + 1, my), eye)
    else:
        cv.fill(line(mx - w + 1, my, mx + w - 1, my), eye)


MOODS = {   # ouverture, inclinaison, paupière inf., (sourcil int., ext.), bouche
    "neutral": (0.9, 0.0, 0.0, (0, 0), {"kind": "line", "curve": 0.3}),
    "happy": (0.55, 0.0, 1.0, (-1, -1), {"kind": "smile", "open_": 2.2, "curve": 1.5}),
    "grin": (0.7, 0.0, 0.6, (-1, -1), {"kind": "smile", "open_": 1.6, "curve": 1.5}),
    "angry": (0.6, 1.8, 0.0, (3, -1), {"kind": "grit", "curve": -0.5}),
    "shout": (0.95, 1.4, 0.0, (3, -1), {"kind": "shout", "open_": 3}),
    "worried": (1.0, -1.2, 0.0, (-2, 1), {"kind": "wavy"}),
    "sad": (0.7, -1.0, 0.0, (-2, 1), {"kind": "line", "curve": -1}),
    "o": (1.1, 0.0, 0.0, (-2, -2), {"kind": "shout", "open_": 1.8}),
    "sleep": (0.12, 0.0, 0.0, (0, 1), {"kind": "line", "curve": -0.3}),
}


def big_face(cv, cx, cy, r, mood="neutral", brow_key=INK, iris=INK, look=0, lashes=False, bags=0,
             mouth_dy=0, mouth_w=None, tongue=None, brow_thick=2):
    """Visage détaillé (traits des gueules) pour une tête de rayon r >= 9."""
    import traits as T
    o, tilt, rs, (bi, bo), mp = MOODS[mood]
    dx = r * 0.42
    ew = max(4, int(round(r * 0.5)))
    eh = max(2.0, r * 0.24)
    ey = cy - r * 0.02
    for s in (-1, 1):
        T.eye(cv, cx + s * dx, ey, w=ew, h=eh, side=s, open_=o, tilt=tilt * eh / 4, raise_=rs, look=look,
              iris=iris, iris_r=max(1.0, eh * 0.45), lashes=lashes, bags=bags, lidfold=r >= 12)
        T.brow(cv, cx + s * (dx + 0.5), ey - eh - 2.5, w=ew + 1, side=s, inner_dy=bi * eh / 4,
               outer_dy=bo * eh / 4, arch=1.2, thick=brow_thick, key=brow_key)
    m = dict(mp)
    m.setdefault("w", mouth_w or max(5, int(r * (0.9 if m["kind"] in ("smile", "shout") else 0.75))))
    if "open_" in m:
        m["open_"] = m["open_"] * r / 12
    if tongue and m["kind"] in ("shout", "smile"):
        m["tongue"] = tongue
    T.mouth(cv, cx, cy + r * 0.55 + mouth_dy, under=r >= 11, **m)


def big_head(S, cx, cy, r, skin="skin", hair="hair", style="short", nose=1.0, cast=(1, 2), jaw=1.0):
    """Tête détaillée : crâne, mâchoire, oreilles, orbites, nez ; style de cheveux comme chibi."""
    layers = {}
    if style in ("long", "curly"):
        hb = S.layer(hair, cast=cast, smooth=1, gain=1.3)
        if style == "long":
            hb.shape(ellipse(cx, cy + r * 0.2, r * 1.2, r * 1.3) - rect(0, int(cy + r * 1.2), 999, 999), h=4, r=4)
        else:
            for k in range(11):
                a = math.radians(165 + 210 * k / 10)
                hb.ell(cx + r * 1.1 * math.cos(a), cy - r * 0.1 + r * 1.0 * math.sin(a), r * 0.45, r * 0.42,
                       h=r * 0.3)
        layers["hair_back"] = hb
    head = S.layer(skin, cast=cast)
    head.ell(cx, cy, r, r * 1.05, h=r * 0.7)
    head.ell(cx, cy + r * 0.45, r * 0.85 * jaw, r * 0.6, h=r * 0.5)
    head.ell(cx - r, cy + r * 0.1, r * 0.2, r * 0.3, h=1.5)
    head.ell(cx + r, cy + r * 0.1, r * 0.2, r * 0.3, h=1.5)
    head.ell(cx - r * 0.42, cy - r * 0.05, r * 0.32, r * 0.18, h=r * 0.12, mode="sub")
    head.ell(cx + r * 0.42, cy - r * 0.05, r * 0.32, r * 0.18, h=r * 0.12, mode="sub")
    head.bias = 0.1
    layers["head"] = head
    nl = S.layer(skin, ink="shadow", ink_dirs="rdl", cast=(1, 1))
    nl.ell(cx + 0.5, cy + r * 0.3, r * 0.2 * nose + 0.5, r * 0.17 * nose + 0.5, h=2)
    layers["nose"] = nl
    if style in ("short", "long", "curly", "bun"):
        hm = ellipse(cx, cy - r * 0.3, r * 1.05, r * 0.85) - rect(0, int(cy - r * 0.3), 999, 999)
        hm |= ellipse(cx - r * 0.95, cy - r * 0.35, r * 0.15, r * 0.3) | ellipse(cx + r * 0.95, cy - r * 0.35, r * 0.15, r * 0.3)
        h = S.layer(hair, cast=cast, smooth=1, gain=1.2)
        h.shape(hm, h=2, r=2)
        h.ell(cx - r * 0.2, cy - r * 0.6, r * 0.8, r * 0.45, h=2, mode="add")
        layers["hair"] = h
        if style == "bun":
            h.ell(cx, cy - r * 1.05, r * 0.4, r * 0.35, h=2, mode="new")
    if style == "spiky":
        hm = ellipse(cx, cy - r * 0.4, r, r * 0.7) - rect(0, int(cy - r * 0.3), 999, 999)
        for k in range(6):
            x = cx - r * 0.85 + k * r * 0.34
            hm |= poly([(x - 2, cy - r * 0.55), (x + 2, cy - r * 0.55), (x + (k - 2.5) * 0.8, cy - r * 1.3)])
        h = S.layer(hair, cast=cast, smooth=1)
        h.shape(hm, h=2, r=2)
        layers["hair"] = h
    if style == "crew":
        h = S.layer(hair, cast=cast, smooth=1)
        h.shape(rect(int(cx - r * 0.85), int(cy - r * 1.05), int(cx + r * 0.85), int(cy - r * 0.55)), h=2, r=2)
        layers["hair"] = h
    return layers


def sweat(cv, x, y, w=("white", 1)):
    cv.put(x, y, INK)
    cv.put(x - 1, y + 1, INK)
    cv.put(x, y + 1, w)
    cv.put(x + 1, y + 1, INK)
    cv.put(x, y + 2, INK)


def stroke(cv, pts, k=INK):
    cv.fill(polyline([(int(round(a)), int(round(b))) for a, b in pts]), k)


def burst(cv, cx, cy, r, k, core):
    m = set()
    for i in range(8):
        a = math.radians(i * 45 + 22)
        rr = r if i % 2 == 0 else r * 0.55
        m |= line(cx, cy, int(round(cx + rr * math.cos(a))), int(round(cy + rr * math.sin(a))))
    cv.fill(m, k)
    cv.fill(ellipse(cx + 0.5, cy + 0.5, 1.5, 1.5), core)
