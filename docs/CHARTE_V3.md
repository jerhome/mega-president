# Charte graphique V3 — interface (tâches 3.01 à 3.04)

Objectif : une interface de jeu de gestion 16 bits haut de gamme, sobre et lisible,
inspirée des beaux écrans de stratégie de fin de vie de la Megadrive, avec une
sensibilité moderne. **Cohérence avant tout** : mêmes cadres, mêmes bandeaux, mêmes
couleurs de texte, mêmes icônes partout.

Tout est produit par programme : `python3 tools/ui/make_ui.py` (ou `make gfx`) écrit
les PNG de `res/gfx/` (police, décor, jauges, icônes, palettes) et les en-têtes
`src/ui/ui_font_map.h` et `src/ui/ui_tiles.h`. `--preview DIR` écrit une maquette du
tableau de bord et une planche des tuiles agrandies.

## 1. Ambiance

- Bleu nuit présidentiel, or, ivoire : « salon de l'Élysée la nuit ».
- Fenêtres bleu roi aux **coins arrondis** (rayon 3 px), liseré clair en haut à gauche,
  plus sombre en bas à droite (relief), contour noir et **ombre portée d'un pixel**.
- **Bandeaux de titre** en dégradé par bandes (bleu moyen → tramage → bleu nuit),
  reflet clair en haut, **filet doré** en bas.
- **Fonds texturés** : grain diagonal discret sur le fond d'écran (4 bandes de densité
  croissante vers le bas), tirets fins tous les 4 pixels dans les fenêtres.
- **Air** : une ligne vide sous les bandeaux, filets gravés entre les groupes
  d'indicateurs, aides de boutons centrées en pied de fenêtre.

## 2. Palettes (contrat V3)

L'interface n'utilise que **PAL0** et **PAL1**. **PAL2** (portrait principal) et
**PAL3** (vignette, bandeau du journal, seconde illustration) sont réservées aux
illustrations (`ui_art`). Couleurs en niveaux VDP (0-7 par composante).

| Indice | Rôle | PAL0 (`pal_ui_text`) | PAL1 (`pal_ui_highlight`) |
|---|---|---|---|
| 0 | transparent / fond d'écran | bleu nuit (0,1,2) | — |
| 1 | ombres, contours | noir | noir |
| 2, 3 | encre A (haut, bas) | ivoire (7,7,7) → (6,6,5) | or (7,6,2) → (6,5,1) |
| 4, 5 | encre B (haut, bas) | gris bleuté (5,5,6) → (4,4,5) | rouge (7,3,2) → (6,2,1) |
| 6, 7 | jauge A, reflet | vert (1,5,2), (4,7,3) | or (6,5,1), (7,7,4) |
| 8, 9 | jauge B, reflet | bleu (1,3,7), (4,6,7) | rouge (6,1,1), (7,4,3) |
| 10 | creux des jauges, fond des médaillons | (0,0,1) | idem |
| 11, 12 | fond des fenêtres, grain | (1,2,4), (1,2,5) | idem |
| 13, 14 | liseré clair, cadre moyen / barre de sélection | (5,6,7), (2,3,6) | idem |
| 15 | grain du fond d'écran / bas des bandeaux | (1,1,3) | bleu nuit (1,1,4) |

Les indices 11 à 14 sont identiques dans les deux lignes : une tuile de décor peut
passer de PAL0 à PAL1 sans changer de fond. `pal_ui_dim` et `pal_ui_alert` restent
déclarées (copies de PAL0) car `ui_art_close()` les recharge dans PAL2/PAL3.

## 3. Typographie : 4 styles avec 2 palettes

La police est chargée **deux fois** en VRAM : jeu d'encre A (indices 2/3) et jeu B
(indices 4/5). Style = ligne de palette (bit 0) + jeu d'encre (bit 1) :

| Style | Palette, encre | Couleur | Emploi |
|---|---|---|---|
| `UI_STYLE_TEXT` | PAL0, A | ivoire | texte courant, valeurs |
| `UI_STYLE_HIGHLIGHT` | PAL1, A | or | titres de bandeau, rubriques, sélection, valeurs clés |
| `UI_STYLE_DIM` | PAL0, B | gris bleuté | libellés, aides des boutons, éléments grisés |
| `UI_STYLE_ALERT` | PAL1, B | rouge | alertes, baisses, refus |

Hiérarchie : **titre** (or, dans un bandeau) > **valeur** (ivoire) > **libellé**
(gris bleuté) ; **alerte** en rouge. Chaque glyphe a une encre en dégradé (4 lignes
claires, 4 lignes plus chaudes) et une ombre portée noire en bas à droite.
Coût : 2 × 128 tuiles (≈ 8 Ko de VRAM), contre 4 palettes entières auparavant ; l'API
`ui_*` n'a pas changé (mêmes styles, mêmes alias de jauges).

Police : base SGDK 8×8 (traits de 2 pixels, lisibles sur téléviseur), 125 glyphes :
ASCII, accents français, « » ° € ’ … ✓, et en V3 **→** (U+2192) et **·** (U+00B7).
Les 38 colonnes utiles et le texte UTF-8 (tampons `UI_LINE_BUF`) restent la règle.

## 4. Composants (API `src/ui/ui.h`)

| Composant | Fonction | Plan | Remarques |
|---|---|---|---|
| Fond d'écran | `ui_clear()` | B | 4 bandes de grain ; déclenche l'apparition douce |
| Fenêtre | `ui_window(x, y, w, h)` | B | coins arrondis, relief, ombre portée |
| Fenêtre à titre | `ui_window_title(x, y, w, h, titre, gauche)` | B + A | bandeau rangées y..y+2, contenu dès y+3 |
| Bandeau seul | `ui_banner(x, y, w, titre, gauche)` | B + A | en-tête d'écran (3 rangées) |
| Filet | `ui_separator(x, y, w)` | B | rangée sans texte, bouts estompés ; derrière un titre de rubrique |
| Cadre de portrait | `ui_medallion(x, y, w, h)` | A + B | cadre doré posé sur le pourtour du portrait, fond sombre |
| Barre de sélection | automatique dans `ui_list_draw` | B | arrondie, reflet ; `UI_LIST_NOBAR` pour une liste hors fenêtre |
| Jauge à reflet | `ui_gauge_draw` | A | bouts arrondis, reflet, creux sombre ; 4 couleurs |
| Icônes | `ui_icon_draw` | A | 32 icônes 8×8, chacune avec sa palette |
| Aide des boutons | `ui_footer(x, y, w, "A : valider   B : retour")` | A | « A : », « B : », « C : », « START : » → pastilles |
| Boîte de message | `ui_msgbox_open` | B + A | fenêtre à titre, dépliement en 3 images |

Icônes : curseur, flèches, puce, cœur (popularité), courbe (croissance), mallette
(chômage), pièce (dette), 6 piliers (personnes, usine, puce, feuille, bouclier, globe),
éclair (tension), point d'exclamation (scandale), visage (défiance), étoile (CP),
hémicycle, calendrier, boutons A/B/C/START, coche, croix, personnage, journal,
flèches de tendance colorées (hausse/baisse favorable ou non).

## 5. Animations (temps réel, identiques en PAL et en NTSC)

| Effet | Durée | Mise en œuvre |
|---|---|---|
| Fondu au noir avant changement d'écran | 120 ms | `screen.c` : non bloquant (musique et bruitages continuent) |
| Préparation du nouvel écran | 1 image | affichage coupé : init + premier dessin invisibles |
| Apparition du nouvel écran | 220 ms | fondu depuis le noir vers les palettes lues en CRAM |
| Apparition douce après un redessin (`ui_clear`) | 150 ms | fondu depuis le tiers de la luminosité |
| Curseur animé | 4 × 100 ms | la tuile du curseur est recopiée depuis `ts_ui_cursor` |
| Dépliement de la boîte de message | 3 images | la fenêtre grandit vers le haut, puis le texte |

Pendant les fondus, la manette est bloquée : les appuis pendant le fondu au noir sont
ignorés, ceux de l'apparition sont rendus à la fin (`input_block`). Les fondus
utilisent le fondu asynchrone de SGDK (aucune allocation, 256 o de RAM pour les
palettes cibles). La carte suspend son clignotement pendant un fondu.

## 6. Grille et emplacements de portraits

Écran 40 × 28 tuiles. Fenêtres pleine largeur (x 0-39), texte de x 1 à 38.
Emplacement de portrait : **gueule V3 de 10 × 12 tuiles (80 × 96 px)**, en PAL2 (seconde
gueule en PAL3). `ui_medallion(x, y, 10, 12)` pose un **cadre doré sur le pourtour même
du portrait** (plan A : contour noir, filet or clair/sombre, coins arrondis peints du fond
de fenêtre) : le cadre ne coûte aucune colonne et laisse 28 colonnes au texte à côté.

Expressions (`Expr`) selon le contexte :

| Écran | Gueule | Position | Expression |
|---|---|---|---|
| Tableau de bord | PM (ou PM de cohabitation) | x 29-38, y 4-15 | content si popularité ≥ 55 %, gêné < 40 % ou scandale, furieux si défiance ou tension fortes ; la cohabitation se réjouit des déboires |
| Journal (une) | président | x 29-38, y 8-19 | content si la popularité monte d'1 point, gêné si elle baisse d'1,5 point ou en cas d'affaire |
| Élection, 2nd tour | les deux finalistes | x 2 et x 28, y 4-15 | content pour le vainqueur, gêné pour le battu |
| Remaniement | PM pressenti | x 29-38, y 4-15 | content si possible, gêné sinon |
| Second mandat | PM ou PM de cohabitation | x 29-38, y 3-14 | selon les législatives |
| Écran titre | président ou PM choisi | x 29-38, y 2-13 (sur l'illustration, PAL3) | content |
| Événement | vignette (PAL3) | x 29-38, y 3-14 | — |

## 6 bis. Hémicycle dynamique

`src/ui/ui_hemi.h` : 577 sièges (Sénat : 348) en demi-cercle de 20 × 10 tuiles, une couleur
par groupe (rouge, rose, jaune orangé, bleu, violet de G1 à G5), tribune et perchoir au
centre, arc extérieur. Plan A, palette PAL3. Au vote, tout s'éteint puis les groupes
« pour » s'allument de gauche à droite (250 ms par groupe) ; le verdict s'affiche ensuite
(A l'accélère). Jauge de majorité avec repère ▼. Vue « Assemblée » : le groupe du joueur
clignote doucement.

## 7. Mises en page (3.05)

- **Tableau de bord** : bandeau (date du tour, ★ CP) ; fenêtre des indicateurs
  (icône, libellé atténué, jauge, valeur à droite ; popularité et électorats, filet,
  économie, filet, 6 piliers, tension, scandale, défiance) avec le médaillon à droite ;
  fenêtre des actions (5 lignes, barre de sélection).
- **Journal** : bandeau illustré, date en or, filet ; la une coule autour de la photo
  (texte sur 27 colonnes à côté, 38 en dessous) ; rubriques dorées suivies d'un filet ;
  indicateurs avec flèche colorée selon que la variation est bonne ou mauvaise.
  Pages « Vie politique » : fenêtre à bandeau « LE QUOTIDIEN DU MATIN ».
- **Événement** : fenêtre à bandeau (titre centré), texte sur 27 colonnes, médaillon ;
  fenêtre « Que décidez-vous ? » ; puis « Votre décision » (coche) et l'écho du
  Quotidien du Matin.

## 8. Budgets

- VRAM de l'interface : police 256 + décor 40 + jauges 54 + icônes 32 = **382 tuiles**
  (≈ 12 Ko), à partir de `TILE_USER_INDEX`. Reste pour illustrations et carte :
  **≈ 1 040 tuiles** (jusqu'à `TILE_USER_MAX_INDEX`).
- Les icônes sont chargées **en dernier** : `ui_tile_icons + ts_ui_icons.numTile` est la
  première tuile libre (convention utilisée par `ui_art.c` et `ui_map.c`).
- RAM statique ajoutée : ≈ 0,3 Ko (palettes cibles des fondus, état des animations).

## 9. ROM de démonstration (captures sans clavier)

`-DMP_DEBUG_SCRIPT="\"w30 S w8 A\""` (via `EXTRA_FLAGS=@fichier`) simule la manette :
`wN` attend N × 100 ms ; `S A B C U D L R` = appui de 100 ms puis 200 ms relâché.
Jamais dans la ROM livrée.
