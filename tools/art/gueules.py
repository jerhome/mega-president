"""Moteur des « gueules » V3 (tâches 3.07 à 3.09) : portraits caricaturaux 80 x 96.

Principe (Python 3 seul, sans PIL) :
- Le personnage est SCULPTÉ en couches (fond, cheveux arrière, buste, cou, tête,
  barbe, cheveux avant, lunettes...). Chaque couche porte une carte de hauteur
  construite avec des ellipsoïdes (crâne, mâchoire, nez, joues, menton, double
  menton...) que l'on ajoute ou retranche, et des formes « gonflées » (polygones
  dont le bord est arrondi par une transformée de distance).
- L'éclairage est calculé à partir de la normale de chaque couche (lumière en
  haut à gauche), puis quantifié sur la rampe de la matière (3 ou 4 tons) :
  ombres et lumières franches, comme un pixel artiste le ferait à la main.
- Les couches hautes projettent une ombre portée sur les couches basses
  (cheveux sur le front, menton sur le cou, tête sur le col).
- Contour d'encre sur la silhouette et entre couches, nettoyé « pixel perfect »
  (suppression des coins en L), puis traits dessinés : yeux, sourcils, bouche,
  rides, cernes, selon l'expression.
- Palette : 15 couleurs + transparent par personnage (expressions comprises).

Une expression ne redessine que les traits du visage : make_art.py extrait le
rectangle de tuiles qui change (patch) pour l'afficher par-dessus le portrait neutre.
"""
import math

from raster import md, ellipse, poly, rect, line, polyline, border

W, H = 80, 96
EXPRS = ("neutre", "content", "furieux", "gene")

_L = (-0.5, -0.65, 1.0)
_n = math.sqrt(sum(v * v for v in _L))
LIGHT = tuple(v / _n for v in _L)

# Seuils d'intensité (lumière . normale) par longueur de rampe (sombre -> clair)
THRESH = {
    1: (),
    2: (0.45,),
    3: (0.48, 0.86),
    4: (0.16, 0.52, 0.88),
    5: (0.22, 0.42, 0.62, 0.88),
}


def tone_of(i, n):
    t = 0
    for th in THRESH[n]:
        if i >= th:
            t += 1
    return t


def ell_iter(cx, cy, rx, ry):
    for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
        for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
            dx = (x + 0.5 - cx) / rx
            dy = (y + 0.5 - cy) / ry
            d = dx * dx + dy * dy
            if d <= 1.0:
                yield x, y, d


def dist_inside(mask):
    """Distance (chanfrein 1/1.4) au bord extérieur, pour chaque pixel du masque."""
    INF = 1e9
    d = {p: INF for p in mask}
    for (x, y) in mask:
        for n in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
            if n not in mask:
                d[(x, y)] = 1.0
                break
    pts = sorted(mask, key=lambda p: (p[1], p[0]))
    for (x, y) in pts:
        v = d[(x, y)]
        for (dx, dy, c) in ((-1, 0, 1), (0, -1, 1), (-1, -1, 1.4), (1, -1, 1.4)):
            q = (x + dx, y + dy)
            if q in d and d[q] + c < v:
                v = d[q] + c
        d[(x, y)] = v
    for (x, y) in reversed(pts):
        v = d[(x, y)]
        for (dx, dy, c) in ((1, 0, 1), (0, 1, 1), (1, 1, 1.4), (-1, 1, 1.4)):
            q = (x + dx, y + dy)
            if q in d and d[q] + c < v:
                v = d[q] + c
        d[(x, y)] = v
    return d


class Layer:
    def __init__(self, mat, cast=None, ink=True, flat=None, gain=1.0, bias=0.0, smooth=2,
                 ink_dirs="rd"):
        self.mat = mat          # nom de la rampe
        self.ink_dirs = ink_dirs  # ink="shadow" : côtés encrés contre la même matière (r, l, d, u)
        self.z = {}             # (x, y) -> hauteur
        self.cast = cast        # (dx, dy) : ombre portée sur les couches dessous
        self.ink = ink          # contour d'encre contre les couches dessous
        self.flat = flat        # ton forcé (indice de rampe) : pas d'éclairage
        self.gain = gain        # exagère le relief (pente)
        self.bias = bias        # éclaircit (+) ou assombrit (-) toute la couche
        self.smooth = smooth    # passes de lissage du relief (évite les taches)

    def smoothed(self):
        z = self.z
        for _ in range(self.smooth):
            nz = {}
            for (x, y), v in z.items():
                acc, n = 0.0, 0
                for dx in (-1, 0, 1):
                    for dy in (-1, 0, 1):
                        w = z.get((x + dx, y + dy))
                        if w is not None:
                            k = 2 if (dx == 0 and dy == 0) else 1
                            acc += w * k
                            n += k
                nz[(x, y)] = acc / n
            z = nz
        return z

    # --- volumes --------------------------------------------------------------
    def ell(self, cx, cy, rx, ry, h=None, mode="max"):
        """Ellipsoïde : 'max' (union), 'add' (bosse sur l'existant), 'new' (bosse qui
        étend la couche), 'sub' (creux sur l'existant)."""
        if h is None:
            h = min(rx, ry)
        for x, y, d in ell_iter(cx, cy, rx, ry):
            v = h * math.sqrt(1.0 - d)
            p = (x, y)
            if mode == "max":
                self.z[p] = max(self.z.get(p, -1e9), v)
            elif mode == "add":
                if p in self.z:
                    self.z[p] += v
            elif mode == "new":
                self.z[p] = self.z.get(p, 0.0) + v
            elif mode == "sub":
                if p in self.z:
                    self.z[p] -= v
        return self

    def shape(self, mask, h=4.0, r=4.0, base=0.0, mode="max"):
        """Forme gonflée : bord arrondi sur r pixels, plateau de hauteur h."""
        d = dist_inside(mask)
        for p, dd in d.items():
            t = min(dd, r) / r
            v = base + h * math.sqrt(max(0.0, 1.0 - (1.0 - t) ** 2))
            if mode == "max":
                self.z[p] = max(self.z.get(p, -1e9), v)
            elif mode == "add" and p in self.z:
                self.z[p] += v
        return self

    def tilt(self, ax=0.0, ay=0.0):
        """Incline toute la couche (pente ajoutée) : ax > 0 assombrit vers la droite."""
        for (x, y) in self.z:
            self.z[(x, y)] -= ax * x + ay * y
        return self

    def cut(self, mask):
        for p in mask:
            self.z.pop(p, None)
        return self

    def clip(self, mask):
        for p in list(self.z):
            if p not in mask:
                del self.z[p]
        return self

    def mask(self):
        return set(self.z)


class Portrait:
    def __init__(self, ramps, bg="bg", w=W, h=H):
        """ramps : nom -> liste de couleurs MD (r, g, b) du plus sombre au plus clair.
        'ink' et 'bg' obligatoires."""
        self.ramps = ramps
        self.w, self.h = w, h
        self.layers = []
        self.bgname = bg
        self.ops = []      # traits dessinés (rejoués pour chaque expression)

    def layer(self, mat, **kw):
        l = Layer(mat, **kw)
        self.layers.append(l)
        return l

    # --- rendu de base -----------------------------------------------------------
    def render_base(self, bg_draw=None):
        owner = [[-1] * self.w for _ in range(self.h)]
        for i, l in enumerate(self.layers):
            for (x, y) in l.z:
                if 0 <= x < self.w and 0 <= y < self.h:
                    owner[y][x] = i
        key = [[(self.bgname, len(self.ramps[self.bgname]) - 1)] * self.w for _ in range(self.h)]
        if bg_draw:
            bg_draw(key)
        tone = [[0] * self.w for _ in range(self.h)]
        zs = [l.smoothed() for l in self.layers]
        for y in range(self.h):
            for x in range(self.w):
                i = owner[y][x]
                if i < 0:
                    continue
                l = self.layers[i]
                n = len(self.ramps[l.mat])
                if l.flat is not None:
                    tone[y][x] = l.flat
                    continue
                z = zs[i]

                def gz(px, py):
                    return z.get((px, py))
                c = z[(x, y)]
                zl, zr, zu, zd = gz(x - 1, y), gz(x + 1, y), gz(x, y - 1), gz(x, y + 1)
                if zl is not None and zr is not None:
                    dx = (zr - zl) / 2
                elif zr is not None:
                    dx = zr - c
                elif zl is not None:
                    dx = c - zl
                else:
                    dx = 0
                if zu is not None and zd is not None:
                    dy = (zd - zu) / 2
                elif zd is not None:
                    dy = zd - c
                elif zu is not None:
                    dy = c - zu
                else:
                    dy = 0
                # bord de couche sans voisin : pente forte vers l'extérieur
                if zl is None and zr is not None:
                    dx = max(dx, 1.2)
                if zr is None and zl is not None:
                    dx = min(dx, -1.2)
                if zu is None and zd is not None:
                    dy = max(dy, 1.2)
                if zd is None and zu is not None:
                    dy = min(dy, -1.2)
                dx *= l.gain
                dy *= l.gain
                nx, ny, nz = -dx, -dy, 1.0
                nn = math.sqrt(nx * nx + ny * ny + nz * nz)
                it = (nx * LIGHT[0] + ny * LIGHT[1] + nz * LIGHT[2]) / nn + l.bias
                tone[y][x] = tone_of(it, n)
        # ombres portées
        for j, l in enumerate(self.layers):
            if not l.cast:
                continue
            sx, sy = l.cast
            for (x, y) in l.z:
                for k in range(1, 1 + max(abs(sx), abs(sy))):
                    px = x + round(sx * k / max(abs(sx), abs(sy)))
                    py = y + round(sy * k / max(abs(sx), abs(sy)))
                    if 0 <= px < self.w and 0 <= py < self.h and 0 <= owner[py][px] < j:
                        tone[py][px] = -1   # marqué : ombre
        for y in range(self.h):
            for x in range(self.w):
                i = owner[y][x]
                if i < 0:
                    continue
                l = self.layers[i]
                t = tone[y][x]
                if t == -1:   # ombre portée : ton juste au-dessus du plus sombre
                    t = 0 if len(self.ramps[l.mat]) <= 3 else 1
                key[y][x] = (l.mat, t)
        # encre
        ink = set()
        for y in range(self.h):
            for x in range(self.w):
                i = owner[y][x]
                if i < 0:
                    continue
                for (qx, qy) in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                    if not (0 <= qx < self.w and 0 <= qy < self.h):
                        continue
                    j = owner[qy][qx]
                    li = self.layers[i]
                    d = "r" if qx > x else ("l" if qx < x else ("d" if qy > y else "u"))
                    if (j < i and j >= 0 and li.ink == "shadow" and
                            d in li.ink_dirs and self.layers[j].mat == li.mat):
                        ink.add((x, y))
                        break
                    if j < 0 or (j < i and li.ink and li.ink != "shadow" and
                                 (li.ink == "always" or self.layers[j].mat != li.mat)) or \
                            (j < i and li.ink == "shadow" and self.layers[j].mat != li.mat):
                        ink.add((x, y))
                        break
        ink = clean_lines(ink)
        for (x, y) in ink:
            key[y][x] = ("ink", 0)
        self.owner = owner
        self.base = key
        return key

    def render(self, expr):
        key = [row[:] for row in self.base]
        cv = Draw(key, self)
        for fn in self.ops:
            fn(cv, expr)
        return key

    def draw(self, fn):
        """Ajoute un trait (fn(cv, expr)) ; décorateur."""
        self.ops.append(fn)
        return fn

    # --- palette -------------------------------------------------------------
    def palette(self, keys_list, strict=True):
        used = set()
        for key in keys_list:
            for row in key:
                used.update(row)
        cols = []
        index = {}
        order = sorted(used, key=lambda k: (k[0] != "bg", k[0] != "ink", k[0], k[1]))
        for k in order:
            rgb = self.ramps[k[0]][k[1]]
            if rgb not in cols:
                cols.append(rgb)
            index[k] = 1 + cols.index(rgb)
        if len(cols) > 15 and not strict:
            print("ATTENTION palette %d couleurs" % len(cols), getattr(self, "name", ""))
            cols = cols[:15]
            index = {k: min(v, 15) for k, v in index.items()}
        elif len(cols) > 15:
            raise ValueError("palette trop grande (%d couleurs) : %s" % (len(cols), cols))
        pal = [md(0, 0, 0)] + [md(*c) for c in cols]
        pal += [md(0, 0, 0)] * (16 - len(pal))
        return pal, index


def clean_lines(ink):
    """Supprime les coins en L des traits (pixel perfect)."""
    ink = set(ink)
    changed = True
    while changed:
        changed = False
        for (x, y) in sorted(ink):
            nb = [(x + dx, y + dy) for dx in (-1, 0, 1) for dy in (-1, 0, 1)
                  if (dx or dy) and (x + dx, y + dy) in ink]
            if len(nb) != 2:
                continue
            (ax, ay), (bx, by) = nb
            orth_a = (ax == x) != (ay == y)
            orth_b = (bx == x) != (by == y)
            if orth_a and orth_b and ax != bx and ay != by:
                ink.discard((x, y))
                changed = True
    return ink


class Draw:
    """Dessin direct sur la carte de clés (rampe, ton), après éclairage."""

    def __init__(self, key, P):
        self.key = key
        self.P = P

    def put(self, x, y, k):
        if 0 <= x < self.P.w and 0 <= y < self.P.h:
            self.key[y][x] = k

    def get(self, x, y):
        return self.key[y][x] if (0 <= x < self.P.w and 0 <= y < self.P.h) else None

    def fill(self, m, k):
        for (x, y) in m:
            self.put(x, y, k)

    def owner_is(self, x, y, layer):
        if not (0 <= x < self.P.w and 0 <= y < self.P.h):
            return False
        i = self.P.owner[y][x]
        return i >= 0 and self.P.layers[i] is layer

    def on(self, m, layer):
        return {p for p in m if self.owner_is(p[0], p[1], layer)}

    def darken(self, m, steps=1, mats=None):
        for (x, y) in m:
            k = self.get(x, y)
            if k and k[0] != "ink" and (mats is None or k[0] in mats):
                self.put(x, y, (k[0], max(0, k[1] - steps)))

    def lighten(self, m, steps=1, mats=None):
        for (x, y) in m:
            k = self.get(x, y)
            if k and k[0] != "ink" and (mats is None or k[0] in mats):
                n = len(self.P.ramps[k[0]])
                self.put(x, y, (k[0], min(n - 1, k[1] + steps)))

    def stroke(self, pts, k, thick=1):
        m = polyline([(int(round(a)), int(round(b))) for a, b in pts])
        if thick > 1:
            m |= {(x, y + 1) for (x, y) in m}
        self.fill(m, k)
        return m

    def curve(self, p0, p1, p2, k=("ink", 0), steps=None, thick=1):
        """Courbe de Bézier quadratique."""
        pts = bez(p0, p1, p2, steps)
        return self.stroke(pts, k, thick)

    def grid(self, x0, y0, rows, cmap, flip=False):
        for j, row in enumerate(rows):
            w = len(row)
            for i, ch in enumerate(row):
                if ch in cmap:
                    xx = x0 + (w - 1 - i if flip else i)
                    self.put(xx, y0 + j, cmap[ch])


def bez(p0, p1, p2, steps=None):
    if steps is None:
        steps = int(abs(p2[0] - p0[0]) + abs(p2[1] - p0[1])) + 2
    pts = []
    for i in range(steps + 1):
        t = i / steps
        a = (1 - t) * (1 - t)
        b = 2 * (1 - t) * t
        c = t * t
        pts.append((a * p0[0] + b * p1[0] + c * p2[0], a * p0[1] + b * p1[1] + c * p2[1]))
    return pts


def to_px(P, key, index):
    return [[index[k] for k in row] for row in key]
