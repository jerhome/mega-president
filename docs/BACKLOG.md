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
| **V3 — Le beau** | Interface moderne, gueules mémorables, musique FM « chill » par écran | ≈ 117 h | ≈ 290 h |
| **V4** | 5 électorats (ex-2.09), budget détaillé (ex-2.10) — voir [SPEC_V4.md](SPEC_V4.md) | ≈ 44 h | ≈ 335 h |

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
| 2.09 | ⏭️ Reporté (V4) — remplacé par la popularité par région (13 régions, carte) | 2 h |
| 2.10 | ⏭️ Reporté (V4) — le budget en un choix suffit | 3 h |
| 2.11 | ✅ Plus de 100 événements au total | 12 h |
| 2.12 | ✅ Plus de 40 lois au total | 5 h |
| 2.13 | ✅ Sauvegarde d'une partie en cours | 3 h |
| 2.14 | ✅ Équilibrage V2 | 5 h | 71/55/25 % ; 60 tours à ±5 points ; passif < 10 % |
| 2.15 | ✅ Mode 60 tours (reporté de la V1, tâche 1.01) | 3 h |

## V2.1 — Correctifs après le premier test sur console (15/11)

Retour du joueur sur Evergenesis : bonne impression générale, premier mandat non terminé à cause d'un plantage. Les autres remarques du joueur seront ajoutées ici.

| # | Tâche | Durée | État |
|---|---|---|---|
| 2.1.1 | **Plantage au passage à la 4e année** (« Offset 360200, Hint 020446 »). 0x020446 se trouve dans `MEM_pack` (tas de SGDK), sur la lecture d'un en-tête de bloc : le tas est probablement corrompu, soit par un débordement de pile, soit par un dépassement de tampon. Étapes : reproduire avec une ROM de debug et une partie automatique jusqu'à l'année 4, journaliser `MEM_getFree` (KLog) et la profondeur de pile, chercher qui appelle `MEM_alloc` ou `MEM_pack` pendant le jeu (piste : `VDP_setPlaneSize` dans `ui.c` recharge la police et appelle `MEM_pack`), puis corriger et ajouter un test de non-régression | 4 h | ✅ Cause : ligne « Bloc présidentiel au Sénat : 210 -> 206 » (41 octets UTF-8) dans un tampon de 40 sur la pile, au renouvellement du Sénat (début de l'année 4). Tampons de ligne portés à 80 octets, garde-fou `-fstack-protector-strong`, ROM de test `make rom-annee4` |
| 2.1.2 | **Lois déjà adoptées repérables** : coche ✓ devant une loi adoptée et marque « en cours » pour une loi votée mais pas encore appliquée. Aujourd'hui, une loi adoptée est seulement grisée, comme une loi trop chère. Vérifier aussi la confusion possible entre « Cantine gratuite » et « Agriculture bio », dont la description parle des cantines, et renommer cette dernière si besoin | 1 h | ✅ |
| 2.1.3 | **Capital politique plus lisible** : le rythme actuel est voulu (1 à 3 CP par trimestre, aucun apport lié au budget). En mode 60 tours, l'apport ne tombe que tous les 3 mois, d'où l'impression que les CP ne se rechargent pas. Afficher « +X CP » au rapport et « prochain apport dans N mois » au tableau de bord ; augmenter le gain si le ressenti ne change pas | 1 h | ✅ |
| 2.1.4 | **Une musique par année du mandat** (5 thèmes, 5 ambiances), pour varier et donner une signature sonore à chaque année | 4 h | ✅ |
| 2.1.5 | **Voix digitalisée « For sure ! »** sur l'écran d'accueil uniquement, au démarrage : voix de synthèse générée par nos soins (pas d'imitation d'une vraie voix), échantillon PCM joué par le Z80. Vérifier qu'elle se mélange bien à la musique PSG et contrôler la taille de la ROM | 2 h | ✅ |
| 2.1.6 | Autres remarques du joueur | — | ✅ Aucune autre remarque (15/11) |
| 2.1.8 | **Voix « For sure ! » inaudible** (2e retour console) : le pilote Z80 était déchargé dès la 1re image, car `SND_PCM_isPlaying()` répondait « arrêté » avant que le Z80 ait pris la commande. Attente fondée sur la durée de l'échantillon (`getTick`), vérifiée sur le canal DAC de BlastEm (`-y`) | 0,5 h | ✅ |
| 2.1.9 | **Pied de page de l'écran d'accueil** : nom de l'auteur, version (`src/version.h`) et « Développé avec Claude ». À garder dans les versions suivantes | 0,5 h | ✅ |
| 2.1.10 | **Retour au menu principal en cours de partie** : dernière ligne du menu de jeu, avec confirmation ; sauvegarde l'état exact (CP dépensés compris), puis écran d'accueil (B : reprendre, START : nouvelle partie) | 0,5 h | ✅ |
| 2.1.7 | Nouveau test sur console : deux mandats complets | 1 h | ✅ Deux mandats complets sur Evergenesis le 15/11 : voix, musiques, lois, passage de la 4e année, réélection, fin de partie OK |

## V3 — Le beau (≈ 117 h)

On passe tout son temps dans les écrans : ils doivent être superbes. Personnages avec de vraies « gueules » dont on se souvient, musique FM douce avec des basses, un thème par écran.

**Contrat de palettes (V3)** : l'interface n'utilise plus que **PAL0 et PAL1** ; **PAL2 et PAL3 sont réservées aux illustrations** (portrait principal en PAL2, seconde illustration ou vignette en PAL3), 15 couleurs chacune.

### Interface (agent `md-dev` interface)
| # | Tâche | Durée | État |
|---|---|---|---|
| 3.01 | Charte graphique : palettes PAL0/PAL1, couleurs de texte, cadres, ombres, dégradés ; document `docs/CHARTE_V3.md` | 6 h | ✅ |
| 3.02 | Nouvelles tuiles d'interface : cadres arrondis et ombrés, jauges, icônes, séparateurs, fonds texturés | 8 h | ✅ |
| 3.03 | Police retravaillée (lisibilité, accents, styles) | 4 h | ✅ |
| 3.04 | Animations : fondus entre écrans, curseur animé, apparition des fenêtres | 4 h | ✅ |
| 3.05 | Refonte : tableau de bord, journal, événement | 8 h | ✅ |
| 3.06 | Refonte des 12 autres écrans (titre, élection, campagne, budget, référendum, remaniement, destitution, carte, fin, scores, aide, 2nd mandat) | 10 h | ✅ |

### Gueules (agent `md-dev` illustrations)
| # | Tâche | Durée | État |
|---|---|---|---|
| 3.07 | Générateur de portraits V3 : grand format, 15 couleurs, éclairage, caricature marquée, 2-3 expressions | 8 h | ✅ Validé par le joueur (15/11) |
| 3.08 | Gueules des 14 personnages existants (présidents, PM, cohabitation, candidats) | 12 h | ✅ |
| 3.09 | Nouvelles gueules : ministres, chefs de parti, journalistes, président du Sénat… | 12 h | ✅ |
| 3.16 | Écran de démarrage redessiné dans le style décalé des gueules (bon enfant, drôle, pointe d'humour de gauche) | 3 h | ✅ |
| 3.17 | Vignettes d'événement et décors redessinés dans le style V3 ; hémicycle dynamique (sièges colorés, vote en direct) ; passe sur tout ce qui fait « cheap » | 8 h | ✅ |
| 3.18 | **Visages animés sans son**, à la Dune : clignements irréguliers, regard qui s'échappe, changement d'expression masqué par un clignement, bouche qui bouge sur les répliques ; une seule gueule animée à la fois (consigne DA, REVUE_DA_V3 § 6) | 8 h | ✅ relu par la DA (4 retouches) |
| 3.19 | **Écran Options** soigné (avancé de la V4 à la demande du joueur) : son, affichage, jeu, données ; accès depuis le titre et le menu de jeu ; réglages mémorisés en SRAM. Conception DA (docs/SPEC_OPTIONS.md), puis md-dev ; le réglage des voix servira en V4 | 9,5 h | ✅ (relecture DA à faire) |
| 3.10 | Intégration des portraits et expressions dans les écrans | 3 h | ✅ |

### Musique (agent `md-dev` son)
| # | Tâche | Durée | État |
|---|---|---|---|
| 3.11 | Passage à la puce FM (YM2612) via XGM2 : outil de composition en code → VGM → XGM2, instruments (basses, nappes, pianos électriques), voix « For sure ! » et bruitages compatibles | 10 h | ✅ |
| 3.12 | Musiques chill : titre, 5 années, élection | 10 h | ✅ |
| 3.13 | Un thème par écran avec un petit cachet lié à ce qu'on voit (journal, Assemblée, carte, budget, référendum, remaniement, destitution, campagne, fin, scores, aide) | 14 h | ✅ |
| 3.14 | Intégration : musique par écran, enchaînements | 3 h | ✅ |

### Jalon V3
| # | Tâche | Durée | État |
|---|---|---|---|
| 3.15 | Budgets ROM/RAM/VRAM, PAL et NTSC, test console du joueur, release | 5 h | ✅ ROM v3.0 livrée (étiquette v3.0, out/mega-president-v3.0.bin) ; test console du joueur le 16/11, retours en V3.1 si besoin |

## V4 — 5 électorats et budget détaillé (≈ 50 h)

Spécification validée : [SPEC_V4.md](SPEC_V4.md). Développement sur la branche `v4-dev`, fusionnée dans main après la sortie de la V3.

| # | Tâche | Durée | État |
|---|---|---|---|
| 4.01 | Arbitrage des questions ouvertes et mise à jour de la spec | 0,5 h | ✅ |
| 4.02 | Cœur 5 électorats : enum, `elec_weight`, tables d'opinion, popularité, opinion de départ, `sim_core_opinion` (législatives, Sénat), référendum, thèmes, clés `EFF_POP_RETRAITES/JEUNES`, tests unitaires | 4 h | ✅ |
| 4.03 | Régions à 5 poids | 1 h | ✅ |
| 4.04 | Générateur de contenu : nouvelles clés, alias `POP_CADRES`, `LAW_MAX_IMMEDIATE` = 4, contrôle des limites | 1 h | ✅ |
| 4.05 | Passe sur les 40 lois (effets immédiats sur 5 électorats) | 3 h | ✅ |
| 4.06 | Passe sur environ 50 événements et 6 nouveaux événements (retraités, jeunes) | 4 h | ✅ |
| 4.07 | Écrans électorats : ligne du tableau de bord avec tendances, écran détail (C), référendum et campagne à 5 lignes, aide | 3 h | ✅ |
| 4.08 | Cœur budget détaillé : `budget_lvl[6]`, tables, multiplicateur, préréglages, reconduction par services votés, garde-fous, orientation de vote, 49.3, aperçus, tests | 5 h | ✅ |
| 4.09 | Budget en cohabitation (plan du PM, ligne défense) et amendement du Sénat (lecture définitive à 1 CP), comptes rendus | 2,5 h | ✅ |
| 4.10 | Textes : noms des lignes, aide C, phrases du journal (reconduction, Sénat, budget du PM), phrases de l'écran électorats | 1,5 h | ✅ |
| 4.11 | Écran budget détaillé (6 lignes × 5 crans, préréglage, aperçu en direct, vote / 49.3, cohabitation, Sénat) | 5 h | ✅ |
| 4.12 | Sauvegarde : `GAMESAVE_VERSION` = 2, message à l'écran titre, test de non-régression | 1 h | ✅ |
| 4.13 | Joueurs automatiques : préréglages, variantes optimiseur, extrême et austère, statistiques par électorat | 3 h | ✅ |
| 4.14 | Équilibrage V4 (cibles de §3.4, 20 et 60 tours) | 5 h | ✅ |
| 4.15 | Documentation : REGLES (V4), SIM, DECISIONS, BACKLOG | 1,5 h | ✅ |
| 4.16 | Contrôle qualité : `make check`, budgets RAM et ROM, BlastEm PAL et NTSC, 38 colonnes, deux mandats, cohabitation forcée, sauvegarde | 2 h | ✅ (0 bloquant ; gênants corrigés avec les retouches DA, fusionnés le 15/11) |
| 4.17 | Jalon : test console du joueur, étiquette git, release | 1 h |  |
| 4.18 | **Voix « charabia » des personnages** (à la Sims) : **2 syllabes très accueillantes** par réplique, timbre propre à chaque personnage, lèvres synchronisées puis seuls les yeux bougent ; **voix désactivables** dans les options (choix du joueur, 15/11 ; détail DA : REVUE_DA_V3 § 7) | 10 h | ✅ (écoute du timbre : test console du joueur) |
| 4.20 | Écran événement : réplique du ministre concerné (gueule + charabia) à la phase « Votre décision » (avis DA, REVUE_DA_V3 § 8.3) | 3 h | ✅ (nom du ministre à l\'écran : à revoir avec la DA) |
| 4.21 | **Passe humour** de tous les textes par l'agent `vannes` (journaux, événements, lois, accroches, répliques, aides) : curseur « au-delà de Ron Gilbert, en frisant Radio Nova, avec subtilité » | 4 h | ✅ (accroches en attente du joueur) |
| 4.22 | Relecture DA de l'écran Options (huissier, icône cartouche, mise en page, lignes grisées) et retouches | 1,5 h | ✅ |
| 4.23 | ⏭️ Reporté — logo dessiné de l'écran titre : le joueur valide l'écran titre actuel (15/11) | 3 h |

## V4.1 — Relecture et factorisation du code (≈ 18 h)

Après le test console de la V4. Relecture par un développeur de haut niveau (Opus), sans assembleur : C uniquement, `src/sim/` reste portable. Aucun changement de règle ni de contenu.

| # | Tâche | Durée | État |
|---|---|---|---|
| 4.1.01 | Captures de référence de chaque écran (ROM de démo) et comparaison automatique avant/après | 2 h | ✅ (outils prêts ; références à générer une fois les écrans stabilisés) |
| 4.1.02 | Audit du code (Opus) : doublons, sûreté mémoire, pile, PAL/NTSC, sauvegarde, cas limites ; rapport classé | 3 h | 🔄 |
| 4.1.03 | Factorisation de l'interface : widgets communs (cadres, listes, jauges, boucle de saisie, boîtes) | 6 h |  |
| 4.1.04 | Corrections de l'audit : robustesse, bornes, simplifications | 3 h |  |
| 4.1.05 | Mesures (temps des écrans, ROM, RAM) et optimisations simples en C si un écran rame | 2 h |  |
| 4.1.06 | Contrôle qualité, comparaison des captures, test console, release | 2 h |  |

## Questions ouvertes

- ~~Licence du dépôt public~~ → MIT.
- Qui réalisera le pixel art de la V2.
- Source de la musique en V2 : composée ou libre de droits.
