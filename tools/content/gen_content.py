#!/usr/bin/env python3
"""Convertit docs/contenu/*.md en C.

Sorties (fichiers générés, ne pas modifier) :
  src/sim/events_ids.h, src/sim/events_data.c   données sans texte
  src/content/content_{events,laws,world,texts}.{h,c}  textes UTF-8

Usage : python3 tools/content/gen_content.py   (depuis la racine du dépôt)
Sortie déterministe. Code de retour 1 et messages clairs si le contenu est invalide.
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
SRC = os.path.join(ROOT, "docs", "contenu")

# Limites (caractères / lignes)
MAX_TITLE, MAX_CHOICE, MAX_LINE = 30, 34, 38
MAX_TEXT_LINES, MAX_JOURNAL_LINES, MAX_LAW_NAME = 6, 2, 24
MAX_LAW_DESC_LINES = 3
MAX_CHOICES = 3
MAX_EFFECTS = 6
MAX_EVENTS = 32   # bitfield u32 dans Country

WEIGHTS = {"faible": 1, "moyenne": 2, "forte": 4}

# Variables de condition : (identifiant C, facteur vers l'unité interne)
COND_VARS = {
    "TENSION": ("EVV_TENSION", 1), "SCANDALE": ("EVV_SCANDALE", 1),
    "DEFIANCE": ("EVV_DEFIANCE", 1), "CP": ("EVV_CP", 1),
    "POPULARITE": ("EVV_POPULARITE", 10), "CHOMAGE": ("EVV_CHOMAGE", 100),
    "DETTE": ("EVV_DETTE", 10), "CROISSANCE": ("EVV_CROISSANCE", 100),
    "SOCIAL": ("EVV_SOCIAL", 10), "ECONOMIE": ("EVV_ECONOMIE", 10),
    "TECHNOLOGIE": ("EVV_TECHNOLOGIE", 10), "ECOLOGIE": ("EVV_ECOLOGIE", 10),
    "SECURITE": ("EVV_SECURITE", 10),
}
COND_OPS = {">": "EVO_GT", ">=": "EVO_GE", "<": "EVO_LT", "<=": "EVO_LE"}

errors = []


def err(where, msg):
    errors.append("%s : %s" % (where, msg))


def read(name):
    with open(os.path.join(SRC, name), encoding="utf-8") as f:
        return f.read().split("\n")


def enum_names(header, prefix):
    """Noms d'un enum C commençant par prefix dans un en-tête de src/sim."""
    with open(os.path.join(ROOT, "src", "sim", header), encoding="utf-8") as f:
        txt = f.read()
    return re.findall(r"\b(%s[A-Z0-9_]+)\b" % prefix, re.sub(r"//.*", "", txt))


def uniq(seq):
    out = []
    for s in seq:
        if s not in out:
            out.append(s)
    return out


EFF_NAMES = [n for n in uniq(enum_names("effects.h", "EFF_")) if n not in ("EFF_NONE", "EFF_COUNT")]
LAW_NAMES = [n for n in uniq(enum_names("laws.h", "LOI_")) if n != "LOI_COUNT"]
CAND_NAMES = [n for n in uniq(enum_names("election.h", "CAND_"))
              if n not in ("CAND_POOL_COUNT", "CAND_JOUEUR")]


def cstr(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n") + '"'


def check_lines(where, what, lines, max_lines, max_len):
    if not lines or any(not l for l in lines):
        err(where, "%s vide" % what)
    if len(lines) > max_lines:
        err(where, "%s : %d lignes (max %d)" % (what, len(lines), max_lines))
    for l in lines:
        if len(l) > max_len:
            err(where, "%s : ligne de %d caractères (max %d) : « %s »" % (what, len(l), max_len, l))


def split_field(line):
    m = re.match(r"^(\s*)([^:]+?)\s*:\s?(.*)$", line)
    return m


def parse_effects(where, text):
    effs = []
    for part in text.split(","):
        part = part.strip()
        m = re.match(r"^([A-Z0-9_]+)\s+([+-]?\d+)$", part)
        if not m:
            err(where, "effet illisible : « %s »" % part)
            continue
        key, val = m.group(1), int(m.group(2))
        if "EFF_" + key not in EFF_NAMES:
            err(where, "clé d'effet inconnue : %s" % key)
            continue
        if not -128 <= val <= 127:
            err(where, "valeur hors s8 : %s %d" % (key, val))
            continue
        effs.append(("EFF_" + key, val))
    if not effs:
        err(where, "aucun effet")
    if len(effs) > MAX_EFFECTS:
        err(where, "%d effets (max %d)" % (len(effs), MAX_EFFECTS))
    return effs


def parse_cond(where, text):
    text = text.strip()
    if text.lower() == "aucune":
        return None
    m = re.match(r"^([A-Z0-9_]+)\s*(>=|<=|>|<)\s*(-?\d+)$", text)
    if not m or m.group(1) not in COND_VARS:
        err(where, "condition invalide : « %s » (variables : %s)" % (text, ", ".join(sorted(COND_VARS))))
        return None
    var, factor = COND_VARS[m.group(1)]
    value = int(m.group(3)) * factor
    if not -32768 <= value <= 32767:
        err(where, "seuil hors s16 : %s" % text)
    return (var, COND_OPS[m.group(2)], value)


def event_files():
    """Tous les fichiers d'événements, dans l'ordre alphabétique (ordre des ids)."""
    return sorted(f for f in os.listdir(SRC)
                  if f.startswith("evenements") and f.endswith(".md"))


def parse_events():
    events = []
    for fname in event_files():
        parse_event_file(fname, events)
    return validate_events(events)


def parse_event_file(fname, events):
    lines = read(fname)
    cur = None       # événement courant
    choice = None    # choix courant
    field = None     # (conteneur, clé) pour les lignes de suite
    for n, raw in enumerate(lines, 1):
        line = raw.rstrip()
        where = "%s:%d" % (fname, n)
        if line.startswith("## "):
            name = line[3:].strip()
            cur = choice = field = None
            if name.startswith("EVT_"):
                if any(e["id"] == name for e in events):
                    err(where, "événement en double : %s" % name)
                cur = {"id": name, "line": n, "file": fname, "cond": None, "prob": None, "title": None,
                       "text": [], "choices": [], "seen": set()}
                events.append(cur)
            continue
        if cur is None or not line.strip():
            continue
        if line.startswith("    ") and field is not None:   # suite de Journal
            field[0][field[1]].append(line.strip())
            continue
        m = split_field(line)
        indented = line.startswith("  ")
        key = m.group(2) if m else None
        if m and not indented and key in ("Condition", "Probabilité", "Titre", "Texte"):
            cur["seen"].add(key)
            val = m.group(3).strip()
            choice = None
            field = None
            if key == "Condition":
                cur["cond"] = parse_cond(where, val)
            elif key == "Probabilité":
                if val not in WEIGHTS:
                    err(where, "probabilité inconnue : « %s » (faible, moyenne, forte)" % val)
                cur["prob"] = val
            elif key == "Titre":
                cur["title"] = val
            else:
                cur["text"] = [val] if val else []
                field = (cur, "text")
        elif m and not indented and re.match(r"^Choix [123]$", key):
            idx = int(key[-1])
            if idx != len(cur["choices"]) + 1:
                err(where, "choix numéroté %d, attendu %d" % (idx, len(cur["choices"]) + 1))
            choice = {"title": m.group(3).strip(), "effects": None, "journal": [], "line": n}
            cur["choices"].append(choice)
            field = None
        elif m and indented and choice is not None and key == "Effets":
            choice["effects"] = parse_effects(where, m.group(3))
            field = None
        elif m and indented and choice is not None and key == "Journal":
            v = m.group(3).strip()
            choice["journal"] = [v] if v else []
            field = (choice, "journal")
        elif indented and field is not None and field[1] == "text":
            field[0]["text"].append(line.strip())
        else:
            err(where, "ligne non reconnue : « %s »" % line.strip())


def validate_events(events):
    for e in events:
        w = "%s (%s:%d)" % (e["id"], e["file"], e["line"])
        for k in ("Condition", "Probabilité", "Titre", "Texte"):
            if k not in e["seen"]:
                err(w, "champ manquant : %s" % k)
        if e["title"] is not None and (not e["title"] or len(e["title"]) > MAX_TITLE):
            err(w, "titre de %d caractères (max %d)" % (len(e["title"]), MAX_TITLE))
        check_lines(w, "texte", e["text"], MAX_TEXT_LINES, MAX_LINE)
        if not 2 <= len(e["choices"]) <= MAX_CHOICES:
            err(w, "%d choix (2 ou 3 attendus)" % len(e["choices"]))
        for i, c in enumerate(e["choices"], 1):
            wc = "%s choix %d" % (w, i)
            if not c["title"] or len(c["title"]) > MAX_CHOICE:
                err(wc, "libellé de %d caractères (max %d)" % (len(c["title"]), MAX_CHOICE))
            if c["effects"] is None:
                err(wc, "Effets manquants")
            check_lines(wc, "journal", c["journal"], MAX_JOURNAL_LINES, MAX_LINE)
    if len(events) > MAX_EVENTS:
        err("docs/contenu/evenements*.md", "%d événements (max %d)" % (len(events), MAX_EVENTS))
    if not events:
        err("docs/contenu/evenements*.md", "aucun événement")
    return events


def law_files():
    """Tous les fichiers de lois (lois_*.md), ordre alphabétique."""
    return sorted(f for f in os.listdir(SRC) if f.startswith("lois") and f.endswith(".md"))


def parse_laws():
    laws = {}
    files = law_files()
    if not files:
        err("docs/contenu/lois*.md", "aucun fichier de lois")
    for fname in files:
        parse_law_file(fname, laws)
    for name in LAW_NAMES:
        if name not in laws:
            err("docs/contenu/lois*.md", "loi absente (présente dans laws.h) : %s" % name)
            continue
        l = laws[name]
        w = "%s (%s:%d)" % (name, l["file"], l["line"])
        if not l["name"] or len(l["name"]) > MAX_LAW_NAME:
            err(w, "nom de %d caractères (max %d)" % (len(l["name"] or ""), MAX_LAW_NAME))
        check_lines(w, "description", l["desc"], MAX_LAW_DESC_LINES, MAX_LINE)
        for k, label in (("ok", "journal si adoptée"), ("ko", "journal si rejetée")):
            check_lines(w, label, [l[k]] if l[k] else [], MAX_JOURNAL_LINES, MAX_LINE)
    return laws


def parse_law_file(fname, laws):
    lines = read(fname)
    cur = None
    field = None
    for n, raw in enumerate(lines, 1):
        line = raw.rstrip()
        where = "%s:%d" % (fname, n)
        if line.startswith("## "):
            name = line[3:].strip()
            cur = field = None
            if name.startswith("LOI_"):
                if name not in LAW_NAMES:
                    err(where, "loi inconnue de laws.h : %s" % name)
                if name in laws:
                    err(where, "loi en double : %s (déjà dans %s)" % (name, laws[name]["file"]))
                cur = {"line": n, "file": fname, "name": None, "desc": [], "ok": None, "ko": None}
                laws[name] = cur
            continue
        if cur is None or not line.strip():
            continue
        m = split_field(line)
        if m and not line.startswith(" "):
            key, val = m.group(2), m.group(3).strip()
            field = None
            if key == "Nom":
                cur["name"] = val
            elif key == "Description":
                cur["desc"] = [val] if val else []
                field = "desc"
            elif key == "Journal si adoptée":
                cur["ok"] = val
            elif key == "Journal si rejetée":
                cur["ko"] = val
            elif key.startswith("Effets suggérés"):
                pass
            else:
                err(where, "champ inconnu : %s" % key)
        elif line.startswith(" ") and field == "desc":
            cur["desc"].append(line.strip())
        else:
            err(where, "ligne non reconnue : « %s »" % line.strip())


# Textes institutionnels (CONTRAT_V1 §4) : clé -> (lignes max, colonnes max par ligne)
TEXT_KEYS = [
    ("TXT_493_UTILISE", 3, 38),
    ("TXT_TV_SUCCES", 3, 38), ("TXT_TV_FLOP", 3, 38),
    ("TXT_CENSURE_DEPOSEE", 3, 38), ("TXT_CENSURE_ADOPTEE", 3, 38), ("TXT_CENSURE_REJETEE", 3, 38),
    ("TXT_NOUVEAU_PM", 3, 38),
    ("TXT_DISSOLUTION", 3, 38), ("TXT_LEGISLATIVES_GAGNEES", 3, 38), ("TXT_LEGISLATIVES_PERDUES", 3, 38),
    ("TXT_BUDGET_ADOPTE", 3, 38), ("TXT_BUDGET_493", 3, 38),
    ("TXT_DESTITUTION_PROCEDURE", 3, 38), ("TXT_DESTITUTION_VOTEE", 3, 38),
    ("TXT_DESTITUTION_REJETEE", 3, 38),
    ("TXT_SECOND_MANDAT", 3, 38), ("TXT_FIN_SECOND_MANDAT", 3, 38),
    ("TXT_PM_TECHNOCRATE_NOM", 1, 24), ("TXT_PM_TECHNOCRATE_DESC", 3, 38),
    ("TXT_PM_RASSEMBLEUR_NOM", 1, 24), ("TXT_PM_RASSEMBLEUR_DESC", 3, 38),
    ("TXT_PM_REFORMATEUR_NOM", 1, 24), ("TXT_PM_REFORMATEUR_DESC", 3, 38),
    ("TXT_DIFF_FACILE", 1, 38), ("TXT_DIFF_NORMAL", 1, 38), ("TXT_DIFF_DIFFICILE", 1, 38),
    ("TXT_THEME_EMPLOI", 1, 24), ("TXT_THEME_SECURITE", 1, 24),
    ("TXT_THEME_POUVOIR_ACHAT", 1, 24), ("TXT_THEME_ECOLOGIE", 1, 24),
    ("TXT_BUD_RIGUEUR", 1, 20), ("TXT_BUD_EQUILIBRE", 1, 20), ("TXT_BUD_RELANCE", 1, 20),
    ("TXT_BUD_RIGUEUR_DESC", 2, 38), ("TXT_BUD_EQUILIBRE_DESC", 2, 38), ("TXT_BUD_RELANCE_DESC", 2, 38),
]


def parse_texts():
    fname = "textes.md"
    texts = {}
    if not os.path.exists(os.path.join(SRC, fname)):
        err("docs/contenu/" + fname, "fichier absent")
        return texts
    limits = dict((k, (nl, nc)) for k, nl, nc in TEXT_KEYS)
    cur = None
    for n, raw in enumerate(read(fname), 1):
        line = raw.rstrip()
        where = "%s:%d" % (fname, n)
        if line.startswith("## "):
            name = line[3:].strip()
            cur = None
            if name.startswith("TXT_"):
                if name not in limits:
                    err(where, "clé inconnue : %s" % name)
                elif name in texts:
                    err(where, "clé en double : %s" % name)
                else:
                    cur = {"line": n, "text": []}
                    texts[name] = cur
            continue
        if cur is None or line.startswith("#"):
            continue
        if line.strip():
            cur["text"].append(line.strip())
    for key, nl, nc in TEXT_KEYS:
        if key not in texts:
            err(fname, "clé absente : %s" % key)
            continue
        t = texts[key]
        check_lines("%s (%s:%d)" % (key, fname, t["line"]), "texte", t["text"], nl, nc)
    return texts


def table_rows(lines, start_marker):
    """Lignes de données de la première table suivant le marqueur de section."""
    i = next((k for k, l in enumerate(lines) if l.startswith(start_marker)), None)
    if i is None:
        err("univers.md", "section absente : %s" % start_marker)
        return []
    rows = []
    for l in lines[i + 1:]:
        if l.startswith("## "):
            break
        if l.startswith("|"):
            cells = [c.strip() for c in l.strip().strip("|").split("|")]
            if cells and not set(cells[0]) <= set("-: "):
                rows.append(cells)
    return rows[1:]   # sans l'en-tête


def parse_world():
    lines = read("univers.md")
    groups = {}
    for r in table_rows(lines, "## 1."):
        if len(r) >= 4:
            groups[r[0]] = (r[1], r[2], r[3])
    for g in ("G1", "G2", "G3", "G4", "G5"):
        if g not in groups:
            err("univers.md", "groupe absent : %s" % g)
        else:
            nm, sg, ds = groups[g]
            if not nm or not sg or not ds:
                err("univers.md", "groupe %s incomplet" % g)
    cands = {}
    for r in table_rows(lines, "## 3."):
        if len(r) >= 4 and r[0].startswith("CAND_"):
            cands[r[0].split()[0]] = (r[1], r[2], r[3])
    for c in CAND_NAMES:
        if c not in cands:
            err("univers.md", "candidat absent : %s" % c)
    return groups, cands


# ---------------------------------------------------------------- génération

HEADER = ("// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :\n"
          "// ne pas modifier à la main (relancer le générateur).\n\n")


def write(path, text):
    full = os.path.join(ROOT, path)
    os.makedirs(os.path.dirname(full), exist_ok=True)
    with open(full, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)


def gen_events(events):
    ids = ["#ifndef SIM_EVENTS_IDS_H\n#define SIM_EVENTS_IDS_H\n\n" + HEADER,
           "// Identifiants d'événements (ordre de docs/contenu/evenements*.md).\n",
           "enum {\n"]
    for e in events:
        ids.append("    %s,\n" % e["id"])
    ids.append("    EVT_COUNT\n};\n\n#endif\n")
    write("src/sim/events_ids.h", "".join(ids))

    d = [HEADER, '#include "sim/events.h"\n\n',
         "// Données des événements : identifiants, conditions, poids, effets. Aucun texte.\n",
         "const EventDef event_defs[EVT_COUNT] = {\n"]
    for e in events:
        if e["cond"]:
            cond = "{ %s, %s, %d }" % e["cond"]
        else:
            cond = "{ EVV_NONE, EVO_GT, 0 }"
        d.append("    { %s, %d, %s, %d, {\n" % (e["id"], WEIGHTS[e["prob"]], cond, len(e["choices"])))
        for c in e["choices"]:
            effs = ", ".join("{ %s, %d }" % x for x in c["effects"])
            d.append("        { %d, { %s } },\n" % (len(c["effects"]), effs))
        d.append("    } },\n")
    d.append("};\n")
    write("src/sim/events_data.c", "".join(d))

    h = ["#ifndef CONTENT_EVENTS_H\n#define CONTENT_EVENTS_H\n\n" + HEADER,
         '#include "sim/events.h"\n\n',
         "// Textes UTF-8 indexés par EVT_*. Les lignes sont séparées par '\\n'.\n",
         "extern const char *const evt_title[EVT_COUNT];\n",
         "extern const char *const evt_text[EVT_COUNT];                       // 6 lignes max\n",
         "extern const char *const evt_choice[EVT_COUNT][EVENT_MAX_CHOICES];   // libellés\n",
         "extern const char *const evt_journal[EVT_COUNT][EVENT_MAX_CHOICES];  // 2 lignes max\n\n",
         "#endif\n"]
    write("src/content/content_events.h", "".join(h))
    c = [HEADER, '#include "content/content_events.h"\n\n', "const char *const evt_title[EVT_COUNT] = {\n"]
    for e in events:
        c.append("    %s, // %s\n" % (cstr(e["title"]), e["id"]))
    c.append("};\n\nconst char *const evt_text[EVT_COUNT] = {\n")
    for e in events:
        c.append("    %s, // %s\n" % (cstr("\n".join(e["text"])), e["id"]))
    c.append("};\n\nconst char *const evt_choice[EVT_COUNT][EVENT_MAX_CHOICES] = {\n")
    for e in events:
        t = [cstr(x["title"]) for x in e["choices"]] + ["0"] * (MAX_CHOICES - len(e["choices"]))
        c.append("    { %s }, // %s\n" % (", ".join(t), e["id"]))
    c.append("};\n\nconst char *const evt_journal[EVT_COUNT][EVENT_MAX_CHOICES] = {\n")
    for e in events:
        t = [cstr("\n".join(x["journal"])) for x in e["choices"]] + ["0"] * (MAX_CHOICES - len(e["choices"]))
        c.append("    { %s }, // %s\n" % (", ".join(t), e["id"]))
    c.append("};\n")
    write("src/content/content_events.c", "".join(c))


def gen_laws(laws):
    h = ["#ifndef CONTENT_LAWS_H\n#define CONTENT_LAWS_H\n\n" + HEADER,
         '#include "sim/laws.h"\n\n',
         "// Textes UTF-8 indexés par LOI_*.\n",
         "extern const char *const law_name[LOI_COUNT];        // 24 car. max\n",
         "extern const char *const law_desc[LOI_COUNT];        // 3 lignes max ('\\n')\n",
         "extern const char *const law_journal_ok[LOI_COUNT];  // si adoptée\n",
         "extern const char *const law_journal_ko[LOI_COUNT];  // si rejetée\n\n#endif\n"]
    write("src/content/content_laws.h", "".join(h))
    c = [HEADER, '#include "content/content_laws.h"\n\n']
    for var, key in (("law_name", "name"), ("law_desc", "desc"),
                     ("law_journal_ok", "ok"), ("law_journal_ko", "ko")):
        c.append("const char *const %s[LOI_COUNT] = {\n" % var)
        for n in LAW_NAMES:
            v = laws[n][key]
            v = "\n".join(v) if isinstance(v, list) else v
            c.append("    %s, // %s\n" % (cstr(v), n))
        c.append("};\n\n")
    write("src/content/content_laws.c", "".join(c).rstrip("\n") + "\n")


def gen_world(groups, cands):
    h = ["#ifndef CONTENT_WORLD_H\n#define CONTENT_WORLD_H\n\n" + HEADER,
         '#include "sim/sim_state.h"\n#include "sim/election.h"\n\n',
         "// Textes UTF-8 : groupes parlementaires (GRP_G1..) et candidats (CAND_*).\n",
         "extern const char *const grp_name[GRP_COUNT];\n",
         "extern const char *const grp_sigle[GRP_COUNT];\n",
         "extern const char *const grp_desc[GRP_COUNT];\n",
         "extern const char *const cand_name[CAND_POOL_COUNT];\n",
         "extern const char *const cand_party[CAND_POOL_COUNT];\n",
         "extern const char *const cand_portrait[CAND_POOL_COUNT];\n\n#endif\n"]
    write("src/content/content_world.h", "".join(h))
    c = [HEADER, '#include "content/content_world.h"\n\n']
    for var, i in (("grp_name", 0), ("grp_sigle", 1), ("grp_desc", 2)):
        c.append("const char *const %s[GRP_COUNT] = {\n" % var)
        for g in ("G1", "G2", "G3", "G4", "G5"):
            c.append("    %s, // %s\n" % (cstr(groups[g][i]), g))
        c.append("};\n\n")
    for var, i in (("cand_name", 0), ("cand_party", 1), ("cand_portrait", 2)):
        c.append("const char *const %s[CAND_POOL_COUNT] = {\n" % var)
        for n in CAND_NAMES:
            c.append("    %s, // %s\n" % (cstr(cands[n][i]), n))
        c.append("};\n\n")
    write("src/content/content_world.c", "".join(c).rstrip("\n") + "\n")


def gen_texts(texts):
    h = ["#ifndef CONTENT_TEXTS_H\n#define CONTENT_TEXTS_H\n\n" + HEADER,
         "// Textes institutionnels UTF-8 (docs/contenu/textes.md), lignes séparées par '\\n'.\n",
         "enum {\n"]
    for k, _, _ in TEXT_KEYS:
        h.append("    %s,\n" % k)
    h.append("    TXT_COUNT\n};\n\nextern const char *const txt[TXT_COUNT];\n\n#endif\n")
    write("src/content/content_texts.h", "".join(h))
    c = [HEADER, '#include "content/content_texts.h"\n\nconst char *const txt[TXT_COUNT] = {\n']
    for k, _, _ in TEXT_KEYS:
        c.append("    %s, // %s\n" % (cstr("\n".join(texts[k]["text"])), k))
    c.append("};\n")
    write("src/content/content_texts.c", "".join(c))


def main():
    events = parse_events()
    laws = parse_laws()
    groups, cands = parse_world()
    texts = parse_texts()
    if errors:
        sys.stderr.write("gen_content : %d erreur(s) de contenu\n" % len(errors))
        for e in errors:
            sys.stderr.write("  - %s\n" % e)
        return 1
    gen_events(events)
    gen_laws(laws)
    gen_world(groups, cands)
    gen_texts(texts)
    print("gen_content : %d événements, %d lois, %d groupes, %d candidats, %d textes" %
          (len(events), len(LAW_NAMES), 5, len(CAND_NAMES), len(TEXT_KEYS)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
