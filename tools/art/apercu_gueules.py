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
    scale = 4
    if "--scale" in args:
        i = args.index("--scale")
        scale = int(args[i + 1])
        del args[i:i + 2]
    sheet(args[0], render_all(args[1:] or None), scale=scale)
