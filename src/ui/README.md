# Couche d'interface (src/ui)

Seul module autorisé à appeler `VDP_*` et `PAL_*`. Le jeu dessine avec `ui.h`,
lit la manette avec `input.h` et s'organise en écrans avec `screen.h`.

## Plans et coordonnées
- Écran 320×224 = 40×28 tuiles de 8×8 ; texte sur 38 colonnes (x = 1 à 38).
- **Plan B** : fenêtres (cadre et fond). **Plan A** : texte, jauges, icônes.
  Le fond des glyphes est transparent : le texte s'affiche sur le fond de la fenêtre.
- Tuiles chargées en VRAM à partir de `TILE_USER_INDEX` : police, cadre, jauge, icônes.

## Ressources (res/gfx_ui.res) : noms stables
En V2, remplacer les PNG de `res/gfx/` en gardant **noms, dimensions et indices de couleur** :
le code ne change pas. PNG indexés 16 couleurs (4 bits).

| Nom | Fichier | Taille | Contenu |
|---|---|---|---|
| `ts_ui_font` | `gfx/font.png` | 128×64 | 121 glyphes, 16 par ligne : ASCII 32-126, puis les accents dans l'ordre de `tools/font/glyphs.txt` |
| `ts_ui_frame` | `gfx/ui_frame.png` | 24×24 | cadre 3×3 tuiles : coins, bords, intérieur |
| `ts_ui_gauge` | `gfx/ui_gauge.png` | 72×8 | 9 tuiles : 0 à 8 pixels remplis |
| `ts_ui_icons` | `gfx/ui_icons.png` | 32×8 | curseur, flèche haut, flèche bas, puce |
| `pal_ui_text` | `gfx/pal_ui_text.png` | 16 couleurs | ligne 0 : texte normal, cadres, jauge positive (verte) |
| `pal_ui_highlight` | `gfx/pal_ui_highlight.png` | 16 couleurs | ligne 1 : sélection, titres (jaune), jauge dorée |
| `pal_ui_dim` | `gfx/pal_ui_dim.png` | 16 couleurs | ligne 2 : texte atténué, jauge neutre (bleue) |
| `pal_ui_alert` | `gfx/pal_ui_alert.png` | 16 couleurs | ligne 3 : alerte (rouge), jauge négative |

Un style (`UiStyle`) = une ligne de palette : les mêmes tuiles changent de couleur
selon la palette. Les 4 palettes suivent donc la **même convention d'indices** :

| Indice | Rôle |
|---|---|
| 0 | transparent (ligne 0 : couleur de fond de l'écran) |
| 1 | intérieur des fenêtres |
| 2, 3, 4 | cadre clair, moyen, sombre |
| 5 | encre du texte (varie selon le style) |
| 6 | ombre du texte |
| 7, 8 | jauge remplie, reflet (varie selon le style) |
| 9, 10 | jauge vide, bord de jauge |
| 11 | curseur et icônes |
| 12-15 | libres |

Les images provisoires sont produites par `python3 tools/font/make_gfx.py`
(Python 3 seul). La police s'édite dans `tools/font/glyphs.txt` (base : police
SGDK) ; le script génère aussi `src/ui/ui_font_map.h` (Unicode → glyphe).

## Texte
- Chaînes C en UTF-8. Glyphes : ASCII 32-126, é è ê ë à â ç î ï ô ù û ü œ É È Ê À Ç Ô « » ° € ’ ….
  Équivalences : tirets – — → `-`, ‘ → `'`, “ ” → `"`. Autre caractère → `?`.
- Espace insécable U+00A0 (ou fine U+202F) : affichée comme une espace, jamais coupée
  (à mettre avant ; : ! ? » % € et après «).
- `ui_text_wrap` coupe sur les mots entiers ; `\n` force un saut de ligne.

## Manette
`input_pressed()` = front ; `input_repeat()` = front + répétition des directions
(330 ms puis toutes les 100 ms, en temps réel : identique en PAL et NTSC).
Valider : A ou C (`INPUT_OK`) ; annuler : B.

## Écrans
`screen_register(id, &screen)` puis `screen_run(premier)`. Chaque image :
`input_update`, `update`, `draw`, synchro verticale. `screen_change` est appliqué
à l'image suivante (`exit`, effacement de l'écran, `init`). Les tuiles restent
affichées : `draw` ne redessine que ce qui a changé.

## Carte de France (src/ui/ui_map.h, res/gfx_map.res)
Carte des 13 régions : 20×20 tuiles (160×160 px) sur le **plan B**, avec **une ligne de
palette entière**. Un indice de couleur par région : colorer une région = changer UNE
couleur de palette (`ui_map_set_region_level`, dégradé rouge → jaune → vert), sans
redessiner de tuile. `ui_map_highlight` fait clignoter la région choisie (appeler
`ui_map_update` à chaque image).

**Palettes** : les 4 lignes sont prises par les styles. La carte occupe **PAL2** pendant
son affichage : ne pas utiliser `UI_STYLE_DIM` / `UI_GAUGE_NEUTRAL` sur l'écran de la
carte ; `ui_map_close()` restaure `pal_ui_dim`. Utiliser les styles TEXT, HIGHLIGHT, ALERT.
Les tuiles de la carte sont chargées juste après celles de `ui.c` (400 tuiles environ).

| Nom | Fichier | Contenu |
|---|---|---|
| `ts_map_france` | `gfx/map_france.png` 160×160 | carte ; indices 1-13 = régions, 14 = frontière, 0 = mer (transparent) |
| `pal_map` | `gfx/pal_map.png` | 0 mer, 1-13 régions (ordre de `REGION_*`), 14 frontière, 15 libre |

Régions dans l'ordre de l'enum `Region` : Hauts-de-France, Normandie, Île-de-France,
Grand Est, Bretagne, Pays de la Loire, Centre-Val de Loire, Bourgogne-Franche-Comté,
Nouvelle-Aquitaine, Auvergne-Rhône-Alpes, Occitanie, PACA, Corse.

Provisoire : `python3 tools/map/make_map.py` génère les PNG depuis `tools/map/regions.txt`
(une lettre par case de 4×4 px, `.` = mer ; lettres H N I E B L C U A R O P K). Pour la V2,
remplacer `map_france.png` en gardant dimensions et indices.

## Illustrations (src/ui/ui_art.h, res/gfx_art.res) — tâche 2.02
Pixel art caricatural façon BD, **généré par programme** : `python3 tools/art/make_art.py`
(Python 3 seul) écrit `res/gfx/art_*.png` et `res/gfx_art.res`. Tous les personnages sont
fictifs. Dessins : `tools/art/title.py`, `portraits.py`, `vignettes.py` + `vignettes2.py`,
`decors.py` ; primitives (ellipses, polygones, contours d'encre, tramage) : `tools/art/raster.py`.
`--preview DIR` écrit aussi des planches agrandies.

API (`ui_art.c` est le seul fichier qui touche VDP/PAL pour les illustrations) :
- `ui_art_draw(id, x, y)` : dessine sur le **plan B** (coin haut gauche en tuiles), palette
  chargée dans **PAL2** ; `ui_art_draw_pal(id, x, y, PAL1|PAL2|PAL3)` choisit la ligne.
- `ui_art_size(id, &w, &h)` (en tuiles), `ui_art_erase(id, x, y)` (efface le plan B).
- `ui_art_close()` : **à appeler en quittant l'écran** (et avant `ui_map_draw`) ; restaure
  `pal_ui_highlight` / `pal_ui_dim` / `pal_ui_alert` et libère la VRAM.
- Macros : `ART_PRES(ORIENT_*)`, `ART_PM(PM_*)`, `ART_COHAB(camp)`, `ART_CAND(CAND_*)`.

**Palettes** : chaque illustration a sa palette (15 couleurs + transparent). Les **14 portraits
partagent la même palette** : plusieurs portraits tiennent sur une seule ligne. Une ligne
empruntée n'est plus utilisable pour son style (PAL1 : HIGHLIGHT, PAL2 : DIM / jauge
neutre, PAL3 : ALERT). Deux illustrations de palettes différentes = deux lignes
(ex. portrait en PAL2 + vignette en PAL3 : n'utiliser alors que TEXT et HIGHLIGHT).

**VRAM** : tuiles chargées juste après celles de `ui.c` (même zone que la carte, jamais
affichées ensemble), empilées si plusieurs illustrations (16 au plus) ; une illustration
déjà chargée est réutilisée. Limite : `TILE_USER_MAX_INDEX` (plus de 1 000 tuiles libres ; après dédoublonnage l'écran
titre en prend 341, un portrait ≈ 43, une vignette ≈ 46, l'hémicycle 120, le journal 83).
ROM : ≈ 64 Ko pour les 33 illustrations.
Images non compressées (DMA direct depuis la ROM, aucun tampon en RAM).

| Id | Taille (tuiles) | Contenu |
|---|---|---|
| `ART_TITLE` | 40 × 16 | palais, drapeau, président qui salue, gardes, journalistes ; lignes 0-2 = ciel uni pour le titre |
| `ART_PRES_GAUCHE/CENTRE/DROITE` | 7 × 7 | le président selon l'orientation |
| `ART_PM_TECHNOCRATE/RASSEMBLEUR/REFORMATEUR` | 7 × 7 | Delaunay, Deschamps, Maréchal |
| `ART_COHAB_GAUCHE/CENTRE/DROITE` | 7 × 7 | Carrel, Lacombe, de Montclar (PM d'opposition) |
| `ART_CAND_ROUSSEL/VASSEUR/MARCHETTI/VALMONT/LENOIR` | 7 × 7 | candidats, ordre `CAND_*` |
| `ART_EVT_*` (15) | 8 × 6 | grève, canicule, inondation, scandale, sommet, bourse, santé, pollution, sécurité, numérique, éducation, énergie, agriculture, industrie, fête |
| `ART_DECOR_HEMICYCLE` | 20 × 9 | Assemblée en séance (votes) |
| `ART_DECOR_URNE` | 8 × 8 | urne, « A VOTÉ » (élection, référendum) |
| `ART_DECOR_JOURNAL` | 38 × 5 | bandeau « Le Quotidien du Matin » (rapport) |

Pour retoucher : modifier le script du dessin, relancer `make_art.py`, recompiler. L'ordre des
`IMAGE` de `gfx_art.res` doit rester celui de l'enum `ArtId` et du tableau de `ui_art.c`.
