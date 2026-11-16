# Mesures de performance de référence (tâche 4.1.R4)

Relevé du 16/11 sur la base V4.1 (commit `817e65c`, avant toute optimisation), avec la ROM `make rom-perf` et `tools/shots.sh` (BlastEm 0.6.2 sur Xvfb privé, audio factice).

## Méthode

- `make rom-perf DEMO=<n> SCRIPT="..."` produit `out/perf.bin` : comme `rom-demo`, plus `-DMP_DEBUG_PERF`. Tout le code est dans `src/ui/screen.c`, sous `#ifdef MP_DEBUG_PERF` ; la ROM normale n'est pas touchée.
- Affichage en bas à droite (plan A, `ui_text_draw_n`) : **`M<ms>`** = maximum, sur l'écran courant, de la durée d'`update()` + `draw()` d'une image ; **`L<n>`** = images perdues. Les deux sont remis à zéro à chaque changement d'écran.
- `getTick()` ne vaut qu'une image (le compteur SGDK avance au VBlank) : les durées utilisent `getSubTick()` (1/76 800 s, affichage au dixième de milliseconde) ; `getTick()` sert au décompte des images perdues (écart entre deux tours de boucle, moins 1).
- Budget d'une image : 20,0 ms en PAL, 16,7 ms en NTSC.
- Scénarios (manette simulée, `wN` = N dixièmes de seconde) ; capture lue sur l'image à l'instant final du script :

| Écran | Démo | Script |
|---|---|---|
| Tableau de bord | 12 | `w20 D D U U w5 D U` |
| Budget | 3 | `w20 D D D U R R L w5` |
| Électorats | 12 | `w20 C w20 D D U w5` |
| Vote d'une loi (hémicycle) | 12 | `w20 A w10 A w10 A w30` |
| Assemblée (hémicycle) | 12 | `w20 D D D A w10 R R w5` |
| Carte | 12 | `w20 D D D D A w30 R R L w5` |
| Titre | 11 | `w30 S w10 D U w5` |

## Résultats

| Écran | PAL M (ms) | PAL L | NTSC M (ms) | NTSC L |
|---|---|---|---|---|
| Tableau de bord | 9,0 | 5 | 8,9 | 6 |
| Budget | 8,0 | 17 | 7,8 | 19 |
| Électorats | 6,0 | 12 | 6,2 | 13 |
| Vote (hémicycle) | 20,0 | 21 | 21,3 | 28 |
| Assemblée (hémicycle) | 23,0 | 18 | 24,8 | 22 |
| Carte | 3,3 | 2 | 1,6 | 5 |
| Titre (sans sauvegarde) | 6,8 | 9 | 8,1 | 12 |

Lecture : les chiffres sont lus sur de petites captures (un « 0 » et un « 8 » se ressemblent : ±0,2 ms possible sur un chiffre). BlastEm tourne en temps réel sur une machine non dédiée : une image perdue peut aussi venir de l'hôte, et `L` cumule tous les dépassements du tour de boucle entier (y compris `ui_frame_end`, `voice_update`, musique, DMA du VBlank), pas seulement update + draw. Ordres de grandeur fiables, valeurs exactes à reprendre sur console.

## Points de l'audit (§ 5)

| N° | Verdict | Mesure |
|---|---|---|
| P1 | Non mesurable ici | Le fondu de `ui_clear()` ne coûte pas de CPU : c'est un retard d'entrée (150 ms), pas une durée d'image. À juger à la manette. |
| P2 | Partiellement confirmé | Électorats : 6 ms max, sous l'image ; pas de dépassement dû au seul `draw()`. Reste utile pour la fluidité (12-13 images perdues sur le relevé, causes hors update + draw à déterminer). Options et destitution non relevées. |
| P3 | Confirmé, non critique | Budget : 8 ms max sur une séquence de mouvements (≈ 4 ms estimés par appui + dessin), sous l'image. |
| P4 | Confirmé, non critique | Inclus dans les 6 ms des électorats ; pas de mesure isolée. |
| P5 | Non confirmé | Titre sans sauvegarde : 6,8 ms PAL, 8,1 ms NTSC. Le `fletcher16` (≈ 12 ms) n'est exercé qu'avec une sauvegarde en SRAM, absente de ce relevé : à refaire avec une partie sauvegardée. |
| P6 | Non confirmé | Carte : 3,3 ms PAL, 1,6 ms NTSC malgré l'écriture de palette à chaque image. Les points parasites ne se voient pas sur émulateur ; vérification sur console. |
| P7 | **Confirmé, seul vrai dépassement** | Hémicycle : 23,0 ms PAL (> 20,0) et 24,8 ms NTSC (> 16,7), soit 1 à 2 images perdues à chaque dessin ; vote : 20,0 ms PAL, 21,3 ms NTSC. Priorité des optimisations. |
| P8, P9 | Hors périmètre de cette mesure | P8 (centrage) invisible dans ces maxima ; P9 est une taille de code (se mesure avec `out/symbol.txt`, pas avec `MP_DEBUG_PERF`). |

## Limites et suite

- Titre avec sauvegarde (P5), options, destitution et fermeture de l'aide du budget (P2) non relevés.
- Le compteur `L` est élevé partout, y compris sur la carte et le tableau de bord ; avant d'optimiser, séparer le temps passé hors update + draw (ajouter un second maximum sur la boucle entière) pour savoir si l'hôte ou `ui_frame_end` est en cause.
- L'affichage de `M`/`L` coûte lui-même un `ui_text_draw_n` par image (hors de la fenêtre mesurée).

## Après P7 (hémicycle, `ui_hemi.c`)

Mêmes scénarios et même méthode (`make rom-perf`, BlastEm, M = maximum d'update + draw).

| Écran | Avant PAL M | Après PAL M | Avant NTSC M | Après NTSC M |
|---|---|---|---|---|
| Vote (hémicycle) | 20,0 ms | 9,0 ms | 21,3 ms | 8,9 ms |
| Assemblée (hémicycle) | 23,0 ms | 9,0 ms | 24,8 ms | 10,0 ms |

Objectif M < 16 ms atteint. Changements : table rang -> groupe construite en un parcours (plus de recherche par pixel pour les 577 sièges) ; tuiles générées 24 par image dans `ui_hemi_update` (tampon statique de 768 o + table de 577 o) et envoyées par `DMA_QUEUE`, au lieu des ≈ 170 envois CPU d'un coup ; palettes en `DMA_QUEUE` au changement d'état. L'hémicycle se complète en 7 images (≈ 120 ms), pendant le fondu d'entrée ; le résultat final est identique au pixel près (`shots_compare` : 2 captures identiques, vote et Assemblée). Le compteur `L` reste à 16-27 comme sur les écrans légers : il vient du tour de boucle hors update + draw ou de l'hôte, pas de l'hémicycle. RAM statique : 7 936 o.

## Correctif P7 : tuiles parasites au fondu d'entrée (`ui_hemi.c`)

Défaut : la carte de tuiles (tilemap) était posée tuile par tuile pendant la génération (24 par image), donc le fondu d'entrée montrait un demi-cercle en construction avec des tuiles non prêtes (couleurs aberrantes) sur 1 à 2 images. Correctif : la carte n'est posée (`place_map`) qu'à l'image suivant l'envoi du dernier lot de tuiles (le DMA a eu lieu au vblank entre-temps) ; `ui_hemi_update` ne signale « terminé » qu'ensuite. Aucune tuile non prête n'est donc visible : l'hémicycle apparaît d'un bloc, ≈ 7 images après l'entrée dans l'écran.

Mesures (`make rom-perf`, vote et Assemblée) : vote PAL M 9,1 ms, NTSC M 9,0 ms ; Assemblée PAL M 9,1 ms, NTSC M 10,0 ms. Gain de P7 conservé (M < 16 ms). Captures fines 4,5 à 5 s du script `w20 A w10 A w10 A w30` : plus de tuiles parasites ; rendu final inchangé (même génération, même carte). La dissolution utilise le même `ui_hemi_draw` : non capturée séparément.

Note : `UI_HEMI_SENATE` n'est appelé par aucun écran (code mort probable), conservé.

## Compteur L (images perdues) : enquête (lot U4)

Question : pourquoi `L` restait à 2-28 même sur la carte et le tableau de bord ? Méthode : instrumentation temporaire de la boucle (relevé des `getTick()` aux points de la boucle, à l'image où une perte est détectée), ROM `make rom-perf`, BlastEm PAL et NTSC.

**1. Artefact de mesure (corrigé).** La première image de chaque écran dessine tout l'écran alors que l'affichage est encore coupé (`ui_transition_hide`) : elle dure 2 à 7 images (PAL : 6 sur le tableau de bord, 5 au budget, 2 sur la carte), invisibles puisque l'écran est noir et fondu en entrée. Elle était comptée dans `L` (5 à 6 images perdues sur chaque écran) et dans `M` (où `getSubTick()` n'est pas fiable affichage coupé : le `M9,0` du tableau de bord était cette première image). Correction dans `screen.c` (`MP_DEBUG_PERF` seulement) : la première image est comptée à part, affichée **`E<n>`** (en images) ; `M` et `L` ne portent que sur les images suivantes. Ni VBlank, ni `SYS_doVBlankProcess` (au plus 1 image d'attente), ni DMA de fin de boucle ne sont en cause : le reste de la boucle (voix, visages, sons, musique, `ui_frame_end`) dure moins de 0,5 ms.

**2. Vraies images perdues : les images de dessin après un appui.** Le tableau de bord au repos (et ses deux appuis), la carte au repos : `L` = 0 à 2. Sur le budget, `L` = 13 pour 7 appuis (`N` = 7 événements de 1 à 3 images chacun) : chaque perte tombe sur l'image où l'écran se redessine après un appui. Le relevé de `getTick()` montre que c'est `draw()` (parfois `update()`) qui s'étale sur 2 à 3 VBlank ; ni `sfx_play` (essai avec le son coupé : même résultat), ni l'affichage de `M`/`L` (affiché 1 image sur 16 : même résultat) ne sont en cause.

**3. `M` (getSubTick) sous-estime ces dessins.** Au même instant, `getSubTick()` annonce 5,7 ms alors que `getTick()` compte 2 à 3 images (40 à 60 ms en PAL). Cause non élucidée (hypothèse : le 68000 est gelé par les écritures VDP en plein affichage, ce que le compteur de ligne ne restitue pas fidèlement). On ajoute donc **`P<n>`** : maximum d'update + draw en images entières, d'après `getTick()` (résolution d'une image, mais fiable). Ordre de grandeur : `M` ne doit plus être lu seul.

Relevé après correction (mêmes scénarios que plus haut ; PAL lu net, NTSC lu sur des captures rognées : ±1) :

| Écran | PAL M (ms) | PAL L | PAL E | PAL P | NTSC L | NTSC P |
|---|---|---|---|---|---|---|
| Tableau de bord | 0,8 | 0 | 5 | 0 | 0 | 0 |
| Budget | 6,0 | 13 | 5 | 3 | 14 | 2 à 3 |
| Électorats | 6,1 | 9 | 4 | 3 | 10 | 2 à 3 |
| Vote (hémicycle) | 7,3 | 16 | 6 | 3 | 20 | 5 |
| Assemblée (hémicycle) | 8,1 | 11 | 6 | 4 | 17 | 6 |
| Carte | 1,5 | 2 | 2 | 0 | 7 | 1 |
| Titre (sans sauvegarde) | 6,8 | 7 | 3 | 3 | 9 | 4 |

**Conclusion.** Il y a de vraies images perdues, mais seulement sur les images qui redessinent un écran chargé après un appui (budget, électorats, hémicycle, titre) : 2 à 4 images en PAL, davantage en NTSC (images plus courtes). Les écrans légers sont à 0-2. Aucune optimisation « simple et sûre » n'a été faite : le coût réel de ces `draw()` n'est pas dans `M` et le détail (quelles écritures VDP) reste à profiler. Pistes : redessiner seulement la ligne modifiée au lieu du panneau entier (budget : `ui_clear_rect` + textes complets), étaler le dessin sur plusieurs images comme pour l'hémicycle (P7), ou passer par `DMA_QUEUE` plutôt que par des écritures CPU en plein affichage. Les `M` du tableau « Après P7 » ci-dessus sont à relire avec cette réserve ; la valeur de `L` sur console reste à mesurer.

## R6 -Os (audit P9)

**Montage.** `tools/sgdk_os.mk` inclut `makefile.gen` de SGDK et ajoute deux règles par fichier : `$(OUT)/src/screens/%.o: CFLAGS += -Os` et `$(OUT)/src/game/game.o: CFLAGS += -Os`. Le `Makefile` l'appelle (`DOCKER_REL`, `--entrypoint make ... -f /src/tools/sgdk_os.mk release`) pour `rom`, `rom-demo`, `rom-perf` et `rom-annee4` ; `make SCREENS_OS=0` revient au release SGDK standard (tout en -O3). Aucun `#pragma`, aucun fichier de `src/` touché.

**Vérification de l'application.** La ligne de compilation (`make` sans silence) montre `-O3 ... -flto -ffat-lto-objects -Os` pour chaque `src/screens/*.c` et `game.c` ; le dernier niveau gagne. Avec la LTO, GCC conserve le niveau d'optimisation par fonction (attribut enregistré dans l'IR) : l'édition de liens sans `-O` ne les ré-optimise pas en -O3. Preuve dans `out/symbol.txt` : `_etext` passe de `0x5cae4` à `0x4f290` (-52 Ko), ce qui ne s'explique que par un code réellement compacté.

**Taille** (taille de fonction = écart avec le symbole suivant, en octets) :

| | -O3 (avant) | -Os (après) |
|---|---|---|
| `_etext` | 0x5cae4 (379 620) | 0x4f290 (324 240), -55 380 |
| `game_update` | 13 890 | 1 758 |
| `game_draw` | 5 818 | 4 882 |
| `budget_draw` | 12 330 | 5 616 |
| `election_init` | 5 558 | 228 |

(Les écarts de `game_update` et `election_init` viennent aussi de l'inlining : en -Os, GCC n'incorpore plus ces fonctions dans leurs appelants.) La ROM reste à 1 179 648 o (arrondie par `sizebnd` à 128 Ko).

**Fluidité** (`make rom-perf`, PAL, mêmes scénarios, capture à 9 s ; M en ms) :

| Écran | -O3 M / L / P | -Os M / L / P |
|---|---|---|
| Tableau de bord | 0,8 / 0 / 0 | 0,5 / 0 / 0 |
| Budget | 6,0 / 13 / 3 | 6,2 / 13 / 3 |
| Électorats | 6,3 / 9 / 3 | 6,5 / 9 / 3 |
| Vote | 8,0 / 16 / 4 | 8,0 / 16 / 4 |
| Titre | 8,8 / 8 / 4 | 8,6 / 8 / 4 |

**Verdict : -Os conservé.** Gain de 52 Ko de ROM, L et P identiques partout, M dans le bruit de lecture (±0,2 ms). Les images perdues observées ne viennent donc pas du calcul du processeur (cohérent avec l'enquête « Compteur L »). Vérification sur console à faire avant de clore la version.
