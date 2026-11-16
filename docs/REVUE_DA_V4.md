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

---

## Vérification après retouches

16/11, directeur artistique. Branche `v4-dev` en d7fffa3 (fusion des retouches de md-dev). Les ROM de démonstration ont été compilées dans un worktree à part. Les captures ont été faites dans BlastEm en PAL, puis recadrées en 320 × 224 et rangées dans `docs/captures/v4/`.

### Verdicts

| Écran | Capture | Verdict |
|---|---|---|
| Budget, Relance, ligne 1 active (DEMO=3) | `budget_relance.png`, `budget_actions.png` | ✅ Les lignes vides en y 16 et le séparateur en y 22 font respirer l'écran. La grille d'opinion tient sur une ligne, ◀ +1 ▶ reste dans la barre et le médaillon est dégagé. « Impôts sociétés » ne touche plus la jauge. |
| Budget, Sénat (DEMO=17) | `budget_senat.png`, `budget_senat_actions.png` | ✅ Le préréglage n'a plus de flèches et s'affiche en DIM. « ✗ Amendement » est en y 21 en phase d'action. Le bas est dense, mais lisible. |
| Budget, cohabitation (DEMO=16) | `budget_cohab.png`, `budget_cohab_aide.png` | ❌ Une correction (C-1). L'écran principal est bon, et l'accord « Matignon impose » est corrigé. |
| Tableau de bord (DEMO=12) | `tableau_de_bord.png` | ✅ Les cellules Pop, Moy et Cad sont en x 3, 11 et 19, alignées sur « Popularité ». |
| Électorats (DEMO=18) | `electorats.png` | ✅ La jauge part de x 22 et le libellé ne la touche plus. |
| Campagne (DEMO=1) | `campagne_securite.png`, `campagne_ecologie.png` | ✅ La légende a disparu, et la description occupe y 23-24 pour les 5 thèmes. Le neutre s'affiche « = » en DIM avec une jauge bleue. |
| Référendum, choix (DEMO=4) | `referendum_choix.png` | ✅ « OUI attendu » est en HIGHLIGHT, avec des lignes vides en y 12 et y 18. **Écart accepté** : l'aide tient sur 5 lignes (y 19-23), et y 24-25 restent libres. |
| Référendum, confirmation et résultat | `referendum_confirmation.png`, `referendum_resultat.png` | ✅ La loi est le titre de la boîte, et les 5 électorats restent visibles. Le bas du cadre de « Jeunes » (y 17) affleure la boîte sans être coupé. |
| Événement, « Votre décision » (DEMO=15, DEMO=13) | `evenement_decision_marches.png`, `evenement_decision_greve.png` | ✅ avec une retouche non bloquante (C-3). « Fontaine, ministre de l'Économie » et la réplique se lisent bien, et la bouche ne lit plus le journal. **Écart accepté** : il n'y a pas de séparateur en y 21. L'icône du journal et l'en-tête DIM suffisent à séparer les blocs, et l'écho garde ses 3 lignes (11 échos sur 153 en ont besoin). |
| Bilan des deux mandats (DEMO=9) | `bilan_deux_mandats.png` | ❌ Une correction (C-2). **Écart accepté** : « Piliers » est remonté en y 18 contre le bloc Économie. L'en-tête HIGHLIGHT et son filet suffisent, et y 25 respire avant le pied de page. |
| Saisie des initiales (DEMO=9 puis A) | `scores_initiales.png` | ✅ avec une retouche P3 (C-4). |
| Destitution (DEMO=6) | `destitution.png` | ✅ La démo affiche « Au moins une chambre vous sauve ». La variante « Deux chambres : vous êtes destitué » n'a pas pu être obtenue. Elle fait 34 colonnes depuis x 3 (fin en x 36) et utilise le même style ALERT que les chiffres du Sénat, donc elle tient. |
| Options, huissier (DEMO=11) | `options_musique.png`, `options_voix.png` | ✅ Tisserand, huissier, figure sous le médaillon. Les aides tiennent sur une ligne et la réplique de l'huissier est centrée entre guillemets (la plus longue finit en x 38). |

### Corrections restantes pour md-dev

- **C-1 (bloquant, 5 min) : titre de l'aide « Défense » en cohabitation.** Dans `scr_budget.c`, `open_help`, la boîte a pour titre `TXT_COHAB_DEFENSE`, un texte de 2 lignes qui s'affiche coupé (« Défense : seule ligne qui vous reste, »). Remplacez-le par `title = bl_long(line_of_focus());` (« Sécurité et défense »), comme pour les autres lignes, et gardez `TXT_COHAB_DEFENSE_AIDE` comme corps du texte. `TXT_COHAB_DEFENSE` n'est plus utilisé : retirez-le de `textes_v4.md` puis régénérez. Critère : sur DEMO=16 avec C, le titre tient sur une ligne.
- **C-2 (bloquant, 5 min) : la chute du bilan des deux mandats est coupée.** `TXT_FIN_SECOND_MANDAT`, recoupé sur 27 colonnes, demande 5 lignes, mais `ui_text_flow(1, 4, 27, 4, …)` s'arrête à « Place aux Mémoires, en », et la vanne « en trois tomes. » est perdue. Dans `scr_end.c` (branche `two`), passez à **5 lignes** (y 4-8) : le séparateur de y 9 sépare ensuite les blocs. Ne touchez pas au cas de la destitution, qui ne dépasse pas 4 lignes. Critère : sur DEMO=9, la phrase se termine par « tomes. ».
- **C-3 (non bloquant, 20 min) : nommer le Premier ministre.** Pour le PM, la ligne y 20 affiche « Matignon » seul, puis la réplique commence par « Matignon vous suit… ». La consigne P1-2 demandait le nom de famille du PM en place, avec « Matignon » pour fonction : « Deschamps, Matignon », ou le nom du PM de cohabitation via `TXT_COHAB_PM_*_NOM`. Utilisez le `surname()` mis en commun. La réplique peut rester telle quelle, puisque le PM qui parle de lui-même à la troisième personne colle au personnage.
- **C-4 (P3, 30 min) : points parasites sur la saisie des initiales.** Sur la ligne y 27, sous le cadre, une ligne de pixels de couleur apparaît après l'écran de fin. Ce sont probablement des tuiles d'un plan qui n'a pas été effacé. Elle est discrète mais reste dans les 224 lignes visibles en PAL. Effacez la ligne 27 (ou le plan concerné) dans `scores_init`.
- **P3, cosmétique** : la boîte de confirmation du référendum (y 18-26) laisse voir le bas du cadre de la fenêtre en y 27, ce qui donne un double filet. C'est acceptable en l'état.

Hors du champ de la DA : le pied de page de l'écran titre affiche encore « v3.0 » (`src/version.h`) sur la branche V4.

### Décisions DA (à reporter dans DECISIONS.md)
- Événement : pas de séparateur entre la réplique du ministre et le journal. L'icône et l'en-tête DIM du *Quotidien* suffisent.
- Référendum : l'aide peut occuper 5 lignes (y 19-23).
- Bilan des deux mandats : « Piliers » en y 18, collé au bloc Économie, est accepté.
- Aide de la Défense en cohabitation : le titre est le libellé long de la ligne (« Sécurité et défense »).

## V4.1 : changements visuels

Revue du 16/11 sur `v4.1-dev` (`7e1209d`), comparée à `v4-dev` (`76094b3`). Captures `tools/shots.sh` en PAL : démos 5, 12 (`w20 A w10 A w10 A w30`, pas de 70 ms entre 4,50 et 5,48 s), 20, 21, 23 (`w20 R w10 R`) et 25.

### Verdicts

| Point | Verdict |
|---|---|
| 1. Redessins nets (`ui_clear_quiet`) | ✅ **On généralise**, avec une seule exception : l'écran de résultat après une décision qui coûte des CP garde le fondu doux (consigne V41-1). Pages de l'aide : la page nette accompagnée de SFX_MOVE est meilleure que le fondu (on feuillette). Retour de la confirmation du remaniement : également meilleur. |
| 2. Hémicycle qui apparaît d'un bloc vers la 7e image | ✅ Acceptable sans effet d'apparition, **mais une régression bloquante est apparue** : l'animation du vote est perdue (V41-2). |
| 3. Légende « Faible » en daltonisme | ❌ À corriger, mais dans le sens inverse de ce qui est demandé : c'est la carte qu'il faut aligner, pas la légende (V41-3). |
| 4. Vignettes des 6 événements V4 | ✅ Les 6 choix conviennent. Une retouche de l'illustration « santé » est à prévoir (V41-4). |

**Point 1, la règle.** Le fondu doux (150 ms, image au tiers puis remontée) marque un **changement de pièce** ; un redessin net marque une action **dans la même pièce**.
- Redessin net : page suivante ou précédente, retour par B ou Annuler (boîte de confirmation, aide, liste), saisie (initiales), fermeture d'une aide, changement de ligne ou d'onglet.
- Fondu doux : passage du tableau de bord au vote, à l'Assemblée ou à la carte ; affichage du **résultat d'une décision payée en CP** (vote, 49.3, dissolution, remaniement, référendum). Sur la démo 5 en V4, le passage à « Remaniement ministériel : nouvelle équipe… » sous le fondu faisait un vrai temps de respiration, accompagné de SFX_ADOPTED et des CP qui passent de 12 à 9. En V4.1, le résultat claque d'un coup et on perd ce temps. C'est le seul cas où la V4 était meilleure.
- Au plus un fondu doux par appui de bouton. Pas de fondu doux si le réglage « Transitions » vaut « Aucune » (déjà géré par `ui_soft_replay`).

**Point 2.** Relevé V4.1 : à 4,50 s, le cadre est vide en pleine lumière (une image) ; de 4,57 à 4,71 s, le fondu doux montre le titre et la légende sans l'hémicycle ; à 4,78 s, l'hémicycle apparaît complet, à la fin du fondu. Un hémicycle qui « tombe » d'un bloc, en sièges éteints, se lit comme le début du vote : pas besoin d'effet. En revanche, **en V4.1 il apparaît déjà allumé, tous groupes compris, et le reste**. L'allumage groupe par groupe de la V4 (UGR, puis RSV, etc., 250 ms par groupe, visible de 4,85 à 5,48 s sur `v4-dev`) a disparu. C'était le suspense du vote : à rétablir avant toute livraison.

**Point 3.** `UI_MAP_LOW_CB` (7,2,0) ne diffère de `UI_MAP_LOW` (7,2,1) que d'un cran de bleu, ce qui est invisible à l'écran : la carte ne passe pas vraiment à l'orangé. Pour un daltonien rouge-vert, le contraste utile est déjà assuré par le passage de « Forte » au bleu ; rouge, jaune et bleu se distinguent aussi par la luminosité. Un vrai orangé (7,4,0) se confondrait avec le jaune « Moyenne » (7,6,1) : c'est à proscrire. La légende rouge est donc juste ; c'est la constante qui ment.

**Point 4.** Vignettes jugées sur les démos 20 et 21 et sur les planches de `res/gfx/` :
- EHPAD → santé (médecin endormi, « 36H DE GARDE ») : ✅ juste (« soins chronométrés »).
- Marche pour le climat → pollution (usine, ours sur son glaçon) : ✅. La vignette « grève » montrerait mieux une foule, mais son panneau « GRÈVE ! » serait faux. On garde « pollution ».
- Fracture numérique → numérique (« 404 ERREUR ») : ✅ excellent.
- Affectation → éducation (« ZÉRO VU ! ») : ✅.
- Chômage des jeunes → industrie (usine « FERMÉ », jeune en casquette) : ✅, le personnage est même du bon âge.
- Gel des pensions → bourse (CAC en chute) : ✅ par défaut. C'est la vignette de Bercy et de la dette, cohérente avec la condition `DETTE > 100`. Une vignette « retraites » dédiée relève de la V5, pas de la V4.1.

### Consignes pour md-dev

- **V41-1 (15 min) : fondu doux au résultat du remaniement, redessins nets ailleurs.**
  - `scr_reshuffle.c`, `rsh_draw`, branche `P_RESULT` : appeler `ui_soft_replay()` après le dessin. La branche `P_PICK` (retour d'Annuler) reste nette.
  - `scr_game.c` : ajouter `set_mode_quiet(Mode)`, qui fait la même chose que `set_mode` en plus d'un drapeau, et faire utiliser `art_reset_quiet()` aux fonctions de dessin de mode quand ce drapeau est levé. L'utiliser pour les retours par B ou Annuler : liste des lois annulée (l. 610), confirmation refusée (l. 626) ou en échec (l. 640 et 649), sortie de l'Assemblée (l. 917). Les passages en avant (`M_VOTE`, `M_RESULT`, `M_ASSEMBLY`, retour au menu après un résultat) gardent `art_reset()`.
  - Même règle pour `scr_referendum.c` (l. 218) et `scr_impeach.c` (l. 289) : fondu doux seulement si l'on affiche un résultat, net sinon. Saisie des initiales (`scr_scores.c`), fermeture de l'aide du budget : nettes.
  - Critères : démo 5 (`w20 A w10 A w15 A`), capture à +100 ms après le A de confirmation, image atténuée ; démo 24 avec Annuler, aucune image atténuée ; démo 23, aucune image atténuée entre deux pages.
- **V41-2 (bloquant, 30 min) : rétablir l'allumage progressif du vote.**
  - Cause probable : `ui_hemi_vote()` éteint les groupes par `set_group` en `DMA_QUEUE`, mais `soft_start()` lit la CRAM (`PAL_getColors`) dans la même image, avant le vidage de la file. Il capture donc la palette allumée comme cible, puis la repose en fin de fondu (`ui_frame_end`, `PAL_setColors(pal_target…)`), ce qui écrase l'état éteint.
  - Correctif : exposer dans `ui.c` une fonction `ui_pal_set(index, colors, n)` qui écrit la couleur **et** met à jour `pal_target` et `pal_from` pendant un fondu, sur le modèle de `colorblind_apply`. L'appeler depuis `set_group`, et aussi depuis le clignotement du groupe en évidence.
  - Critère : démo 12 (`w20 A w10 A w10 A w30`) ; à la fin du fondu, tous les groupes sont éteints, puis ils s'allument un par un toutes les 250 ms, comme sur `v4-dev`. Vérifier aussi en « Transitions : Rapides » (75 ms, le fondu finit avant l'hémicycle) et « Aucune ».
- **V41-3 (5 min) : carte daltonienne.** Dans `ui_map.h`, poser `UI_MAP_LOW_CB = UI_MAP_LOW`, ou supprimer la constante et son test dans `ui_map.c`, et corriger le commentaire (« orangé »). La légende de `scr_map.c` ne change pas. Critère : démo 25 avec une région faible, même rouge sur la pastille et sur la carte (mesure au pixel).
- **V41-4 (P3, 15 min, `tools/art/` puis `make art`) : la vignette « santé » est rognée par le médaillon.** Le cadre posé sur l'image (`ui_medallion` sur `ax, ay`) mange environ 4 px de chaque bord : sur la démo 20, on lit « 6H / DL / ARDE » au lieu de « 36H DE GARDE ». Décaler le texte rouge de 4 px vers la droite dans le générateur de la vignette (ou réduire sa police d'un cran) pour qu'aucun pixel de texte ne soit à moins de 5 px du bord. Règle générale pour les vignettes d'événement : aucun texte ni détail lisible dans les 5 px du bord.
- **V41-5 (P2, 20 min) : flash d'une image avant le fondu doux.** Déjà présent en V4 : à l'entrée du vote, une image montre le nouveau cadre en pleine lumière avant l'atténuation (à 4,50 s sur la démo 12 ; en V4, l'hémicycle partiel apparaissait aussi). `soft_start()` ne s'exécute qu'en fin d'image, après un dessin qui dure plus d'une image. Correctif : dans `ui_clear()`, poser **tout de suite** la palette atténuée (calculer `pal_target` et `pal_from`, puis écrire `pal_from` en CPU) avant d'effacer les plans, puis lancer le `PAL_fade` en fin d'image comme aujourd'hui. Critère : démo 12, aucune image en pleine lumière entre le menu et le vote atténué.

### Décisions DA (à reporter dans DECISIONS.md)
- Transitions internes : redessin net par défaut. Le fondu doux est réservé aux changements de pièce et aux résultats de décisions payées en CP.
- L'hémicycle peut apparaître d'un bloc à la fin du fondu, sans effet propre, à condition qu'il apparaisse éteint.
- Daltonisme : « Faible » reste rouge (7,2,1), sur la carte comme dans la légende ; seul « Forte » passe au bleu.
- Vignettes des événements V4 : bourse, santé, numérique, éducation, industrie et pollution sont validées ; une vignette « retraites » est renvoyée en V5.
- Vignettes d'événement : marge de 5 px sans texte, à cause du cadre du médaillon.
