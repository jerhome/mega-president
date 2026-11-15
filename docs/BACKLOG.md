# Mega Président — Backlog

> **Statut : validé le 2026-11-14.**
> Les durées sont des **heures de travail effectif** : le code de Claude, plus les tests et retours du joueur.
> Les règles sont décrites dans [REGLES.md](REGLES.md).
> **Suivi** : ajouter ✅ (fait) ou 🔄 (en cours, non compté dans le %) dans la ligne d'une tâche. `python3 tools/progress.py` calcule l'avancement, pondéré par les durées.

## Synthèse

| Version | Contenu | Effort | Cumul |
|---|---|---|---|
| **V0 — Démo** | Un mandat jouable : 5 lois, 10 événements, Assemblée, élection | ≈ 38 h | 38 h |
| **V1 — Jeu complet simplifié** | Règles V1 complètes, 2 mandats, destitution, high scores, bruitages | ≈ 50 h | ≈ 88 h |
| **V2 — Version enrichie** | Carte des régions, pixel art, musique, institutions complètes | ≈ 70 h, plus la réalisation du pixel art | ≈ 160 h |

La V0 n'est pas un prototype jetable : c'est une première tranche de la V1. Les durées détaillées ci-dessous sont plus précises que l'estimation donnée oralement (≈ 30 h pour la V0 et ≈ 80 h pour la V1).

## Décisions déjà prises

- Chaque version (V0, V1, V2) se termine par une **étiquette git** et une **release GitHub** avec la ROM.

- Ton sérieux et réaliste, avec parfois une pointe d'humour de gauche (événements, journal).
- Partis et personnalités fictifs.
- Mode 20 ou 60 tours au choix en début de partie.
- Pas de high score pour une partie terminée par une destitution.
- Tableau du 2nd mandat : score cumulé sur 10 ans.
- Carte des régions repoussée en V2.
- Console cible PAL, mais le jeu fonctionne aussi en NTSC (les deux sont testés dans BlastEm).
- Tests en cours de développement dans **BlastEm** (sous WSL2, via WSLg), sans passer par l'EverDrive.

---

## V0 — Démo jouable (≈ 38 h)

### E0 — Infrastructure (≈ 5 h)

| # | Tâche | Durée | État |
|---|---|---|---|
| 0.1 | Compilation via l'image Docker SGDK 2.09 : `make`, `make run`, `make clean` | 0,5 h | ✅ |
| 0.2 | Lancement automatique de BlastEm après la compilation (WSLg) | 0,5 h | ✅ PAL et NTSC (sans OpenGL sous WSLg) |
| 0.3 | En-tête de ROM personnalisé (titre, SRAM déclarée, régions JUE) | 0,5 h | ✅ |
| 0.4 | ROM de test (texte, manette, compteur sauvegardé), validée dans BlastEm puis sur l'EverDrive MD V3 | 1 h | ✅ OK sur PC ; test EverDrive reporté en fin de projet (1.21) |
| 0.5 | Cœur de simulation compilable aussi sur PC, avec des tests unitaires | 2 h | ✅ |
| 0.6 | Dépôt git, conventions, CLAUDE.md, dépôt GitHub public | 0,5 h | ✅ |

### E1 — Moteur et architecture prête pour la V2 (≈ 10 h)

| # | Tâche | Durée |
|---|---|---|
| 1.1 | ✅ Gestion des écrans (titre, jeu, événement, élection, fin) | 1,5 h |
| 1.2 | ✅ **Couche d'interface** (fenêtres, listes, jauges, curseur) : le jeu ne manipule jamais les tuiles directement, ce qui permet de passer au pixel art sans réécriture | 3,5 h |
| 1.3 | ✅ Police 8×8 avec les accents français et codage des textes | 2 h |
| 1.4 | ✅ Manette (répétition des touches, A/B/C/Start) | 1 h |
| 1.5 | ✅ Graphismes provisoires avec des noms fixes : en V2, on remplace les fichiers sans toucher au code | 1,5 h |
| 1.6 | ✅ Hasard reproductible à partir d'une graine | 0,5 h |

### E2 — Simulation (≈ 8 h)

| # | Tâche | Durée |
|---|---|---|
| 2.1 | ✅ État du pays : 3 électorats, croissance, chômage, dette, 5 piliers (nombres entiers) | 1,5 h |
| 2.2 | ✅ Boucle de tour : lois, puis événements, puis opinion, puis Parlement | 2,5 h |
| 2.3 | ✅ Lois décrites comme des données (coût, délai, effets différés, orientation) et **5 lois** | 2 h |
| 2.4 | ✅ Calcul de l'opinion par électorat | 2 h |

### E3 — Institutions (≈ 4,5 h)

| # | Tâche | Durée |
|---|---|---|
| 3.1 | ✅ Assemblée de 577 sièges et 5 groupes fictifs, avec leurs relations et les votes | 2,5 h |
| 3.2 | ✅ Présidentielle à deux tours, un seul mandat | 2 h |

### E4 — Événements (≈ 5 h)

| # | Tâche | Durée |
|---|---|---|
| 4.1 | ✅ Moteur d'événements (conditions, probabilités, choix, conséquences) | 2 h |
| 4.2 | ✅ Fichier texte de contenu converti automatiquement en C, pour écrire du contenu sans toucher au code | 1,5 h |
| 4.3 | ✅ **10 événements** | 1,5 h |

### E6 — Écrans (≈ 4 h)

| # | Tâche | Durée |
|---|---|---|
| 6.1 | ✅ Tableau de bord principal | 2 h |
| 6.2 | ✅ Rapport du tour (journal simple) | 1 h |
| 6.3 | ✅ Écran titre (choix de l'orientation) et écran de fin avec le score | 1 h |

### Jalon V0 (≈ 1,5 h)

| # | Tâche | Durée |
|---|---|---|
| J0 | ✅ Stabilisation, essai sur la vraie console, notes de version | 1,5 h |

---

## V1 — Jeu complet simplifié (≈ 50 h)

### Règles et institutions

| # | Tâche | Durée |
|---|---|---|
| 1.02 | ✅ Choix du Premier ministre (3 profils) | 1,5 h |
| 1.03 | ✅ 3 niveaux de difficulté | 1 h |
| 1.04 | ✅ Jauges de scandale et de défiance | 1,5 h |
| 1.05 | ✅ Actions 49.3 et intervention télévisée | 2 h |
| 1.06 | ✅ Budget annuel simplifié : rigueur, équilibre ou relance | 1 h |
| 1.07 | ✅ Motion de censure et changement de Premier ministre | 2 h |
| 1.08 | ✅ Dissolution et législatives, y compris après une réélection | 3 h |
| 1.09 | ✅ Majorité relative après des législatives perdues | 0,5 h |
| 1.10 | ✅ Destitution : procédure, défense, vote aux deux tiers de l'Assemblée | 2 h |
| 1.11 | ✅ Second mandat, usure du pouvoir, thème de campagne | 2 h |

### Contenu

| # | Tâche | Durée |
|---|---|---|
| 1.12 | ✅ 10 lois de plus, soit **15** au total | 2 h |
| 1.13 | ✅ 20 événements de plus, soit **30** au total, avec des chaînes d'événements | 5 h |
| 1.14 | ✅ Relecture (ton, cohérence, textes dans 38 colonnes) | 2 h |

### Scores et sauvegarde

| # | Tâche | Durée |
|---|---|---|
| 1.15 | ✅ Calcul du score (1er mandat, cumul sur 10 ans) | 1,5 h |
| 1.16 | ✅ 2 tableaux de high scores en SRAM, avec contrôle d'intégrité et saisie des initiales | 3 h |

### Écrans et son

| # | Tâche | Durée |
|---|---|---|
| 1.17 | ✅ Écrans budget, groupes parlementaires et aide | 3 h |
| 1.18 | ✅ Bruitages des menus | 2 h |

### Équilibrage et qualité

| # | Tâche | Durée |
|---|---|---|
| 1.19 | ✅ Simulateur sur PC : des milliers de parties automatiques et leurs statistiques | 3 h |
| 1.20 | ✅ Passes d'équilibrage | 4 h |
| 1.21 | ✅ Tests sur la vraie console (PAL) et dans BlastEm (PAL et NTSC) | 2 h | BlastEm OK ; vraie console regroupée en fin de projet |
| 1.22 | ✅ Vérification automatique de la mémoire vive et de la taille de la ROM | 1 h |

---

## V2 — Version enrichie (≈ 70 h, plus la réalisation du pixel art)

| # | Tâche | Durée |
|---|---|---|
| 2.01 | ✅ **Carte de France des 13 régions**, avec la popularité par région | 8 h |
| 2.02 | ✅ Intégration du pixel art : portraits, Élysée, écrans illustrés (le dessin n'est pas compté) | 8 h |
| 2.03 | ✅ Musique (2 ou 3 morceaux XGM2) | 6 h |
| 2.04 | ✅ Cohabitation | 3 h |
| 2.05 | ✅ Sénat, renouvelé par moitié, et pris en compte dans la destitution | 2 h |
| 2.06 | ✅ Référendum | 2 h |
| 2.07 | ✅ Remaniement et ministres un par un | 4 h |
| 2.08 | ✅ 6e pilier, Rayonnement international : diplomatie, Union européenne, sommets | 5 h |
| 2.09 | ⏭️ Reporté (V3) — remplacé par la popularité par région (13 régions, carte) | 2 h |
| 2.10 | ⏭️ Reporté (V3) — le budget en un choix suffit | 3 h |
| 2.11 | ✅ Plus de 100 événements au total | 12 h |
| 2.12 | ✅ Plus de 40 lois au total | 5 h |
| 2.13 | ✅ Sauvegarde d'une partie en cours | 3 h |
| 2.14 | ✅ Équilibrage V2 | 5 h | 71/55/25 % ; 60 tours à ±5 points ; passif < 10 % |
| 2.15 | ✅ Mode 60 tours (reporté de la V1, tâche 1.01) | 3 h |

### V2.1

| # | Tâche | Durée |
|---|---|---|
| 2.1.5 | ✅ Voix digitalisée « For sure ! » à l'accueil, une fois par allumage (Z80 PCM, tools/voice/) | 2 h |

## Questions ouvertes

- ~~Licence du dépôt public~~ → MIT.
- Qui réalisera le pixel art de la V2.
- Source de la musique en V2 : composée ou libre de droits.
