#!/usr/bin/env python3
"""Calcule l'avancement de chaque version à partir de docs/BACKLOG.md.

Convention : une ligne de tableau contenant ✅ est faite (100 %) ; 🔄 signale
une tâche en cours, comptée à part (0 % dans le pourcentage, pour ne pas le
gonfler quand plusieurs tâches sont déléguées en même temps). L'avancement est pondéré par la durée estimée de chaque tâche.
La « version en cours » est la première qui n'est pas à 100 %.
"""
import re
import sys
from datetime import datetime
from pathlib import Path

# Début réel du développement (premier commit) : sert à mesurer la vitesse réelle
# (heures estimées faites par heure réelle) pour projeter le temps restant.
PROJECT_START = datetime(2026, 11, 14, 21, 30)

BACKLOG = Path(__file__).resolve().parent.parent / "docs" / "BACKLOG.md"
HOURS = re.compile(r"\|\s*(\d+(?:,\d+)?)\s*h\s*\|")


def parse(text):
    versions, cur, epic = {}, None, None
    for line in text.splitlines():
        m = re.match(r"^## (V\d+)\b(.*)", line)
        if m:
            cur = m.group(1)
            versions[cur] = {"title": line[3:].strip(), "epics": {}}
            epic = "Général"
            continue
        if line.startswith("## "):
            cur = None
            continue
        if cur is None:
            continue
        m = re.match(r"^### (.+)", line)
        if m:
            epic = re.sub(r"\s*\(≈.*\)$", "", m.group(1)).strip()
            continue
        if not line.startswith("|"):
            continue
        h = HOURS.search(line)
        if not h:
            continue
        hours = float(h.group(1).replace(",", "."))
        done = 1.0 if "✅" in line else 0.0
        e = versions[cur]["epics"].setdefault(epic, [0.0, 0.0, 0, 0, 0])
        e[0] += hours
        e[1] += hours * done
        e[2] += 1
        e[3] += 1 if done == 1.0 else 0
        e[4] += 1 if "🔄" in line else 0
    return versions


def bar(p, width=20):
    n = round(p / 100 * width)
    return "█" * n + "░" * (width - n)


def main():
    versions = parse(BACKLOG.read_text(encoding="utf-8"))
    current = None
    for name, v in versions.items():
        tot = sum(e[0] for e in v["epics"].values())
        don = sum(e[1] for e in v["epics"].values())
        v["pct"] = 100 * don / tot if tot else 0
        v["tot"], v["don"] = tot, don
        if current is None and v["pct"] < 100:
            current = name
    print("Avancement global")
    for name, v in versions.items():
        mark = "  ← en cours" if name == current else ""
        rest = v['tot'] - v['don']
        print(f"  {name:3} {bar(v['pct'])} {v['pct']:5.1f} %  ({v['don']:.1f} / {v['tot']:.1f} h, reste {rest:.1f} h){mark}")
    done_all = sum(v['don'] for v in versions.values())
    elapsed = (datetime.now() - PROJECT_START).total_seconds() / 3600
    if elapsed > 0.25 and done_all > 0:
        speed = done_all / elapsed
        print(f"\nVitesse réelle : {speed:.1f} h estimées par heure réelle "
              f"({done_all:.1f} h faites en {elapsed:.1f} h)")
        for name, v in versions.items():
            rest = v['tot'] - v['don']
            if rest > 0:
                mins = rest / speed * 60
                print(f"  {name} : reste {rest:.1f} h estimées ≈ {mins:.0f} min réelles à ce rythme")
    if current:
        v = versions[current]
        print(f"\nDétail {v['title']}")
        for epic, (tot, don, n, ndone, nwip) in v["epics"].items():
            pct = 100 * don / tot if tot else 0
            wip = f", {nwip} en cours" if nwip else ""
            print(f"  {bar(pct, 10)} {pct:5.1f} %  {epic}  [{ndone}/{n} faites{wip}]")


if __name__ == "__main__":
    sys.exit(main())
