#!/usr/bin/env python3
"""Compare out/shots aux captures de référence (tests/shots_ref), pixel par pixel.

  python3 tools/shots_compare.py [--ref DIR] [--new DIR] [--diff DIR] [--tol N] [--update NOM...]

Affiche les écrans qui diffèrent (nombre de pixels), écrit une image de
différence (pixels différents en rouge sur l'écran atténué) dans out/shots_diff/.
Code de sortie 1 si au moins un écran diffère ou manque.
--update NOM... copie les captures indiquées (sans « .png ») dans la référence ;
--update seul (sans nom) les copie toutes.
Utilise PIL si disponible, sinon `compare` d'ImageMagick.
"""
import argparse
import os
import shutil
import subprocess
import sys

try:
    from PIL import Image, ImageChops
    HAVE_PIL = True
except ImportError:
    HAVE_PIL = False


def diff_pil(ref, new, out):
    a = Image.open(ref).convert("RGB")
    b = Image.open(new).convert("RGB")
    if a.size != b.size:
        return -1
    mask = ImageChops.difference(a, b).convert("L").point(lambda v: 255 if v else 0)
    n = sum(1 for v in mask.getdata() if v)
    if n and out:
        base = Image.blend(b, Image.new("RGB", b.size, (0, 0, 0)), 0.6)
        base.paste(Image.new("RGB", b.size, (255, 0, 0)), mask=mask)
        base.save(out)
    return n


def diff_im(ref, new, out):
    sz = lambda f: subprocess.run(["identify", "-format", "%wx%h", f],
                                  capture_output=True, text=True).stdout
    if sz(ref) != sz(new):
        return -1
    r = subprocess.run(["compare", "-metric", "AE", ref, new, out or "null:"],
                       capture_output=True, text=True)
    try:
        return int(float(r.stderr.split()[0]))
    except (ValueError, IndexError):
        return -1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ref", default="tests/shots_ref")
    ap.add_argument("--new", default="out/shots")
    ap.add_argument("--diff", default="out/shots_diff")
    ap.add_argument("--tol", type=int, default=0, metavar="N",
                    help="tolérer jusqu'à N pixels différents (curseurs clignotants)")
    ap.add_argument("--update", nargs="*", default=None, metavar="NOM")
    args = ap.parse_args()

    news = {f[:-4] for f in os.listdir(args.new) if f.endswith(".png")} if os.path.isdir(args.new) else set()
    if args.update is not None:
        os.makedirs(args.ref, exist_ok=True)
        for n in (args.update or sorted(news)):
            shutil.copy(os.path.join(args.new, n + ".png"), os.path.join(args.ref, n + ".png"))
            print("référence mise à jour :", n)
        return 0

    refs = {f[:-4] for f in os.listdir(args.ref) if f.endswith(".png")} if os.path.isdir(args.ref) else set()
    if not refs:
        print("aucune référence dans", args.ref)
        return 1
    os.makedirs(args.diff, exist_ok=True)
    fn = diff_pil if HAVE_PIL else diff_im
    bad = 0
    for n in sorted(refs):
        if n not in news:
            print(f"MANQUE   {n} (pas de capture dans {args.new})")
            bad += 1
            continue
        out = os.path.join(args.diff, n + "_diff.png")
        k = fn(os.path.join(args.ref, n + ".png"), os.path.join(args.new, n + ".png"), out)
        if 0 <= k <= args.tol:
            print(f"identique {n}" + (f" ({k} px tolérés)" if k else ""))
        elif k < 0:
            print(f"DIFFERE  {n} (taille différente ou comparaison impossible)")
            bad += 1
        else:
            print(f"DIFFERE  {n} : {k} pixels -> {out}")
            bad += 1
    for n in sorted(news - refs):
        print(f"nouveau  {n} (sans référence)")
    print(f"{bad} écran(s) différent(s) sur {len(refs)}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
