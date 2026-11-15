# Audit du code V4.1 (tâche 4.1.02)

Relecture en lecture seule par un agent Opus, 16/11, sur `v4-dev` au commit `8b9e40a`. Les numéros de ligne renvoient à ce commit ; ceux des écrans retouchés ensuite (budget, événement, campagne, référendum, options, scores, impeach, fin, `voice.c`) ont bougé. **Toutes les corrections vont en V4.1** (décision du joueur, 16/11) ; rien n'est corrigé en V4.

## 1. Synthèse

1. **Architecture saine** : `src/sim` est portable (ni SGDK, ni float, ni malloc), aucun appel VDP ou palette hors de `src/ui`. Mais `src/ui` dépend de `game`, `save` et `audio` (`screen.c`, `ui_widgets.c`, `ui_art.c`).
2. **Budgets larges** : RAM statique 6,5 Ko sur 32, ROM 1,15 Mo sur 2, pile sous 0,5 Ko.
3. **Aucun débordement actif**, mais des tampons fixes sans marge ni protection : `q[48]` (`scr_event.c`, 8 octets de marge), `flat[256]` (24 octets), `menu_items[12]` (plein).
4. **Duplication massive dans `src/screens`** : 13 copies des fonctions `cat_*`, 191 `strcat` sans borne, 5 copies du bandeau « titre + CP », 7 variantes de la ligne « icône, libellé, jauge, valeur », 6 confirmations modales recodées.
5. **Défauts visibles** : 6 événements V4 sans vignette (image du scandale, ministre de l'Intérieur) ; `ui_clear()` sur les redessins partiels relance un fondu et retarde la manette.
6. **Sauvegarde** : somme de contrôle correcte, mais version indépendante du contenu, aucune validation après chargement, un seul emplacement, écriture non atomique.
7. **Images par seconde** : clignotement de la carte et dépliement des boîtes comptés en images ; horloges avançant par tour de boucle.
8. **Aléa** bien séparé, sauf le générateur du charabia qui peut geler la console (rare).
9. **Performance** : pas de point chaud évident ; leviers = redessins inutiles, aperçus du budget, taille du code (`-O3` + LTO).
10. **Plan** : 16 lots de 1 à 2 h pour deux agents (section 6).

## 2. Factorisation de l'interface (4.1.03)

Avec `-O3`, LTO et `-fipa-icf`, le gain est surtout en lignes et en sûreté ; la ROM se mesure avant/après avec `out/symbol.txt`.

| Motif | Où (principaux) | API proposée | Gain |
|---|---|---|---|
| **M1** Formatage | `cat_*` dans scr_game, budget, referendum, reshuffle, elec, report, impeach, mandate2, game.c ; 191 `strcat` ; motif « avant → après » recodé 9 fois | `src/util/strbuf.[ch]` portable : `StrBuf`, `sb_init`, `sb_str` (coupe sur frontière UTF-8), `sb_num`, `sb_signed`, `sb_arrow`, macro `SB_LINE` ; tests PC | 240-290 lignes, 1-3 Ko, débordements impossibles |
| **M2** Bandeau titre + CP | scr_game, budget, referendum, reshuffle, elec | `scr_kit.h` : `kit_header(title)`, `kit_page(title)` | ≈ 75 lignes |
| **M3** Ligne d'indicateur | `stat_row`, `gauge_row`, `draw_row` (elec, election), `draw_evo`/`stat`, `indicator`, campagne, `draw_top`, aperçu du budget | `ui_value_right()`, `UiStatRow` + `ui_stat_row()` | ≈ 60 lignes, alignement unique |
| **M4** Confirmation modale | scr_game, referendum, reshuffle, title, options (2 étapes), budget | `ui_confirm_open(title, text, yes, no, focus_no)`, `ui_confirm_update()` (B = non) | ≈ 40 lignes, B homogène |
| **M5** Listes grisées | `refresh_disabled` ×3, « premier actif » ×3, forçage du redessin | `ui_list_set_enabled_by`, `ui_list_select_first_enabled`, `ui_list_redraw` | ≈ 30 lignes ; corrige la sélection grisée du budget en mode Sénat |
| **M6** Médaillon + gueule | 19 appels dans 13 écrans | `ui_portrait(id, expr, x, y, pal)` | ≈ 20 lignes |
| **M7** Redessin discret | budget (aide), options, referendum, reshuffle, title, game, scores, help | `ui_clear_quiet()`, `art_reset_quiet()` | saisies nettes (I3) |
| **M8** Navigation | game (Assemblée), elec, map, options, budget, scores, help | `ui_nav_cycle(sel, first, count, dec, inc, wrap)`, `input_go()`, `input_back()` | ≈ 30 lignes, son homogène |
| **M9** Erreurs d'action | game, referendum, reshuffle, budget | `kit_err_line(y, KitErr)` | ≈ 15 lignes ; message juste pour « file pleine » |
| **M10** Libellés en double | noms des piliers ×3, `START_YEAR` ×2, nom du PM recopié 7 fois (une seule copie bornée) | `pillar_name`, `pm_name`, `pm_desc` bornés dans `scr_txt.h`, `game_calendar_year()` | ≈ 25 lignes, index bornés |
| **M11** Sièges par groupe | `group_legend`, dissolution, mandat 2 | `kit_group_seats(y, seats, before, pour)` | ≈ 25 lignes |
| **M12** Texte autour d'un portrait | `flatten()` (scr_event) refait `ui_text_flow` | supprimer `flatten` | 10 lignes, 256 o de RAM, corrige I6 |

Total ≈ 580 lignes sur 5 400 (≈ 11 %), 2 à 5 Ko de ROM (plus avec R6).

**Ordre de migration** : fondations (U1 StrBuf, U2 widgets, U3 kit) → écrans libres (elec, reshuffle, map, help ; puis game, report, mandate2) → écrans retouchés en V4 (referendum, impeach, end, scores ; event, campaign, election ; options et budget en dernier).

## 3. Défauts

### Critiques (latents, déclencheur banal)

- **C1. Pile, réplique du ministre** (`scr_event.c`, `char q[48]` + guillemets UTF-8) : une réplique de 42 octets ou plus écrase la pile → `SYS_die`. Aucun test ne borne `rep_texte`. Correctif : StrBuf ou 3 appels de dessin ; test ≤ 36 colonnes.
- **C2. Gel du charabia** (`voice.c`) : `s_seed ^= getTick() | 1` peut donner 0, le xorshift reste à 0 et `do … while (pair == s_last_pair)` boucle à l'infini (≈ 1 sur 10⁶ par réplique). Correctif : graine non nulle forcée, correction en un coup au lieu de la boucle.

### Importants

- **I1. Six événements V4 sans vignette** (`evt_art.c`) : `EVT_PENSIONS`, `EVT_EHPAD`, `EVT_FRACTURE_NUM`, `EVT_AFFECTATION`, `EVT_CHOMAGE_JEUNES`, `EVT_MARCHE_CLIMAT` → vignette « scandale » et ministre de l'Intérieur. Correctif : champ « vignette » émis par `gen_content.py`, test de couverture.
- **I2. Sauvegarde** : (a) version sans empreinte du contenu (insérer un événement corrompt silencieusement les anciennes sauvegardes) → empreinte 16 bits dans l'en-tête ; (b) aucune validation (`turns_per_year == 0` → division par zéro, `orientation >= 3` → hors table) → `game_sanitize()` ; (c) un seul emplacement écrit octet par octet → deux emplacements alternés ; (d) `g_budget_seen`, `g_opinion_prev`, `g_report_snap` gardés par les écrans et non sauvegardés → dans `Game` / `TurnReport`.
- **I3. Fondu et manette bloquée sur redessins partiels** : saisie des initiales (chaque lettre = fondu de 150 ms, répétition saccadée), fermeture de l'aide du budget, annulations, pages de l'aide. Correctif : M7.
- **I4. `MENU_MAX = 12` déjà atteint** (`scr_game.c`) sans garde : la prochaine action écrira hors tableau. Correctif : borne ou assertion.
- **I5. Ordre des tables supposé sans assertion** : `TXT_PM_*_NOM + 2*pm` et voisins, `EFF_*` ↔ `PIL_*`/`JAUGE_*`, ordre des ministres dans `evt_mood` et `content_repliques.h`, `CHB_COUNT == GUEULE_COUNT`. Correctif : `_Static_assert`.
- **I6. `flatten()` coupe en plein caractère UTF-8** au-delà de 255 octets. Correctif : M12.
- **I7. `ui_footer` ne tronque pas** au-delà de `w`.

### Mineurs

- Clignotement de la carte (`BLINK_FRAMES 16`) et dépliement des boîtes (3 images) en images ; horloges de `screen.c` par tour de boucle → `getTick()`.
- Palettes écrites par le CPU pendant l'affichage (`ui_map_update` à chaque image, `ui_hemi`) : points parasites possibles sur console → écrire au changement, `DMA_QUEUE`.
- `scr_map.c` : `num[12]` au lieu de 13 ; `scr_help.c` : numéro de page casse à 10 pages.
- Index de PM non bornés (`scr_mandate2.c`, `scr_reshuffle.c`) : `PM_NONE` afficherait « Facile ».
- Second mandat : `pending` non vidé → une loi en attente peut s'appliquer deux fois (**règle à valider par le joueur**).
- `sim_law_adopt` ne vérifie pas `SIM_MAX_PENDING` ; `cohab_start` sans assertion sur `best`.
- Libellés « trimestre » en mode 60 tours ; aide périmée (« 3 CP par trimestre », « Cinq piliers » au lieu de 6) → rédacteur.
- `fletcher16` : 2 modulos par octet, ≈ 12 ms, 2 à 3 fois à l'écran titre.
- Pile ≈ 400 o au pire : garder les tampons locaux ≤ `UI_LINE_BUF`.

## 4. Simplifications

- **Code mort** : `sim_budget_deficit/growth/tension/pillar`, `voice_preview`, `ui_art_erase`, `ui_gueule_erase`, `ui_map_clear_region`, `ui_soft_cancel`. Fonctions utilisées seulement par les tests (`sim_budget_orientation/opinion/preview/vote/force`, `sim_init`, `sim_cohab_budget`) → `budget_compat.c` réservé aux tests. Doublon `sim_quarter_tick_at`. Pilote Z80 chargé deux fois (`sfx.c`, `music.c`).
- **RAM gaspillée** : `grp_label[5][80]` (`scr_impeach.c`), `msg[4][80]`, `confirm_text`, libellés divers → tampon de travail partagé entre écrans.
- **Couplages** : `ui/screen.c` inclut `game.h`, `sram.h`, `settings_apply.h` → rappel d'entrée d'écran enregistré par `main.c`. `game.c` inclut `ui.h` pour `game_turn_label` → vers le kit d'écran. Variables globales de partie dans `screens.h` et `extern` à la main → `game.h`. `evt_mood`/`evt_minister` (logique de jeu) dans `evt_art.c` → `game/` ou `content/`, testables. `scr_map.c` lit `settings_get(SET_COLORS)` → `ui_colorblind()`.
- **Docs désynchronisées** : `src/ui/README.md` (C n'est plus « valider »), `CLAUDE.md` (`src/ui` « à venir »), commentaires `MARKET_DIV` et `PILLAR_RECALL`, en-tête de `scr_map.c`.

## 5. Performance (C seulement, à mesurer d'abord)

| N° | Constat | Piste |
|---|---|---|
| P1 | `ui_clear()` dans les redessins partiels (fondu 150 ms) | `ui_clear_quiet`, zone touchée seulement |
| P2 | Redessins complets à chaque mouvement : électorats, options (9 lignes), destitution (recharge une gueule, 3,8 Ko de DMA), fermeture de l'aide du budget | redessiner ce qui change |
| P3 | Budget : ≈ 22 `sim_plan_opinion` par appui (≈ 4 ms) | cache recalculé au changement de plan |
| P4 | `sim_opinion_target` : ≈ 10 divisions 32 bits, 7 appels par mouvement (électorats) | même cache |
| P5 | `fletcher16` ≈ 12 ms par validation | modulo différé, validité en cache |
| P6 | Palettes écrites à chaque image | au changement, `DMA_QUEUE` |
| P7 | `ui_hemi_draw` : ≈ 200 tuiles par le CPU pendant l'affichage | mesurer ; préparer pendant l'écran noir |
| P8 | Centrage : chaîne décodée deux fois | une passe |
| P9 | Code gonflé par `-O3` + LTO (`game_draw` 15,8 Ko, `game_update` 13,6 Ko, `budget_draw` 12,2 Ko) | `#pragma GCC optimize("Os")` dans `src/screens`, `noinline` sur les outils ; 20 à 40 Ko estimés |

Mesure : mode `MP_DEBUG_PERF` dans `screen.c` (temps d'update + draw, images perdues, maximum affiché dans un coin), relevé PAL et NTSC sur tableau de bord, budget, électorats, hémicycle et carte.

## 6. Plan de travail (4.1.03 et 4.1.04)

Deux agents sans fichiers communs : **F** (fondations : `src/ui`, `src/util`, `src/save`, `src/game`, `tests`, `tools`) et **E** (écrans : `src/screens`). `make check`, `check_cols.py` et la comparaison des captures (4.1.01) après chaque lot.

| Agent | Lot | Durée | Contenu | Dépend de |
|---|---|---|---|---|
| F | U1 | 1 h 30 | StrBuf (M1) + tests PC ; `ui_format_number` délègue | — |
| F | U2 | 2 h | Widgets M3 à M8, troncature de `ui_footer` (I7) | — |
| F | U3 | 1 h 30 | Kit d'écran (M2, M9, M11), `pm_name`/`pillar_name` (M10), `art_reset_quiet` | U1, U2 |
| F | R3 | 1 h 30 | Horloges sur `getTick()`, carte et boîtes en ms, palettes en `DMA_QUEUE`, correctif C2 | R4 |
| F | R2 | 2 h | Sauvegarde I2 (empreinte, `GAMESAVE_VERSION 3`, `game_sanitize`, 2 emplacements, Fletcher différé, état d'écran dans `Game`) | E3 |
| F | U4 | 1 h | Code mort, `budget_compat.c`, Z80, commentaires, README de `src/ui`, CLAUDE.md, rappel d'entrée d'écran | R2, R3 |
| E | R1 | 2 h | Vignettes générées (I1), tests de `rep_texte`, `evt_choice`, `evt_text`, `_Static_assert` (I4, I5) | — |
| E | R4 | 1 h 30 | `MP_DEBUG_PERF`, `make rom-perf`, tableau de mesures de référence | — |
| E | E1 | 1 h 30 | Migration : elec, reshuffle, map, help | U1-U3 |
| E | E2 | 2 h | scr_game (1/2) : bandeau, indicateurs, menu borné, confirmations, liste des lois | U1-U3 |
| E | E3 | 2 h | scr_game (2/2), scr_report, scr_mandate2, `game_turn_label` | E2 |
| E | E4 | 2 h | referendum, impeach, end, scores | U1-U3 |
| E | E5 | 1 h 30 | event (C1, M12), campaign, election | U1-U3, R1 |
| E | E6 | 2 h | options, budget (cache P3, aide sans redessin, Sénat) | U1-U3, R2 |
| E | R6 | 1 h | `-Os` sur les écrans, mesure taille et fluidité, garder si aucune régression | lots E |

Ordre : F = U1 → U2 → U3 → R3 → R2 → U4 ; E = R1 → R4 → E1 → E2 → E3 → E4 → E5 → E6 → R6. Le fondu supprimé (I3) est le seul changement visuel attendu, à faire valider par la DA.
