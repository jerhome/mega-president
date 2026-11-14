"""Portraits caricaturaux (56 x 56 px, 7 x 7 tuiles), palette commune pal_art_portrait.

Tous les personnages sont fictifs. Chaque portrait est construit à partir de
primitives (tête, cou, buste, cheveux en masques géométriques) puis de petites
grilles dessinées à la main pour les yeux, sourcils, nez et bouches.
"""
from raster import (Canvas, md, ellipse, rect, poly, line, polyline, thick, shift,
                    mirror, border, arc)

S = 56

# Palette commune des portraits (indices nommés)
T, INK, DARK, SKSH, SKIN, PINK, WHITE, LGREY, GREY, NAVY, BLUE, RED, GREEN, YELLOW, CHEST, BG = range(16)
PALETTE = [
    md(0, 0, 0),    # 0  transparent
    md(1, 0, 1),    # 1  encre (contours)
    md(3, 1, 1),    # 2  brun très sombre (cheveux noirs, ombres profondes)
    md(5, 3, 2),    # 3  peau, ombre
    md(7, 5, 4),    # 4  peau
    md(7, 3, 3),    # 5  rose (joues, lèvres)
    md(7, 7, 7),    # 6  blanc
    md(5, 5, 6),    # 7  gris clair (cheveux blancs, reflets)
    md(3, 3, 4),    # 8  gris moyen
    md(1, 1, 3),    # 9  bleu marine (costumes)
    md(2, 3, 6),    # 10 bleu
    md(6, 1, 1),    # 11 rouge
    md(2, 5, 1),    # 12 vert
    md(7, 6, 2),    # 13 jaune / blond
    md(5, 2, 1),    # 14 châtain / roux
    md(5, 6, 7),    # 15 fond
]

# Ombre associée à chaque couleur de remplissage
SHADOW = {SKIN: SKSH, SKSH: DARK, WHITE: LGREY, LGREY: GREY, GREY: NAVY, NAVY: INK,
          BLUE: NAVY, RED: DARK, GREEN: DARK, YELLOW: CHEST, CHEST: DARK, DARK: INK,
          PINK: RED, BG: LGREY}

FRAME = rect(0, 0, S - 1, S - 1)

# Caractères des grilles
CM = {'k': INK, 'd': DARK, 's': SKSH, 'o': SKIN, 'p': PINK, 'w': WHITE, 'l': LGREY,
      'g': GREY, 'n': NAVY, 'b': BLUE, 'r': RED, 'v': GREEN, 'y': YELLOW, 'c': CHEST, 'f': BG}


class Face:
    """Portrait en cours : canevas + masque de la tête pour découper les détails."""

    def __init__(self, bg=BG, bg2=None):
        self.c = Canvas(S, S, bg)
        if bg2 is not None:   # fond bicolore tramé (bas plus sombre)
            self.c.dither(rect(0, 30, S - 1, S - 1), bg, bg2, 4)
            self.c.dither(rect(0, 40, S - 1, S - 1), bg, bg2, 8)
        self.head = set()
        self.cx = 28

    # --- corps -------------------------------------------------------------
    def body(self, suit=NAVY, shirt=WHITE, tie=RED, top=44, wide=27, lapels=True,
             tie_kind="tie", vdepth=12, vw=8):
        """Buste : veste (suit), col de chemise (shirt), cravate ou nœud (tie)."""
        c = self.c
        w = wide
        torso = poly([(28 - w - 4, S + 2), (28 - w, top + 9), (28 - w + 4, top + 4), (28 - w + 10, top + 1),
                      (28 - 6, top - 1), (28 + 6, top - 1), (28 + w - 10, top + 1), (28 + w - 4, top + 4),
                      (28 + w, top + 9), (28 + w + 4, S + 2)]) & FRAME
        c.paint(torso, suit, edge=False)
        c.shade(torso - border(torso), SHADOW[suit], -4, 0)
        self.torso = torso
        if shirt is not None:
            v = poly([(28 - vw, top - 2), (28 + vw, top - 2), (28, top + vdepth)]) & FRAME
            c.fill(v, shirt)
            c.fill(rect(28 - vw + 2, top - 3, 28 + vw - 3, top - 1), shirt)
            if lapels:   # bords des revers de la veste
                c.fill(line(28 - vw, top - 1, 27, top + vdepth), INK)
                c.fill(line(28 + vw - 1, top - 1, 28, top + vdepth), INK)
                c.fill(line(28 - vw + 1, top + 2, 26, top + vdepth - 1) & torso - v, SHADOW[suit])
            # col de chemise : deux pointes blanches cernées d'encre
            c.paint(poly([(28 - vw + 2, top - 4), (28, top + 1), (25, top + 5), (28 - vw + 1, top + 1)]), shirt)
            c.paint(poly([(28 + vw - 2, top - 4), (28, top + 1), (31, top + 5), (28 + vw - 1, top + 1)]), shirt)
        if tie_kind == 'tie' and tie is not None:
            blade = poly([(26.5, top + 3), (29.5, top + 3), (31, top + 10), (28, top + 14), (25, top + 10)]) & FRAME
            c.paint(blade, tie, edge=False)
            c.paint(poly([(25.5, top - 1), (30.5, top - 1), (29.5, top + 3), (26.5, top + 3)]), tie)
            for (x, y) in ((29, top + 4), (29, top + 5), (30, top + 7), (30, top + 8)):
                c.put(x, y, SHADOW[tie])
        elif tie_kind == 'bow' and tie is not None:
            self.g(22, top - 2, ["kkk....kkk",
                                 "krrk..krrk",
                                 "krrrkkrrrk",
                                 "krrdrrdrrk",
                                 "krrrkkrrrk",
                                 "krrk..krrk",
                                 "kkk....kkk"], r=tie, d=SHADOW[tie])

    def neck(self, w=10, top=34, bottom=46, skin=SKIN):
        c = self.c
        n = rect(28 - w // 2, top, 28 + w // 2 - 1, bottom)
        c.paint(n, skin, edge=False)
        c.fill(rect(28 - w // 2 + 1, bottom, 28 + w // 2 - 2, bottom), skin)
        c.fill(rect(28 - w // 2 + 1, top, 28 + w // 2 - 2, top + 3), SHADOW[skin])
        self.skin = skin

    # --- tête --------------------------------------------------------------
    def make_head(self, cx=28, cy=24, rx=15, ry=17, jaw=None, chin=None, skin=SKIN, ears=True,
                  ear_y=25, shade=True):
        c = self.c
        h = ellipse(cx, cy, rx, ry)
        if jaw:   # (rx, ry, cy) mâchoire : ellipse inférieure plus large
            jrx, jry, jcy = jaw
            h |= ellipse(cx, jcy, jrx, jry)
        if chin:
            h |= chin
        self.cx = cx
        self.skin = skin
        if ears:
            l = ellipse(cx - rx + 0.5, ear_y, 3, 4.5)
            r = ellipse(cx + rx - 0.5, ear_y, 3, 4.5)
            c.paint(l, skin); c.paint(r, skin)
            for dy in (-1, 0, 1):
                c.put(int(cx - rx) - 1, ear_y + dy, SHADOW[skin])
                c.put(int(cx + rx), ear_y + dy, SHADOW[skin])
        c.paint(h, skin)
        if shade:
            inner = h - border(h)
            c.shade(inner, SHADOW[skin], -2, -1)
        self.head = h
        self.inner = h - border(h)
        return h

    def double_chin(self, y, w=8):
        c = self.c
        c.fill(arc(28, y - 2, w, 3, 20, 160), INK)

    # --- cheveux -----------------------------------------------------------
    def hair(self, m, color, shade=True, clip_frame=True, ink=INK):
        c = self.c
        if clip_frame:
            m = m & FRAME
        c.paint(m, color, ink=ink)
        if shade:
            inner = m - border(m)
            c.shade(inner, SHADOW[color], -2, 0)
        return m

    # --- détails (grilles) --------------------------------------------------
    def g(self, x, y, rows, flip=False, **extra):
        cmap = dict(CM)
        cmap.update(extra)
        self.c.grid(x, y, rows, cmap, flip)

    def eyes(self, y, kind='open', dx=6, look=0):
        """Paire d'yeux symétriques autour de cx ; look : -1 gauche, 0, 1 droite."""
        rows = EYES[kind]
        w = len(rows[0])
        xl = self.cx - dx - w // 2 - 1
        xr = self.cx + dx - w // 2 + 1
        if kind in ('open', 'big', 'wide', 'small'):
            self.g(xl, y, shift_pupil(rows, look))
            self.g(xr, y, shift_pupil(rows, look))
        else:
            self.g(xl, y, rows)
            self.g(xr, y, rows, flip=True)

    def brows(self, y, kind='flat', dx=6, color=DARK):
        rows = BROWS[kind]
        w = len(rows[0])
        xl = self.cx - dx - w // 2 - 1
        xr = self.cx + dx - w // 2 + 1
        self.g(xl, y, rows, h=color)
        self.g(xr, y, rows, flip=True, h=color)

    def nose(self, y, kind='round'):
        rows = NOSES[kind]
        w = len(rows[0])
        self.g(self.cx - w // 2, y, rows, o=self.skin, s=SHADOW[self.skin])

    def bulb(self, x, y, rx, ry, nostril=True):
        """Nez en bulbe : ellipse de peau, contour d'encre en bas et à droite seulement."""
        c = self.c
        m = ellipse(x, y, rx, ry)
        c.fill(m, self.skin)
        inner = m - border(m)
        c.shade(inner, SHADOW[self.skin], -1, -1)
        for (px, py) in border(m):
            if py + 0.5 > y - ry * 0.2 or px + 0.5 > x + rx * 0.4:
                c.put(px, py, INK)
        if nostril:
            c.put(int(x - rx * 0.5), int(y + ry * 0.3), INK)
            c.put(int(x + rx * 0.5) - 1, int(y + ry * 0.3), INK)
        return m

    def mouth(self, y, kind='smile'):
        rows = MOUTHS[kind]
        w = len(rows[0])
        self.g(self.cx - w // 2, y, rows, o=self.skin, s=SHADOW[self.skin])

    def cheeks(self, y, dx=10, w=2):
        for x in (self.cx - dx - w, self.cx + dx):
            for i in range(w):
                self.c.put(x + i, y, PINK)

    def frame(self):
        c = self.c
        for x in range(S):
            c.put(x, 0, INK); c.put(x, S - 1, INK)
        for y in range(S):
            c.put(0, y, INK); c.put(S - 1, y, INK)


def shift_pupil(rows, look):
    """Regard vers la gauche : œil en miroir (la pupille est dessinée à droite)."""
    return [r[::-1] for r in rows] if look < 0 else rows


# Yeux (œil gauche à l'écran ; l'œil droit est le même ou son miroir)
EYES = {
    'open': [".kkk.",
             "kwwwk",
             "kwwkk",
             "kwkkk",
             ".kkk."],
    'big': [".kkkk.",
            "kwwwwk",
            "kwwwkk",
            "kwwkxk",
            "kwwkkk",
            ".kkkk."],
    'small': ["kkk",
              "wkk",
              ".k."],
    'happy': [".kkk.",
              "k...k",
              "....."],
    'closed': ["kkkkk",
               ".kkk."],
    'half': ["kkkkk",
             "kwkkk",
             ".kkk."],
    'sly': ["kkkkk",
            "wwkkk",
            ".kkk."],
    'dots': [".kk",
             "kkk",
             "kk."],
    'stern': ["kkkkkk",
              ".wwkkk",
              "..kkk."],
}

BROWS = {
    'flat': ["hhhhh"],
    'thick': ["hhhhh.",
              "hhhhhh"],
    'arch': [".hhh.",
             "h...h"],
    'up': ["...hh",
           ".hhh.",
           "hh..."],
    'angry': ["hh...",
              ".hhh.",
              "...hh"],
    'worried': ["...hh",
                "hhh.."],
    'bushy': [".hhhhh",
              "hhhhhh",
              "h.h.h."],
    'thin_arch': [".hhh..",
                  "h...h."],
}

NOSES = {
    'round': ["..k...",
              "..sk..",
              ".ooosk",
              "ksooos",
              ".kkkk."],
    'big': ["..k....",
            "..sk...",
            "..osk..",
            ".oooosk",
            "koooooo",
            "ksoooos",
            ".kkkkk."],
    'long': ["..k..",
             "..sk.",
             "..ok.",
             "..osk",
             "..ook",
             "kooos",
             ".kkk."],
    'small': [".k.",
              ".sk",
              "kk."],
    'button': [".sk.",
               "kook",
               ".kk."],
    'hook': ["..k...",
             "..ok..",
             "..ook.",
             "..ooks",
             ".ooook",
             "ksoss.",
             ".kk..."],
}

MOUTHS = {
    'smile': ["k........k",
              ".kkkkkkkk.",
              "..kwwwwk..",
              "...kkkk..."],
    'grin': ["k..........k",
             "kkkkkkkkkkkk",
             ".kwwwwwwwwk.",
             "..kwwwwwwk..",
             "...kppppk...",
             "....kkkk...."],
    'big_grin': ["kk..........kk",
                 ".kkkkkkkkkkkk.",
                 ".kwwwwwwwwwwk.",
                 "..kwwwwwwwwk..",
                 "..kdpppppddk..",
                 "...kkpppkkk...",
                 ".....kkk......"],
    'shout': ["...kkkkk...",
              "..kwwwwwk..",
              ".kdddddddk.",
              ".kdddddddk.",
              ".kddpppddk.",
              "..kpppppk..",
              "...kkkkk..."],
    'smirk': ["........k",
              ".kkkkkkk.",
              "...sss..."],
    'flat': ["kkkkkkk"],
    'wavy': [".kk...kk.",
             "k..kkk..k"],
    'lips': ["..kkkkk..",
             ".kppkppk.",
             "kpppppppk",
             ".kpppppk.",
             "..kkkkk.."],
    'lips_smile': ["k.........k",
                   ".kkkkkkkkk.",
                   ".kpwwwwwpk.",
                   "..kpppppk..",
                   "...kkkkk..."],
    'o': [".kkk.",
          "kdddk",
          "kdpdk",
          ".kkk."],
    'small_smile': ["k.....k",
                    ".kkkkk."],
    'frown': [".kkkkk.",
              "k.....k"],
}


# --- Accessoires -----------------------------------------------------------------

def curls(cx, cy, rx, ry, r=3.2, n=14, a0=180, a1=360):
    """Chevelure bouclée : chapelet de petits cercles le long d'un arc d'ellipse."""
    import math
    m = set()
    for i in range(n + 1):
        a = math.radians(a0 + (a1 - a0) * i / n)
        m |= ellipse(cx + rx * math.cos(a), cy + ry * math.sin(a), r, r)
    return m


def glasses(f, y, kind='rect', dx=6, h=5, w=7, color=INK, lens=None):
    """Lunettes autour des yeux (y = haut des verres)."""
    c = f.c
    xl = f.cx - dx - w // 2 - 1
    xr = f.cx + dx - w // 2 + 1
    for x0 in (xl, xr):
        if kind == 'round':
            m = ellipse(x0 + w / 2, y + h / 2, w / 2, h / 2)
        else:
            m = rect(x0, y, x0 + w - 1, y + h - 1)
        if lens is not None:
            inner = m - border(m)
            for (px, py) in inner:
                if c.get(px, py) == f.skin or c.get(px, py) == SHADOW[f.skin]:
                    c.put(px, py, lens)
        c.fill(border(m), color)
        c.put(x0 + 1, y + 1, WHITE)   # reflet
    c.fill(line(xl + w, y + 1, xr - 1, y + 1), color)          # pont
    c.fill(line(xl - 1, y + 1, xl - 3, y + 1), color)          # branches
    c.fill(line(xr + w, y + 1, xr + w + 2, y + 1), color)


def curly(f, centers, r, color):
    """Cheveux bouclés : boucles peintes une à une (contour d'encre), reflet en haut à gauche."""
    allm = set()
    for (x, y) in centers:
        allm |= ellipse(x, y, r, r)
    f.c.paint(allm & FRAME, color)
    for (x, y) in centers:
        m = ellipse(x, y, r, r) & FRAME
        f.c.fill(arc(x, y, r - 0.5, r - 0.5, 20, 160) & m, INK)
        f.c.put(int(x - r / 2), int(y - r / 2), CHEST)
        f.c.put(int(x - r / 2) + 1, int(y - r / 2), CHEST)


def stubble(f, m, density=5):
    """Barbe naissante : tramage d'ombre de peau."""
    f.c.dither({p for p in m if f.c.get(*p) == f.skin}, f.skin, SHADOW[f.skin], density)


# --- Personnages -----------------------------------------------------------------
# Tête par défaut : cx 28, cy 23, rx 16, ry 18 -> front y 5, menton y 41.

def pres_gauche():
    """Le président (orientation gauche) : boucles brunes, barbe courte, col ouvert."""
    f = Face(BG, LGREY)
    f.neck(top=36, bottom=44)
    f.body(NAVY, WHITE, None, top=43, tie_kind=None, vw=6, vdepth=9)
    f.make_head(rx=15, ry=18, cy=23, jaw=(14, 10, 30), ear_y=25)
    # barbe courte qui suit la mâchoire
    beard = (ellipse(28, 30, 15, 12) - ellipse(28, 24, 13, 13)) & f.head
    beard |= ellipse(28, 35, 6, 3) & f.head
    f.c.fill(beard - border(f.head), DARK)
    f.c.fill(ellipse(28, 35.5, 5, 2) & f.head, SKIN)
    curly(f, [(14, 20), (14, 14), (18, 9), (24, 6), (31, 6), (37, 8), (41, 13), (42, 19),
              (21, 12), (28, 10), (35, 12)], 4.2, DARK)
    f.brows(16, 'thick')
    f.eyes(19, 'big')
    f.c.fill(line(27, 21, 26, 26), SKSH)
    f.bulb(28, 28, 3.5, 3)
    f.mouth(32, 'grin')
    f.cheeks(29, dx=10)
    f.frame()
    return f.c


def pres_centre():
    """Le président (orientation centre) : raie sur le côté, cravate bleue, sourire de pub."""
    f = Face(BG, LGREY)
    f.neck(top=36, bottom=44)
    f.body(NAVY, WHITE, BLUE, top=43)
    f.make_head(rx=16, ry=18, cy=23, jaw=(14, 10, 31), ear_y=26)
    hm = ellipse(28, 16, 17, 11) - rect(0, 17, 55, 55)
    hm |= ellipse(13, 19, 3, 7) | ellipse(43, 19, 3, 7)
    hm -= ellipse(34, 18, 10, 5)
    hm |= ellipse(22, 9, 9, 5)   # mèche
    f.hair(hm, CHEST)
    f.c.fill(line(19, 9, 25, 7), YELLOW)
    f.brows(17, 'thick')
    f.eyes(20, 'big')
    f.c.fill(line(27, 21, 26, 27), SKSH)
    f.bulb(28, 29, 4, 3)
    f.mouth(33, 'big_grin')
    f.cheeks(30, dx=11)
    f.frame()
    return f.c


def pres_droite():
    """Le président (orientation droite) : cheveux gris plaqués, mâchoire carrée, cravate rouge."""
    f = Face(BG, LGREY)
    f.neck(w=12, top=36, bottom=44)
    f.body(NAVY, WHITE, RED, top=43)
    f.c.paint(rect(39, 47, 41, 49), WHITE)             # pin's
    f.c.put(40, 48, BLUE)
    f.make_head(rx=16, ry=17, cy=23, jaw=(16, 11, 30), ear_y=25)
    hm = ellipse(28, 15, 17, 10) - rect(0, 16, 55, 55)
    hm |= ellipse(12.5, 18, 3, 6) | ellipse(43.5, 18, 3, 6)
    hm -= ellipse(28, 17, 12, 6)                      # tempes dégarnies
    hm |= ellipse(28, 9, 12, 4)
    f.hair(hm, LGREY)
    for x in range(17, 40, 4):                        # cheveux peignés en arrière
        f.c.fill(line(x, 7, x + 2, 11) & hm - border(hm), GREY)
    f.brows(16, 'bushy', color=GREY)
    f.eyes(19, 'stern')
    f.c.fill(line(27, 21, 27, 26), SKSH)
    f.bulb(28, 28, 4, 3)
    f.mouth(33, 'smirk')
    f.c.fill(line(20, 31, 22, 34), SKSH); f.c.fill(line(36, 31, 34, 34), SKSH)   # plis
    f.c.fill(arc(28, 39, 4, 1.5, 20, 160), SKSH)      # fossette du menton
    f.frame()
    return f.c


def pm_technocrate():
    """Pierre Delaunay : énarque, crâne dégarni, lunettes rectangulaires, costume gris."""
    f = Face(BG, LGREY)
    f.neck(w=8, top=36, bottom=44)
    f.body(GREY, WHITE, NAVY, top=43, wide=25)
    f.make_head(rx=13, ry=19, cy=23, jaw=(11, 9, 32), ear_y=24)
    hm = (ellipse(16, 18, 4, 7) | ellipse(40, 18, 4, 7)) - rect(0, 24, 55, 55)
    f.hair(hm, GREY)
    f.c.fill(arc(28, 13, 7, 3, 200, 340), SKSH)       # rides du front
    f.c.fill(arc(28, 9, 3, 2, 200, 340), WHITE)       # reflet du crâne
    f.brows(17, 'flat', color=GREY)
    f.eyes(20, 'small', dx=6)
    glasses(f, 19, 'rect', w=8, h=5)
    f.c.fill(line(28, 23, 28, 29), SKSH)
    f.nose(26, 'long')
    f.mouth(35, 'flat')
    f.c.put(24, 35, SKSH); f.c.put(32, 35, SKSH)
    f.frame()
    return f.c


def pm_rassembleur():
    """Marianne Deschamps : rassembleuse, carré blond volumineux, veste rouge, collier."""
    f = Face(BG, LGREY)
    back = ellipse(28, 24, 20, 19) & rect(0, 0, 55, 40)
    f.hair(back, YELLOW)
    f.neck(w=9, top=36, bottom=46)
    f.body(RED, None, None, top=45, wide=24, tie_kind=None)
    v = poly([(22, 43), (34, 43), (28, 51)])
    f.c.paint(v, SKIN, edge=False)
    for x in range(22, 35, 2):                        # collier de perles
        y = 45 + (abs(x - 28) < 3) + (abs(x - 28) < 6)
        f.c.put(x, y, WHITE)
    f.make_head(rx=14, ry=17, cy=24, jaw=(12, 9, 31), ears=False)
    hm = ellipse(28, 14, 17, 10) | ellipse(13, 24, 5, 13) | ellipse(43, 24, 5, 13)
    hm -= ellipse(30, 20, 11, 8) & rect(0, 13, 55, 55)
    hm &= rect(0, 0, 55, 40)
    f.hair(hm, YELLOW)
    f.c.fill(arc(26, 12, 8, 4, 190, 300), WHITE)      # reflet
    f.brows(17, 'thin_arch', color=CHEST)
    f.eyes(20, 'happy')
    f.c.put(20, 19, INK); f.c.put(36, 19, INK)        # cils
    f.c.fill(line(27, 22, 26, 27), SKSH)
    f.bulb(28, 28, 3, 2.5, nostril=False)
    f.mouth(32, 'lips_smile')
    f.cheeks(28, dx=9, w=3)
    for (x, y) in ((14, 34), (42, 34)):               # boucles d'oreilles
        f.c.paint(ellipse(x + 0.5, y + 0.5, 1.6, 1.6), YELLOW)
    f.frame()
    return f.c


def pm_reformateur():
    """Julien Maréchal : jeune, pressé, banane gominée, goutte de sueur, sans cravate."""
    f = Face(BG, LGREY)
    f.neck(w=9, top=36, bottom=44)
    f.body(BLUE, WHITE, None, top=43, tie_kind=None, wide=24, vw=6, vdepth=8)
    f.make_head(rx=14, ry=17, cy=24, jaw=(12, 9, 31), ear_y=25)
    hm = ellipse(28, 15, 15, 9) - rect(0, 15, 55, 55)
    hm |= poly([(16, 12), (22, 2), (36, 1), (42, 6), (40, 12)])   # banane
    hm |= ellipse(14.5, 17, 2.5, 5) | ellipse(41.5, 17, 2.5, 5)
    f.hair(hm, DARK)
    f.c.fill(line(23, 3, 33, 2), CHEST); f.c.fill(line(21, 6, 27, 4), CHEST)
    f.brows(16, 'up')
    f.eyes(19, 'big', look=1)
    f.c.fill(line(27, 21, 26, 26), SKSH)
    f.bulb(28, 27, 3, 2.5)
    f.mouth(32, 'grin')
    # goutte de sueur
    f.g(46, 10, ["..k...",
                "..k...",
                ".kbk..",
                ".kbk..",
                "kwbbk.",
                "kwbbk.",
                "kbbnk.",
                ".kkk.."])
    f.c.fill(line(47, 22, 51, 22) | line(46, 25, 51, 25) | line(47, 28, 51, 28), WHITE)   # traits de vitesse
    f.frame()
    return f.c


def cohab_gauche():
    """Sophie Carrel : ancienne syndicaliste, cheveux courts poivre et sel, foulard rouge."""
    f = Face(BG, LGREY)
    f.neck(w=9, top=36, bottom=44)
    f.body(NAVY, None, None, top=44, wide=24, tie_kind=None)
    scarf = ellipse(28, 45, 11, 4) | poly([(30, 45), (36, 44), (38, 55), (32, 55)])
    f.c.paint(scarf & FRAME, RED, edge=False)
    f.c.fill(line(33, 48, 35, 54), DARK)
    f.make_head(rx=14, ry=17, cy=24, jaw=(12, 9, 31), ear_y=25)
    hm = ellipse(28, 16, 16, 11) - rect(0, 17, 55, 55)
    hm |= ellipse(14, 20, 3, 6) | ellipse(42, 20, 3, 5)
    hm -= poly([(18, 17), (24, 9), (30, 12), (36, 9), (40, 17)])   # frange en pointes
    hm |= poly([(18, 12), (24, 9), (22, 15)]) | poly([(30, 11), (36, 9), (34, 14)])
    f.hair(hm, LGREY)
    f.c.dither(hm - border(hm), LGREY, GREY, 5)
    f.brows(17, 'angry', color=GREY)
    f.eyes(20, 'open')
    f.c.fill(line(27, 22, 26, 27), SKSH)
    f.bulb(28, 28, 3, 2.5)
    f.mouth(33, 'smirk')
    f.c.put(21, 30, SKSH); f.c.put(35, 30, SKSH)
    f.frame()
    return f.c


def cohab_centre():
    """Bernard Lacombe : gestionnaire prudent, chauve à couronne grise, moustache, inquiet."""
    f = Face(BG, LGREY)
    f.neck(w=12, top=36, bottom=44)
    f.body(LGREY, WHITE, GREY, top=43)
    f.make_head(rx=16, ry=17, cy=24, jaw=(15, 11, 30), ear_y=25)
    hm = (ellipse(14, 22, 4, 7) | ellipse(42, 22, 4, 7)) - rect(0, 27, 55, 55)
    f.hair(hm, LGREY)
    f.c.fill(arc(28, 10, 4, 2, 200, 340), WHITE)
    f.c.fill(arc(28, 14, 6, 2, 200, 340), SKSH)
    f.brows(16, 'worried', color=GREY)
    f.eyes(19, 'open', look=-1)
    glasses(f, 18, 'round', w=8, h=7)
    f.c.fill(line(27, 23, 26, 26), SKSH)
    f.bulb(28, 28, 4, 3)
    f.g(21, 31, [".kkkkk.kkkkk.",
                 "klllllkllllllk",
                 "kgllgkkkglllgk",
                 ".kk.......kk."])
    f.mouth(35, 'wavy')
    f.double_chin(42, 6)
    f.frame()
    return f.c


def cohab_droite():
    """Philippe de Montclar : notable libéral, nœud papillon, monocle, menton conquérant."""
    f = Face(BG, LGREY)
    f.neck(w=10, top=36, bottom=44)
    f.body(NAVY, WHITE, RED, top=43, tie_kind='bow', vw=6)
    f.c.paint(poly([(9, 49), (14, 47), (14, 50), (9, 51)]), WHITE)   # pochette
    f.make_head(rx=14, ry=18, cy=22, jaw=(13, 11, 30), chin=ellipse(28, 38, 7, 4), ear_y=24)
    hm = ellipse(28, 14, 16, 10) - rect(0, 15, 55, 55)
    hm |= ellipse(14, 17, 3, 6) | ellipse(42, 17, 3, 6)
    f.hair(hm, GREY)
    for x in range(16, 40, 3):
        f.c.put(x, 8, LGREY); f.c.put(x + 1, 9, LGREY)
    f.c.fill(line(18, 6, 36, 5), LGREY)
    f.brows(16, 'arch', color=GREY)
    f.eyes(19, 'sly')
    f.c.paint(ellipse(35, 20.5, 4.5, 4.5) - ellipse(35, 20.5, 3.5, 3.5), YELLOW)   # monocle
    f.c.put(33, 18, WHITE)
    f.c.fill(polyline([(38, 24), (40, 30), (41, 40)]), YELLOW)
    f.c.fill(line(27, 21, 26, 27), SKSH)
    f.bulb(27.5, 28, 3.5, 3)
    f.g(21, 31, ["kk.........kk",
                 "kgkkk...kkkgk",
                 ".kgggk.kgggk.",
                 "..kkk...kkk.."])                     # moustache en guidon
    f.mouth(35, 'smirk')
    f.frame()
    return f.c


def cand_roussel():
    """Camille Roussel : ancienne inspectrice du travail, coupe courte rousse, harangue."""
    f = Face(BG, LGREY)
    f.neck(w=9, top=36, bottom=44)
    f.body(RED, None, None, top=44, wide=25, tie_kind=None)
    col = poly([(19, 43), (37, 43), (34, 48), (22, 48)])
    f.c.paint(col & FRAME, DARK)
    f.c.fill(rect(24, 49, 25, 55), WHITE); f.c.fill(rect(31, 49, 32, 55), WHITE)   # cordons
    f.make_head(rx=14, ry=17, cy=24, jaw=(12, 9, 31), ear_y=25)
    hm = ellipse(28, 15, 16, 10) - rect(0, 16, 55, 55)
    hm |= ellipse(14, 19, 3.5, 7) | ellipse(42, 19, 3.5, 6)
    hm |= poly([(12, 12), (16, 2), (22, 7), (26, 0), (31, 6), (37, 1), (39, 8), (45, 6), (43, 14)])
    f.hair(hm, CHEST)
    f.c.fill(line(20, 8, 24, 5), YELLOW); f.c.fill(line(29, 6, 33, 4), YELLOW)
    f.brows(16, 'angry', color=DARK)
    f.eyes(19, 'open', look=1)
    glasses(f, 18, 'rect', w=7, h=5, color=RED)
    f.c.fill(line(27, 22, 26, 26), SKSH)
    f.bulb(28, 27, 3, 2.5)
    f.mouth(31, 'shout')
    f.frame()
    return f.c


def cand_vasseur():
    """Noémie Vasseur : climatologue, casque de vélo vert, tresse blonde, taches de rousseur."""
    f = Face(BG, LGREY)
    f.neck(w=9, top=36, bottom=44)
    f.body(GREEN, None, None, top=44, wide=24, tie_kind=None)
    f.c.paint(poly([(23, 43), (33, 43), (28, 52)]), WHITE, edge=False)
    f.c.fill(line(28, 47, 28, 55), INK)                # fermeture éclair
    for i in range(6):                                  # tresse sur l'épaule
        f.c.paint(ellipse(41 - i * 0.6, 34 + i * 3.4, 3, 2.4), YELLOW)
        f.c.put(int(40 - i * 0.6), int(33 + i * 3.4), WHITE)
    f.c.paint(rect(37, 54, 41, 55), RED, edge=False)
    f.c.paint(rect(12, 48, 16, 52), YELLOW)            # badge
    f.c.fill(rect(13, 49, 15, 51), GREEN)
    f.make_head(rx=14, ry=17, cy=25, jaw=(12, 9, 32), ear_y=26)
    hm = ellipse(28, 18, 15, 9) - rect(0, 19, 55, 55)
    hm |= ellipse(14.5, 22, 2.5, 6) | ellipse(41.5, 22, 2.5, 6)
    f.hair(hm, YELLOW)
    helmet = ellipse(28, 15, 17, 12) - rect(0, 15, 55, 55)
    f.c.paint(helmet, GREEN)
    for x in (20, 27, 34):                             # aérations
        f.c.paint(rect(x, 7, x + 2, 11), DARK, ink=None)
    f.c.fill(line(17, 8, 22, 5), WHITE)
    f.c.fill(line(15, 22, 15, 33) | line(41, 22, 41, 33), INK)   # sangles
    f.brows(19, 'arch', color=CHEST)
    f.eyes(22, 'open')
    glasses(f, 21, 'round', w=7, h=6, color=CHEST)
    for (x, y) in ((20, 29), (22, 30), (19, 31), (35, 29), (33, 30), (36, 31)):
        f.c.put(x, y, CHEST)
    f.bulb(28, 29, 2.5, 2, nostril=False)
    f.mouth(33, 'smile')
    f.frame()
    return f.c


def cand_valmont():
    """Édouard Valmont : notable de province, visage rond, double menton, sourire de banquier."""
    f = Face(BG, LGREY)
    f.neck(w=14, top=38, bottom=45)
    f.body(NAVY, WHITE, YELLOW, top=44, wide=28)
    for x in range(4, 52, 5):                          # fines rayures
        f.c.fill({(x + (y - 44) // 6, y) for y in range(44, 56)} & f.torso - border(f.torso), GREY)
    f.make_head(rx=17, ry=17, cy=24, jaw=(18, 12, 30), ear_y=24)
    hm = (ellipse(13, 18, 4, 6) | ellipse(43, 18, 4, 6)) - rect(0, 23, 55, 55)
    f.hair(hm, GREY)
    f.c.fill(polyline([(15, 12), (22, 8), (32, 7), (40, 9)]), GREY)   # mèche rabattue
    f.c.fill(polyline([(16, 13), (24, 10), (34, 9), (40, 11)]), GREY)
    f.brows(17, 'arch', color=GREY)
    f.eyes(20, 'happy')
    f.nose(24, 'button')
    f.cheeks(27, dx=11, w=4)
    f.cheeks(28, dx=11, w=3)
    f.mouth(30, 'grin')
    f.c.fill(line(28, 33, 28, 33), INK)
    f.c.fill(arc(28, 38.5, 9, 2.5, 15, 165), INK)     # double menton
    f.frame()
    return f.c


def cand_lenoir():
    """Gaspard Lenoir : tribun de plateau télé, cheveux noirs gominés, bouche grande ouverte."""
    f = Face(BG, LGREY)
    f.neck(w=12, top=36, bottom=44)
    f.body(DARK, WHITE, NAVY, top=43, wide=27)
    f.make_head(rx=15, ry=17, cy=23, jaw=(15, 11, 30), ear_y=24)
    hm = ellipse(28, 11, 17, 8) - rect(0, 12, 55, 55)
    hm |= poly([(13, 11), (17, 2), (26, -1), (40, 0), (45, 9), (42, 12)])
    hm |= ellipse(13.5, 15, 2.5, 6) | ellipse(42.5, 15, 2.5, 6)
    f.hair(hm, INK, ink=INK, shade=False)
    f.c.fill(line(18, 6, 26, 3), GREY); f.c.fill(line(28, 3, 36, 4), GREY)   # gomina
    f.c.fill(line(20, 8, 24, 6), GREY)
    f.c.fill(thick(line(18, 14, 24, 17), 0) | shift(line(18, 14, 24, 17), 0, 1), INK)
    f.c.fill(line(38, 14, 32, 17) | shift(line(38, 14, 32, 17), 0, 1), INK)
    f.eyes(19, 'open')
    f.c.fill(line(27, 21, 27, 25), SKSH)
    f.bulb(28, 26, 3.5, 2.5)
    f.mouth(29, 'shout')
    f.c.fill(line(18, 29, 20, 35), SKSH); f.c.fill(line(38, 29, 36, 35), SKSH)
    f.frame()
    return f.c


def cand_marchetti():
    """Hélène Marchetti : maire, chignon brun, écharpe tricolore, sourire conciliant."""
    f = Face(BG, LGREY)
    f.neck(w=9, top=36, bottom=46)
    f.body(LGREY, None, None, top=45, wide=24, tie_kind=None)
    f.c.paint(poly([(23, 43), (33, 43), (28, 50)]), SKIN, edge=False)
    f.c.fill(line(23, 44, 28, 50) | line(33, 44, 28, 50), INK)
    # écharpe tricolore en bandoulière (épaule gauche -> hanche droite)
    for col, o in ((BLUE, 0), (WHITE, 3), (RED, 6)):
        f.c.fill(poly([(10, 38 + o), (10, 41 + o), (56, 63 + o), (56, 60 + o)]) & f.torso, col)
    f.c.fill(line(10, 38, 56, 60) & f.torso, INK)
    f.c.fill(line(10, 47, 56, 69) & f.torso, INK)
    f.c.paint(ellipse(28, 5, 7, 5), DARK)             # chignon
    f.make_head(rx=14, ry=17, cy=24, jaw=(12, 9, 31), ears=True, ear_y=25)
    hm = ellipse(28, 16, 16, 10) - rect(0, 17, 55, 55)
    hm |= ellipse(14, 20, 3, 7) | ellipse(42, 20, 3, 7)
    hm -= poly([(22, 17), (30, 9), (40, 17)])          # raie et front
    f.hair(hm, DARK)
    f.c.fill(line(17, 11, 22, 8), CHEST); f.c.fill(line(24, 3, 28, 2), CHEST)
    f.brows(17, 'thin_arch', color=DARK)
    f.eyes(20, 'half')
    f.c.fill(line(27, 22, 26, 27), SKSH)
    f.bulb(28, 28, 3, 2.5, nostril=False)
    f.mouth(32, 'lips_smile')
    f.cheeks(29, dx=9)
    f.c.paint(ellipse(14, 31, 1.6, 1.6), YELLOW); f.c.paint(ellipse(42, 31, 1.6, 1.6), YELLOW)
    f.frame()
    return f.c


# Ordre = enum ArtId (ART_PRES_*, ART_PM_*, ART_COHAB_*, ART_CAND_*) : ne pas changer sans
# mettre à jour ui_art.h. Présidents et PM d'opposition : ordre ORIENT_* ; PM : ordre
# PM_* ; candidats : ordre CAND_* (sim/election.h).
PORTRAITS = [
    ("art_pres_gauche", pres_gauche),
    ("art_pres_centre", pres_centre),
    ("art_pres_droite", pres_droite),
    ("art_pm_technocrate", pm_technocrate),
    ("art_pm_rassembleur", pm_rassembleur),
    ("art_pm_reformateur", pm_reformateur),
    ("art_cohab_gauche", cohab_gauche),
    ("art_cohab_centre", cohab_centre),
    ("art_cohab_droite", cohab_droite),
    ("art_cand_roussel", cand_roussel),       # CAND_GAUCHE_RADICALE
    ("art_cand_vasseur", cand_vasseur),       # CAND_ECOLO
    ("art_cand_marchetti", cand_marchetti),   # CAND_CENTRE
    ("art_cand_valmont", cand_valmont),       # CAND_DROITE
    ("art_cand_lenoir", cand_lenoir),         # CAND_NATIONALISTE
]
