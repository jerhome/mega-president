"""Mini-bibliothèque de pixel art pour Mega Président (Python 3 seul, sans PIL).

- Couleurs Megadrive : niveaux 0-7 par composante (VDP 9 bits).
- Canvas : image indexée (indices 0-15, 0 = transparent).
- Masques : ensembles de pixels (x, y) combinables (|, &, -), construits à partir
  de primitives (ellipse, polygone, rectangle, trait) ; Canvas.paint() remplit
  un masque et trace son contour d'encre (style BD), shade() ajoute une ombre
  latérale, dither() un tramage ordonné (Bayer 4x4) pour les dégradés.
- write_png() : PNG indexé 4 bits (même format que tools/font/make_gfx.py).
"""
import struct
import zlib

LEVELS = [0x00, 0x24, 0x49, 0x6D, 0x92, 0xB6, 0xDB, 0xFF]


def md(r, g, b):
    """Couleur Megadrive (niveaux 0-7) -> 0xRRGGBB."""
    return (LEVELS[r] << 16) | (LEVELS[g] << 8) | LEVELS[b]


BAYER4 = [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]


# --- Masques ----------------------------------------------------------------

def ellipse(cx, cy, rx, ry):
    """Ellipse pleine ; centre en coordonnées de bord de pixel (28 = entre 27 et 28)."""
    m = set()
    for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
        for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
            dx = (x + 0.5 - cx) / rx
            dy = (y + 0.5 - cy) / ry
            if dx * dx + dy * dy <= 1.0:
                m.add((x, y))
    return m


def rect(x0, y0, x1, y1):
    """Rectangle plein, bornes incluses."""
    return {(x, y) for y in range(y0, y1 + 1) for x in range(x0, x1 + 1)}


def poly(points):
    """Polygone plein (règle pair-impair, test au centre des pixels)."""
    m = set()
    ys = [p[1] for p in points]
    n = len(points)
    for y in range(int(min(ys)) - 1, int(max(ys)) + 2):
        yc = y + 0.5
        xs = []
        for i in range(n):
            x0, y0 = points[i]
            x1, y1 = points[(i + 1) % n]
            if (y0 <= yc < y1) or (y1 <= yc < y0):
                xs.append(x0 + (yc - y0) * (x1 - x0) / (y1 - y0))
        xs.sort()
        for i in range(0, len(xs) - 1, 2):
            for x in range(int(xs[i] - 0.5) - 1, int(xs[i + 1] + 0.5) + 2):
                if xs[i] <= x + 0.5 < xs[i + 1]:
                    m.add((x, y))
    return m


def line(x0, y0, x1, y1):
    """Trait de Bresenham (pixels entiers)."""
    m = set()
    dx, dy = abs(x1 - x0), -abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx + dy
    while True:
        m.add((x0, y0))
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x0 += sx
        if e2 <= dx:
            err += dx
            y0 += sy
    return m


def polyline(points):
    m = set()
    for i in range(len(points) - 1):
        m |= line(*points[i], *points[i + 1])
    return m


def thick(m, r=1):
    """Épaissit un masque (croix de rayon r)."""
    out = set(m)
    for (x, y) in m:
        for d in range(1, r + 1):
            out |= {(x + d, y), (x - d, y), (x, y + d), (x, y - d)}
    return out


def shift(m, dx, dy):
    return {(x + dx, y + dy) for (x, y) in m}


def mirror(m, axis2):
    """Symétrie verticale : x -> axis2 - 1 - x (axis2 = 2 * axe)."""
    return {(axis2 - 1 - x, y) for (x, y) in m}


def border(m, outer=False):
    """Pixels de bord du masque (4-voisinage) ; outer : pixels juste à l'extérieur."""
    out = set()
    for (x, y) in m:
        for n in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
            if n not in m:
                if outer:
                    out.add(n)
                else:
                    out.add((x, y))
                    break
    return out


def arc(cx, cy, rx, ry, a0, a1, steps=64):
    """Arc d'ellipse (angles en degrés, 0 = droite, 90 = bas)."""
    import math
    pts = []
    for i in range(steps + 1):
        a = math.radians(a0 + (a1 - a0) * i / steps)
        pts.append((int(round(cx + rx * math.cos(a) - 0.5)), int(round(cy + ry * math.sin(a) - 0.5))))
    m = set()
    for i in range(len(pts) - 1):
        m |= line(*pts[i], *pts[i + 1])
    return m


# --- Canvas -----------------------------------------------------------------

class Canvas:
    def __init__(self, w, h, fill=0):
        self.w, self.h = w, h
        self.px = [[fill] * w for _ in range(h)]

    def inside(self, x, y):
        return 0 <= x < self.w and 0 <= y < self.h

    def get(self, x, y):
        return self.px[y][x] if self.inside(x, y) else 0

    def put(self, x, y, c):
        if self.inside(x, y):
            self.px[y][x] = c

    def fill(self, m, c):
        for (x, y) in m:
            self.put(x, y, c)

    def paint(self, m, c, ink=1, edge=True, clip=None):
        """Remplit le masque puis trace son contour intérieur d'encre.
        edge=False : le bord du canevas ne reçoit pas de contour."""
        if clip is not None:
            m = m & clip
        self.fill(m, c)
        if ink is None:
            return m
        for (x, y) in m:
            for n in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                if n not in m and (edge or self.inside(*n)):
                    self.put(x, y, ink)
                    break
        return m

    def shade(self, m, c, dx, dy, keep=None):
        """Ombre : pixels de m non recouverts par m décalé de (dx, dy).
        Ex. dx = -2 : bande de 2 px sur le côté droit."""
        band = m - shift(m, dx, dy)
        if keep is not None:
            band &= keep
        self.fill(band, c)
        return band

    def dither(self, m, c1, c2, level):
        """Tramage ordonné : level 0..16 = proportion de c2."""
        for (x, y) in m:
            self.put(x, y, c2 if BAYER4[y & 3][x & 3] < level else c1)

    def grid(self, x0, y0, rows, cmap, flip=False):
        """Colle une grille de caractères ; '.' (ou absent de cmap) = inchangé."""
        for j, row in enumerate(rows):
            w = len(row)
            for i, ch in enumerate(row):
                if ch in cmap:
                    xx = x0 + (w - 1 - i if flip else i)
                    self.put(xx, y0 + j, cmap[ch])

    def blit(self, other, x0, y0, transparent=0):
        for y in range(other.h):
            for x in range(other.w):
                c = other.px[y][x]
                if c != transparent:
                    self.put(x0 + x, y0 + y, c)

    def recolor(self, m, mapping):
        for (x, y) in m:
            if self.inside(x, y):
                self.px[y][x] = mapping.get(self.px[y][x], self.px[y][x])

    def mask_of(self, colors):
        return {(x, y) for y in range(self.h) for x in range(self.w) if self.px[y][x] in colors}

    def mirror_from_left(self, axis2):
        """Copie la moitié gauche sur la moitié droite (symétrie)."""
        for y in range(self.h):
            for x in range(axis2 // 2):
                xx = axis2 - 1 - x
                if 0 <= xx < self.w:
                    self.px[y][xx] = self.px[y][x]


# --- PNG ----------------------------------------------------------------------

def write_png(path, pixels, palette):
    """PNG indexé 4 bits. pixels : liste de lignes d'indices 0-15 ; palette : 16 x 0xRRGGBB."""
    h, w = len(pixels), len(pixels[0])
    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for x in range(0, w, 2):
            raw.append((row[x] << 4) | (row[x + 1] if x + 1 < w else 0))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    pal = list(palette) + [0] * (16 - len(palette))
    plte = b"".join(struct.pack("BBB", (c >> 16) & 255, (c >> 8) & 255, c & 255) for c in pal)
    data = (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 4, 3, 0, 0, 0))
            + chunk(b"PLTE", plte)
            + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
            + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def write_rgb_png(path, rgb_rows):
    """PNG RVB 8 bits (aperçus) : rgb_rows = lignes de 0xRRGGBB."""
    h, w = len(rgb_rows), len(rgb_rows[0])
    raw = bytearray()
    for row in rgb_rows:
        raw.append(0)
        for c in row:
            raw += bytes(((c >> 16) & 255, (c >> 8) & 255, c & 255))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    data = (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
            + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def preview(path, images, cols=4, scale=3, gap=4, bg=0x202020):
    """Planche d'aperçu : images = liste de (Canvas, palette)."""
    cw = max(c.w for c, _ in images)
    ch = max(c.h for c, _ in images)
    rows = (len(images) + cols - 1) // cols
    W = cols * (cw + gap) + gap
    H = rows * (ch + gap) + gap
    out = [[bg] * (W * scale) for _ in range(H * scale)]
    for k, (c, pal) in enumerate(images):
        ox = gap + (k % cols) * (cw + gap)
        oy = gap + (k // cols) * (ch + gap)
        for y in range(c.h):
            for x in range(c.w):
                v = c.px[y][x]
                col = 0x808080 if v == 0 and ((x // 4 + y // 4) & 1) else (0x606060 if v == 0 else pal[v])
                for sy in range(scale):
                    r = out[(oy + y) * scale + sy]
                    for sx in range(scale):
                        r[(ox + x) * scale + sx] = col
    write_rgb_png(path, out)
