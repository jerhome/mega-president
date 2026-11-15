#!/usr/bin/env python3
"""Aperçu agrandi des gueules : python3 tools/art/apercu_gueules.py SORTIE.png [nom...] [--scale N]"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import gueules   # noqa: E402
import persos    # noqa: E402
from raster import write_rgb_png   # noqa: E402


def render_all(names=None):
    out = []
    for name, fn in persos.ALL if hasattr(persos, "ALL") else persos.DEMO:
        if names and name not in names:
            continue
        P = fn()
        P.name = name
        P.render_base()
        keys = [P.render(e) for e in gueules.EXPRS]
        pal, index = P.palette(keys, strict=False)
        out.append((name, [gueules.to_px(P, k, index) for k in keys], pal))
    return out


def sheet(path, items, scale=4, gap=6, bg=0x202020):
    cols = len(gueules.EXPRS)
    rows = len(items)
    Wt = cols * (gueules.W + gap) + gap
    Ht = rows * (gueules.H + gap) + gap
    img = [[bg] * (Wt * scale) for _ in range(Ht * scale)]
    for r, (name, pxs, pal) in enumerate(items):
        for c, px in enumerate(pxs):
            ox = gap + c * (gueules.W + gap)
            oy = gap + r * (gueules.H + gap)
            for y in range(gueules.H):
                for x in range(gueules.W):
                    col = pal[px[y][x]]
                    for sy in range(scale):
                        row = img[(oy + y) * scale + sy]
                        for sx in range(scale):
                            row[(ox + x) * scale + sx] = col
    write_rgb_png(path, img)


def contact(path, items, per_row=3, scale=4, gap=6, bg=0x202020, extra=None):
    """Planche-contact : chaque personnage = bloc de 4 expressions, per_row blocs par ligne.
    extra : liste de (pixels, palette) d'images supplémentaires ajoutées en bas (ex. titre)."""
    n = len(gueules.EXPRS)
    bw = n * (gueules.W + 2) + gap
    rows = (len(items) + per_row - 1) // per_row
    Wt = per_row * bw + gap
    Ht = rows * (gueules.H + gap) + gap
    ex_h = sum(len(px) + gap for px, _ in (extra or []))
    Wt = max(Wt, max([len(px[0]) + 2 * gap for px, _ in (extra or [])] or [0]))
    img = [[bg] * Wt for _ in range(Ht + ex_h)]
    for k, (name, pxs, pal) in enumerate(items):
        ox = gap + (k % per_row) * bw
        oy = gap + (k // per_row) * (gueules.H + gap)
        for c, px in enumerate(pxs):
            for y in range(gueules.H):
                for x in range(gueules.W):
                    img[oy + y][ox + c * (gueules.W + 2) + x] = pal[px[y][x]]
    oy = Ht
    for px, pal in (extra or []):
        for y, row in enumerate(px):
            for x, v in enumerate(row):
                img[oy + y][gap + x] = pal[v]
        oy += len(px) + gap
    out = []
    for row in img:
        r = []
        for c in row:
            r += [c] * scale
        for _ in range(scale):
            out.append(list(r))
    write_rgb_png(path, out)


def scaled(px, pal, scale):
    out = []
    for row in px:
        r = []
        for v in row:
            r += [pal[v]] * scale
        for _ in range(scale):
            out.append(list(r))
    return out


def export(outdir):
    """Chaque gueule et expression au format natif et agrandie x4, la planche, et le
    portrait V2 du président de gauche (x4) pour comparaison."""
    os.makedirs(outdir, exist_ok=True)
    items = render_all()
    for name, pxs, pal in items:
        for e, px in zip(gueules.EXPRS, pxs):
            write_rgb_png(os.path.join(outdir, "%s_%s.png" % (name, e)), scaled(px, pal, 1))
            write_rgb_png(os.path.join(outdir, "%s_%s_x4.png" % (name, e)), scaled(px, pal, 4))
    sheet(os.path.join(outdir, "planche_x4.png"), items, scale=4)
    import portraits
    c = portraits.pres_gauche()
    write_rgb_png(os.path.join(outdir, "v2_pres_gauche_x4.png"), scaled(c.px, portraits.PALETTE, 4))


if __name__ == "__main__":
    args = sys.argv[1:]
    if args and args[0] == "--export":
        export(args[1])
        sys.exit(0)
    if args and args[0] == "--contact":   # --contact SORTIE.png [échelle]
        import title
        t = title.TITLE[0][1]()
        contact(args[1], render_all(), scale=int(args[2]) if len(args) > 2 else 4,
                extra=[(t.px, title.PALETTE)])
        sys.exit(0)
    scale = 4
    if "--scale" in args:
        i = args.index("--scale")
        scale = int(args[i + 1])
        del args[i:i + 2]
    sheet(args[0], render_all(args[1:] or None), scale=scale)
