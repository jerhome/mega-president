# Revue DA des écrans V4

15/11, directeur artistique. Cette revue répond au point « Écrans V4 » de `A_VALIDER.md`, recommandation (a). Elle porte sur l'écran budget détaillé, le tableau de bord avec ses électorats, l'écran des électorats, la campagne, le référendum et le ministre de l'écran événement (tâche 4.20).

**Captures** : ROM de démonstration (`make rom-demo`, DEMO = 3, 16, 17, 12, 18, 1, 4, 13 et 15, manette scriptée), BlastEm en PAL, fenêtre 640 × 480. Elles sont rangées dans
`/tmp/claude-1000/-home-jerome-claude-mega-president/3d99453c-606e-4762-80a2-7b158a65e7cd/scratchpad/revue_v4/` (`docs/captures/` n'existe pas). Ce dossier est temporaire : copiez les captures si vous voulez les garder.

**Repères** : les coordonnées sont en tuiles (x de 0 à 39, y de 0 à 27). Le texte utile va de x 1 à x 38 et le pied de page est en y 26. Dans `ui_text_right(x, …)`, x est la colonne **exclue** : `ui_text_right(28, …)` finit en colonne 27.

## Verdicts

| Écran | Verdict | En bref |
|---|---|---|
| Budget détaillé (normal, cohabitation, Sénat) | ❌ | Les 6 lignes × 5 crans tiennent. C'est la grille d'opinion (3 lignes de chiffres) qui rend l'écran trop dense : 23 lignes pleines sans aucune respiration. Les flèches de la ligne active sortent de la barre et touchent le médaillon. |
| Tableau de bord, ligne des électorats | ✅ avec une retouche | Lisible et bien groupé sous « Popularité ». Les cellules sont décalées d'une colonne vers la gauche. |
| Écran des électorats (C) | ✅ avec une retouche | Écran aéré et clair. Les libellés de 18 caractères collent aux jauges. |
| Campagne | ❌ (mineur) | Plus de chevauchement. Le bloc du bas est collé (électorats, description, légende) et le « 0 » neutre s'affiche avec une jauge non vide. |
| Référendum | ❌ (mineur) | Plus de chevauchement. « OUI attendu » se confond avec les 5 électorats, et la confirmation met le nom de la loi après la vanne. L'écran de résultat est ✅. |
| Événement, « Votre décision » | ❌ | Le ministre n'a pas de nom, et sa bouche parle sur l'écho du *Quotidien*, ce qui enfreint la règle de la charte § 6 (« la bouche ne bouge que sur une réplique attribuée »). Pour la grève, la gueule est celle de la Première ministre : sans nom, le joueur ne peut pas comprendre. |

---

## P1 : à corriger avant de montrer au joueur

### P1-1. Budget : alléger la zone « Effets du plan » (`src/screens/scr_budget.c`)
Captures : `budget_relance_ligne.png`, `budget_zoom_effets.png`, `budget_senat_actions.png`.

Constat : de y 4 à y 26, il n'y a aucune ligne vide. Le bloc d'aide (13-15) est collé à « Effets du plan » (16). Le tableau Mesures / Marchés / Total ajoute 15 nombres rouges et blancs. Or la ligne « Marchés » répète l'alerte « Marchés inquiets : jusqu'à −3,2 pt ». En phase d'action, la ligne y 22 et la liste 23-25 forment un bloc compact jusqu'au pied de page.

Décision DA : on garde une seule ligne de chiffres par électorat (le total), et on supprime « Mesures » et « Marchés ». L'effet des marchés reste signalé par l'alerte, et le détail par ligne par le bloc d'aide.

Consigne md-dev, nouvelle grille (colonne de gauche x 1-27 et médaillon x 29-38 inchangés) :

| y | Contenu |
|---|---|
| 4 | préréglage (inchangé) |
| 5 | séparateur |
| 6-11 | 6 lignes (voir P2-1) |
| 12 | séparateur |
| 13-15 | aide de la ligne ou du préréglage (inchangée) |
| **16** | **vide** |
| 17 | « Effets du plan » (HIGHLIGHT) + « Popularité » et valeur, à droite (inchangé, décalé d'une ligne) |
| 18 | Déficit, Croiss. (inchangés) |
| 19 | abréviations des électorats (DIM, colonnes inchangées : `ui_text_right(14 + e*5 + 5, …)`), sans libellé à gauche |
| 20 | libellé « Opinion » (TEXT) à x 1, puis les 5 totaux (`opinion_cell(…, TRUE)`) |
| 21 | aperçu du vote (icône x 1, jauge x 3-20, « 410 voix : adopté » à droite) |
| **22** | **`ui_separator(1, 22, 38)`** |
| 23-25 | alertes ou liste d'actions (inchangé) |
| 26 | pied de page |

- `Y_EFFECT` passe de 16 à 17, et `draw_effect` efface `ui_clear_rect(1, Y_EFFECT, 38, 4)`. Supprimez les lignes « Mesures » et « Marchés », ainsi que la boucle `lines`. Le calcul de `worst` dans `draw_alerts` reste.
- Toutes les écritures en y 22 passent en **y 21** : aperçu du vote, message « Rejeté : … », « Capital politique insuffisant », « ✗ Amendement : … » du mode Sénat (pas d'aperçu du vote dans ce mode) et titre « BUDGET ADOPTÉ … » du résultat. Le séparateur de y 22 est redessiné après chaque `ui_clear_rect` qui le toucherait.
- Critères d'acceptation : sur DEMO=3 (Relance, ligne 1 active), DEMO=16 et DEMO=17 en phase d'action, les lignes y 16 et y 22 séparent visiblement les blocs. Aucun texte ne dépasse x 38. Le calcul du vote et des effets n'est pas modifié.

### P1-2. Événement : nommer le ministre et lui donner une vraie réplique (`src/screens/scr_event.c`, textes : `redacteur`)
Captures : `evenement_decision_greve.png` (on voit Deschamps, Première ministre, sans nom) et `evenement_decision_marches.png` (Fontaine).

Constat : à la phase « Votre décision », la gueule remplace la vignette sans aucun nom. `ui_gueule_say` reçoit la durée du texte du *Quotidien du Matin* : la bouche du ministre « lit » le journal. C'est l'avis de la DA de la V3 (REVUE_DA_V3 § 8.3) qui n'a pas été suivi.

Consigne md-dev, fenêtre du bas en phase P_JOURNAL (y 16-27) :

| y | Contenu |
|---|---|
| 17 | titre « Votre décision » (inchangé) |
| **18** | ✓ x 1 + choix x 3 (HIGHLIGHT), auparavant en y 20 |
| **19** | x 3 : **Nom** (HIGHLIGHT) + `, ` + fonction (DIM), par exemple « Fontaine, ministre de l'Économie » (36 colonnes au plus) |
| **20** | x 3 : réplique entre guillemets (TEXT), 36 colonnes au plus, sur 1 ligne : « « Bien, Monsieur le Président. » » |
| 21 | séparateur (inchangé) |
| 22 | icône journal + « Le Quotidien du Matin » (inchangé) |
| 23-25 | écho du journal, `ui_text_wrap(3, 23, 36, 3, …)` (inchangé) |
| 26 | pied de page |

- La bouche parle sur la **réplique** : `ui_gueule_say(ui_gueule_talk_ms(replique))`, et plus jamais sur `evt_journal`. Le charabia (4.18) joue sur la même durée.
- Nom : le nom de famille seul, au plus 9 caractères (Fontaine, Fermont, Varenne, Marchal, Kervella, Valcourt, Delaunay, Deschamps, Lestrade, et le PM de cohabitation via `TXT_COHAB_PM_*_NOM`, coupé après la dernière espace comme `surname()` dans `scr_game.c`). Mettez `surname()` en commun dans `scr_txt.h`, sans la dupliquer.
- Fonctions (textes, 26 caractères au plus) : « ministre de l'Économie », « ministre de l'Intérieur », « ministre de la Santé », « ministre de l'Éducation », « ministre de l'Écologie », « Affaires étrangères » (la forme longue ferait 42 colonnes avec le nom), « Matignon » pour le PM, quel que soit son genre (repli de `evt_minister`).
- Répliques, commande au `redacteur` : 7 personnages (6 ministres + PM) × 4 humeurs (`EXPR_CONTENT`, `NEUTRE`, `GENE`, `FURIEUX`, selon `evt_mood`) = **28 répliques de 32 caractères au plus** (36 colonnes moins les guillemets), dans `docs/contenu/textes_v4.md`. Exemples de ton : Fontaine content « Les marchés vont adorer. », gêné « Bercy fera… avec. ». La relecture des vannes est souhaitable.
- Repli tant que les répliques manquent : la ligne y 20 reste vide, mais le nom (y 19) est **obligatoire dès maintenant**, et la bouche bouge alors 2 syllabes (durée fixe), jamais sur le journal.
- Critères d'acceptation : sur DEMO=13 (grève, PM) et DEMO=15 (marchés, Fontaine), le nom et la fonction sont lisibles sous le choix. La bouche s'arrête avant la fin de la lecture de l'écho. Le texte ne dépasse pas x 38.

---

## P2 : à corriger dans le même lot

### P2-1. Budget : géométrie des 6 lignes (`scr_budget.c`, `draw_rows`)
Capture : `budget_zoom_lignes.png`.

Constat : « Impôts entreprises » (18 caractères, x 2-19) touche la jauge (x 20). Sur la ligne active, ◀ +1 ▶ s'entassent : ▶ en x 28 sort de la barre de sélection (x 1-27) et touche le cadre du médaillon (x 29).

Consigne :
- Libellé en x 1, sur 17 colonnes : `ui_text_draw_n(1, y, bl_nom(i), 17, 17, st)`. La colonne x 18 reste vide.
- Jauge en x 19, sur 5 tuiles (x 19-23).
- ◀ en x 24, valeur `ui_text_right(27, y, …)` (x 25-26), ▶ en x 27. Tout tient dans la barre x 1-27, et x 28 reste vide devant le médaillon.
- Texte (`redacteur`, `textes_v4.md` puis `content_texts.c`) : `TXT_BL_5_NOM` « Impôts entreprises » devient **« Impôts sociétés »** (15 caractères ; c'est aussi le terme exact, l'IS). Les 5 autres libellés font déjà 17 caractères au plus.

### P2-2. Budget, modes Sénat et cohabitation
Captures : `budget_senat.png`, `budget_cohab.png`.
- En mode Sénat, la ligne du préréglage affiche ◀ ▶ alors qu'elle n'est pas réglable. **Ne dessinez pas les flèches** quand `mode != B_NORMAL` : le libellé reste centré, en DIM.
- En cohabitation, le texte du plan imposé (`ui_text_wrap(1, 23, 38, 3, …)`, 3 lignes forcées par `\n`, donc ça tient) est collé au pied de page. Acceptable avec P1-1.
- Accord à corriger (`redacteur`) : `TXT_COHAB_PLAN_GAUCHE` dit « Le Premier ministre impose… » alors que la PM de gauche est Sophie Carrel (`budget_cohab.png`). Écrivez « La Première ministre impose un plan », sur 36 caractères au plus par ligne.

### P2-3. Tableau de bord : aligner les électorats sur « Popularité » (`src/screens/scr_game.c`, `draw_electorates`)
Capture : `tableau_de_bord_zoom.png`.
- `u16 x = 2 + (e % 3) * 8;` devient `u16 x = 3 + (e % 3) * 8;`. Les cellules commencent alors en x 3, 11 et 19, alignées sur le « P » de « Popularité » (x 3). Le « C détail » reste en x 19 / 21 et s'aligne désormais sous « Cad ».
- Critère : la 3e cellule finit en x 26 au plus (x 27 pour une valeur de 100), avant `DASH_X_END` (28).

### P2-4. Écran des électorats : décoller les jauges (`src/screens/scr_elec.c`, `draw_row`)
Capture : `electorats.png`.
- « Classes populaires » (x 3-20) touche la jauge en x 21. Passez à `ui_gauge_draw(22, y, 8, …)` (x 22-29) et laissez la valeur et la tendance telles quelles (x 30-38).

### P2-5. Campagne : resserrer le bas, rendre le neutre neutre (`src/screens/scr_campaign.c`)
Captures : `campagne_emploi.png`, `campagne_sante.png`.
- Supprimez la légende « ++ fort + favorable − défavorable » (y 24) : les jauges colorées et les signes suffisent.
- Placez la description du thème en **y 23-24**, avec `ui_clear_rect(1, 23, 38, 2)` et `ui_text_wrap(1, 23, 38, 2, …)`. Les lignes y 22 et y 25 restent vides.
- Effet neutre (|b| < 15) : signe **« = »** en DIM (comme la tendance « stable ») au lieu de « 0 », et jauge `UI_GAUGE_NEUTRAL` (bleue) au lieu de l'or. Aujourd'hui, « 0 » s'affiche à côté d'une jauge dorée non vide.
- Texte (`redacteur`) : seul le thème 5 a une description. Il faut écrire les 4 autres (`TXT_THEME_*_DESC`, 76 caractères au plus, 2 lignes de 38), sinon la zone y 23-24 est vide pour 4 thèmes sur 5.

### P2-6. Référendum : hiérarchiser l'aperçu, corriger la confirmation (`src/screens/scr_referendum.c`)
Captures : `referendum_choix.png`, `referendum_confirmation.png`.
- `draw_detail` : y 10 nom de la loi (inchangé), y 11 « OUI attendu » avec un libellé **HIGHLIGHT** (`gauge_row` prend un style en paramètre), **y 12 vide**, y 13-17 les 5 électorats, **y 18 vide**, y 19-22 l'aide ou le message d'erreur (`ui_text_wrap(1, 19, 38, 4, …)`). `ui_clear_rect(1, 10, 38, 16)` est inchangé.
- Confirmation : le nom de la loi arrive après la vanne, en 3e ligne. Utilisez `ui_msgbox_open(law_name[chosen], txt[TXT_REF_CONFIRME], ch, 2)` : la loi devient le titre de la boîte, le bandeau dit déjà « Référendum », et `confirm_text` disparaît. La boîte fait alors 9 lignes (y 18-26) et laisse visibles les électorats (y 13-17).
- Critère : sur DEMO=4, aucun électorat n'est masqué par la boîte de confirmation.

---

## P3 : finitions, quand il restera du temps

1. **Budget, crans discrets** : la jauge continue (le cran 0 donne une barre à moitié pleine) ne se lit pas comme 5 crans. La maquette SPEC_V4 § 2.6 prévoyait des pastilles « ▮▮▮□□ ». Il faut 3 tuiles d'interface (pleine, vide, centre marqué) dans `tools/ui/`, PAL0, et 5 tuiles par ligne. Environ 1 h.
2. **Repère de majorité** : un trait de 1 px à 50 % (référendum) et à 289 voix (aperçu du vote du budget) dans `ui_gauge_draw`, en option. Environ 1 h, 1 tuile.
3. **Budget, ligne du vote** : ajoutez le libellé « Assemblée » (DIM) après l'icône et raccourcissez la jauge à 12 tuiles (x 13-24), car l'icône seule ne suffit pas.
4. **Électorats** : `elec_update` remet `dirty` (fenêtre entière redessinée) à chaque déplacement. Ne redessinez que `draw_rows` et le détail, pour éviter un clignotement sur console.
5. **Ligne amendée par le Sénat** : le rouge sur la barre de sélection bleu clair contraste peu. C'est acceptable, à revoir si le test console le confirme.

## Réponses aux questions posées

- **Budget, 6 × 5 crans sur 23 lignes, trop dense ?** Le bloc des 6 lignes, non. L'écran entier, oui, à cause de la grille d'opinion sur 3 lignes. Avec P1-1 et P2-1, il garde 21 lignes pleines et 2 lignes de respiration, sans perdre d'information utile.
- **Tableau de bord** : la ligne des électorats sur 2 rangées est une bonne solution et reste lisible. Il ne manque que l'alignement (P2-3).
- **Campagne et référendum** : je n'ai plus vu de chevauchement à l'écran. Il reste de l'entassement (P2-5, P2-6).
- **Nom du ministre (4.20)** : c'est indispensable, voir P1-2. Le nom va sous le choix plutôt que sur le médaillon, parce que la gueule est sur le plan B et que le texte du plan A n'a pas de fond opaque : un bandeau sur les épaules serait illisible sans nouveau composant.

## Décisions DA (à reporter dans DECISIONS.md)
- Budget : une seule ligne d'opinion par électorat (le total), sans « Mesures » ni « Marchés ».
- Budget : `TXT_BL_5_NOM` devient « Impôts sociétés ».
- Campagne : la légende des signes est supprimée, et le neutre s'écrit « = » avec une jauge bleue.
- Événement : nom et fonction du ministre sur une ligne propre. La bouche ne parle que sur une réplique attribuée, jamais sur l'écho du journal.
- Confirmation du référendum : le nom de la loi devient le titre de la boîte.
