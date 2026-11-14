# Contrat technique V0

> Document de coordination entre agents. Il fixe les **unités**, les **clés d'effets** et le **partage des fichiers**.
> Toute modification passe par la session principale.

## 1. Partage des fichiers (pas de chevauchement)

| Propriétaire | Fichiers | Tâches |
|---|---|---|
| `md-dev` interface (agent A) | `src/ui/`, `res/`, `tools/font/`, `src/main.c` | 1.1 à 1.5 |
| `md-dev` simulation (agent B) | `src/sim/` (sauf `rng.*`, déjà fait), `tests/`, `docs/SIM.md` | 2.1 à 2.4, 3.1, 3.2 |
| `redacteur` (agent C) | `docs/contenu/` | 4.3 (brouillon), univers fictif |
| Session principale | `docs/BACKLOG.md`, `docs/REGLES.md`, `docs/CONTRAT_V0.md`, `Makefile`, `CLAUDE.md`, git | coordination, intégration |

Aucun agent ne commite. Aucun agent ne modifie un fichier qui ne lui appartient pas : s'il en a besoin, il le signale dans son rapport.

## 2. Unités de l'état du pays

Tout est entier : pas de float, pas de double.

| Grandeur | Type | Unité interne | Exemple |
|---|---|---|---|
| Opinion d'un électorat | s16 | dixièmes de % (0–1000) | 425 = 42,5 % |
| Popularité globale | calculée | moyenne pondérée des électorats : populaires 40 %, moyennes 40 %, aisées 20 % | |
| Croissance | s16 | centièmes de point | 120 = +1,20 % |
| Chômage | s16 | centièmes de point | 750 = 7,50 % |
| Dette | s16 | dixièmes de point de PIB | 1100 = 110,0 % |
| Piliers (5) | s16 | dixièmes (0–1000), départ ≈ 500 | |
| Jauges : tension, scandale, défiance | u8 | 0–100 | V0 : seule la tension est active |
| Relation d'un groupe parlementaire | u8 | 0–100 | |
| Capital politique | u8 | points, plafond 15 | |

**Mode de jeu** : la V0 n'a que le mode 20 tours (1 tour = 1 trimestre). Le code prévoit un facteur `turns_per_year` (4 ou 12) pour ajouter le mode 60 tours en V1 sans réécriture.

## 3. Clés d'effets (communes aux lois et aux événements)

Un effet est un couple (clé, valeur) exprimé en **unités lisibles** dans le contenu. La simulation le convertit en unités internes.

| Clé | Unité dans le contenu | Exemple |
|---|---|---|
| `POP_POPULAIRES`, `POP_MOYENNES`, `POP_AISEES` | points de % d'opinion | `POP_AISEES -3` |
| `POP_TOUS` | points de % appliqués aux 3 électorats | `POP_TOUS +2` |
| `CROISSANCE` | dixièmes de point | `CROISSANCE +2` = +0,2 % |
| `CHOMAGE` | dixièmes de point | `CHOMAGE -3` = −0,3 % |
| `DETTE` | points de PIB | `DETTE +1` |
| `SOCIAL`, `ECONOMIE`, `TECHNOLOGIE`, `ECOLOGIE`, `SECURITE` | points de pilier (0–100) | `ECOLOGIE +4` |
| `TENSION`, `SCANDALE`, `DEFIANCE` | points de jauge (0–100) | `TENSION +10` |
| `CP` | capital politique | `CP -1` |
| `REL_G1` … `REL_G5` | points de relation avec un groupe | `REL_G4 -5` |

**Valeurs modérées** : un effet d'événement dépasse rarement ±5 (±10 pour une jauge).

## 4. Groupes parlementaires (identifiants fixes)

Les noms sont choisis par le `redacteur` dans `docs/contenu/univers.md`. La simulation n'utilise que les identifiants.

| Id | Orientation (−100 gauche … +100 droite) | Sièges de départ (difficulté Normal) |
|---|---|---|
| G1 | −80, gauche radicale | 75 |
| G2 | −40, gauche sociale-démocrate et écologiste | 140 |
| G3 | 0, centre | 160 |
| G4 | +40, droite | 120 |
| G5 | +80, droite nationaliste | 82 |

Total : 577 sièges. Majorité absolue : 289.

Le joueur choisit son orientation : gauche (son parti est G2), centre (G3) ou droite (G4). Son parti part avec une relation de 80, les groupes voisins avec 55, les autres avec 30 et 15.

## 5. Candidats à la présidentielle

Pool : CAND_GAUCHE_RADICALE (−80), CAND_ECOLO (−50), CAND_CENTRE (0), CAND_DROITE (+40), CAND_NATIONALISTE (+80).
Le joueur affronte 4 candidats. Gauche ou centre : tous sauf CAND_CENTRE. Droite : tous sauf CAND_DROITE.
Noms validés par le joueur : voir `docs/contenu/univers.md`.

## 6. Textes

- Les chaînes sont écrites en **UTF-8** dans le code C et dans le contenu. L'interface décode les accents français (é è ê ë à â ç î ï ô ù û ü œ É È Ê À Ç Ô « » ° € ’).
- 38 colonnes maximum par ligne affichée.

## 7. Enchaînement des écrans (V0)

Module commun : `src/game/game.h` (état `g_game`, transitions). Les écrans ne modifient la simulation que par ses fonctions.

```
TITRE (choix de l'orientation, START) --game_new--> JEU
JEU (tableau de bord, proposer une loi, « Fin du trimestre ») --game_end_turn--> RAPPORT
RAPPORT (journal) --game_after_report--> ÉVÉNEMENT si tiré, sinon JEU (ou ÉLECTION si mandat fini)
ÉVÉNEMENT (choix) --game_event_chosen--> JEU ou ÉLECTION
ÉLECTION (game_run_election : 1er et 2nd tour) --> FIN (score) --> TITRE
```

| Écran | Fichier | Propriétaire |
|---|---|---|
| Jeu, rapport, événement | `src/screens/scr_game.c`, `scr_report.c`, `scr_event.c` | agent écrans A |
| Titre, élection, fin | `src/screens/scr_title.c`, `scr_election.c`, `scr_end.c` | agent écrans B |
| Enregistrement des écrans | `src/main.c` | session principale |

Chaque fichier d'écran expose `extern const Screen scr_xxx;` déclaré dans `src/screens/screens.h` (session principale).
