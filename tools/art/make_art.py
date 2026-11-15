#!/usr/bin/env python3
"""Génère toutes les illustrations de Mega Président (tâche 2.02, pixel art V2).

Sorties : res/gfx/art_*.png   PNG indexés 4 bits (16 couleurs, indice 0 transparent)
          res/gfx_art.res     ressources SGDK (IMAGE, ordre = enum ArtId de src/ui/ui_art.h)
Option  : --preview DIR       écrit aussi des planches d'aperçu agrandies (PNG RVB)

Python 3 seul, sans PIL. Usage : python3 tools/art/make_art.py
Les dessins sont décrits dans title3.py, g_*.py (gueules), vig3.py et decors3.py (V3) ;
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
import title3      # noqa: E402
import portraits   # noqa: E402
import vig3       # noqa: E402
import decors3    # noqa: E402
import gueules     # noqa: E402
import persos      # noqa: E402

# (groupe, palette, liste de (nom, fonction)) : l'ordre global = enum ArtId
GROUPS = [
    ("Écran titre (320 x 128)", title.PALETTE, title3.TITLE),
    ("Portraits (56 x 56, palette commune)", portraits.PALETTE, []),   # V2 retirés (gueules V3)
]
for name, fn, pal in vig3.vignettes():
    GROUPS.append(("Vignette d'événement (64 x 48)", pal, [(name, fn)]))
for name, fn, pal in decors3.DECORS:
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


ANIMS = ("blink", "look", "talk1", "talk2")   # images d'animation (tâche 3.18)
EYE_ANIMS = ("blink", "look")
MOUTH_ANIMS = ("talk1", "talk2")
LOOK_EXPRS = (0, 1)   # regard de côté : neutre et content seulement (budget ROM)


def render_gueule(fn):
    """Rend un personnage : renvoie (P, palette, pxs, anim) avec pxs[e] les 4 expressions
    et anim[e][a] les images d'animation (a dans ANIMS) de l'expression e."""
    import traits
    P = fn()
    P.render_base()
    keys = [P.render(e) for e in gueules.EXPRS]
    akeys = []
    for e in gueules.EXPRS:
        row = []
        for a in ANIMS:
            traits.ANIM = a
            try:
                row.append(P.render(e))
            finally:
                traits.ANIM = None
        akeys.append(row)
    pal, index = P.palette(keys + [k for row in akeys for k in row])
    pxs = [gueules.to_px(P, k, index) for k in keys]
    anim = [[gueules.to_px(P, k, index) for k in row] for row in akeys]
    return P, pal, pxs, anim


def tile_diff(a, b):
    """Tuiles (tx, ty) qui diffèrent entre deux images de gueule."""
    return {(tx, ty) for ty in range(gueules.H // 8) for tx in range(gueules.W // 8)
            if any(a[ty * 8 + j][tx * 8:tx * 8 + 8] != b[ty * 8 + j][tx * 8:tx * 8 + 8]
                   for j in range(8))}


def bbox(tiles):
    if not tiles:
        return (0, 0, 0, 0)
    x0 = min(t[0] for t in tiles)
    y0 = min(t[1] for t in tiles)
    return (x0, y0, max(t[0] for t in tiles) - x0 + 1, max(t[1] for t in tiles) - y0 + 1)


def crop(px, x, y, w, h):
    return [row[x * 8:(x + w) * 8] for row in px[y * 8:(y + h) * 8]]


def gueule_layout(pxs, anim):
    """Rectangles (en tuiles) : patch d'expression, yeux, bouche ; et drapeaux par
    expression (bit 0 clignement, bit 1 regard, bit 2 parole) si l'image diffère."""
    base = pxs[0]
    expr_t = set()
    for k in range(1, len(pxs)):
        expr_t |= tile_diff(pxs[k], base)
    eye_t, mouth_t = set(), set()
    flags = []
    for e in range(len(pxs)):
        f = 0
        for a, im in zip(ANIMS, anim[e]):
            d = tile_diff(im, pxs[e])
            if not d or (a == "look" and e not in LOOK_EXPRS):
                continue
            if a in EYE_ANIMS:
                eye_t |= d
                f |= 1 if a == "blink" else 2
            else:
                mouth_t |= d
                f |= 4
        flags.append(f)
    eye = bbox(eye_t)
    mouth = bbox(mouth_t)
    if eye[2] and mouth[2]:
        ox = eye[0] < mouth[0] + mouth[2] and mouth[0] < eye[0] + eye[2]
        oy = eye[1] < mouth[1] + mouth[3] and mouth[1] < eye[1] + eye[3]
        if ox and oy:
            raise ValueError("yeux et bouche partagent des tuiles : séparer les rectangles")
    # Le patch d'expression couvre aussi les yeux et la bouche : retour à l'expression
    # = sous-rectangle du patch (ou du portrait de base pour le neutre).
    patch = bbox(expr_t | {(x, y) for (x0, y0, w, h) in (eye, mouth) if w
                           for y in range(y0, y0 + h) for x in range(x0, x0 + w)})
    return patch, eye, mouth, flags


def anim_sheet(e_px, anim_e, flags, eye, mouth):
    """Image d'animation d'une expression : [clignement | regard] sur la rangée des yeux,
    [parole 1 | parole 2] en dessous. Images absentes (drapeau nul) : tuiles vides."""
    ew, eh = eye[2], eye[3]
    mw, mh = mouth[2], mouth[3]
    tw = max(2 * ew, 2 * mw, 1)
    th = max(eh + mh, 1)
    img = [[0] * (tw * 8) for _ in range(th * 8)]

    def paste(src, tx, ty):
        for j, row in enumerate(src):
            img[ty * 8 + j][tx * 8:tx * 8 + len(row)] = row
    if ew:
        if flags & 1:
            paste(crop(anim_e[0], *eye), 0, 0)
        if flags & 2:
            paste(crop(anim_e[1], *eye), ew, 0)
    if mw and flags & 4:
        paste(crop(anim_e[2], *mouth), 0, eh)
        paste(crop(anim_e[3], *mouth), mw, eh)
    return img


def gueules_export(preview_dir=None):
    """Gueules V3 : portrait neutre + un patch (rectangle de tuiles) par autre expression,
    + une planche d'animation par expression (yeux fermés, regard, bouche qui parle).
    Écrit res/gfx/gueule_*.png et src/ui/ui_art_gueules.h ; renvoie les lignes du .res."""
    lines = ["// Gueules V3 (80 x 96, une palette par personnage) : portrait neutre, puis un",
             "// patch par expression (content, furieux, gêné) à poser par-dessus (tools/art/g_*.py),",
             "// puis une planche d'animation par expression (clignement, regard, parole : 3.18)."]
    enum = []
    table = []
    report = []
    tot_anim = 0
    for name, fn in persos.ALL:
        P, pal, pxs, anim = render_gueule(fn)
        base = pxs[0]
        patch, eye, mouth, flags = gueule_layout(pxs, anim)
        x0, y0, pw, ph = patch
        gname = "gueule_" + name
        raster.write_png(os.path.join(GFX, gname + ".png"), base, pal)
        lines.append('IMAGE %-30s "gfx/%s.png" NONE ALL' % (gname, gname))
        ntiles = unique_tiles(base)
        for k, e in enumerate(gueules.EXPRS[1:], 1):
            c = crop(pxs[k], x0, y0, pw, ph)
            raster.write_png(os.path.join(GFX, "%s_%s.png" % (gname, e)), c, pal)
            lines.append('IMAGE %-30s "gfx/%s_%s.png" NONE ALL' % ("%s_%s" % (gname, e), gname, e))
            ntiles += unique_tiles(c)
        atiles = []
        for k, e in enumerate(gueules.EXPRS):
            sh = anim_sheet(pxs[k], anim[k], flags[k], eye, mouth)
            raster.write_png(os.path.join(GFX, "%s_%s_anim.png" % (gname, e)), sh, pal)
            lines.append('IMAGE %-30s "gfx/%s_%s_anim.png" NONE ALL'
                         % ("%s_%s_anim" % (gname, e), gname, e))
            atiles.append(unique_tiles(sh))
        tot_anim += sum(atiles)
        enum.append("GUEULE_" + name.upper())
        table.append("    { &%s, { &%s_content, &%s_furieux, &%s_gene }, %d, %d, %d, %d,\n"
                     "      { &%s_neutre_anim, &%s_content_anim, &%s_furieux_anim, &%s_gene_anim },"
                     " { %s }, %d, %d, %d, %d, %d, %d, %d, %d },"
                     % (gname, gname, gname, gname, x0, y0, pw, ph,
                        gname, gname, gname, gname, ", ".join(str(f) for f in flags),
                        eye[0], eye[1], eye[2], eye[3], mouth[0], mouth[1], mouth[2], mouth[3]))
        report.append("%-26s patch %dx%d en (%d,%d), yeux %dx%d, bouche %dx%d, %d tuiles"
                      " + anim %s = %d (%d o)"
                      % (name, pw, ph, x0, y0, eye[2], eye[3], mouth[2], mouth[3], ntiles,
                         "/".join(str(t) for t in atiles), sum(atiles), sum(atiles) * 32))
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
    print("  animations : %d tuiles (%d Ko), %d o par personnage en moyenne"
          % (tot_anim, tot_anim * 32 // 1024, tot_anim * 32 // max(1, len(enum))))
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
