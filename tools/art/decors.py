"""Décors : hémicycle (votes), urne (élection, référendum), bandeau de journal (rapport)."""
import math

from raster import Canvas, ellipse, rect, poly, line, polyline, border, shift, arc, thick
from vignettes import Pal, text8, text3, star_burst


# --- Hémicycle (160 x 72) ---------------------------------------------------------------

def hemicycle():
    W, H = 160, 72
    p = Pal(('ink', (1, 0, 1)), ('wall', (6, 5, 4)), ('wall2', (4, 3, 3)), ('wood', (4, 2, 1)),
            ('wood2', (2, 1, 1)), ('red', (6, 1, 1)), ('red2', (4, 0, 1)), ('gold', (7, 6, 2)),
            ('skin', (7, 5, 4)), ('skin2', (5, 3, 2)), ('hair', (2, 1, 1)), ('grey', (5, 5, 6)),
            ('suit', (1, 1, 3)), ('white', (7, 7, 7)), ('blue', (2, 3, 7)))
    c = Canvas(W, H, p.wall)
    full = rect(0, 0, W - 1, H - 1)
    # mur du fond : colonnes et tentures
    for x in range(4, W, 20):
        c.paint(rect(x, 0, x + 5, 30), p.white, ink=p.ink)
        c.fill(rect(x + 4, 1, x + 4, 29), p.grey)
        c.fill(rect(x - 1, 0, x + 6, 2), p.gold)
    for x in range(10, W - 10, 20):
        drape = rect(x + 1, 3, x + 12, 30)
        c.fill(drape, p.red2)
        for xx in range(x + 2, x + 12, 3):
            c.fill(rect(xx, 3, xx, 30), p.red)
        c.fill(arc(x + 6.5, 3, 6, 4, 0, 180), p.gold)
    c.fill(rect(0, 0, W - 1, 0), p.ink)
    # drapeaux de part et d'autre du perchoir
    for fx in (62, 92):
        c.fill(rect(fx, 4, fx, 30), p.gold)
        fl = poly([(fx + 1, 5), (fx + 7, 6), (fx + 7, 16), (fx + 1, 15)])
        for (x, y) in fl:
            c.put(x, y, p.blue if x < fx + 3 else (p.white if x < fx + 5 else p.red))
        c.fill(border(fl), p.ink)
    # gradins en arcs de cercle (centre sous l'image), du fond vers l'avant :
    # banquette rouge, députés (tête + épaules), puis pupitre de bois devant eux
    cx, cy, e = 80, 104, 0.72
    tiers = [(100, 88), (86, 74), (72, 60), (58, 46)]
    for k, (r0, r1) in enumerate(tiers):
        band = (ellipse(cx, cy, r0, r0 * e) - ellipse(cx, cy, r1, r1 * e)) & full
        c.paint(band, p.red2, ink=p.ink, edge=False)
        c.fill((ellipse(cx, cy, r0 - 2, (r0 - 2) * e) - ellipse(cx, cy, r0 - 5, (r0 - 5) * e)) & band, p.red)
        rr = r1 + 6
        n = int(math.pi * rr / 9)
        for i in range(n):
            a = math.radians(180 + 180 * (i + 0.5) / n)
            x = cx + rr * math.cos(a)
            y = cy + rr * e * math.sin(a) - 1
            if y < 28 or y > H + 2:
                continue
            seed = (i * 7 + k * 5) % 11
            if seed == 3:
                continue   # siège vide
            body = ellipse(x, y + 6, 4, 3.5) & full
            c.paint(body, (p.suit, p.grey, p.suit, p.wood)[seed % 4], ink=p.ink)
            c.put(int(x), int(y) + 4, p.white)
            head = ellipse(x, y, 3, 3.2)
            c.paint(head, p.skin2 if seed == 5 else p.skin, ink=p.ink)
            hc = (p.hair, p.grey, p.hair, p.gold, p.wood)[seed % 5]
            c.fill(ellipse(x, y - 2, 3, 1.8) & head, hc)
            if seed in (1, 8):   # main levée pour voter
                c.fill(line(int(x) + 4, int(y) + 4, int(x) + 5, int(y) - 4), p.ink)
                c.paint(ellipse(int(x) + 5.5, int(y) - 5, 1.5, 1.5), p.white, ink=p.ink)
        desk = (ellipse(cx, cy, r1 + 3, (r1 + 3) * e) - ellipse(cx, cy, r1, r1 * e)) & full
        c.paint(desk, p.wood, ink=p.ink, edge=False)
        c.fill((ellipse(cx, cy, r1 + 2, (r1 + 2) * e) - ellipse(cx, cy, r1 + 1, (r1 + 1) * e)) & desk, p.gold)
    c.fill((ellipse(cx, cy, 46, 46 * e) - ellipse(cx, cy, 0.1, 0.1)) & full, p.wall2)
    # tribune et perchoir au centre
    # orateur à la tribune
    c.paint(ellipse(80, 62, 6, 5) & rect(70, 57, 90, 64), p.suit, ink=p.ink)
    c.fill(rect(79, 58, 81, 60), p.white)
    c.paint(ellipse(80, 54, 4, 4.5), p.skin, ink=p.ink)
    c.fill(ellipse(80, 51, 4, 2.2) & ellipse(80, 54, 4, 4.5), p.grey)
    c.put(78, 54, p.ink); c.put(82, 54, p.ink)
    c.fill(rect(79, 56, 81, 57), p.ink)
    c.fill(line(85, 60, 88, 52), p.ink)
    c.paint(ellipse(88.5, 51, 1.8, 1.8), p.skin, ink=p.ink)
    c.paint(poly([(66, 72), (68, 62), (92, 62), (94, 72)]), p.wood, ink=p.ink, edge=False)
    c.fill(rect(69, 63, 91, 64), p.gold)
    c.paint(ellipse(80, 68, 2.5, 2.5), p.gold, ink=p.ink)
    c.fill(line(83, 62, 84, 59), p.ink)            # micro
    c.put(84, 58, p.grey)
    return c, p.list


# --- Urne (64 x 64) --------------------------------------------------------------------------

def urne():
    W, H = 64, 64
    p = Pal(('ink', (1, 0, 1)), ('bg', (2, 3, 6)), ('bg2', (3, 4, 7)), ('glass', (5, 6, 7)),
            ('glass2', (6, 7, 7)), ('wood', (5, 3, 1)), ('wood2', (3, 1, 1)), ('steel', (5, 5, 6)),
            ('steel2', (3, 3, 4)), ('env', (6, 5, 3)), ('env2', (5, 4, 2)), ('skin', (7, 5, 4)),
            ('skin2', (5, 3, 2)), ('white', (7, 7, 7)), ('red', (6, 1, 1)))
    c = Canvas(W, H, p.bg)
    for y in range(0, H):
        c.dither(rect(0, y, W - 1, y), p.bg, p.bg2, max(0, 16 - y // 3))
    # table
    c.paint(rect(0, 54, W - 1, H - 1), p.wood, ink=p.ink, edge=False)
    c.fill(rect(0, 56, W - 1, 56), p.wood2)
    # urne transparente
    box = rect(10, 24, 53, 55)
    c.fill(box, p.glass)
    for y in range(25, 55):
        c.dither(rect(11, y, 52, y), p.glass, p.glass2, 4)
    # bulletins dans l'urne
    for (x, y, a) in ((14, 48, 0), (24, 50, 1), (36, 47, 0), (44, 50, 1), (18, 43, 1), (30, 44, 0), (40, 42, 1)):
        e = poly([(x, y), (x + 9, y - 1 - a), (x + 10, y + 4 - a), (x + 1, y + 5)])
        c.paint(e, p.env, ink=p.env2)
    c.fill(border(box), p.ink)
    c.fill(rect(10, 24, 13, 55) | rect(50, 24, 53, 55), p.steel)        # montants
    c.fill(rect(10, 24, 10, 55) | rect(53, 24, 53, 55) | rect(13, 25, 13, 54) | rect(50, 25, 50, 54), p.ink)
    c.fill(rect(12, 26, 12, 53), p.white)
    c.fill(line(16, 52, 26, 27) | line(19, 52, 29, 27), p.white)        # reflets du verre
    # couvercle avec fente et compteur
    lid = rect(7, 18, 56, 24)
    c.paint(lid, p.steel, ink=p.ink)
    c.fill(rect(8, 23, 55, 23), p.steel2)
    c.fill(rect(28, 20, 43, 21), p.ink)
    c.paint(rect(46, 19, 53, 22), p.white, ink=p.ink)
    c.put(48, 20, p.ink); c.put(50, 20, p.ink); c.put(52, 20, p.ink)
    # main qui glisse l'enveloppe dans la fente
    env = poly([(31, 4), (41, 4), (41, 21), (31, 21)])
    c.paint(env, p.env, ink=p.ink)
    c.fill(line(31, 4, 36, 9) | line(41, 4, 36, 9), p.env2)
    c.paint(rect(49, 2, 63, 13), p.red, ink=p.ink, edge=False)        # manche
    c.paint(rect(49, 2, 52, 13), p.white, ink=p.ink)
    hand = ellipse(45, 8, 6, 5.5)
    c.paint(hand, p.skin, ink=p.ink)
    c.grid(38, 4, ["kkkkkk.",
                   "kooook.",
                   "kkkkkk.",
                   "kooook.",
                   "kkkkk..",
                   "kooook.",
                   "kkkkk.."], {'k': p.ink, 'o': p.skin})
    c.fill(arc(46, 7, 4, 3, 200, 260), p.white)
    # bulle « A VOTÉ »
    bubble = ellipse(15, 8, 15, 7)
    c.paint(poly([(15, 12), (21, 12), (25, 18)]), p.white, ink=p.ink)
    c.paint(bubble, p.white, ink=p.ink)
    c.fill(rect(16, 13, 20, 13), p.white)
    text3(c, 4, 6, "A VOTÉ", p.ink)
    c.put(25, 4, p.ink); c.put(26, 3, p.ink)   # accent de É
    return c, p.list


# --- Bandeau de journal (304 x 40) -------------------------------------------------------------

def journal():
    W, H = 304, 40
    p = Pal(('ink', (1, 0, 1)), ('paper', (7, 7, 6)), ('paper2', (6, 6, 5)), ('grey', (4, 4, 4)),
            ('red', (6, 1, 1)), ('blue', (2, 3, 6)), ('white', (7, 7, 7)), ('dark', (2, 2, 2)))
    c = Canvas(W, H, p.paper)
    c.fill(border(rect(0, 0, W - 1, H - 1)), p.ink)
    # filets doubles
    for y in (3, 5, 30, 32):
        c.fill(rect(4, y, W - 5, y), p.ink)
    # titre en grand (police du jeu agrandie x2)
    title = "Le Quotidien du Matin"
    tmp = Canvas(len(title) * 7 + 2, 9, 0)
    text8(tmp, 0, 0, title, 1)
    tw = tmp.w * 2
    x0 = (W - tw) // 2 + 2
    for y in range(tmp.h):
        for x in range(tmp.w):
            if tmp.px[y][x]:
                for dy in (0, 1):
                    for dx in (0, 1):
                        c.put(x0 + 2 * x + dx + 1, 8 + 2 * y + dy + 1, p.grey)
    for y in range(tmp.h):
        for x in range(tmp.w):
            if tmp.px[y][x]:
                for dy in (0, 1):
                    for dx in (0, 1):
                        c.put(x0 + 2 * x + dx, 8 + 2 * y + dy, p.ink)
    # ligne d'information
    text3(c, 6, 34, "ÉDITION SPÉCIALE", p.ink)
    text3(c, W - 6 - 4 * 3 + 1, 34, "1 €", p.ink)
    c.fill(rect(W // 2 - 14, 33, W // 2 + 13, 38), p.red)
    text3(c, W // 2 - 9, 34, "BILAN", p.white)
    # coq stylisé à gauche du titre ... petit soleil levant à droite
    return c, p.list


DECORS = [("art_decor_hemicycle",) + (lambda r=hemicycle(): (r[0], r[1]))(),
          ("art_decor_urne",) + (lambda r=urne(): (r[0], r[1]))(),
          ("art_decor_journal",) + (lambda r=journal(): (r[0], r[1]))()]
DECORS = [(n, (lambda c=c: c), pal) for (n, c, pal) in DECORS]
