"""Illustration de l'écran titre (320 x 128 px, 40 x 16 tuiles), palette pal_art_title.

Façade stylisée d'un palais présidentiel (fictif, inspiré du style classique
français), drapeau, président fictif qui salue sur le perron, gardes et
journalistes au premier plan. Les 3 premières lignes de tuiles (y < 24) sont un
ciel uni : le titre « MEGA PRÉSIDENT » s'écrit par-dessus avec la police.
"""
from raster import Canvas, md, ellipse, rect, poly, line, polyline, border, shift, arc, thick

W, H = 320, 128
GROUND = 104

T, INK, BLUE, SKY2, SKY3, WHITE, STONE, STONE_SH, SLATE, NAVY, RED, SKIN, BROWN, GRAVEL, GREEN, GOLD = range(16)
PALETTE = [
    md(0, 0, 0),    # 0  transparent (inutilisé)
    md(1, 0, 1),    # 1  encre
    md(1, 2, 6),    # 2  bleu (haut du ciel, drapeau)
    md(3, 4, 7),    # 3  ciel
    md(5, 6, 7),    # 4  ciel clair, reflets des vitres
    md(7, 7, 7),    # 5  blanc
    md(7, 7, 5),    # 6  pierre
    md(5, 5, 4),    # 7  pierre, ombre
    md(2, 3, 4),    # 8  ardoise
    md(1, 1, 3),    # 9  marine (vitres, costumes)
    md(6, 1, 1),    # 10 rouge (tapis, drapeau, plumets)
    md(7, 5, 4),    # 11 peau
    md(4, 2, 1),    # 12 brun (ombre de peau, cheveux, bois)
    md(6, 5, 3),    # 13 gravier
    md(2, 4, 1),    # 14 vert (buis)
    md(7, 6, 2),    # 15 or
]

CM = {'k': INK, 'b': BLUE, 's': SKY2, 'l': SKY3, 'w': WHITE, 'p': STONE, 'q': STONE_SH, 'a': SLATE,
      'n': NAVY, 'r': RED, 'o': SKIN, 'c': BROWN, 'g': GRAVEL, 'v': GREEN, 'y': GOLD}


def sky(c):
    c.fill(rect(0, 0, W - 1, H - 1), BLUE)
    c.dither(rect(0, 24, W - 1, 39), BLUE, SKY2, 0)
    for y in range(24, 40):
        c.dither(rect(0, y, W - 1, y), BLUE, SKY2, (y - 24) + 1)
    c.fill(rect(0, 40, W - 1, 55), SKY2)
    for y in range(52, 72):
        c.dither(rect(0, y, W - 1, y), SKY2, SKY3, min(16, (y - 52) + 1))
    c.fill(rect(0, 72, W - 1, H - 1), SKY3)
    # nuages
    for (x, y, s) in ((34, 40, 1.0), (282, 34, 1.2), (226, 46, 0.7), (96, 30, 0.6)):
        m = (ellipse(x, y, 12 * s, 5 * s) | ellipse(x - 9 * s, y + 2 * s, 8 * s, 4 * s)
             | ellipse(x + 10 * s, y + 2 * s, 9 * s, 4 * s) | ellipse(x + 2 * s, y - 3 * s, 7 * s, 5 * s))
        m &= rect(0, 0, W - 1, y + int(5 * s))
        c.fill(m, WHITE)
        c.fill(m - shift(m, 0, -2), SKY3)


def window(c, x, y, w, h, arched=False, balcony=False):
    """Fenêtre à petits carreaux : cadre de pierre, vitres marine et reflet."""
    frame = rect(x - 1, y - 1, x + w, y + h)
    if arched:
        frame |= ellipse(x + w / 2, y, w / 2 + 1, w / 2 + 1) & rect(x - 1, y - w // 2 - 2, x + w, y)
    c.fill(frame, STONE_SH)
    glass = rect(x, y, x + w - 1, y + h - 1)
    if arched:
        glass |= ellipse(x + w / 2, y, w / 2, w / 2) & rect(x, y - w // 2 - 1, x + w - 1, y)
    c.fill(glass, NAVY)
    # reflet diagonal
    for k in range(h + w):
        for d in (0, 1):
            px, py = x + k - d, y + h - 1 - k
            if (px, py) in glass and k % 7 < 3:
                c.put(px, py, SLATE)
    c.fill(line(x + 1, y + 2, x + 2, y + 1) & glass, SKY3)
    # meneaux
    c.fill(line(x + w // 2, y - (w // 2 if arched else 0), x + w // 2, y + h - 1) & glass, WHITE)
    for yy in range(y + 4, y + h - 1, 5):
        c.fill(rect(x, yy, x + w - 1, yy) & glass, WHITE)
    # appui
    c.fill(rect(x - 2, y + h, x + w + 1, y + h), WHITE)
    c.fill(rect(x - 2, y + h + 1, x + w + 1, y + h + 1), STONE_SH)
    if balcony:
        c.fill(rect(x - 2, y + h - 4, x + w + 1, y + h - 4), INK)
        for xx in range(x - 1, x + w + 1, 2):
            c.fill(rect(xx, y + h - 3, xx, y + h - 1), INK)


def building(c):
    # --- ailes ---------------------------------------------------------------
    wall = rect(6, 64, W - 7, GROUND - 1)
    c.paint(wall, STONE)
    roof = poly([(6, 64), (14, 52), (W - 14, 52), (W - 6, 64)])
    c.paint(roof, SLATE)
    for y in range(55, 64, 3):                        # rangs d'ardoises
        c.fill(rect(10, y, W - 11, y) & roof - border(roof), NAVY)
    c.fill(rect(7, 64, W - 8, 65), WHITE)             # corniche
    c.fill(rect(7, 66, W - 8, 66), STONE_SH)
    c.fill(rect(7, 82, W - 8, 82), STONE_SH)          # bandeau entre les étages
    # chaînages d'angle (pierres en saillie)
    for x0 in (6, W - 13):
        for y in range(67, GROUND, 6):
            c.fill(rect(x0 + 1, y, x0 + 6, y + 2), STONE_SH)
    # cheminées
    for x in (40, 92, W - 48, W - 100):
        c.paint(rect(x, 44, x + 6, 53), STONE)
        c.fill(rect(x, 44, x + 6, 45), STONE_SH)
    # lucarnes rondes (œils-de-bœuf)
    for x in list(range(24, 112, 24)) + list(range(W - 24, 208, -24)):
        m = ellipse(x, 58, 4, 4)
        c.paint(m, STONE)
        c.fill(ellipse(x, 58, 2.2, 2.2), NAVY)
        c.put(x - 1, 57, SKY3)
    # fenêtres des ailes
    for x in list(range(16, 108, 16)) + list(range(W - 23, 212, -16)):
        window(c, x, 68, 7, 11, balcony=False)
        window(c, x, 89, 7, 13, arched=True)
    c.fill(rect(6, GROUND - 2, W - 7, GROUND - 1), STONE_SH)   # soubassement

    # --- pavillon central -----------------------------------------------------
    pav = rect(112, 48, 207, GROUND - 1)
    c.paint(pav, STONE)
    c.fill(rect(113, 48, 115, GROUND - 2), STONE_SH)   # ombres des arêtes
    c.fill(rect(204, 48, 206, GROUND - 2), STONE_SH)
    # balustrade du toit
    c.paint(rect(110, 42, 209, 44), WHITE)
    for x in range(113, 207, 4):
        c.fill(rect(x, 45, x + 1, 47), STONE_SH)
    c.fill(rect(110, 47, 209, 48), STONE_SH)
    c.fill(rect(110, 49, 209, 49), INK)
    # fronton cintré au centre
    ped = ellipse(160, 48, 26, 9) & rect(130, 38, 190, 47)
    c.paint(ped, STONE)
    c.fill(arc(160, 48, 22, 6, 190, 350), STONE_SH)
    c.paint(ellipse(160, 44, 3.5, 3.5), GOLD)         # médaillon doré
    c.fill(rect(110, 62, 209, 62), WHITE)
    c.fill(rect(110, 63, 209, 63), STONE_SH)
    c.fill(rect(110, 80, 209, 80), WHITE)
    c.fill(rect(110, 81, 209, 81), STONE_SH)
    # étage : fenêtres à balcon
    for x in (120, 140, 173, 193):
        window(c, x, 66, 7, 11, balcony=True)
    window(c, 155, 64, 10, 13, arched=True, balcony=True)
    # colonnes du rez-de-chaussée
    for x in (118, 132, 180, 194):
        col = rect(x, 83, x + 6, GROUND - 1)
        c.paint(col, WHITE)
        c.fill(rect(x + 4, 84, x + 5, GROUND - 2), STONE_SH)
        c.fill(rect(x - 1, 82, x + 7, 83), STONE)      # chapiteau
        c.fill(rect(x - 1, 82, x + 7, 82), INK)
    # portes-fenêtres entre les colonnes
    for x in (125, 187):
        window(c, x + 1, 89, 5, 13, arched=True)
    # porte d'entrée
    door = rect(146, 86, 173, GROUND - 1) | (ellipse(160, 86, 14, 10) & rect(146, 75, 173, 86))
    c.fill(thick(door, 1), STONE_SH)
    c.paint(door, SLATE)
    c.fill(rect(159, 80, 160, GROUND - 1), INK)
    for (x0, x1) in ((149, 156), (163, 170)):
        c.fill(rect(x0, 90, x1, 96) - rect(x0 + 1, 91, x1 - 1, 95), NAVY)
    c.fill(arc(160, 86, 10, 7, 190, 350), GOLD)       # imposte dorée
    for x in (150, 154, 160, 166, 170):
        c.put(x, 84, GOLD)
    # lanternes
    for x in (140, 179):
        c.paint(rect(x, 86, x + 3, 91), GOLD)
        c.fill(rect(x + 1, 87, x + 2, 89), WHITE)
        c.fill(rect(x + 1, 84, x + 2, 85), INK)

    # --- drapeau ----------------------------------------------------------------
    c.fill(rect(159, 22, 160, 41), INK)
    c.put(159, 21, GOLD); c.put(160, 21, GOLD)
    flag = poly([(161, 23), (171, 21), (181, 24), (190, 22), (190, 35), (181, 37), (171, 34), (161, 36)])
    for (x, y) in flag:
        if x < 171:
            col = BLUE
        elif x < 181:
            col = WHITE
        else:
            col = RED
        c.put(x, y, col)
    c.fill(border(flag) - rect(161, 0, 161, 99), INK)
    for (x, y) in ((176, 26), (177, 27), (186, 30), (187, 31), (166, 31), (167, 32)):   # plis
        c.put(x, y, {BLUE: NAVY, WHITE: STONE_SH, RED: BROWN}[c.get(x, y)] if c.get(x, y) in (BLUE, WHITE, RED) else c.get(x, y))


def courtyard(c):
    yard = rect(0, GROUND, W - 1, H - 1)
    c.fill(yard, GRAVEL)
    for (x, y) in yard:
        if ((x * 7 + y * 13) % 17 == 0) or ((x * 3 + y * 11) % 23 == 0):
            c.put(x, y, STONE_SH)
        elif (x * 5 + y * 3) % 29 == 0:
            c.put(x, y, STONE)
    c.fill(rect(0, GROUND, W - 1, GROUND), STONE_SH)
    # ombre portée du bâtiment
    for x in range(6, W - 6):
        c.put(x, GROUND + 1, BROWN if x % 2 else STONE_SH)
    # perron : trois marches
    for i, (x0, x1) in enumerate(((140, 179), (136, 183), (132, 187))):
        y = GROUND - 4 + i * 3
        step = rect(x0, y, x1, y + 2)
        c.paint(step, WHITE)
        c.fill(rect(x0 + 1, y + 2, x1 - 1, y + 2), STONE_SH)
    # tapis rouge
    carpet = poly([(150, GROUND - 4), (169, GROUND - 4), (176, H), (143, H)])
    c.paint(carpet, RED, edge=True)
    c.fill(line(152, GROUND + 6, 146, H) | line(167, GROUND + 6, 173, H), GOLD)
    for i in range(3):
        c.fill(rect(151 - i, GROUND - 2 + i * 3, 168 + i, GROUND - 2 + i * 3), BROWN)
    # cordons et poteaux dorés le long du tapis
    for x, y in ((140, 110), (179, 110), (136, 120), (183, 120)):
        c.paint(rect(x, y, x + 1, y + 7), GOLD)
        c.paint(ellipse(x + 1, y, 1.6, 1.6), GOLD)


def topiary(c, x, y):
    """Buis taillé en cône dans un bac."""
    m = poly([(x, y - 18), (x + 6, y - 4), (x - 6, y - 4)]) | ellipse(x, y - 6, 6, 3)
    c.paint(m, GREEN)
    c.fill({(px, py) for (px, py) in m - border(m) if px > x + 1 and (px + py) % 2}, INK)
    c.fill(line(x - 2, y - 13, x - 3, y - 10), GOLD)
    box = rect(x - 5, y - 4, x + 4, y + 3)
    c.paint(box, WHITE)
    c.fill(rect(x - 4, y - 3, x - 4, y + 2), GREEN)
    c.fill(rect(x + 3, y - 3, x + 3, y + 2), GREEN)


def guard(c, x, y, flip=False):
    """Garde républicain (fictif) : casque doré à plumet rouge, tunique marine, pantalon blanc."""
    rows = [
        "...rr...",
        "..rrr...",
        "..ryk...",
        ".kyyyk..",
        "kyyyyyk.",
        "kcccccc.",
        ".kooook.",
        ".kooook.",
        "..kook..",
        ".knnnnk.",
        "knynnynk",
        "knnyynnk",
        "konnnnok",
        "konnnnok",
        ".kwwwwk.",
        ".kwwkwk.",
        ".kwwkwk.",
        ".kwwkwk.",
        ".kkkkkk.",
    ]
    c.grid(x, y, rows, CM, flip)
    # fusil
    gx = x - 1 if not flip else x + 8
    c.fill(rect(gx, y + 2, gx, y + 15), BROWN)
    c.put(gx, y + 1, WHITE); c.put(gx, y, WHITE)


def president(c, x, y):
    """Président fictif qui salue : grosse tête, grand sourire, bras levé.
    (x, y) = pieds."""
    cx = x
    # jambes et chaussures
    c.paint(rect(cx - 5, y - 10, cx - 1, y - 2), NAVY)
    c.paint(rect(cx + 1, y - 10, cx + 5, y - 2), NAVY)
    c.paint(rect(cx - 7, y - 2, cx - 1, y), INK)
    c.paint(rect(cx + 1, y - 2, cx + 7, y), INK)
    c.put(cx - 6, y - 2, SLATE); c.put(cx + 6, y - 2, SLATE)
    # torse (veste)
    torso = poly([(cx - 8, y - 9), (cx - 9, y - 22), (cx - 5, y - 26), (cx + 5, y - 26), (cx + 9, y - 22), (cx + 8, y - 9)])
    c.paint(torso, NAVY)
    c.fill(poly([(cx - 3, y - 26), (cx + 3, y - 26), (cx, y - 18)]), WHITE)
    c.fill(poly([(cx - 1, y - 25), (cx + 1, y - 25), (cx + 1.5, y - 19), (cx, y - 17), (cx - 1.5, y - 19)]), RED)
    c.fill(line(cx - 4, y - 26, cx, y - 16) | line(cx + 4, y - 26, cx, y - 16), INK)
    c.fill(rect(cx - 8, y - 12, cx + 7, y - 12), INK)
    c.put(cx, y - 14, GOLD)
    c.paint(ellipse(cx + 5.5, y - 22, 1.2, 1.2), WHITE)   # cocarde... petite pochette
    # bras gauche le long du corps (à l'écran)
    arm_l = poly([(cx - 9, y - 23), (cx - 6, y - 23), (cx - 7, y - 11), (cx - 11, y - 11)])
    c.paint(arm_l, NAVY)
    c.paint(ellipse(cx - 9, y - 10, 2.5, 2.5), SKIN)
    # bras droit levé : il salue (épaule -> coude écarté -> main en l'air)
    upper = poly([(cx + 6, y - 25), (cx + 9, y - 26), (cx + 19, y - 22), (cx + 17, y - 18)])
    fore = poly([(cx + 16, y - 21), (cx + 20, y - 21), (cx + 22, y - 36), (cx + 18, y - 36)])
    c.paint(upper | fore, NAVY)
    c.fill(line(cx + 17, y - 20, cx + 19, y - 22), SLATE)
    c.grid(cx + 15, y - 47, [".k.k.k.k..",
                             "kokokokok.",
                             "kokokokok.",
                             "kokokokok..",
                             "kooooooookk",
                             "koooooooook",
                             ".koooooook.",
                             "..koooook..",
                             "..kwwwwwk..",
                             "..kkkkkkk.."], CM)
    # traits de mouvement
    c.fill(arc(cx + 20, y - 42, 9, 8, 205, 245), WHITE)
    c.fill(arc(cx + 20, y - 42, 9, 8, 295, 335), WHITE)
    # tête (grosse : caricature)
    hy = y - 37
    c.paint(rect(cx - 2, hy + 9, cx + 1, hy + 11), SKIN)            # cou
    c.paint(ellipse(cx - 9, hy + 1, 2.5, 3.5) | ellipse(cx + 9, hy + 1, 2.5, 3.5), SKIN)   # oreilles
    head = ellipse(cx, hy, 9, 10) | ellipse(cx, hy + 3, 8, 7)
    c.paint(head, SKIN)
    c.shade(head - border(head), BROWN, -1, 0)
    hair = (ellipse(cx, hy - 4, 10, 7) - rect(cx - 12, hy - 3, cx + 12, hy + 20)) | ellipse(cx - 8, hy - 2, 2, 4) | ellipse(cx + 8, hy - 2, 2, 4)
    hair -= ellipse(cx + 3, hy - 3, 6, 3)
    c.paint(hair, BROWN)
    c.fill(line(cx - 6, hy - 8, cx - 2, hy - 10), GOLD)
    # visage : yeux plissés de joie, sourcils hauts, gros nez, grand sourire
    c.grid(cx - 7, hy - 4, ["kkk.....kkk",
                            "...........",
                            "...........",
                            ".kkk...kkk.",
                            "k...k.k...k"], CM)
    c.grid(cx - 2, hy, [".c..",
                        ".c..",
                        "oook",
                        "kooc",
                        ".kk."], CM)
    c.grid(cx - 6, hy + 4, ["k...........k",
                            ".kkkkkkkkkkk.",
                            ".kwwwwwwwwwk.",
                            "..kwwwwwwwk..",
                            "...krrrrrk...",
                            "....kkkkk...."], CM)
    c.put(cx - 7, hy + 3, RED); c.put(cx + 6, hy + 3, RED)   # joues


def journalists(c):
    """Premier plan : journalistes de dos, caméras, perche, micros, flashs."""
    # --- groupe de gauche ---
    def person(x, y, hair, jacket, w=11, bald=False):
        body = ellipse(x, y + 16, w, 12) & rect(x - w - 1, y + 4, x + w + 1, H)
        c.paint(body, jacket)
        c.shade(body - border(body), {SLATE: NAVY, BROWN: INK, NAVY: INK, GREEN: INK, RED: BROWN, STONE_SH: SLATE}[jacket], -3, 0)
        head = ellipse(x, y, 6, 7)
        c.paint(head, SKIN if bald else hair)
        if not bald:
            c.fill(arc(x, y, 4.5, 5, 200, 250), {BROWN: GOLD, INK: SLATE, GOLD: WHITE, STONE_SH: WHITE}[hair])
        else:
            c.put(x - 2, y - 3, WHITE); c.put(x - 1, y - 4, WHITE)
            c.fill(ellipse(x, y + 4, 6, 3) & head - border(head), BROWN)
        c.paint(ellipse(x - 6, y + 1, 1.5, 2.5), SKIN)
        c.paint(ellipse(x + 6, y + 1, 1.5, 2.5), SKIN)

    def camera(x, y, flip=False):
        rows = ["...kkkk......",
                "..kaaaak.....",
                "kkkkkkkkkkkk.",
                "knnnnnnnnnnkk",
                "knaaannkkknlk",
                "knnnnnnkwknlk",
                "knnnnnnkkknkk",
                "kkkkkkkkkkkk.",
                "....kcck.....",
                "....kcck....."]
        c.grid(x, y, rows, CM, flip)

    def flash(x, y, r=6):
        star = set()
        for (dx, dy) in ((1, 0), (0, 1), (1, 1), (1, -1)):
            star |= line(x - dx * r, y - dy * r, x + dx * r, y + dy * r)
        star |= ellipse(x + 0.5, y + 0.5, 2.5, 2.5)
        c.fill(thick(star, 1) & thick(ellipse(x + 0.5, y + 0.5, r * 0.6, r * 0.6), 0), GOLD)
        c.fill(star, WHITE)

    def boom(x0, y0, x1, y1):
        c.fill(line(x0, y0, x1, y1) | line(x0, y0 + 1, x1, y1 + 1), INK)
        fur = ellipse(x1, y1, 7, 4)
        c.paint(fur, STONE_SH)
        for (px, py) in fur - border(fur):
            if (px + py * 2) % 3 == 0:
                c.put(px, py, WHITE)

    # perche au-dessus de la foule (vers le perron)
    boom(30, 126, 118, 86)
    boom(296, 124, 214, 90)

    person(14, 106, BROWN, SLATE)
    person(40, 110, INK, BROWN, w=12)
    camera(30, 96)
    flash(26, 94, 6)
    person(66, 108, GOLD, GREEN)
    person(92, 113, STONE_SH, NAVY, bald=True)
    # micro tendu
    c.fill(line(100, 112, 112, 102), INK)
    c.paint(ellipse(113.5, 100.5, 2.5, 2.5), STONE_SH)
    c.paint(rect(106, 104, 110, 107), RED)

    person(226, 112, BROWN, RED)
    person(250, 107, INK, SLATE, w=12)
    camera(256, 96, flip=True)
    flash(272, 95, 6)
    person(278, 110, GOLD, NAVY)
    person(304, 106, STONE_SH, BROWN, bald=True)
    c.fill(line(218, 113, 206, 102), INK)
    c.paint(ellipse(205.5, 100.5, 2.5, 2.5), STONE_SH)
    c.paint(rect(209, 104, 213, 107), BLUE)


def make_title():
    c = Canvas(W, H, BLUE)
    sky(c)
    building(c)
    courtyard(c)
    topiary(c, 104, GROUND)
    topiary(c, 216, GROUND)
    guard(c, 135, GROUND - 22)
    guard(c, 177, GROUND - 22, flip=True)
    president(c, 160, GROUND - 4)
    journalists(c)
    return c


TITLE = [("art_title", make_title)]
