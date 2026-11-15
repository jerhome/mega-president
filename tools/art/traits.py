"""Traits du visage des gueules V3 : yeux, sourcils, bouches, rides, selon l'expression.

Expressions : 'neutre', 'content', 'furieux', 'gene' (gêné). Chaque fonction reçoit
le dessin (gueules.Draw) et des paramètres de forme propres au personnage ; les
expressions modifient ces paramètres (ouverture des yeux, angle des sourcils...).
"""
import math

from gueules import bez, ell_iter
from raster import line, polyline, poly

INK = ("ink", 0)

# Image d'animation en cours de rendu (tâche 3.18, visages animés) : None (expression
# telle quelle), "blink" (yeux fermés), "look" (iris sur le côté), "talk1" (bouche
# mi-ouverte), "talk2" (bouche ouverte). Positionné par make_art.py autour de P.render().
ANIM = None


def sk(t):
    return ("skin", t)


# --- yeux -----------------------------------------------------------------------

def eye(cv, cx, cy, w=9, h=4.0, side=-1, open_=1.0, tilt=0.0, raise_=0.0, look=0,
        iris=("iris", 0), iris_r=2.0, lashes=False, lid=1, bags=0, lidfold=True,
        closed_smile=False, white=("white", 1), white_sh=("white", 0), pupil=True,
        look_y=0, droop=0.0):
    """Œil en amande. side : -1 œil à gauche de l'écran, +1 à droite.
    tilt > 0 : coin intérieur abaissé (colère) ; < 0 : relevé (inquiétude).
    raise_ : paupière inférieure remontée (sourire). droop : paupière lourde (coin
    extérieur tombant). bags : cernes (0, 1, 2 lignes)."""
    if ANIM == "look":   # regard qui s'échappe : vers la droite, ou retour au centre
        look = look - 2 if look > 0 else look + (2 if w >= 7 else 1)
    if ANIM == "blink" and not closed_smile:
        _closed_eye(cv, cx, cy, w, h, side, lashes, lid, bags, raise_)
        return
    x0 = cx - w / 2.0
    top = {}
    bot = {}
    for i in range(w):
        t = (i + 0.5) / w
        # t = 0 au coin extérieur pour side -1 (gauche de l'écran)
        tin = t if side < 0 else 1 - t      # 0 = coin extérieur, 1 = coin intérieur
        s = math.sin(math.pi * t) ** 0.7
        ty = cy - h * open_ * s + tilt * (tin - 0.5) * 2 + droop * (1 - tin) * 1.5
        by = cy + h * 0.55 * s - raise_ * s
        top[i] = ty
        bot[i] = by
    if closed_smile:
        pts = []
        for i in range(w):
            t = (i + 0.5) / w
            pts.append((x0 + i + 0.5, cy + 1 - h * 0.9 * math.sin(math.pi * t) ** 0.8))
        cv.stroke(pts, INK)
        cv.stroke([(p[0], p[1] - 1) for p in pts[1:-1]], INK)
        return
    inside = set()
    for i in range(w):
        x = int(math.floor(x0 + i + 0.5))
        for y in range(int(math.floor(top[i])) + 1, int(math.ceil(bot[i])) + 1):
            if y + 0.5 > top[i] and y - 0.5 < bot[i]:
                inside.add((x, y))
    if not inside:
        return
    xs = sorted({p[0] for p in inside})
    # blanc de l'œil, ombré sous la paupière
    for (x, y) in inside:
        cv.put(x, y, white)
    for x in xs:
        col = sorted(y for (xx, y) in inside if xx == x)
        cv.put(x, col[0], white_sh)
    for x in (xs[0], xs[-1]):
        for y in [y for (xx, y) in inside if xx == x]:
            cv.put(x, y, white_sh)
    # iris + pupille
    icx = cx + look + 0.5 * side * 0
    icy = cy + 0.5 + look_y
    if iris_r > 0:
        for x, y, d in ell_iter(icx, icy, iris_r, iris_r + 0.4):
            if (x, y) in inside:
                cv.put(x, y, iris)
        if pupil:
            for x, y, d in ell_iter(icx, icy, iris_r * 0.55, iris_r * 0.65):
                if (x, y) in inside:
                    cv.put(x, y, INK)
            hx, hy = int(math.floor(icx - iris_r * 0.5)), int(math.floor(icy - iris_r * 0.6))
            if (hx, hy) in inside:
                cv.put(hx, hy, white)
    # paupière supérieure : trait d'encre épais
    lidpts = []
    for i in range(w):
        lidpts.append((x0 + i + 0.5, top[i]))
    cv.stroke([(a, math.floor(b + 0.5)) for a, b in lidpts], INK)
    if lid >= 2:
        cv.stroke([(a, math.floor(b + 0.5) - 1) for a, b in lidpts[1:-1]], INK)
    # coin extérieur prolongé (trait de caractère) / cils
    ox = xs[0] if side < 0 else xs[-1]
    oy = int(math.floor(top[0 if side < 0 else w - 1] + 0.5))
    cv.put(ox + side * -1 * -1 - (1 if side < 0 else -1), oy + 1, INK)
    if lashes:
        cv.put(ox - (2 if side < 0 else -2), oy, INK)
        cv.put(ox - (1 if side < 0 else -1), oy - 1, INK)
    # paupière inférieure : ton sombre de la peau
    for i in range(1, w - 1):
        x = int(math.floor(x0 + i + 0.5))
        y = int(math.ceil(bot[i])) + 1
        if cv.get(x, y) and cv.get(x, y)[0] == "skin":
            cv.put(x, y, sk(1))
    # pli de la paupière (au-dessus)
    if lidfold:
        for i in range(2, w - 2):
            x = int(math.floor(x0 + i + 0.5))
            y = int(math.floor(top[i] + 0.5)) - (3 if lid >= 2 else 2)
            if cv.get(x, y) and cv.get(x, y)[0] == "skin":
                cv.put(x, y, sk(1))
    # cernes
    for b in range(bags):
        for i in range(1 + b, w - 1 - b):
            x = int(math.floor(x0 + i + 0.5))
            y = int(math.ceil(bot[i])) + 2 + b * 2
            if cv.get(x, y) and cv.get(x, y)[0] == "skin":
                cv.put(x, y, sk(1 if b == 0 else 1))


def _closed_eye(cv, cx, cy, w, h, side, lashes, lid, bags, raise_):
    """Œil fermé (clignement) : paupière supérieure baissée, arc d'encre vers le bas."""
    x0 = cx - w / 2.0
    pts = []
    for i in range(w):
        t = (i + 0.5) / w
        s = math.sin(math.pi * t) ** 0.8
        pts.append((x0 + i + 0.5, cy + 0.5 + h * 0.3 * s - raise_ * 0.3 * s))
    m = cv.stroke([(a, math.floor(b + 0.5)) for a, b in pts], INK)
    if lid >= 2:
        cv.stroke([(a, math.floor(b + 0.5) - 1) for a, b in pts[1:-1]], INK)
    # paupière baissée : ombre douce au-dessus du trait (peau seulement)
    for i in range(1, w - 1):
        x = int(math.floor(x0 + i + 0.5))
        y = int(math.floor(pts[i][1] + 0.5)) - (2 if lid >= 2 else 1)
        k = cv.get(x, y)
        if k and k[0] == "skin" and (x, y) not in m:
            cv.put(x, y, sk(1))
    # coin extérieur et cils
    ox = int(math.floor(x0 + 0.5)) if side < 0 else int(math.floor(x0 + w - 0.5))
    oy = int(math.floor(pts[0 if side < 0 else w - 1][1] + 0.5))
    if lashes:
        cv.put(ox + (-1 if side < 0 else 1), oy + 1, INK)
        cv.put(ox + (1 if side < 0 else -1), oy + 2, INK)
    for b in range(bags):
        for i in range(1 + b, w - 1 - b):
            x = int(math.floor(x0 + i + 0.5))
            y = int(math.ceil(cy + h * 0.55 * math.sin(math.pi * (i + 0.5) / w) ** 0.7)) + 2 + b * 2
            k = cv.get(x, y)
            if k and k[0] == "skin":
                cv.put(x, y, sk(1))


def brow(cv, cx, cy, w=10, side=-1, inner_dy=0.0, outer_dy=0.0, arch=1.5, thick=2,
         key=INK, key2=None, taper=True):
    """Sourcil : de l'intérieur (près du nez) vers l'extérieur.
    inner_dy > 0 : bout intérieur baissé (colère) ; < 0 : relevé (inquiétude)."""
    sgn = side   # extérieur du côté de side
    xi = cx - sgn * w / 2.0
    xo = cx + sgn * w / 2.0
    p0 = (xi, cy + inner_dy)
    p2 = (xo, cy + outer_dy + 1)
    p1 = ((xi + xo) / 2, cy - arch + (inner_dy + outer_dy) / 2 - 1)
    pts = bez(p0, p1, p2, steps=int(w) * 2)
    m = set()
    n = len(pts)
    for k, (x, y) in enumerate(pts):
        t = k / (n - 1)
        th = thick if not taper else thick * (1.0 - 0.45 * t) + 0.3
        for dy in range(int(round(th))):
            m.add((int(math.floor(x)), int(math.floor(y)) + dy))
    cv.fill(m, key)
    if key2:   # reflet sur le haut du sourcil
        for (x, y) in m:
            if (x, y - 1) not in m and (x, y + 1) in m:
                cv.put(x, y, key2)
    return m


# --- bouches --------------------------------------------------------------------

def mouth(cv, cx, cy, w=12, kind="line", curve=0.0, open_=4, teeth=True, lip=None,
          lip_sh=None, inner=("ink", 0), tongue=None, skew=0.0, under=True, upper=None):
    """Bouche centrée en (cx, cy). kind : line, smile, grin, shout, grit, wavy, smirk, pout.
    curve > 0 : coins relevés."""
    if ANIM in ("talk1", "talk2"):
        kind, w, curve, open_, teeth, tongue = _talk(ANIM == "talk2", kind, w, curve, open_)
    hw = w / 2.0
    L = (cx - hw, cy - curve + skew)
    R = (cx + hw, cy - curve - skew)
    if kind == "line":
        cv.curve(L, (cx, cy + curve), R, INK)
        cv.put(int(L[0]) - 1, int(L[1]) - (1 if curve > 0.5 else (-1 if curve < -0.5 else 0)), sk(1))
        cv.put(int(R[0]) + 1, int(R[1]) - (1 if curve > 0.5 else (-1 if curve < -0.5 else 0)), sk(1))
        if lip:
            cv.curve((cx - hw * 0.6, cy + 1), (cx, cy + 2.5), (cx + hw * 0.6, cy + 1), lip)
        if under:
            cv.curve((cx - hw * 0.4, cy + 3), (cx, cy + 3.5), (cx + hw * 0.4, cy + 3), sk(1))
        return
    if kind == "wavy":
        pts = []
        for i in range(int(w) + 1):
            x = cx - hw + i
            pts.append((x, cy + 0.9 * math.sin(i * 1.3) + skew * (i / w - 0.5) * 2))
        cv.stroke(pts, INK)
        if under:
            cv.curve((cx - hw * 0.3, cy + 3), (cx, cy + 3.5), (cx + hw * 0.3, cy + 3), sk(1))
        return
    if kind == "smirk":
        cv.curve((cx - hw, cy + 0.5), (cx, cy + 1 + curve * 0.3), (cx + hw, cy - curve - 1), INK)
        cv.put(int(cx + hw) + 1, int(cy - curve - 2), sk(1))
        if under:
            cv.curve((cx - hw * 0.3, cy + 3), (cx, cy + 3.5), (cx + hw * 0.3, cy + 3), sk(1))
        return
    if kind == "pout":   # moue
        cv.curve((cx - hw, cy + 1), (cx, cy - 1.5), (cx + hw, cy + 1), INK)
        if under:
            cv.curve((cx - hw * 0.4, cy + 2), (cx, cy + 3), (cx + hw * 0.4, cy + 2), sk(1))
        return
    # bouches ouvertes : contour supérieur et inférieur
    if kind in ("smile", "grin"):
        top = bez(L, (cx, cy + (1 if kind == "smile" else 0)), R)
        bot = bez(L, (cx, cy + open_ * 2), R)
    elif kind == "shout":
        L = (cx - hw, cy + 1 + skew)
        R = (cx + hw, cy + 1 - skew)
        top = bez(L, (cx, cy - open_ * 0.9), R)
        bot = bez(L, (cx, cy + open_ * 2.2), R)
    elif kind == "grit":
        L = (cx - hw, cy + 1 + skew)
        R = (cx + hw, cy + 1 - skew)
        top = bez(L, (cx, cy - 2 - curve), R)
        bot = bez(L, (cx, cy + 4 - curve), R)
    else:
        raise ValueError(kind)
    area = set()
    tops = {}
    bots = {}
    for (x, y) in top:
        xi = int(math.floor(x))
        tops[xi] = min(tops.get(xi, 1e9), y)
    for (x, y) in bot:
        xi = int(math.floor(x))
        bots[xi] = max(bots.get(xi, -1e9), y)
    for xi in tops:
        if xi in bots:
            for y in range(int(math.floor(tops[xi])), int(math.ceil(bots[xi])) + 1):
                area.add((xi, y))
    cv.fill(area, inner)
    if teeth:
        tw = ("white", 1)
        ts = ("white", 0)
        if kind == "grit":
            for (x, y) in area:
                cv.put(x, y, tw)
            ymid = {}
            for xi in tops:
                if xi in bots:
                    ymid[xi] = int(round((tops[xi] + bots[xi]) / 2))
            for xi, y in ymid.items():
                cv.put(xi, y, INK)
            for xi in list(ymid)[1:-1]:
                if (xi - int(cx)) % 3 == 0:
                    for y in range(int(tops[xi]), int(bots[xi]) + 1):
                        cv.put(xi, y, ts)
        else:
            nrow = 2 if kind != "shout" else 2
            for xi in tops:
                if xi in bots:
                    y0 = int(math.floor(tops[xi])) + 1
                    span = int(math.ceil(bots[xi])) - y0
                    for k in range(min(nrow, max(0, span - 1))):
                        cv.put(xi, y0 + k, tw if k == 0 else ts)
            if kind == "shout":   # dents du bas
                for xi in list(bots)[2:-2]:
                    if xi in tops:
                        cv.put(xi, int(math.ceil(bots[xi])) - 1, ts)
        if tongue and kind in ("shout", "grin", "smile") and open_ >= 3:
            for x, y, d in ell_iter(cx + 0.5, max(bots.values()) - 0.5, hw * 0.45, 1.6):
                if (x, y) in area and cv.get(x, y) == inner:
                    cv.put(x, y, tongue)
    # contour d'encre
    edge = set()
    for (x, y) in area:
        for q in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
            if q not in area:
                edge.add((x, y))
                break
    cv.fill(edge, INK)
    if lip:   # lèvre inférieure colorée
        for xi in bots:
            if xi in tops and abs(xi - cx) < hw - 1:
                cv.put(xi, int(math.ceil(bots[xi])) + 1, lip)
    if lip_sh:
        for xi in bots:
            if xi in tops and abs(xi - cx) < hw - 2:
                cv.put(xi, int(math.ceil(bots[xi])) + 2, lip_sh)
    if under and kind != "grit":
        by = max(bots.values())
        cv.curve((cx - hw * 0.35, by + 2.5), (cx, by + 3), (cx + hw * 0.35, by + 2.5), sk(1))
    # fossettes aux coins
    for (x, y), s in ((L, -1), (R, 1)):
        cv.put(int(math.floor(x)) + s, int(math.floor(y)) - (1 if curve > 0 else 0), sk(1))


def _talk(wide, kind, w, curve, open_):
    """Bouche qui parle : mi-ouverte (wide False) ou ouverte, à partir de la bouche de
    l'expression. Renvoie (kind, w, curve, open_, teeth, tongue)."""
    if kind in ("smile", "grin"):
        o = open_ + 1.3 if wide else max(1.2, open_ * 0.5)
        return kind, w, curve, o, True, None
    if kind == "shout":
        o = open_ * 1.25 if wide else max(1.3, open_ * 0.55)
        return "shout", w, curve, o, True, None
    if kind == "grit":
        return "shout", w * 0.9, 0, (2.4 if wide else 1.4), True, None
    # bouches fermées (line, wavy, smirk, pout) : petite ouverture ovale
    c = max(-1.0, min(1.0, curve)) * 0.5
    return "smile", max(6, w * 0.85), c, (2.0 if wide else 1.2), wide, None


# --- rides, gouttes ----------------------------------------------------------------

def wrinkle(cv, pts, key=None, only=("skin",)):
    """Ride (polyligne) sur la peau seulement."""
    key = key or sk(1)
    m = polyline([(int(round(a)), int(round(b))) for a, b in pts])
    for (x, y) in m:
        k = cv.get(x, y)
        if k and k[0] in only:
            cv.put(x, y, key)


def fold(cv, p0, p1, p2, key=None):
    key = key or sk(1)
    pts = bez(p0, p1, p2)
    wrinkle(cv, pts, key)


def sweat(cv, x, y):
    cv.grid(x, y, ["..k..",
                   ".kwk.",
                   "kwwwk",
                   "kwwsk",
                   ".kkk."], {"k": INK, "w": ("white", 1), "s": ("white", 0)})


def blush(cv, x, y, w=5, key=None):
    """Rougeur gênée : hachures sur la joue."""
    key = key or sk(0)
    for i in range(0, w, 2):
        cv.put(x + i, y + 1, key)
        cv.put(x + i + 1, y, key)


def vein(cv, x, y, key=("acc", 0)):
    """Veine de colère (marque de BD) sur le front."""
    cv.grid(x, y, [".k.k.",
                   "k...k",
                   ".....",
                   "k...k",
                   ".k.k."], {"k": key})
