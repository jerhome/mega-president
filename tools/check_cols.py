#!/usr/bin/env python3
"""Contrôle statique des 38 colonnes utiles (colonnes 1 à 38, la 0 et la 39 sont des marges).

  python3 tools/check_cols.py [--strict] [FICHIER.c ...]

1. Appels de texte de src/ (ui_text_draw, _draw_n, _center, _right, _wrap, _flow,
   _wrap_line, ui_footer, ui_banner) dont une chaîne littérale dépasse la zone :
   x + longueur > 39 (ui_text_draw), longueur > largeur w (center, wrap, flow avec
   « \\n » explicites), x_end - longueur < 1 (ui_text_right), etc.
   Les expressions ternaires sont suivies : chaque branche littérale est testée.
2. Tableaux de chaînes (const char *x[] = {...}) dans src/ et chaînes de
   src/content/*.c : toute ligne (séparée par « \\n ») de plus de 38 caractères.
   Les accents comptent pour 1 caractère.

Les x ou largeurs qui ne sont pas des constantes entières sont ignorés (UI_COLS,
UI_TEXT_COLS = 40 et 38 sont connus). Mode avertissement : code de sortie 0,
sauf avec --strict.
"""
import glob
import os
import re
import sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
CONSTS = {"UI_COLS": 40, "UI_TEXT_COLS": 38}
LIMIT = 38

# nom -> (indice de x, indice de largeur, indice de la chaîne, genre)
CALLS = {
    "ui_text_draw":      (0, None, 2, "draw"),
    "ui_text_draw_n":    (0, 3, 2, "n"),
    "ui_text_center":    (0, 2, 3, "width"),
    "ui_text_right":     (0, None, 2, "right"),
    "ui_text_wrap":      (0, 2, 4, "width"),
    "ui_text_flow":      (0, 2, 4, "width"),
    "ui_text_wrap_line": (0, 2, 3, "width"),
    "ui_footer":         (0, 2, 3, "width"),
    "ui_banner":         (0, 2, 3, "width"),
}


def strip_comments(src):
    """Supprime les commentaires en conservant les retours à la ligne et les chaînes."""
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and src[j] != c:
                j += 2 if src[j] == "\\" else 1
            out.append(src[i:j + 1])
            i = j + 1
        elif src.startswith("//", i):
            j = src.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif src.startswith("/*", i):
            j = src.find("*/", i)
            j = n if j < 0 else j + 2
            out.append(re.sub(r"[^\n]", " ", src[i:j]))
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


ESC = {"n": "\n", "t": "\t", '"': '"', "'": "'", "\\": "\\", "0": ""}


def unescape(s):
    def rep(m):
        e = m.group(1)
        if e[0] == "x":
            return chr(int(e[1:], 16))
        return ESC.get(e, e)
    return re.sub(r"\\(x[0-9a-fA-F]{2}|.)", rep, s)


def decode(s):
    """Octets UTF-8 écrits en \\xNN : on recompose pour compter les caractères."""
    try:
        return s.encode("latin-1").decode("utf-8")
    except (UnicodeEncodeError, UnicodeDecodeError):
        return s


STR_RE = re.compile(r'"((?:[^"\\\n]|\\.)*)"')


def split_args(text, start):
    """text[start] est '(' : renvoie (liste d'arguments, indice après ')')."""
    depth, args, cur, i, n = 0, [], [], start, len(text)
    while i < n:
        c = text[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            cur.append(text[i:j + 1])
            i = j + 1
            continue
        if c in "([{":
            depth += 1
            if depth > 1:
                cur.append(c)
        elif c in ")]}":
            depth -= 1
            if depth == 0:
                args.append("".join(cur))
                return args, i + 1
            cur.append(c)
        elif c == "," and depth == 1:
            args.append("".join(cur))
            cur = []
        else:
            cur.append(c)
        i += 1
    return args, n


def int_of(expr):
    expr = expr.strip()
    if re.fullmatch(r"\d+", expr):
        return int(expr)
    return CONSTS.get(expr)


def literals(expr):
    """Chaînes littérales d'une expression ; les littéraux adjacents sont concaténés."""
    res = []
    for m in re.finditer(r'(?:"(?:[^"\\\n]|\\.)*"\s*)+', expr):
        parts = STR_RE.findall(m.group(0))
        res.append(decode(unescape("".join(parts))))
    return res


def lineno(text, pos):
    return text.count("\n", 0, pos) + 1


def check_file(path, warn):
    with open(path, encoding="utf-8") as f:
        raw = f.read()
    text = strip_comments(raw)
    rel = os.path.relpath(path, ROOT)
    is_content = "/content/" in path.replace("\\", "/")

    # 1. appels de texte
    for m in re.finditer(r"\b(ui_[a-z_]+)\s*\(", text):
        name = m.group(1)
        if name not in CALLS:
            continue
        xi, wi, si, kind = CALLS[name]
        args, _ = split_args(text, m.end() - 1)
        if len(args) <= si:
            continue
        x = int_of(args[xi])
        w = int_of(args[wi]) if wi is not None else None
        ln = lineno(text, m.start())
        for s in literals(args[si]):
            lines = s.split("\n")
            longest = max(len(l) for l in lines)
            shown = s.replace("\n", "\\n")
            if kind == "draw" and x is not None:
                if x + longest > 39:
                    warn(rel, ln, f"{name} x={x} + {longest} car. -> colonne {x + longest - 1} > 38 : « {shown} »")
                    continue
            elif kind == "right" and x is not None:
                if x - longest < 1:
                    warn(rel, ln, f"{name} x_end={x} - {longest} car. -> débute colonne {x - longest} < 1 : « {shown} »")
                    continue
            elif kind in ("width", "n"):
                if x is not None and w is not None and x + w > 39 and not (name in ("ui_text_center", "ui_banner") and w == 40):
                    warn(rel, ln, f"{name} x={x} + largeur {w} dépasse la colonne 38")
                if w is not None and longest > w and not (kind == "width" and "\n" not in s and name in ("ui_text_wrap", "ui_text_flow", "ui_text_wrap_line")):
                    warn(rel, ln, f"{name} largeur {w} < {longest} car. : « {shown} »")
                    continue
            if longest > LIMIT and not name.startswith("ui_text_wrap") and name != "ui_text_flow":
                warn(rel, ln, f"{name} : {longest} car. > {LIMIT} : « {shown} »")
            elif longest > LIMIT and "\n" in s:
                warn(rel, ln, f"{name} : ligne de {longest} car. > {LIMIT} : « {shown} »")

    # 2. tableaux de chaînes (hors appels) et fichiers de contenu
    seen = set()
    if is_content:
        zones = [(0, len(text))]
    else:
        zones = [(m.start(), m.end()) for m in re.finditer(
            r"const\s+char\s*\*\s*(?:const\s*)?\w+\s*\[[^\]]*\]\s*=\s*\{.*?\};", text, re.S)]
    for a, b in zones:
        for m in STR_RE.finditer(text, a, b):
            pos = m.start()
            if pos in seen:
                continue
            seen.add(pos)
            s = decode(unescape(m.group(1)))
            # littéraux adjacents : ligne de code suivante déjà concaténée par la lecture isolée
            for l in s.split("\n"):
                if len(l) > LIMIT:
                    warn(rel, lineno(text, pos), f"ligne de {len(l)} car. > {LIMIT} : « {l} »")


def main():
    strict = "--strict" in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not files:
        files = sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True))
    found = []

    def warn(f, ln, msg):
        found.append((f, ln, msg))

    for p in files:
        check_file(p, warn)
    uniq = sorted(set(found))
    for f, ln, msg in uniq:
        print(f"{f}:{ln}: avertissement colonnes : {msg}")
    print(f"check_cols : {len(uniq)} avertissement(s)")
    return 1 if (strict and uniq) else 0


if __name__ == "__main__":
    sys.exit(main())
