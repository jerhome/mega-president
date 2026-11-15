"""Décors V3 (tâche 3.17) : hémicycle (160 x 72), urne (64 x 64), bandeau du journal (304 x 40).

Même style que les gueules V3 (volumes éclairés, contour sombre). L'hémicycle sert
d'ambiance (l'interface dessine en plus les sièges des groupes) : séance animée,
perchoir, députés caricaturés. Tout est fictif.
"""
import math
import random

from raster import ellipse, rect, poly, line, border
from scene import Scene, chibi, face, big_head, big_face, sweat, stroke, burst, INK
from vignettes import text8, text3, text8_width, FONT3, FONT8
import traits as T

FONT3.setdefault("Z", ["###", "..#", ".#.", "#..", "###"])
FONT3.setdefault("'", [".#.", ".#.", "...", "...", "..."])
FONT3.setdefault(":", ["...", ".#.", "...", ".#.", "..."])
FONT3.setdefault(".", ["...", "...", "...", "...", ".#."])
SKIN = [(3, 1, 1), (5, 3, 2), (7, 5, 4)]


# --- Hémicycle ---------------------------------------------------------------------

def hemicycle():
    W, H = 160, 72
    S = Scene(W, H, {"bg": [(2, 1, 1), (3, 2, 1), (5, 3, 2)], "red": [(3, 0, 0), (5, 0, 1), (7, 1, 1)],
                     "skin": SKIN, "hair": [(1, 0, 1), (3, 2, 1), (6, 6, 6)], "suit": [(1, 1, 2), (2, 2, 4)],
                     "gold": [(7, 6, 2)], "white": [(5, 5, 5), (7, 7, 7)]})
    S.P.ramps["blond"] = [(3, 2, 1), (7, 6, 2)]

    @S.background
    def bg(g):
        g.gradient(0, 30, ("bg", 1), ("bg", 0))
        for x in range(4, W, 16):                    # colonnes et tentures
            g.rect(x, 0, x + 2, 26, ("bg", 2))
            g.rect(x + 3, 0, x + 12, 22, ("red", 1))
            for xx in range(x + 4, x + 12, 3):
                g.rect(xx, 0, xx, 21, ("red", 0))
            g.fill(ellipse(x + 7.5, 22, 5, 2) - rect(0, 0, 999, 21), ("red", 1))
        g.rect(0, 0, W - 1, 1, ("gold", 0))

    rng = random.Random(11)
    # gradins : pour chaque rang, les députés puis le pupitre rouge devant eux
    cx0, cy0 = 80, 124
    rows = [(100, 4.3), (86, 4.8), (72, 5.4)]
    styles = ["short", "bald", "short", "long", "curly", "bun", "short", "bald"]
    hairs = ["hair", "hair", "blond", "hair"]
    heads = []
    for ri, (rad, r) in enumerate(rows):
        n = int(math.pi * rad / (r * 2.6))
        for k in range(n):
            a = math.radians(196 + 148 * (k + 0.5) / n)
            x = cx0 + rad * math.cos(a)
            y = cy0 + rad * math.sin(a) - 3
            if not (3 < x < W - 4 and 6 < y < H):
                continue
            if abs(x - 80) < 20 and y > 26:      # place pour le perchoir
                continue
            st = styles[rng.randrange(len(styles))]
            hm = "blond" if (st in ("long", "bun") and rng.random() < 0.5) else rng.choice(hairs)
            chibi(S, x, y, r, hair=hm, style=st, body="suit", body_w=r * 1.3, body_h=7)
            heads.append((x, y, r))
        desk = S.layer("red", cast=(1, 2))
        ring = ellipse(cx0, cy0, rad + 9, rad + 9) - ellipse(cx0, cy0, rad + 4, rad + 4)
        desk.shape({p for p in ring if 0 <= p[0] < W and 0 <= p[1] < H and abs(p[0] - 80) >= 20}, h=3, r=3)
    # perchoir et tribune
    chibi(S, 80, 36, 5.5, hair="hair", style="bald", body="suit", body_w=7, body_h=8)   # président de séance
    perch = S.layer("bg", cast=(1, 2))
    perch.shape(rect(64, 44, 96, 71), h=4, r=3)
    chibi(S, 80, 52, 5, hair="blond", style="short", body="suit", body_w=6, body_h=6)   # orateur
    trib = S.layer("bg", cast=(1, 2))
    trib.shape(rect(70, 58, 90, 71), h=3, r=2)

    @S.draw
    def d(cv):
        moods = ["neutral", "happy", "angry", "sleep", "shout", "o", "neutral", "worried"]
        for i, (x, y, r) in enumerate(heads):
            m = moods[(i * 7 + 3) % len(moods)]
            face(cv, x, y, r, m)
            if m == "sleep" and i % 3 == 0:
                text3(cv, int(x + 3), int(y - 9), "Z", ("white", 1))
        face(cv, 80, 36, 5.5, "angry")
        face(cv, 80, 52, 5, "shout")
        cv.grid(86, 33, [".y.", "yyy", "yyy", "k.k"], {"y": ("gold", 0), "k": INK})   # la sonnette
        cv.fill(rect(66, 47, 94, 47), ("gold", 0))
        cv.fill(rect(72, 61, 88, 61), ("gold", 0))
        cv.fill(rect(77, 63, 83, 66), ("white", 1))   # le micro, la feuille
        # gags : avion en papier, journal déplié, téléphone
        cv.grid(30, 30, ["w....", ".ww..", "..www", ".ww..", "w...."], {"w": ("white", 1)})
        stroke(cv, [(22, 34), (26, 33), (29, 32)], ("white", 0))
        cv.fill(rect(118, 52, 130, 58), ("white", 1))
        cv.fill(border(rect(118, 52, 130, 58)), INK)
        for y in (54, 56):
            cv.fill(line(120, y, 128, y), ("white", 0))
        text3(cv, 132, 30, "!!", ("gold", 0)) if "!" in FONT3 else None
    return S.render(frame=False)


# --- Urne --------------------------------------------------------------------------

def urne():
    """Urne transparente, enveloppes, main qui vote, assesseur moustachu ravi : « A VOTÉ ! »."""
    W, H = 64, 64
    S = Scene(W, H, {"bg": [(1, 2, 4), (2, 3, 6)], "glass": [(3, 4, 6), (5, 6, 7), (7, 7, 7)],
                     "env": [(4, 2, 1), (6, 5, 3), (7, 7, 5)], "skin": SKIN, "hair": [(5, 6, 7), (7, 7, 7)],
                     "red": [(4, 0, 0), (7, 1, 1)], "wood": [(3, 1, 1), (4, 2, 1)], "suit": [(1, 1, 2)]})
    S.P.ramps["white"] = [(5, 6, 7), (7, 7, 7)]

    @S.background
    def bg(g):
        g.gradient(0, 63, ("bg", 1), ("bg", 0))
        g.rect(0, 57, 63, 63, ("wood", 1))
        g.rect(0, 57, 63, 57, ("wood", 0))

    # l'assesseur, derrière l'urne
    chibi(S, 53, 17, 9, hair="hair", style="bald", body="suit", body_w=11, body_h=24)
    hs = S.layer("hair", cast=(1, 1), smooth=1)
    hs.shape(ellipse(44.5, 15, 2, 4) | ellipse(61.5, 15, 2, 4), h=1, r=1)
    mu = S.layer("hair", cast=(1, 1), smooth=1)
    mu.ell(50, 21, 4, 2, h=1.5)
    mu.ell(56, 21, 4, 2, h=1.5)
    box = S.layer("glass", ink=True, gain=0.5, cast=(1, 2))
    box.shape(rect(6, 28, 52, 57), h=2, r=6)
    lid = S.layer("glass", cast=(1, 2))
    lid.shape(rect(3, 24, 55, 29), h=2, r=2)
    rng = random.Random(3)
    envs = []
    for k, (x, y) in enumerate(((9, 49), (21, 50), (33, 48), (15, 42), (28, 41), (40, 50))):
        env = S.layer("env", cast=(1, 1), flat=1 + k % 2)
        env.shape(rect(x, y, x + 9, y + 5), h=1, r=1)
        envs.append((x, y, k))
    envd = S.layer("env", cast=(1, 2))
    envd.shape(rect(34, 12, 44, 25), h=2, r=2)
    sleeve = S.layer("red", cast=(1, 2))
    sleeve.shape(rect(35, 0, 44, 7), h=3, r=2)
    hand = S.layer("skin", cast=(1, 2))
    hand.ell(39.5, 10, 6, 4.5, h=3)

    @S.draw
    def d(cv):
        face(cv, 53, 17, 9, "happy")
        cv.fill(rect(32, 26, 46, 26), INK)          # la fente
        for k in range(4):                           # reflets du verre
            cv.fill(line(10 + k, 36 + k * 3, 14 + k, 32 + k * 3), ("glass", 2))
        cv.fill(line(34, 13, 39, 17) | line(39, 17, 44, 13), ("env", 0))
        for (x, y, k) in envs:
            cv.fill(line(x + 1, y + 1, x + 4, y + 3) | line(x + 5, y + 3, x + 8, y + 1), ("env", 0))
        # bulle « A VOTÉ ! »
        m = ellipse(18, 9, 17, 7.5)
        tail = poly([(24, 14), (29, 13), (32, 21)])
        cv.fill(m | tail, ("white", 1))
        cv.fill(border(m | tail), INK)
        text3(cv, 3, 7, "A VOTÉ !", INK)
        cv.fill(rect(10, 60, 54, 61), ("wood", 0))
    return S.render()


# --- Bandeau du journal ------------------------------------------------------------

def journal():
    W, H = 304, 40
    S = Scene(W, H, {"bg": [(6, 6, 5), (7, 7, 6)], "ink2": [(3, 3, 3)], "red": [(5, 0, 0), (7, 1, 1)],
                     "skin": SKIN, "hair": [(5, 5, 5), (7, 7, 7)], "suit": [(3, 2, 1), (5, 3, 2)],
                     "tie": [(5, 0, 0)], "sky": [(3, 4, 6), (5, 6, 7)], "sun": [(7, 6, 2)]})
    S.P.ramps["white"] = [(5, 5, 5), (7, 7, 7)]

    @S.background
    def bg(g):
        g.rect(0, 0, W - 1, H - 1, ("bg", 1))
        for y in range(H):                           # grain du papier
            for x in range(W):
                if (x * 7 + y * 13) % 31 == 0:
                    g.put(x, y, ("bg", 0))
        for y in (3, 5, 30, 32):
            g.rect(4, y, W - 5, y, INK)
        g.rect(4, 4, W - 5, 4, ("red", 1))

    # oreille gauche : l'éditorialiste
    chibi(S, 20, 19, 7.5, hair="hair", style="bald", body="suit", body_w=9, body_h=8)
    hl = S.layer("hair", cast=(1, 1), smooth=1)
    hl.shape(ellipse(12.5, 19, 2.5, 5) | ellipse(27.5, 19, 2.5, 5), h=1, r=1)
    # oreille droite : la météo politique
    cloud = S.layer("sky", cast=(1, 1))
    for (x, y, r) in ((278, 19, 6), (286, 17, 7), (293, 20, 5)):
        cloud.ell(x, y, r, r * 0.8, h=3)
    sun = S.layer("sun")
    sun.ell(272, 13, 4.5, 4.5, h=3)

    @S.draw
    def d(cv):
        face(cv, 20, 19, 7.5, "neutral", glasses=True)
        cv.fill(line(14, 14, 18, 15) | line(22, 15, 26, 13), INK)    # sourcil levé
        cv.grid(18, 26, ["kk.kk", "krkrk", "kk.kk"], {"k": INK, "r": ("tie", 0)})   # nœud papillon
        text3(cv, 34, 12, "L'ÉDITO", ("red", 0))
        text3(cv, 34, 19, "DE H.", INK)
        text3(cv, 34, 25, "LENGLET", INK)
        text3(cv, 240, 12, "MÉTÉO", ("red", 0))
        text3(cv, 240, 19, "ORAGES", INK)
        text3(cv, 240, 25, "AU SÉNAT", INK) if "N" in FONT3 else None
        cv.fill(line(283, 25, 281, 29) | line(289, 25, 287, 29), ("sky", 0))
        # titre : capitales condensées hautes (police du jeu, en gras, x2 en hauteur)
        title = "Le Quotidien du Matin"
        tw = len(title) * 8
        x0 = (W - tw) // 2
        for k, ch in enumerate(title):
            g = FONT8.get(ch, FONT8["?"])
            for j, row in enumerate(g):
                for i, v in enumerate(row):
                    if v == "#":
                        for dy in (0, 1):
                            for dx in (0, 1):
                                cv.put(x0 + k * 8 + i + dx + 1, 8 + 2 * j + dy + 1, ("ink2", 0))
                        for dy in (0, 1):
                            for dx in (0, 1 if False else 0, 1):
                                cv.put(x0 + k * 8 + i + dx, 8 + 2 * j + dy, INK)
        for x in ():                                # (pas de fleurons : place des oreilles)
            cv.grid(x, 14, [".r.", "rrr", ".r."], {"r": ("red", 1)})
        text3(cv, 6, 34, "ÉDITION SPÉCIALE", INK)
        text3(cv, W - 6 - 4 * 3 + 1, 34, "1 €", INK)
        cv.fill(rect(W // 2 - 14, 33, W // 2 + 13, 38), ("red", 1))
        text3(cv, W // 2 - 9, 34, "BILAN", ("white", 1))
    return S.render()


DECORS = []
for _name, _fn in (("art_decor_hemicycle", hemicycle), ("art_decor_urne", urne), ("art_decor_journal", journal)):
    _img, _pal = _fn()
    DECORS.append((_name, (lambda c=_img: c), _pal))
