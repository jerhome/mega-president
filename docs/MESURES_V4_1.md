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
