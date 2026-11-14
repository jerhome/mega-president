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
