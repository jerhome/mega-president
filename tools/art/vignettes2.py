"""Vignettes d'événements, suite (même style que vignettes.py)."""
import math

from raster import Canvas, ellipse, rect, poly, line, polyline, border, shift, arc, thick
from vignettes import (Pal, VW, VH, FRAME, text8, text3, text8_width, frame, sky_gradient,
                       star_burst)


# --- 6. Économie, bourse ---------------------------------------------------------------

def v_bourse():
    p = Pal(('ink', (1, 0, 1)), ('bg', (1, 1, 2)), ('grid', (2, 2, 4)), ('green', (2, 6, 2)),
            ('green2', (1, 3, 1)), ('red', (7, 1, 1)), ('red2', (4, 0, 0)), ('white', (7, 7, 7)),
            ('gold', (7, 6, 1)), ('gold2', (5, 3, 0)), ('frame', (4, 4, 5)), ('frame2', (6, 6, 7)),
            ('skin', (7, 5, 4)), ('suit', (3, 3, 4)), ('pink', (7, 3, 3)))
    c = Canvas(VW, VH, p.frame)
    # écran de cotations
    scr = rect(3, 3, 60, 37)
    c.paint(scr, p.bg, ink=p.ink)
    for x in range(8, 60, 8):
        c.fill(rect(x, 4, x, 36), p.grid)
    for y in range(9, 37, 7):
        c.fill(rect(4, y, 59, y), p.grid)
    c.fill(rect(1, 1, 62, 1), p.frame2)
    # chandeliers
    vals = [(7, 14, 20, 1), (12, 12, 18, 1), (17, 10, 15, 0), (22, 8, 13, 1), (27, 9, 16, 0),
            (32, 14, 22, 0), (37, 18, 25, 0), (42, 20, 29, 0)]
    for (x, y0, y1, up) in vals:
        col = p.green if up else p.red
        c.fill(rect(x + 1, y0 - 2, x + 1, y1 + 2), col)
        c.paint(rect(x, y0, x + 2, y1), col, ink=None)
    # grosse flèche rouge qui plonge et sort de l'écran
    path = [(5, 18), (14, 11), (22, 16), (30, 9), (40, 25), (47, 21), (57, 38)]
    body = set()
    for i in range(len(path) - 1):
        body |= line(*path[i], *path[i + 1])
    body = thick(body, 2)
    head = poly([(51, 38), (62, 31), (61, 46)])
    c.paint(body | head, p.red, ink=p.ink)
    c.fill(thick(set(line(*path[0], *path[1])) | line(*path[2], *path[3]), 0), p.pink)
    # bandeau défilant
    band = rect(3, 38, 60, 45)
    c.paint(band, p.ink, ink=p.ink)
    text3(c, 5, 40, "-12%", p.red)
    text3(c, 23, 40, "-8%", p.red)
    text3(c, 38, 40, "+1%", p.green)
    c.fill(poly([(52, 40), (56, 40), (54, 44)]), p.red)
    # pièces qui tombent
    for (x, y) in ((50, 8), (55, 15), (44, 4)):
        coin = ellipse(x, y, 3, 3)
        c.paint(coin, p.gold, ink=p.ink)
        c.put(x - 1, y - 1, p.white)
        c.put(x, y, p.gold2)
    frame(c, p.ink)
    return c, p.list


# --- 7. Santé, hôpital --------------------------------------------------------------------

def v_sante():
    p = Pal(('ink', (1, 0, 1)), ('sky', (4, 6, 7)), ('sky2', (6, 7, 7)), ('wall', (7, 7, 7)),
            ('wall2', (5, 5, 6)), ('win', (2, 4, 6)), ('win2', (5, 6, 7)), ('blue', (1, 2, 6)),
            ('red', (7, 1, 1)), ('orange', (7, 4, 0)), ('road', (3, 3, 4)), ('green', (2, 5, 2)),
            ('tire', (2, 2, 2)), ('yel', (7, 7, 2)), ('grey', (4, 4, 4)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky, p.sky2, 0, 30)
    # hôpital
    h = rect(6, 8, 57, 36)
    c.paint(h, p.wall, ink=p.ink)
    c.fill(rect(50, 9, 56, 35), p.wall2)
    c.paint(rect(4, 6, 59, 8), p.wall2, ink=p.ink)
    for wy in (12, 20):
        for wx in range(9, 56, 7):
            if 25 <= wx <= 36:
                continue
            c.paint(rect(wx, wy, wx + 4, wy + 4), p.win, ink=p.ink)
            c.put(wx + 1, wy + 1, p.win2)
    # enseigne « H »
    sign = rect(25, 1, 38, 14)
    c.paint(sign, p.blue, ink=p.ink)
    text8(c, 28, 4, "H", p.wall)
    # entrée vitrée
    c.paint(rect(24, 24, 39, 36), p.win, ink=p.ink)
    c.fill(rect(31, 25, 32, 35), p.ink)
    c.fill(line(26, 34, 29, 26), p.win2)
    c.paint(rect(22, 20, 41, 23), p.green, ink=p.ink)      # auvent
    # route
    c.fill(rect(0, 37, VW - 1, VH - 1), p.road)
    for x in range(2, VW, 10):
        c.fill(rect(x, 43, x + 4, 43), p.wall)
    # ambulance
    amb = rect(8, 30, 40, 41)
    c.paint(amb, p.wall, ink=p.ink)
    cab = poly([(40, 33), (47, 33), (52, 38), (52, 41), (40, 41)])
    c.paint(cab, p.wall, ink=p.ink)
    c.paint(poly([(42, 34), (46, 34), (49, 37), (42, 37)]), p.win, ink=p.ink)
    c.fill(rect(9, 36, 51, 37), p.orange)
    c.fill(rect(9, 38, 51, 38), p.red)
    # croix « étoile de vie » bleue
    c.fill(rect(20, 31, 22, 36) | rect(18, 33, 24, 34), p.blue)
    c.fill(rect(27, 32, 35, 34), p.grey)
    for wx in (14, 45):
        c.paint(ellipse(wx, 42, 3, 3), p.tire, ink=p.ink)
        c.put(wx, 42, p.grey)
    # gyrophares
    c.paint(rect(12, 28, 15, 29), p.blue, ink=p.ink)
    c.paint(rect(33, 28, 36, 29), p.blue, ink=p.ink)
    star_burst(c, 13, 26, 4, p.win2, p.wall)
    frame(c, p.ink)
    return c, p.list


# --- 8. Écologie, pollution -----------------------------------------------------------------

def v_pollution():
    p = Pal(('ink', (1, 0, 1)), ('sky', (5, 5, 3)), ('sky2', (6, 6, 4)), ('smoke', (2, 2, 2)),
            ('smoke2', (4, 3, 3)), ('brick', (5, 2, 1)), ('brick2', (3, 1, 1)), ('grey', (4, 4, 4)),
            ('water', (2, 4, 2)), ('water2', (3, 5, 3)), ('oil', (1, 1, 1)), ('white', (7, 7, 7)),
            ('fish', (6, 6, 5)), ('tree', (4, 3, 1)), ('yel', (7, 6, 1)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.smoke2, p.sky, 0, 28)
    # usine et cheminées
    c.paint(rect(4, 18, 34, 33), p.brick, ink=p.ink)
    for x in range(4, 34, 10):
        c.paint(poly([(x, 18), (x + 10, 12), (x + 10, 18)]), p.grey, ink=p.ink)
    for wx in range(7, 33, 6):
        c.paint(rect(wx, 22, wx + 3, 26), p.yel, ink=p.ink)
    for x in range(5, 34, 4):
        c.put(x, 29, p.brick2); c.put(x + 2, 31, p.brick2)
    for (x, top) in ((40, 4), (50, 8)):
        ch = rect(x, top, x + 5, 33)
        c.paint(ch, p.brick, ink=p.ink)
        c.fill(rect(x + 1, top + 3, x + 4, top + 4), p.white)
        c.fill(rect(x + 4, top + 1, x + 4, 32), p.brick2)
    # fumée noire
    smoke = set()
    for (x, y, r) in ((42, 2, 4), (36, 0, 5), (28, 2, 6), (20, 0, 6), (52, 5, 4), (58, 2, 5), (12, 3, 6), (4, 1, 6)):
        smoke |= ellipse(x, y, r, r * 0.8)
    c.paint(smoke & FRAME, p.smoke, ink=p.ink, edge=False)
    for (x, y) in ((30, 1), (18, 2), (56, 2)):
        c.fill(arc(x, y, 3, 2, 200, 320), p.smoke2)
    # rivière polluée
    c.fill(rect(0, 34, VW - 1, VH - 1), p.water)
    c.fill(rect(0, 34, VW - 1, 34), p.ink)
    for y in range(37, VH, 4):
        for x in range((y * 3) % 9, VW, 9):
            c.fill(rect(x, y, x + 3, y), p.water2)
    # nappe d'huile
    slick = ellipse(22, 41, 14, 3.5)
    c.fill(slick, p.oil)
    c.fill(arc(22, 41, 11, 2, 190, 260), p.yel)
    c.fill(arc(22, 41, 9, 1.5, 280, 340), p.water2)
    # poisson le ventre en l'air
    fish = ellipse(46, 40, 7, 3)
    c.paint(fish, p.fish, ink=p.ink)
    c.paint(poly([(52, 40), (57, 37), (57, 43)]), p.fish, ink=p.ink)
    c.grid(41, 38, ["k.k", ".k.", "k.k"], {'k': p.ink})
    c.fill(rect(43, 42, 49, 42), p.grey)
    # arbre mort
    c.fill(polyline([(60, 34), (60, 22), (57, 17)]) | line(60, 26, 63, 22) | line(60, 22, 62, 19), p.tree)
    c.fill(line(59, 34, 59, 24), p.tree)
    frame(c, p.ink)
    return c, p.list


# --- 9. Sécurité, police, émeutes -----------------------------------------------------------

def v_securite():
    p = Pal(('ink', (1, 0, 1)), ('night', (1, 1, 3)), ('night2', (2, 2, 4)), ('wall', (3, 3, 5)),
            ('win', (7, 6, 2)), ('white', (7, 7, 7)), ('grey', (4, 4, 5)), ('blue', (1, 3, 7)),
            ('blue2', (4, 6, 7)), ('red', (7, 1, 1)), ('tape', (7, 6, 0)), ('road', (2, 2, 3)),
            ('tire', (1, 1, 1)), ('glass', (3, 4, 6)), ('pink', (7, 4, 4)))
    c = Canvas(VW, VH, p.night)
    sky_gradient(c, p.night, p.night2, 0, 24)
    for (x0, x1, top) in ((-1, 12, 8), (13, 26, 4), (27, 44, 10), (45, 64, 6)):
        c.paint(rect(x0, top, x1, 34), p.wall, ink=p.ink)
        for wx in range(x0 + 3, x1 - 2, 5):
            for wy in range(top + 3, 30, 6):
                if (wx * 7 + wy) % 3 == 0:
                    c.fill(rect(wx, wy, wx + 2, wy + 3), p.win)
    c.fill(rect(0, 35, VW - 1, VH - 1), p.road)
    # voiture de police
    car = poly([(6, 34), (6, 28), (14, 27), (19, 21), (40, 21), (46, 27), (57, 28), (58, 34), (58, 39), (6, 39)])
    c.paint(car, p.white, ink=p.ink)
    c.paint(poly([(20, 22), (29, 22), (29, 27), (16, 27)]), p.glass, ink=p.ink)
    c.paint(poly([(31, 22), (39, 22), (44, 27), (31, 27)]), p.glass, ink=p.ink)
    c.fill(line(22, 23, 20, 26), p.blue2)
    c.fill(rect(7, 31, 57, 33), p.blue)
    c.fill(rect(7, 34, 57, 34), p.red)
    c.fill(rect(30, 28, 30, 38), p.ink)
    c.fill(rect(57, 29, 57, 30), p.tape)
    for wx in (15, 48):
        c.paint(ellipse(wx, 39, 4, 4), p.tire, ink=p.ink)
        c.paint(ellipse(wx, 39, 1.8, 1.8), p.grey, ink=None)
    # gyrophares : bleu à gauche (allumé)
    c.paint(rect(25, 18, 29, 20), p.blue, ink=p.ink)
    c.paint(rect(31, 18, 35, 20), p.red, ink=p.ink)
    beam = poly([(27, 19), (0, 6), (0, 16)])
    for (x, y) in beam:
        if c.get(x, y) in (p.night, p.night2, p.wall) and (x + y) % 2 == 0:
            c.put(x, y, p.blue)
    star_burst(c, 27, 17, 5, p.blue2, p.white)
    # rubalise au premier plan
    tape = set()
    for (a, b) in (((-1, 40), (64, 44)),):
        tape |= line(*a, *b) | line(a[0], a[1] + 1, b[0], b[1] + 1) | line(a[0], a[1] + 2, b[0], b[1] + 2)
    for (x, y) in tape:
        c.put(x, y, p.tape if (x // 4) % 2 == 0 else p.ink)
    frame(c, p.ink)
    return c, p.list


# --- 10. Numérique, cyberattaque -------------------------------------------------------------

def v_numerique():
    p = Pal(('ink', (1, 0, 1)), ('bg', (1, 1, 2)), ('bg2', (2, 1, 3)), ('green', (2, 7, 2)),
            ('green2', (1, 4, 1)), ('case', (5, 5, 6)), ('case2', (3, 3, 4)), ('white', (7, 7, 7)),
            ('red', (7, 1, 1)), ('red2', (4, 0, 0)), ('gold', (7, 6, 1)), ('gold2', (5, 3, 0)),
            ('screen', (0, 1, 1)), ('desk', (4, 2, 1)), ('blue', (2, 3, 7)))
    c = Canvas(VW, VH, p.bg)
    # pluie de chiffres binaires
    for k in range(70):
        x = (k * 37) % 62 + 1
        y = (k * 53) % 44 + 1
        text3(c, x, y, "01"[k % 2], p.green2)
    # écran
    mon = rect(10, 4, 53, 34)
    c.paint(mon, p.case, ink=p.ink)
    c.fill(rect(50, 5, 52, 33), p.case2)
    scr = rect(13, 7, 50, 30)
    c.paint(scr, p.screen, ink=p.ink)
    for y in range(9, 29, 3):
        w = (y * 7) % 20 + 6
        c.fill(rect(15, y, 15 + w, y), p.green2)
    # tête de mort rouge sur l'écran
    skull = ellipse(32, 16, 8, 7) | rect(27, 18, 37, 25)
    c.paint(skull, p.white, ink=p.ink)
    c.paint(ellipse(28.5, 16.5, 2.5, 2.5), p.red, ink=None)
    c.paint(ellipse(35.5, 16.5, 2.5, 2.5), p.red, ink=None)
    c.fill(rect(31, 20, 32, 21), p.ink)
    for x in range(28, 37, 2):
        c.fill(rect(x, 23, x, 25), p.ink)
    c.fill(rect(27, 26, 37, 26), p.ink)
    # pied et bureau
    c.paint(rect(28, 35, 35, 38), p.case2, ink=p.ink)
    c.fill(rect(0, 39, VW - 1, VH - 1), p.desk)
    c.fill(rect(0, 39, VW - 1, 39), p.ink)
    c.paint(rect(16, 41, 47, 45), p.case, ink=p.ink)     # clavier
    for x in range(18, 46, 3):
        c.put(x, 43, p.case2)
    # cadenas brisé
    c.paint(rect(51, 30, 61, 39), p.gold, ink=p.ink)
    c.fill(arc(54, 30, 3.5, 5, 180, 300), p.ink)
    c.fill(arc(58, 26, 3.5, 4, 240, 360), p.ink)
    c.put(56, 34, p.ink); c.put(56, 35, p.ink)
    c.fill(rect(52, 31, 52, 37), p.white)
    # alerte
    c.paint(poly([(6, 4), (12, 15), (0, 15)]) & FRAME, p.red, ink=p.ink)
    c.fill(rect(6, 8, 6, 11), p.white); c.put(6, 13, p.white)
    frame(c, p.ink)
    return c, p.list


# --- 11. Éducation ----------------------------------------------------------------------------

def v_education():
    p = Pal(('ink', (1, 0, 1)), ('wall', (6, 6, 4)), ('wall2', (5, 5, 3)), ('board', (1, 3, 2)),
            ('board2', (2, 4, 3)), ('chalk', (7, 7, 7)), ('wood', (5, 3, 1)), ('wood2', (3, 2, 1)),
            ('red', (7, 1, 1)), ('green', (2, 5, 1)), ('blue', (2, 3, 6)), ('yel', (7, 6, 1)),
            ('skin', (7, 5, 4)), ('grey', (4, 4, 5)), ('pink', (7, 4, 4)))
    c = Canvas(VW, VH, p.wall)
    for y in range(0, VH, 6):
        c.fill(rect(0, y, VW - 1, y), p.wall2)
    # tableau noir (vert)
    c.paint(rect(4, 3, 59, 30), p.wood, ink=p.ink)
    board = rect(6, 5, 57, 28)
    c.paint(board, p.board, ink=p.ink)
    c.fill(line(8, 27, 20, 7), p.board2)
    text8(c, 9, 7, "ABC", p.chalk)
    text3(c, 34, 8, "1+1=2", p.chalk)
    text3(c, 34, 15, "2+2=4", p.chalk)
    c.fill(arc(20, 22, 10, 3, 0, 180), p.chalk)          # soulignement
    c.paint(rect(40, 29, 47, 30), p.chalk, ink=p.ink)     # brosse
    c.fill(rect(41, 29, 46, 29), p.wood)
    # bureau
    c.paint(rect(0, 36, VW - 1, 40), p.wood, ink=p.ink)
    c.fill(rect(1, 39, VW - 2, 39), p.wood2)
    c.fill(rect(0, 41, VW - 1, VH - 1), p.wood2)
    # pile de livres
    for i, col in enumerate((p.red, p.blue, p.green)):
        y = 31 + i * 0 - i * 3 + 3
        b = rect(6 + i, y, 22 - i, y + 2)
        c.paint(b, col, ink=p.ink)
        c.fill(rect(20 - i, y + 1, 21 - i, y + 1), p.chalk)
    # pomme
    apple = ellipse(32, 32, 4, 4)
    c.paint(apple, p.red, ink=p.ink)
    c.put(30, 30, p.pink); c.put(31, 30, p.pink)
    c.fill(line(32, 28, 33, 26), p.wood2)
    c.paint(ellipse(35, 27, 2, 1.2), p.green, ink=p.ink)
    # cartable
    bag = rect(42, 30, 58, 41)
    c.paint(bag, p.yel, ink=p.ink)
    c.fill(arc(50, 30, 4, 3, 180, 360), p.ink)
    c.paint(rect(42, 30, 58, 34), p.red, ink=p.ink)
    c.paint(rect(48, 33, 51, 36), p.grey, ink=p.ink)
    # cloche de l'école (sonnerie) en haut
    c.paint(ellipse(61, 2, 3, 3) & FRAME, p.yel, ink=p.ink)
    frame(c, p.ink)
    return c, p.list


# --- 12. Énergie, électricité ---------------------------------------------------------------------

def v_energie():
    p = Pal(('ink', (1, 0, 1)), ('night', (1, 1, 3)), ('night2', (2, 2, 5)), ('steel', (4, 4, 5)),
            ('steel2', (2, 2, 3)), ('yel', (7, 7, 2)), ('yel2', (7, 5, 0)), ('white', (7, 7, 7)),
            ('glass', (6, 7, 7)), ('city', (2, 2, 4)), ('win', (6, 6, 2)), ('cable', (1, 1, 1)),
            ('grey', (5, 5, 5)), ('blue', (3, 5, 7)), ('orange', (7, 4, 1)))
    c = Canvas(VW, VH, p.night)
    sky_gradient(c, p.night, p.night2, 0, VH - 1)
    for (x, y) in ((4, 4), (14, 9), (58, 3), (50, 12), (24, 2)):
        c.put(x, y, p.white)
    # ville (moitié éteinte)
    for (x0, x1, top) in ((-1, 9, 34), (10, 18, 30), (19, 29, 36), (40, 49, 31), (50, 58, 35), (59, 64, 28)):
        c.paint(rect(x0, top, x1, VH - 1), p.city, ink=p.ink)
        for wx in range(x0 + 2, x1 - 1, 3):
            for wy in range(top + 2, VH - 2, 4):
                if x0 > 35 and (wx + wy) % 2 == 0:
                    c.put(wx, wy, p.win)
    # pylône
    py = set()
    py |= line(32, 4, 24, 46) | line(32, 4, 40, 46)
    for (y, hw) in ((12, 7), (20, 9)):
        py |= line(32 - hw, y, 32 + hw, y)
    for y in range(14, 46, 8):
        dx = 2 + (y - 4) * 8 // 42
        py |= line(32 - dx, y, 32 + dx, y + 6) | line(32 + dx, y, 32 - dx, y + 6)
    c.fill(thick(py, 1), p.ink)
    c.fill(py, p.steel)
    # câbles
    for (x, y) in ((25, 12), (39, 12), (23, 20), (41, 20)):
        side = -1 if x < 32 else 1
        end = 0 if side < 0 else VW - 1
        c.fill(polyline([(x, y + 1), ((x + end) // 2, y + 5), (end, y + 2)]), p.cable)
    # éclair qui frappe le câble
    bolt = poly([(52, 0), (59, 0), (55, 7), (60, 7), (50, 20), (53, 10), (48, 10)])
    c.paint(bolt, p.yel, ink=p.ink)
    star_burst(c, 50, 21, 4, p.yel2, p.white)
    # grosse ampoule
    bulb = ellipse(13, 18, 8, 8) | poly([(9, 22), (17, 22), (16, 28), (10, 28)])
    c.paint(bulb, p.glass, ink=p.ink)
    c.fill(arc(12, 17, 5, 5, 190, 260), p.white)
    c.fill(polyline([(10, 24), (11, 17), (13, 21), (15, 17), (16, 24)]), p.orange)
    c.paint(rect(9, 28, 17, 33), p.grey, ink=p.ink)
    c.fill(rect(10, 30, 16, 30), p.steel2)
    c.fill(rect(12, 34, 14, 34), p.ink)
    frame(c, p.ink)
    return c, p.list


# --- 13. Agriculture ------------------------------------------------------------------------------

def v_agriculture():
    p = Pal(('ink', (1, 0, 1)), ('sky', (4, 6, 7)), ('sky2', (6, 7, 7)), ('grass', (3, 6, 1)),
            ('grass2', (2, 4, 1)), ('white', (7, 7, 7)), ('grey', (5, 5, 6)), ('black', (1, 1, 1)),
            ('pink', (7, 4, 4)), ('barn', (5, 1, 1)), ('barn2', (3, 0, 0)), ('yel', (7, 6, 1)),
            ('steel', (4, 4, 5)), ('brown', (4, 2, 1)), ('sun', (7, 7, 3)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky, p.sky2, 0, 20)
    c.paint(ellipse(56, 6, 5, 5), p.sun, ink=None)
    # collines et grange
    c.fill(ellipse(16, 34, 30, 12) | ellipse(52, 36, 26, 12), p.grass2)
    c.fill(rect(0, 30, VW - 1, VH - 1), p.grass)
    barn = rect(4, 12, 20, 28) | poly([(2, 13), (12, 5), (22, 13)])
    c.paint(barn, p.barn, ink=p.ink)
    c.paint(poly([(2, 13), (12, 5), (22, 13)]), p.barn2, ink=p.ink)
    c.paint(rect(9, 19, 15, 28), p.white, ink=p.ink)
    c.fill(line(9, 19, 15, 28) | line(15, 19, 9, 28), p.white)
    c.fill(line(10, 20, 14, 27) | line(14, 20, 10, 27), p.ink)
    # vache (grosse tête de face, caricature)
    body = ellipse(40, 33, 17, 9)
    c.paint(body, p.white, ink=p.ink)
    for (x, y, r) in ((34, 30, 3.5), (47, 34, 4), (41, 38, 2.5)):
        c.fill(ellipse(x, y, r, r * 0.8) & body - border(body), p.black)
    for x in (28, 34, 46, 52):
        c.paint(rect(x, 38, x + 3, 45), p.white, ink=p.ink)
        c.fill(rect(x, 44, x + 3, 45), p.black)
    c.paint(ellipse(42, 42, 3, 2), p.pink, ink=p.ink)          # pis
    # tête
    head = ellipse(23, 26, 8, 8)
    c.paint(ellipse(14, 21, 4, 2), p.white, ink=p.ink)
    c.paint(ellipse(32, 21, 4, 2), p.white, ink=p.ink)
    c.paint(poly([(16, 17), (14, 12), (18, 15)]), p.yel, ink=p.ink)
    c.paint(poly([(30, 17), (32, 12), (28, 15)]), p.yel, ink=p.ink)
    c.paint(head, p.white, ink=p.ink)
    c.fill(ellipse(20, 22, 3, 3) & head - border(head), p.black)
    c.grid(18, 22, ["kk...kk", "wk...wk"], {'k': p.ink, 'w': p.white})
    muzzle = ellipse(23, 31, 7, 4.5)
    c.paint(muzzle, p.pink, ink=p.ink)
    c.put(20, 30, p.ink); c.put(26, 30, p.ink)
    c.fill(arc(23, 32, 3, 1.5, 20, 160), p.ink)
    # cloche
    c.paint(poly([(21, 35), (25, 35), (26, 39), (20, 39)]), p.yel, ink=p.ink)
    # bidon de lait
    can = rect(55, 31, 61, 41)
    c.paint(can, p.steel, ink=p.ink)
    c.paint(rect(56, 28, 60, 31), p.grey, ink=p.ink)
    c.fill(rect(56, 33, 56, 39), p.white)
    # touffes d'herbe
    for (x, y) in ((4, 40), (12, 44), (52, 45), (60, 44)):
        c.fill(polyline([(x, y), (x + 1, y - 2), (x + 2, y), (x + 3, y - 3), (x + 4, y)]), p.grass2)
    frame(c, p.ink)
    return c, p.list


# --- 14. Industrie, emploi -----------------------------------------------------------------------

def v_industrie():
    p = Pal(('ink', (1, 0, 1)), ('sky', (4, 4, 5)), ('sky2', (5, 5, 6)), ('brick', (5, 2, 1)),
            ('brick2', (3, 1, 1)), ('roof', (3, 3, 4)), ('win', (2, 3, 4)), ('steel', (5, 5, 6)),
            ('steel2', (3, 3, 4)), ('gold', (7, 6, 1)), ('white', (7, 7, 7)), ('red', (7, 1, 1)),
            ('skin', (7, 5, 4)), ('blue', (2, 3, 6)), ('yel', (7, 7, 2)))
    c = Canvas(VW, VH, p.sky)
    sky_gradient(c, p.sky, p.sky2, 0, 30)
    # usine à toits en dents de scie
    fac = rect(2, 14, 61, 38)
    c.paint(fac, p.brick, ink=p.ink)
    for x in range(2, 60, 12):
        c.paint(poly([(x, 14), (x + 12, 6), (x + 12, 14)]), p.roof, ink=p.ink)
        c.fill(line(x + 11, 8, x + 11, 13), p.win)
    for x in range(4, 61, 4):
        for y in range(17, 38, 4):
            c.put(x + (y // 4) % 2 * 2, y, p.brick2)
    c.paint(rect(48, 0, 53, 14), p.brick, ink=p.ink)        # cheminée éteinte
    c.fill(rect(49, 2, 52, 3), p.white)
    for wx in (6, 46):
        c.paint(rect(wx, 18, wx + 9, 26), p.win, ink=p.ink)
        c.fill(rect(wx + 4, 18, wx + 5, 26) | rect(wx, 22, wx + 9, 22), p.ink)
        c.fill(line(wx + 1, 25, wx + 3, 19), p.ink)             # vitre cassée
    # portail fermé avec chaîne et cadenas
    gate = rect(18, 18, 44, 41)
    c.fill(gate, p.win)
    for x in range(19, 44, 3):
        c.fill(rect(x, 18, x, 41), p.steel)
    c.fill(rect(18, 18, 44, 19) | rect(18, 40, 44, 41) | rect(18, 29, 44, 29), p.steel2)
    c.fill(border(gate), p.ink)
    c.fill(rect(31, 18, 31, 41), p.ink)
    chain = set()
    for k in range(9):
        chain |= ellipse(22 + k * 2.4, 29 + (1 if k % 2 else 0), 1.6, 1.2)
    c.fill(border(chain), p.ink)
    c.paint(rect(29, 31, 34, 36), p.gold, ink=p.ink)
    c.fill(arc(31.5, 31, 2, 2.5, 180, 360), p.ink)
    # panneau « FERMÉ »
    sign = rect(13, 7, 49, 15)
    c.paint(sign, p.white, ink=p.ink)
    w = text8_width("FERMÉ")
    text8(c, 31 - w // 2, 8, "FERMÉ", p.red)
    # sol
    c.fill(rect(0, 42, VW - 1, VH - 1), p.steel2)
    c.fill(rect(0, 42, VW - 1, 42), p.ink)
    # ouvrier triste, casque jaune
    c.paint(ellipse(56, 36, 5, 6) & rect(50, 33, 62, 47), p.blue, ink=p.ink)
    c.paint(ellipse(56, 30, 3.5, 4), p.skin, ink=p.ink)
    c.paint(ellipse(56, 27.5, 4, 2.5) - rect(50, 28, 62, 40), p.yel, ink=p.ink)
    c.fill(rect(51, 27, 61, 27), p.yel)
    c.put(55, 30, p.ink); c.put(57, 30, p.ink)
    c.put(55, 32, p.ink); c.put(56, 31, p.ink); c.put(57, 32, p.ink)
    frame(c, p.ink)
    return c, p.list


# --- 15. Fête, culture, sport, bonne nouvelle ---------------------------------------------------

def v_fete():
    p = Pal(('ink', (1, 0, 1)), ('night', (1, 1, 3)), ('night2', (3, 1, 4)), ('gold', (7, 6, 1)),
            ('gold2', (5, 3, 0)), ('white', (7, 7, 7)), ('red', (7, 1, 2)), ('blue', (2, 4, 7)),
            ('green', (2, 6, 2)), ('pink', (7, 3, 6)), ('orange', (7, 4, 0)), ('wood', (3, 2, 1)),
            ('cyan', (3, 7, 7)), ('grey', (4, 4, 5)), ('yel', (7, 7, 3)))
    c = Canvas(VW, VH, p.night)
    sky_gradient(c, p.night, p.night2, 0, VH - 1)
    # feux d'artifice
    def firework(cx, cy, r, col, col2):
        for k in range(12):
            a = math.radians(k * 30)
            for t in range(3, r + 1, 2):
                x = int(round(cx + t * math.cos(a)))
                y = int(round(cy + t * math.sin(a)))
                c.put(x, y, col if t < r - 1 else col2)
        c.put(cx, cy, p.white)
    firework(12, 11, 9, p.red, p.yel)
    firework(52, 9, 8, p.blue, p.cyan)
    firework(34, 6, 5, p.green, p.white)
    firework(56, 30, 6, p.pink, p.white)
    # coupe dorée
    cup = ellipse(32, 22, 10, 9) - rect(20, 0, 44, 15)
    cup |= rect(22, 14, 41, 21)
    c.paint(ellipse(21, 20, 4, 4) - ellipse(21, 20, 2, 2), p.gold, ink=p.ink)
    c.paint(ellipse(43, 20, 4, 4) - ellipse(43, 20, 2, 2), p.gold, ink=p.ink)
    c.paint(cup, p.gold, ink=p.ink)
    c.shade(cup - border(cup), p.gold2, -3, 0)
    c.fill(rect(25, 16, 26, 25), p.yel)
    c.fill(rect(22, 14, 41, 14), p.ink)
    c.paint(rect(30, 30, 33, 35), p.gold, ink=p.ink)
    c.paint(rect(25, 36, 38, 38), p.gold2, ink=p.ink)
    c.paint(rect(23, 39, 40, 44), p.wood, ink=p.ink)
    c.fill(rect(27, 41, 36, 42), p.gold)
    star = poly([(32, 16), (33.5, 19.5), (37, 20), (34.5, 22.5), (35.5, 26), (32, 24), (28.5, 26), (29.5, 22.5),
                 (27, 20), (30.5, 19.5)])
    c.paint(star, p.white, ink=p.gold2)
    # confettis
    cols = [p.red, p.blue, p.green, p.pink, p.yel, p.cyan, p.orange]
    for k in range(34):
        x = (k * 29 + 7) % 62 + 1
        y = (k * 41 + 3) % 46 + 1
        if c.get(x, y) in (p.night, p.night2):
            c.put(x, y, cols[k % len(cols)])
            if k % 3 == 0:
                c.put(x + 1, y, cols[k % len(cols)])
    frame(c, p.ink)
    return c, p.list
