# Mega Président — consignes pour Claude

Jeu de gestion présidentielle au tour par tour pour **Megadrive d'origine** (PAL, EverDrive MD V3), en C avec **SGDK 2.09**.
Le joueur parle français : documents, textes du jeu et rapports en français.

## Documents de référence
- `docs/REGLES.md` : règles validées (V1 et V2 distinguées).
- `docs/BACKLOG.md` : tâches V0, V1 et V2. Marquer ✅ (fait) ou 🔄 (en cours) dans la ligne de la tâche ; `python3 tools/progress.py` calcule l'avancement.
- `docs/A_VALIDER.md` : décisions importantes en attente d'accord du joueur (mode retrait).
- `docs/AGENTS.md` : agents `md-dev`, `qa-rom`, `redacteur`, `directeur-artistique` (Opus, décisions graphiques et relecture visuelle avant le joueur), `vannes` (Opus, relecture et écriture de l'humour) (`.claude/agents/`), en Sonnet. **3 agents simultanés au maximum** ; un 4e temporaire si gain de temps, avec notification mobile (PushNotification). Opus ponctuellement si gros gain, en prévenant aussi.

## Commandes
| Commande | Effet |
|---|---|
| `make` | ROM `out/rom.bin` via Docker (`ghcr.io/stephane-d/sgdk`) |
| `make run` / `make run-ntsc` | Compile puis lance BlastEm (WSLg) en PAL ou NTSC |
| `make test` | Tests du cœur de simulation sur PC (gcc natif) |
| `make check` | Tests + ROM + budgets (`tools/budget.sh`) |
| `./tools/status.sh` | Ressources système |

Capture de l'écran de l'émulateur (vérification visuelle) :
`xwd -id $(xwininfo -root -tree | grep 'MEGA PRESIDENT' | awk '{print $1}') -silent | convert xwd:- capture.png`
BlastEm doit tourner sans OpenGL (`gl off` dans sa config) : sous WSLg, le réglage vsync d'OpenGL ouvre une boîte d'erreur.
Arrêter l'émulateur avec `pkill -x blastem` (jamais `pkill -f`, qui tue le shell courant).

## Architecture
- `src/sim/` : **cœur de simulation portable**, sans SGDK. Il compile pour le 68000 et pour le PC. Types via `sim/sim_types.h`. Interdits : float, double, malloc, stdio.
- `src/ui/` (à venir) : seule couche autorisée à manipuler tuiles, palettes et plans du VDP, pour permettre le pixel art de la V2 sans réécriture.
- `src/boot/rom_head.c` : en-tête de cartouche, avec la SRAM déclarée en 0x200001-0x20FFFF (octets impairs).
- `tests/` : tests PC (`CHECK`, `CHECK_EQ`).

## Contraintes Megadrive
- 68000 sans FPU : entiers ou point fixe uniquement.
- 64 Ko de RAM : RAM statique ≤ 32 Ko (le reste sert au tas SGDK et à la pile), données constantes en `const` (ROM).
- ROM ≤ 2 Mo (la SRAM est mappée à 0x200000).
- Écran 320×224 : 40 colonnes, **38 utiles** pour le texte.
- Fonctionner en PAL et en NTSC ; le jeu ne dépend jamais du nombre d'images par seconde.

## Contenu
Ton sérieux et réaliste, avec parfois une pointe d'humour de gauche. **Partis, personnalités, entreprises et médias exclusivement fictifs.**

## Git
- Dépôt public `github.com/jerhome/mega-president`, push en HTTPS via `gh` (compte `jerhome`).
- E-mail local : adresse noreply GitHub (ne jamais exposer l'adresse professionnelle).
- `out/` et `tools/blastem*/` ne sont pas versionnés.
