# Couche d'interface (src/ui)

Seul module autorisé à appeler `VDP_*` et `PAL_*`. Le jeu dessine avec `ui.h`,
lit la manette avec `input.h` et s'organise en écrans avec `screen.h`.

## Plans et coordonnées
- Écran 320×224 = 40×28 tuiles de 8×8 ; texte sur 38 colonnes (x = 1 à 38).
- **Plan B** : fenêtres (cadre et fond). **Plan A** : texte, jauges, icônes.
  Le fond des glyphes est transparent : le texte s'affiche sur le fond de la fenêtre.
- Tuiles chargées en VRAM à partir de `TILE_USER_INDEX` : police, cadre, jauge, icônes.

## Ressources (res/gfx_ui.res) : noms stables — charte V3
Charte complète : `docs/CHARTE_V3.md`. Tout est généré par `python3 tools/ui/make_ui.py`
(`make gfx`) : PNG indexés 16 couleurs, plus `src/ui/ui_font_map.h` (Unicode → glyphe)
et `src/ui/ui_tiles.h` (indices des tuiles de décor, palettes des icônes).

| Nom | Fichier | Contenu |
|---|---|---|
| `ts_ui_font` | `gfx/font.png` 128×128 | 2 jeux de 128 glyphes : encre A (indices 2/3), encre B (indices 4/5) ; ordre de `tools/font/glyphs.txt` |
| `ts_ui_frame` | `gfx/ui_frame.png` | décor du plan B : fonds d'écran, fenêtre, bandeaux, barre de sélection, filet, médaillon (enum `UI_T_*`) |
| `ts_ui_gauge` | `gfx/ui_gauge.png` | jauges à reflet : 2 jeux × (bout gauche, milieu, bout droit) × 9 remplissages |
| `ts_ui_icons` | `gfx/ui_icons.png` | 32 icônes (`UiIcon`) ; chargées **en dernier** (les illustrations suivent) |
| `ts_ui_cursor` | `gfx/ui_cursor.png` | 4 images du curseur animé (copiées dans la tuile du curseur) |
| `pal_ui_text` | PAL0 | encre ivoire / gris bleuté, jauges verte / bleue, fenêtres, fond |
| `pal_ui_highlight` | PAL1 | encre or / rouge, jauges or / rouge, bandeaux |
| `pal_ui_dim`, `pal_ui_alert` | — | copies de PAL0, rechargées par `ui_art_close()` dans PAL2/PAL3 |

**Contrat V3** : l'interface n'utilise que PAL0 et PAL1 ; PAL2 et PAL3 sont aux
illustrations (portrait en PAL2, vignette ou second décor en PAL3). Un style
(`UiStyle`) = ligne de palette (bit 0) + jeu d'encre de la police (bit 1) : TEXT
(PAL0, A), HIGHLIGHT (PAL1, A), DIM (PAL0, B), ALERT (PAL1, B). Tous les styles
sont utilisables quelles que soient les illustrations affichées.

## Texte
- Chaînes C en UTF-8. Glyphes : ASCII 32-126, é è ê ë à â ç î ï ô ù û ü œ É È Ê À Ç Ô « » ° € ’ … ✓ → ·.
  Équivalences : tirets – — → `-`, ‘ → `'`, “ ” → `"`. Autre caractère → `?`.
- Espace insécable U+00A0 (ou fine U+202F) : affichée comme une espace, jamais coupée
  (à mettre avant ; : ! ? » % € et après «).
- `ui_text_wrap` coupe sur les mots entiers ; `\n` force un saut de ligne.

## Manette
`input_pressed()` = front ; `input_repeat()` = front + répétition des directions
(330 ms puis toutes les 100 ms, en temps réel : identique en PAL et NTSC).
Valider : A (`INPUT_OK`) ; annuler : B.
`input_go()` / `input_back()` : vrais à l'image de l'appui de A / B, avec le son
(`SFX_OK` / `SFX_CANCEL`) ; à préférer aux tests de `input_pressed()` dans les écrans.

## Écrans
`screen_register(id, &screen)` puis `screen_run(premier)`. Chaque image :
`input_update`, `update`, `draw`, `ui_frame_end` (fondus, curseur animé), synchro
verticale. `screen_change` lance un fondu au noir (120 ms, non bloquant), puis
`exit`, `ui_art_close`, palettes de l'interface, effacement, `init` et premier dessin
affichage coupé, puis apparition (220 ms). `ui_clear` déclenche une apparition douce. Les tuiles restent
affichées : `draw` ne redessine que ce qui a changé.

## Widgets réutilisables (V4.1, lot U2)
- `ui_clear_quiet()` : comme `ui_clear`, sans apparition douce ni blocage de la manette ;
  pour les redessins partiels (saisie, aide, changement de page).
- `ui_confirm_open(titre, texte, oui, non, focus_non)` / `ui_confirm_update()` : confirmation
  à deux choix (boîte de message). Renvoie -1 en attente, 1 oui, 0 non ; **B = non**.
  `focus_non` met le curseur sur « non » (actions destructrices). `ui_confirm_close()` retire
  la boîte ; les chaînes doivent rester valides pendant qu'elle est ouverte.
- Listes (`UiList`) : `ui_list_set_enabled_by(l, fn, ctx)` grise tous les éléments d'après
  `fn(i, ctx)` ; `ui_list_select_first_enabled(l)` garde la sélection si elle est active, sinon
  prend le premier élément actif (FALSE si tous grisés) ; `ui_list_redraw(l)` redessine tout,
  barre comprise (après un changement de grisés ou de textes).
- `ui_nav_cycle(sel, first, count, dec, inc, wrap)` : pas de valeur dans [first, first + count[
  avec rebouclage ou butée, `SFX_MOVE` si elle change ; renvoie la nouvelle valeur.
- `ui_value_right(x_end, y, w, str, style)` : valeur alignée à droite dans une zone de w
  colonnes effacée d'abord. `UiStatRow` + `ui_stat_row(y, &ligne)` : icône (x 1), libellé
  (x 3, 13 col.), jauge (x 16 à x_end - 8), valeur sur 7 colonnes finissant en `x_end`.
- `ui_portrait(id, expr, x, y, pal)` (ui_art.h) : `ui_medallion` + `ui_gueule_draw`.
- `ui_footer` tronque à droite : rien au-delà de `x + w`.

## Carte de France (src/ui/ui_map.h, res/gfx_map.res)
Carte des 13 régions : 20×20 tuiles (160×160 px) sur le **plan B**, avec **une ligne de
palette entière**. Un indice de couleur par région : colorer une région = changer UNE
couleur de palette (`ui_map_set_region_level`, dégradé rouge → jaune → vert), sans
redessiner de tuile. `ui_map_highlight` fait clignoter la région choisie (appeler
`ui_map_update` à chaque image).

**Palettes** : la carte occupe **PAL2** pendant son affichage (ligne réservée aux
illustrations en V3 : tous les styles de texte restent utilisables) ; `ui_map_close()`
restaure `pal_ui_dim`. Le clignotement est suspendu pendant les fondus.
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

## Illustrations (src/ui/ui_art.h, res/gfx_art.res) — tâches 2.02, 3.07 à 3.10
Pixel art caricatural façon BD, **généré par programme** : `python3 tools/art/make_art.py`
(Python 3 seul) écrit `res/gfx/art_*.png`, `res/gfx/gueule_*.png`, `res/gfx_art.res` et
`src/ui/ui_art_gueules.h`. Tous les personnages sont fictifs. `--preview DIR` écrit des
planches agrandies.

API (`ui_art.c` est le seul fichier qui touche VDP/PAL pour les illustrations) :
- `ui_art_draw(id, x, y)` : dessine sur le **plan B**, palette dans **PAL2** ;
  `ui_art_draw_pal(id, x, y, PAL2|PAL3)` choisit la ligne (toute autre valeur → PAL2).
- `ui_art_size(id, &w, &h)` (en tuiles), `ui_art_erase(id, x, y)` (efface le plan B).
- **Gueules V3** : `ui_gueule_draw(GueuleId, Expr, x, y, PAL2|PAL3)` (10 × 12 tuiles,
  80 × 96 px, une palette par personnage), `ui_gueule_expr` (change l'expression :
  `EXPR_NEUTRE`, `EXPR_CONTENT`, `EXPR_FURIEUX`, `EXPR_GENE`), `ui_gueule_erase`.
  Raccourcis : `GUEULE_PRES(ORIENT_*)`, `GUEULE_PM(PM_*)`, `GUEULE_COHAB(camp)`,
  `GUEULE_CAND(CAND_*)`, `GUEULE_CHEF(GRP_*)`, `GUEULE_MIN(i)`. Les anciens portraits
  56 × 56 (`ART_PRES_*`, `ART_PM_*`, `ART_COHAB_*`, `ART_CAND_ROUVIERE…`) sont retirés.
- **Visages animés (3.18, sans son)** : la dernière gueule dessinée par `ui_gueule_draw`
  s'anime seule (clignements irréguliers, regard de côté, changement d'expression masqué
  par un clignement de 80 ms) ; `ui_gueule_talk(ms)` la fait parler (bouche irrégulière,
  arrêt à la fin ou sur A/B/C/START), à appeler **après** `ui_gueule_draw` ;
  `ui_gueule_talk_ms(texte)` donne une durée selon la longueur de la réplique.
  Mise à jour par `screen.c` (`ui_gueule_anim_update`), suspendue pendant les fondus et
  sous une boîte de message ; arrêt automatique si le visage est recouvert (fenêtre,
  `ui_clear`, `ui_gueule_erase`, illustration, carte) ou à `ui_art_close`.
  VRAM : une planche d'animation (≈ 15 à 30 tuiles) chargée à la première image animée.
- Cadre : `ui_medallion(x, y, UI_PORTRAIT_W, UI_PORTRAIT_H)` pose un cadre doré sur le
  pourtour de la gueule (plan A) et un fond sombre (plan B) avant le dessin.
- `ui_art_reserve(clé, n)` / `ui_art_claim_palette(pal)` : zone VRAM et ligne de palette
  pour un dessin produit hors de `ui_art` (hémicycle dynamique, `ui_hemi.c`).
- `ui_art_close()` : **à appeler en quittant l'écran** (et avant `ui_map_draw`) ; restaure
  PAL2/PAL3 et libère la VRAM (fait aussi automatiquement à chaque changement d'écran).

**Palettes** : chaque illustration a sa palette (15 couleurs + transparent) ; chaque gueule
aussi. Deux illustrations de palettes différentes = deux lignes : portrait principal en
**PAL2**, vignette, seconde gueule ou hémicycle en **PAL3**. PAL0/PAL1 = interface.

**VRAM** : tuiles chargées juste après celles de `ui.c` (zone partagée avec la carte, jamais
affichées ensemble), empilées (24 jeux au plus) ; un jeu déjà chargé est réutilisé. Limite :
`TILE_USER_MAX_INDEX` (≈ 1 040 tuiles libres). Images non compressées (DMA direct depuis la ROM).

| Id | Taille (tuiles) | Contenu |
|---|---|---|
| `ART_TITLE` | 40 × 16 | écran titre ; lignes 0-2 = ciel (plaque du titre) |
| `ART_EVT_*` (15) | 8 × 6 | vignettes d'événements |
| `ART_DECOR_HEMICYCLE` | 20 × 9 | Assemblée en séance (illustration) |
| `ART_DECOR_URNE` | 8 × 8 | urne (élection, référendum) |
| `ART_DECOR_JOURNAL` | 38 × 5 | bandeau « Le Quotidien du Matin » (en PAL3 sur la une) |
| `GUEULE_*` (30) | 10 × 12 | présidents, PM, PM de cohabitation, candidats, ministres, chefs de groupe, journalistes, président du Sénat, syndicat, patronat |

## Hémicycle dynamique (src/ui/ui_hemi.h) — tâche 3.17
Demi-cercle de 20 × 10 tuiles, un point par siège (Assemblée 577, Sénat 348), colorés par
groupe de G1 (gauche) à G5 (droite), sur le **plan A** avec la palette en **PAL3**.
`ui_hemi_draw(x, y, UI_HEMI_ASSEMBLY|UI_HEMI_SENATE, sièges[5])`, `ui_hemi_vote(pour[5],
animer)` (les « pour » s'allument de gauche à droite), `ui_hemi_focus(groupe)`
(clignotement), `ui_hemi_update()` à chaque image, `ui_hemi_majority(x, y, w, voix, total,
majorité)` (jauge et repère). Géométrie précalculée par `tools/ui/make_hemi.py`
(`src/ui/ui_hemi_data.h`) ; tuiles générées à l'exécution, sans tampon en RAM.
