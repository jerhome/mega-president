#!/usr/bin/env python3
"""Génère toutes les illustrations de Mega Président (tâche 2.02, pixel art V2).

Sorties : res/gfx/art_*.png   PNG indexés 4 bits (16 couleurs, indice 0 transparent)
          res/gfx_art.res     ressources SGDK (IMAGE, ordre = enum ArtId de src/ui/ui_art.h)
Option  : --preview DIR       écrit aussi des planches d'aperçu agrandies (PNG RVB)

Python 3 seul, sans PIL. Usage : python3 tools/art/make_art.py
Les dessins sont décrits dans title.py, portraits.py, vignettes.py et decors.py ;
raster.py fournit les primitives (ellipses, polygones, contours d'encre, tramage).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
GFX = os.path.join(ROOT, "res", "gfx")
RES = os.path.join(ROOT, "res", "gfx_art.res")

import raster      # noqa: E402
import title       # noqa: E402
import portraits   # noqa: E402
import vignettes   # noqa: E402
import decors      # noqa: E402
import gueules     # noqa: E402
import persos      # noqa: E402

# (groupe, palette, liste de (nom, fonction)) : l'ordre global = enum ArtId
GROUPS = [
    ("Écran titre (320 x 128)", title.PALETTE, title.TITLE),
    ("Portraits (56 x 56, palette commune)", portraits.PALETTE, portraits.PORTRAITS),
]
for name, fn, pal in vignettes.VIGNETTES:
    GROUPS.append(("Vignette d'événement (64 x 48)", pal, [(name, fn)]))
for name, fn, pal in decors.DECORS:
    GROUPS.append(("Décor", pal, [(name, fn)]))


def check(name, c, pal):
    assert c.w % 8 == 0 and c.h % 8 == 0, name + " : dimensions non multiples de 8"
    assert len(pal) == 16, name
    for row in c.px:
        for v in row:
            assert 0 <= v < 16, name


def unique_tiles(px, x0=0, y0=0, tw=None, th=None):
    """Nombre de tuiles après dédoublonnage (identiques ou retournées), comme rescomp ALL."""
    tw = tw if tw is not None else len(px[0]) // 8
    th = th if th is not None else len(px) // 8
    seen = set()
    for ty in range(y0, y0 + th):
        for tx in range(x0, x0 + tw):
            t = tuple(tuple(px[ty * 8 + j][tx * 8:tx * 8 + 8]) for j in range(8))
            variants = [t, tuple(r[::-1] for r in t), t[::-1], tuple(r[::-1] for r in t[::-1])]
            if not any(v in seen for v in variants):
                seen.add(t)
    return len(seen)


def gueules_export(preview_dir=None):
    """Gueules V3 : portrait neutre + un patch (rectangle de tuiles) par autre expression.
    Écrit res/gfx/gueule_*.png et src/ui/ui_art_gueules.h ; renvoie les lignes du .res."""
    lines = ["// Gueules V3 (80 x 96, une palette par personnage) : portrait neutre, puis un",
             "// patch par expression (content, furieux, gêné) à poser par-dessus (tools/art/g_*.py)."]
    enum = []
    table = []
    report = []
    for name, fn in persos.ALL:
        P = fn()
        P.render_base()
        keys = [P.render(e) for e in gueules.EXPRS]
        pal, index = P.palette(keys)
        pxs = [gueules.to_px(P, k, index) for k in keys]
        base = pxs[0]
        tw, th = gueules.W // 8, gueules.H // 8
        diff = [(tx, ty) for ty in range(th) for tx in range(tw)
                if any(pxs[k][ty * 8 + j][tx * 8:tx * 8 + 8] != base[ty * 8 + j][tx * 8:tx * 8 + 8]
                       for k in range(1, len(pxs)) for j in range(8))]
        x0 = min(t[0] for t in diff)
        x1 = max(t[0] for t in diff)
        y0 = min(t[1] for t in diff)
        y1 = max(t[1] for t in diff)
        pw, ph = x1 - x0 + 1, y1 - y0 + 1
        gname = "gueule_" + name
        raster.write_png(os.path.join(GFX, gname + ".png"), base, pal)
        lines.append('IMAGE %-30s "gfx/%s.png" NONE ALL' % (gname, gname))
        ntiles = unique_tiles(base)
        for k, e in enumerate(gueules.EXPRS[1:], 1):
            crop = [row[x0 * 8:(x1 + 1) * 8] for row in pxs[k][y0 * 8:(y1 + 1) * 8]]
            raster.write_png(os.path.join(GFX, "%s_%s.png" % (gname, e)), crop, pal)
            lines.append('IMAGE %-30s "gfx/%s_%s.png" NONE ALL' % ("%s_%s" % (gname, e), gname, e))
            ntiles += unique_tiles(crop)
        enum.append("GUEULE_" + name.upper())
        table.append("    { &%s, { &%s_content, &%s_furieux, &%s_gene }, %d, %d, %d, %d },"
                     % (gname, gname, gname, gname, x0, y0, pw, ph))
        report.append("%-16s patch %dx%d en (%d,%d), %d tuiles (base %d)"
                      % (name, pw, ph, x0, y0, ntiles, unique_tiles(base)))
    lines.append("")
    h = ["// FICHIER GÉNÉRÉ par tools/art/make_art.py : ne pas modifier à la main.",
         "// Gueules V3 : ordre = res/gfx_art.res (tools/art/persos.py, ALL).",
         "#ifndef UI_ART_GUEULES_H",
         "#define UI_ART_GUEULES_H",
         "",
         "typedef enum {"]
    h += ["    %s," % e for e in enum]
    h += ["    GUEULE_COUNT", "} GueuleId;", "",
          "#define GUEULE_W %d   // en tuiles" % (gueules.W // 8),
          "#define GUEULE_H %d" % (gueules.H // 8), "",
          "#endif", "",
          "#ifdef UI_ART_GUEULES_TABLE",
          "static const GueuleDef gueule_defs[GUEULE_COUNT] = {"]
    h += table
    h += ["};", "#endif", ""]
    with open(os.path.join(ROOT, "src", "ui", "ui_art_gueules.h"), "w", encoding="utf-8") as f:
        f.write("\n".join(h))
    for r in report:
        print("  " + r)
    return lines


def main():
    preview_dir = None
    if "--preview" in sys.argv:
        preview_dir = sys.argv[sys.argv.index("--preview") + 1]
        os.makedirs(preview_dir, exist_ok=True)
    os.makedirs(GFX, exist_ok=True)
    lines = [
        "// Mega Président — illustrations (tâche 2.02), générées par tools/art/make_art.py.",
        "// Ne pas éditer : relancer le script. L'ordre des IMAGE = enum ArtId (src/ui/ui_art.h).",
        "// Pas de compression (chargement direct en VRAM par DMA, aucun tampon en RAM) ;",
        "// tuiles dédoublonnées (ALL : identiques et retournées).",
        "",
    ]
    sheets = {}
    total = 0
    for label, pal, items in GROUPS:
        lines.append("// " + label)
        for name, fn in items:
            c = fn()
            check(name, c, pal)
            raster.write_png(os.path.join(GFX, name + ".png"), c.px, pal)
            lines.append('IMAGE %-22s "gfx/%s.png" NONE ALL' % (name, name))
            sheets.setdefault(label, []).append((c, pal))
            total += 1
        lines.append("")
    lines += gueules_export(preview_dir)
    with open(RES, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("%d illustrations -> res/gfx/art_*.png, res/gfx_art.res" % total)
    if preview_dir:
        for k, (label, imgs) in enumerate(sheets.items()):
            w = imgs[0][0].w
            cols = 1 if w >= 160 else (5 if w <= 64 else 3)
            raster.preview(os.path.join(preview_dir, "sheet_%d.png" % k), imgs, cols=cols, scale=3)
        vig = [ci for label, imgs in sheets.items() if label.startswith("Vignette") for ci in imgs]
        if vig:
            raster.preview(os.path.join(preview_dir, "vignettes.png"), vig, cols=5, scale=3)


if __name__ == "__main__":
    main()
