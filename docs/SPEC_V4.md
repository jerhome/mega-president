# Mega Président — Spécification V4 : 5 électorats et budget détaillé

> **Statut : proposition du 15/11, à valider par le joueur** (questions en fin de document).
> Reprend les tâches reportées 2.09 (5 électorats) et 2.10 (budget détaillé).
> Toutes les valeurs sont des points de départ, à caler avec le simulateur PC (`tests/test_sim_v1.c`).
> Unités internes inchangées (`docs/SIM.md`) : opinion en dixièmes de %, déficit en dixièmes de point de PIB par an, croissance en centièmes, piliers en dixièmes.

## 0. Principes

1. **Ne pas casser l'équilibrage V2** : avec les réglages « neutres » (budget Équilibre, contenu inchangé), la moyenne pondérée des électorats doit reproduire la popularité V2 à ±1 point. Les nouvelles tables sont construites pour ça (vérification en §1.3).
2. **Compatibilité du contenu** : les clés d'effets existantes (`POP_POPULAIRES`, `POP_MOYENNES`, `POP_AISEES`, `POP_TOUS`) restent valides ; 3 clés s'ajoutent.
3. **Le budget détaillé garde un raccourci** : 3 préréglages (Rigueur, Équilibre, Relance) reproduisent à peu près le budget V2. Le joueur pressé valide en deux pressions, le joueur curieux ajuste ligne par ligne.
4. Tout reste dans `src/sim/` (portable, entiers) ; seuls les écrans touchent au VDP.

---

## 1. Cinq électorats

### 1.1 Découpage proposé : classe sociale pour les actifs, âge pour les autres

| # | Électorat | Abrév. (tableau de bord) | Qui | Poids électoral |
|---|---|---|---|---|
| 0 | Classes populaires | `Pop` | ouvriers et employés actifs, 30-64 ans | 26 |
| 1 | Classes moyennes | `Moy` | professions intermédiaires, artisans, commerçants, petits indépendants, agriculteurs | 22 |
| 2 | Cadres | `Cad` | cadres, professions libérales, chefs d'entreprise | 12 |
| 3 | Retraités | `Ret` | retraités, quel que soit leur ancien métier | 28 |
| 4 | Jeunes | `Jeu` | 18-29 ans (étudiants, jeunes actifs, chômeurs) | 12 |

Les catégories sont **exclusives** (chaque électeur est dans une seule). Les poids correspondent à la part des **suffrages exprimés** : l'abstention est déjà prise en compte, ce qui explique que les retraités pèsent 28 et les jeunes seulement 12.

**Pourquoi ce découpage** : depuis vingt ans, l'âge divise autant le vote français que la classe sociale. Les retraités forment le socle du bloc central et de la droite ; les jeunes votent écologiste ou à gauche radicale, ou s'abstiennent. Un découpage purement social (ouvriers, employés, intermédiaires, cadres, indépendants) donnerait 5 électorats qui réagissent presque de la même façon. Ici, chaque électorat a un levier bien identifiable : les **retraites** et la sécurité pour les retraités, l'**écologie**, l'**éducation** et l'emploi pour les jeunes. Le clivage ville/campagne reste géré par les régions.

`ELEC_AISEES` devient `ELEC_CADRES` (même index 2) ; `ELEC_RETRAITES` (3) et `ELEC_JEUNES` (4) s'ajoutent. `ELEC_COUNT` = 5.

### 1.2 Sensibilités (`opinion.c`)

| | Pop | Moy | Cad | Ret | Jeu |
|---|---|---|---|---|---|
| Affinité gauche (dixièmes) | +30 | 0 | −30 | −10 | +30 |
| Affinité centre | −20 | +10 | +15 | +15 | −20 |
| Affinité droite | −40 | 0 | +40 | +10 | −35 |
| `k_unemp` (chômage) | 5 | 4 | 3 | 3 | 6 |
| `k_growth` (croissance) | 1 | 2 | 4 | 1 | 2 |
| Social | 6 | 3 | 1 | 5 | 4 |
| Économie | 3 | 4 | 6 | 3 | 2 |
| Technologie | 1 | 2 | 4 | 0 | 3 |
| Écologie | 2 | 3 | 3 | 1 | 6 |
| Sécurité | 4 | 4 | 2 | 7 | 1 |
| *(somme des 5 piliers)* | *16* | *16* | *16* | *16* | *16* |
| International (hors somme) | 1 | 1 | 2 | 1 | 1 |
| Opinion de départ : gauche / centre / droite (%) | 58 / 46 / 44 | 53 / 54 / 55 | 40 / 54 / 62 | 48 / 56 / 60 | 60 / 44 / 38 |
| Sensibilité au « sérieux budgétaire » `k_s` (§2.3) | 0 | 3 | 10 | 6 | 0 |

**Popularité** = (26 Pop + 22 Moy + 12 Cad + 28 Ret + 12 Jeu) / 100, au lieu de (4 Pop + 4 Moy + 2 Ais) / 10. Les poids sont rangés dans une table `elec_weight[ELEC_COUNT]` en ROM, réutilisée par la popularité, la campagne et le référendum.

### 1.3 Vérification de l'équilibre (moyennes pondérées)

| Grandeur | V2 (3 électorats) | V4 (5 électorats) |
|---|---|---|
| Affinité gauche / centre / droite | +4 / +1 / −8 | +5 / +0,6 / −7 |
| `k_unemp` moyen | 4,2 | 4,1 |
| `k_growth` moyen | 1,8 | 1,7 |
| Opinion de départ gauche / centre / droite | 52,8 / 51,6 / 52,6 | 52,2 / 51,3 / 52,3 |

Écarts inférieurs à 1 point : `OPN_BASE` ne devrait pas bouger, et l'écart droite-gauche se réduit légèrement (la droite perdait ≈ 3 points de réélection en V2).

### 1.4 Lois et événements

- Nouvelles clés d'effets, ajoutées **à la fin** de `EffectKey` (aucune renumérotation) : `EFF_POP_RETRAITES`, `EFF_POP_JEUNES`. `POP_CADRES` est un **alias** de `POP_AISEES` dans `gen_content.py` (même clé C).
- `POP_TOUS` touche les 5 électorats. `POP_POPULAIRES` et `POP_MOYENNES` gardent leur sens.
- **Passe de contenu** (rédacteur) :
  - les **40 lois** : effets immédiats revus sur 5 électorats, par exemple réforme des retraites `Ret −5, Jeu +1` ; police `Ret +2, Jeu −2` ; cantine bio et rénovation `Jeu +2` ; ISF `Ret −1`. `LAW_MAX_IMMEDIATE` passe de 3 à 4 (+40 × 2 octets de ROM) ;
  - **environ 50 événements** sur les plus de 100 (les plus évidents : canicule, grève lycéenne, EHPAD, Parcoursup fictif, marche pour le climat, etc.) ;
  - **6 nouveaux événements** ciblés sur les retraités et les jeunes.
  - Les autres événements restent tels quels : leurs effets portent sur 3 électorats, et la dynamique de fond fait vivre les 2 nouveaux.
- **Régions** : effets de lois (`region_laws`) inchangés.

### 1.5 Régions (`region_profiles.weight`, somme = 100)

| Région | Pop | Moy | Cad | Ret | Jeu |
|---|---|---|---|---|---|
| Hauts-de-France | 32 | 22 | 8 | 24 | 14 |
| Normandie | 28 | 22 | 9 | 30 | 11 |
| Île-de-France | 18 | 22 | 26 | 18 | 16 |
| Grand Est | 30 | 23 | 9 | 26 | 12 |
| Bretagne | 24 | 24 | 10 | 31 | 11 |
| Pays de la Loire | 25 | 24 | 10 | 29 | 12 |
| Centre-Val de Loire | 27 | 23 | 9 | 30 | 11 |
| Bourgogne-Franche-Comté | 28 | 22 | 8 | 31 | 11 |
| Nouvelle-Aquitaine | 26 | 22 | 9 | 32 | 11 |
| Auvergne-Rhône-Alpes | 24 | 23 | 15 | 25 | 13 |
| Occitanie | 26 | 22 | 11 | 28 | 13 |
| PACA | 25 | 21 | 12 | 31 | 11 |
| Corse | 30 | 20 | 8 | 32 | 10 |

Sensibilités aux piliers et au chômage, ainsi que la formule de l'écart régional : inchangées. La carte contrastera davantage : les régions de retraités (Bretagne, Nouvelle-Aquitaine, Corse) et l'Île-de-France (cadres, jeunes) réagiront différemment.

### 1.6 Élection, législatives, Sénat, référendum

- **Campagne** (`sim_campaign_popularity`) : poids `elec_weight`. Poids des thèmes (/4) :

| Thème | Pop | Moy | Cad | Ret | Jeu |
|---|---|---|---|---|---|
| Emploi | 4 | 3 | 1 | 0 | 4 |
| Sécurité | 2 | 3 | 2 | 4 | 0 |
| Pouvoir d'achat | 4 | 4 | 0 | 3 | 2 |
| Écologie | 1 | 3 | 3 | 0 | 4 |

  Crédibilité du pouvoir d'achat : le test `budget == BUDGET_RIGUEUR` (−1 point) devient « ligne Social < 0 ou impôts des ménages > 0 ».
- **Législatives et Sénat** : le code actuel fait `opinion[c->orientation]` (alignement implicite ORIENT/ELEC, **à supprimer**). Électorat-cœur : gauche = (Pop + Jeu) / 2, centre = (Moy + Ret) / 2, droite = (Cad + Ret) / 2. Une fonction `sim_core_opinion(c)` est utilisée par `institutions.c` et `senate.c`.
- **Référendum** : orientation des électorats Pop −30, Moy 0, Cad +40, Ret +20, Jeu −40 (au lieu de −40 / 0 / +40) ; total pondéré par `elec_weight`. Les 5 « oui » s'affichent sur 5 lignes.
- **Cohabitation** : la règle « exposition aux deux tiers » s'applique par électorat, sans changement.

### 1.7 Affichage (38 colonnes)

- **Tableau de bord**, ligne 5 (actuellement `Pop. 45,2  Moy. 45,2  Ais. 45,2 %`, 36 colonnes) : les valeurs passent en entiers.
  `Pop 45 Moy 52 Cad 38 Ret 61 Jeu 29 %`, soit 36 colonnes. Chaque valeur est colorée selon sa tendance (vert si elle monte, rouge si elle baisse, comparée au tour précédent). La couleur reprend le style « positif / négatif » de la charte V3 et ne demande pas de nouvelle palette.
- **Détail par le bouton C** sur la popularité : un écran « Électorats » avec 5 jauges, la valeur à une décimale, la variation du tour et une phrase de contexte (« Les retraités s'inquiètent pour leurs pensions »).
- **Écrans référendum et campagne** : listes de 5 lignes (noms complets : `Classes populaires`, `Classes moyennes`, `Cadres`, `Retraités`, `Jeunes`).
- **Écran budget** : aperçu de l'opinion en symboles (§2.6).

---

## 2. Budget détaillé

### 2.1 Postes et niveaux

**6 lignes** (4 dépenses, 2 impôts) et **5 crans par ligne** : −2, −1, 0 (reconduction des crédits), +1, +2. Pour les impôts, + signifie une hausse.

| # | Ligne | Domaine |
|---|---|---|
| 0 | Social et retraites | santé, pensions, minima sociaux |
| 1 | Éducation et recherche | école, université, recherche |
| 2 | Sécurité et défense | police, justice, armées (**domaine réservé** en cohabitation) |
| 3 | Transition et réseaux | énergie, transports, infrastructures |
| 4 | Impôts des ménages | impôt sur le revenu, TVA, CSG |
| 5 | Impôts des entreprises et du capital | impôt sur les sociétés, fiscalité du patrimoine |

Le choix de 6 lignes reste lisible sur un seul écran sans défilement. Il est cohérent avec les 6 piliers et laisse au joueur un vrai arbitrage fiscal (ménages ou entreprises) sans aller jusqu'aux 6 postes et 3 impôts de REGLES §7.

### 2.2 Effets par cran (+1), tables en ROM (`budget.c`)

| Ligne (+1) | Déficit (dixièmes de pt/an) | Croissance (centièmes) | Piliers (dixièmes par trimestre) | Tension | Pop | Moy | Cad | Ret | Jeu | Orientation (vote) |
|---|---|---|---|---|---|---|---|---|---|---|
| Social et retraites | +5 | +5 | Soc +3 | −2 | +10 | +3 | −3 | +10 | +3 | −15 |
| Éducation et recherche | +3 | +5 | Tech +3, Soc +1 | 0 | +2 | +5 | +3 | 0 | +10 | −10 |
| Sécurité et défense | +3 | 0 | Séc +3, Intl +1 | 0 | +2 | +3 | +2 | +8 | −3 | +15 |
| Transition et réseaux | +4 | +10 | Écol +3, Éco +1 | −1 | 0 | +3 | +3 | −2 | +8 | −10 |
| Impôts des ménages (hausse) | −6 | −10 | — | +2 | −6 | −10 | −8 | −6 | −3 | 0 |
| Impôts des entreprises (hausse) | −5 | −15 | Éco −3 | −1 | +3 | 0 | −10 | −2 | +2 | −20 |

Les colonnes Pop à Jeu sont des effets sur la cible d'opinion, en dixièmes de %.

**Effet d'un niveau** :
- **déficit, croissance, tension et orientation : linéaires** (niveau × valeur) ;
- **piliers et opinion : asymétriques**, avec le multiplicateur `m[niveau + 2] = { −5, −2, 0, 2, 3 }` puis une division par 2. Ainsi +2 ne rapporte que 1,5 fois l'effet d'un cran (rendement décroissant), tandis que −2 coûte 2,5 fois (une coupe franche fait très mal). Le calcul reste entier.

**Branchement dans la simulation** (aux mêmes endroits que le budget V2) :
- `economy.c` : déficit du plan dans le déficit annuel ; croissance du plan dans `growth_trend` ; tension du plan dans la cible de tension ;
- `budget_quarter_update` : piliers ;
- `opinion.c` : `sim_budget_opinion(plan, e)`.

### 2.3 Garde-fous (pas de stratégie dominante)

- **Sérieux budgétaire** : opinion de l'électorat e += −déficit_plan × `k_s[e]` / 10. Un point de déficit en moins rapporte +1 point chez les cadres, +0,6 chez les retraités et +0,3 chez les classes moyennes. C'est ce qui fait que la rigueur plaît encore aux cadres, comme le budget Rigueur plaisait aux aisés en V2.
- **Budget « insincère »** : si le déficit du plan dépasse +1,5 point, chaque groupe hors du parti du joueur perd (déficit − 15) / 2 points de score de vote. Si la dette dépasse 120 %, une **prime de risque** retire 5 centièmes de croissance par dixième de déficit au-delà de 10.
- Le joueur automatique « optimiseur » (§3.4) vérifie qu'aucun plan extrême (« tout à +2, impôts à −2, 49.3 chaque année ») ne bat le joueur actif.

### 2.4 Préréglages (ligne 0 de l'écran, raccourci)

| Préréglage | Soc | Édu | Séc | Trans | Mén | Ent | Déficit | Croissance | Popularité pondérée (V2 entre parenthèses) |
|---|---|---|---|---|---|---|---|---|---|
| Rigueur | −1 | 0 | 0 | −1 | +1 | 0 | −1,5 (−1,5) | −0,25 (−0,30) | −0,9 point (−0,7) |
| Équilibre | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| Relance | +1 | +1 | 0 | +1 | 0 | 0 | +1,2 (+1,5) | +0,20 (+0,30) | +0,6 point (+0,9) |

Dès que le joueur modifie une ligne, le préréglage affiché devient « Personnalisé ». Le champ `Country.budget` est conservé pour mémoriser le préréglage d'origine (`BUDGET_PERSO` = 3), utile au journal.

### 2.5 Vote, 49.3, reconduction, cohabitation, Sénat

- **Orientation de vote** = orientation du joueur / 2 + Σ niveau × orientation de la ligne, bornée à ±100, puis le même `parl_vote`. L'aperçu du vote est recalculé à chaque changement de cran (5 groupes, calcul immédiat).
- **49.3** : inchangé (4 CP, illimité, défiance +8). Le plan passe tel quel.
- **Reconduction** (aucun budget adopté en fin d'année) : les dépenses se rapprochent de 0 d'un cran, selon le principe des « services votés », et les impôts ne changent pas. Cette règle remplace « reconduction = Équilibre » et empêche d'imposer une fois un plan extrême en 49.3 puis de le laisser se reconduire indéfiniment.
- **Cohabitation** : le PM d'opposition impose son plan. Camp gauche : `+1 +1 0 +1 0 +1`. Centre : tout à 0. Droite : `−1 0 +1 −1 0 −1`. Le président garde la main sur la **ligne Sécurité et défense**, à ±1 cran autour du plan du PM, sans coût en CP. C'est cohérent avec REGLES §8, où la défense reste au président. L'écran budget s'ouvre en lecture seule, avec cette seule ligne modifiable.
- **Sénat** : si le bloc présidentiel n'y a pas la majorité (`sim_senate_has_majority` = 0), le Sénat **amende** le budget adopté, par vote comme par 49.3 : la ligne la plus éloignée de son orientation (centre-droit, environ +20) recule d'un cran vers celle-ci. Le joueur peut demander la **lecture définitive** pour 1 CP et rétablir son plan (l'Assemblée a le dernier mot). Le journal l'annonce : « Le Sénat rabote le budget social ». Le Sénat n'intervient pas en cohabitation.
- Calendrier inchangé : présentation au 4e trimestre (les 3 derniers mois en mode 60 tours), application toute l'année suivante.

### 2.6 Ergonomie (320 × 224, 38 colonnes, manette 3 boutons)

```
 Budget 2029                  CP 7/15
 Préréglage      < Personnalisé >
 ────────────────────────────────────
 Social, retraites  ◄ ▮▮▮▮□ ►  +1
 Éducation, rech.   ◄ ▮▮▮□□ ►   0
 Sécurité, défense  ◄ ▮▮▮□□ ►   0
 Transition, rés.   ◄ ▮▮□□□ ►  -1
 Impôts ménages     ◄ ▮▮▮□□ ►   0
 Impôts entreprises ◄ ▮▮▮▮▮ ►  +2
 ────────────────────────────────────
 Déficit   +0,4 pt/an   Croiss. -0,20
 Tension   +1           Piliers ...
 Pop +  Moy =  Cad --  Ret +  Jeu ++
 Vote : 301 / 577  ██████████░░░  OK
 [A] Voter  [C] Aide  [B] Retour
```

- **Haut/bas** : choisir la ligne (la ligne 0 est le préréglage). **Gauche/droite** : changer de cran (ou de préréglage). **A** : menu Voter / 49.3 / Annuler. **B** : retour au tableau de bord, le plan en cours est gardé jusqu'à la fin de l'année. **C** : aide sur la ligne (effets en clair).
- La zone d'effets et l'aperçu du vote se redessinent à chaque cran (rafraîchissement partiel, déjà pratiqué par `scr_budget.c` avec `detail_dirty`).
- Opinion en symboles : `++` au-delà de +1,5 point, `+` à partir de +0,5, `=` entre les deux, puis `-` et `--` (5 × 6 caractères, ce qui tient dans 38 colonnes).
- En cohabitation : les lignes verrouillées sont grisées et le titre devient « Budget du gouvernement ».
- Le préréglage permet de valider en deux pressions : gauche/droite sur la ligne 0, puis A.

---

## 3. Contraintes techniques

### 3.1 Calcul

Tout en entiers (`s8`/`s16` en ROM, calculs en `s32`) ; le multiplicateur asymétrique se résume à une table et une division par 2. Aucun flottant, aucune allocation.

### 3.2 Mémoire

| Champ de `Country` | V2 | V4 | Écart |
|---|---|---|---|
| `opinion[ELEC_COUNT]` (s16) | 6 | 10 | +4 |
| `acc_opinion[ELEC_COUNT]` (s16) | 6 | 10 | +4 |
| `budget_lvl[6]` (s8, plan en vigueur) | — | 6 | +6 |
| `opinion_prev[5]` pour les flèches de tendance | — | (dans `Game`, hors `Country`) | 0 |
| **Total** | **172 octets** (PC, mesuré le 15/11) | **≈ 186** | **+14** |

- Le test `sizeof(Country) <= 220` reste valable, avec une marge de 34 octets.
- `Game` gagne aussi `opinion_prev[5]` (s16 : +10) et un drapeau d'amendement du Sénat dans `TurnReport` (+1 à 2).
- Comme `sram_game.c` garde un tampon statique de la taille de `Game`, la RAM statique augmente au total d'environ **+50 octets**, à quoi s'ajoutent environ 8 octets pour l'état de l'écran budget.
- ROM : moins de 1 Ko de tables, plus les textes et les 6 événements (environ 3 Ko).

### 3.3 Sauvegarde SRAM

- `sizeof(Game)` change, donc une sauvegarde V3 serait de toute façon refusée par le contrôle de taille. On porte en plus `GAMESAVE_VERSION` de 1 à 2, pour que le refus soit explicite.
- **Recommandation : pas de migration.** L'écran titre affiche « Partie sauvegardée d'une version précédente : impossible à reprendre » et propose une nouvelle partie. Les high scores (bloc « MPV1 », offset 0) ne sont pas touchés : la partie en cours commence à l'offset 256 et dispose de bien plus que les ≈ 600 octets nécessaires.
- Si le joueur tient à la migration : une fonction `gamesave_migrate_v1` relirait l'ancienne disposition à partir d'une copie figée `CountryV2` / `GameV2`. Les nouveaux champs seraient remplis ainsi : Ret = Moy, Jeu = Pop, plan tiré du préréglage. Coût ≈ +2 h, avec un risque de layout sur 68000.

### 3.4 Équilibrage

- Cibles identiques à la V2 : réélection du joueur actif **71 / 55 / 25 % (±3)** selon la difficulté ; écart entre orientations ≤ 10 points ; joueur passif < 10 % ; mode 60 tours à ±5 points ; destitution rare.
- Joueur automatique : remplacer le choix rigueur / relance par les **préréglages** (même logique d'orientation). Trois variantes s'ajoutent :
  - « **optimiseur** » : glouton, il choisit chaque année le plan qui maximise la popularité prévue ;
  - « **extrême** » : tout à +2, impôts à −2, 49.3 ;
  - « **austère** » : tout à −2.
  Aucune des trois ne doit dépasser le joueur actif de plus de 5 points de réélection.
- Nouvelles mesures du simulateur : popularité finale par électorat et par orientation (aucun électorat durablement sous 20 % ou au-dessus de 80 %), dette finale par préréglage, fréquence des amendements du Sénat.
- Leviers de calage, dans cet ordre : `OPN_BASE`, les poids `k_s`, puis l'échelle des colonnes d'opinion de §2.2.

---

## 4. Tâches du backlog V4

Les tâches de simulation (`src/sim/`, `tests/`, `tools/content/`) touchent des fichiers disjoints de ceux de la V3 et peuvent démarrer **pendant** la V3. Les tâches d'écran (4.07, 4.11) attendent la refonte V3 des écrans (3.05 et 3.06) pour éviter les conflits.

| # | Tâche | Durée | Agent | Dépend de | Fichiers |
|---|---|---|---|---|---|
| 4.01 | Arbitrage des questions ouvertes et mise à jour de la spec | 0,5 h | session principale | — | `docs/SPEC_V4.md`, `docs/DECISIONS.md` |
| 4.02 | Cœur 5 électorats : enum, `elec_weight`, tables d'opinion, popularité, opinion de départ, `sim_core_opinion` (législatives, Sénat), référendum, thèmes, clés `EFF_POP_RETRAITES/JEUNES`, tests unitaires | 4 h | md-dev | 4.01 | `sim_state.h`, `opinion.c`, `sim.c`, `effects.*`, `institutions.c`, `senate.c`, `actions.*`, `election.c`, `tests/test_sim*.c` |
| 4.03 | Régions à 5 poids | 1 h | md-dev | 4.02 | `regions.*`, `tests/test_regions.c` |
| 4.04 | Générateur de contenu : nouvelles clés, alias `POP_CADRES`, `LAW_MAX_IMMEDIATE` = 4, contrôle des limites | 1 h | md-dev | 4.02 | `tools/content/gen_content.py`, `laws.h` |
| 4.05 | Passe sur les 40 lois (effets immédiats sur 5 électorats) | 3 h | redacteur | 4.04 | `docs/contenu/lois_*.md` |
| 4.06 | Passe sur environ 50 événements et 6 nouveaux événements (retraités, jeunes) | 4 h | redacteur | 4.04 | `docs/contenu/evenements_*.md` |
| 4.07 | Écrans électorats : ligne du tableau de bord avec tendances, écran détail (C), référendum et campagne à 5 lignes, aide | 3 h | md-dev | 4.02, V3 3.05-3.06 | `scr_game.c`, `scr_referendum.c`, `scr_campaign.c`, `scr_help.c`, `game.h` |
| 4.08 | Cœur budget détaillé : `budget_lvl[6]`, tables, multiplicateur, préréglages, reconduction par services votés, garde-fous, orientation de vote, 49.3, aperçus, tests | 5 h | md-dev | 4.02 | `budget.*`, `ids_v1.h`, `sim_state.h`, `economy.c`, `opinion.c`, `sim.c` |
| 4.09 | Budget en cohabitation (plan du PM, ligne défense) et amendement du Sénat (lecture définitive à 1 CP), comptes rendus | 2,5 h | md-dev | 4.08 | `cohab.c`, `senate.c`, `budget.c`, `sim.h` (TurnReport) |
| 4.10 | Textes : noms des lignes, aide C, phrases du journal (reconduction, Sénat, budget du PM), phrases de l'écran électorats | 1,5 h | redacteur | 4.01 | `docs/contenu/textes_v4.md` |
| 4.11 | Écran budget détaillé (6 lignes × 5 crans, préréglage, aperçu en direct, vote / 49.3, cohabitation, Sénat) | 5 h | md-dev | 4.08, 4.09, 4.10, V3 3.06 | `scr_budget.c`, `content_texts.*` |
| 4.12 | Sauvegarde : `GAMESAVE_VERSION` = 2, message à l'écran titre, test de non-régression | 1 h | md-dev | 4.02, 4.08 | `gamesave.h`, `sram_game.c`, `scr_title.c`, `tests/test_gamesave.c` |
| 4.13 | Joueurs automatiques : préréglages, variantes optimiseur, extrême et austère, statistiques par électorat | 3 h | md-dev | 4.08 | `tests/test_sim_v1.c`, `tests/test_sim_v2.c` |
| 4.14 | Équilibrage V4 (cibles de §3.4, 20 et 60 tours) | 5 h | md-dev | 4.05, 4.06, 4.09, 4.13 | tables de `opinion.c`, `budget.c`, `docs/SIM.md` |
| 4.15 | Documentation : REGLES (V4), SIM, DECISIONS, BACKLOG | 1,5 h | redacteur | 4.14 | `docs/*.md` |
| 4.16 | Contrôle qualité : `make check`, budgets RAM et ROM, BlastEm PAL et NTSC, 38 colonnes, deux mandats, cohabitation forcée, sauvegarde | 2 h | qa-rom | 4.07, 4.11, 4.12, 4.14 | — |
| 4.17 | Jalon : test console du joueur, étiquette git, release | 1 h | qa-rom + joueur | 4.16 | `src/version.h` |
| | **Total** | **≈ 44 h** | | | |

**Parallélisme** (3 agents au maximum) :
- vague 1 : 4.02, puis 4.03, 4.04 et 4.08 en parallèle de 4.10 (rédacteur) ;
- vague 2 : 4.05 et 4.06 (rédacteur), 4.09, 4.12 et 4.13 (md-dev) ;
- vague 3 : 4.14, puis les écrans 4.07 et 4.11 (après la V3) ;
- vague 4 : 4.15, 4.16, 4.17.

Durée calendaire estimée : environ 20 h.

**Raccourci possible (≈ 28 h)** : supprimer l'amendement du Sénat (−1,5 h), les 6 nouveaux événements et la moitié de la passe sur les événements (−2,5 h), l'écran détail des électorats (−1 h), les variantes « austère » et « extrême » (−1 h), et ramener le budget à 4 lignes × 3 crans (−4 h sur 4.08, 4.11 et 4.14 réunies). Ce raccourci reste cohérent, mais plus pauvre.

---

## 5. Questions ouvertes pour le joueur

1. **Découpage des électorats** : âge et classe (populaires, moyennes, cadres, retraités, jeunes : proposé), ou découpage purement social (ouvriers, employés, intermédiaires, cadres, indépendants et agriculteurs) ?
2. **Finesse du budget** : 6 lignes × 5 crans avec préréglages (proposé), ou 4 lignes × 3 crans (plus rapide, environ −4 h) ?
3. **Rôle du Sénat dans le budget** : amendement d'une ligne, annulable par la lecture définitive pour 1 CP (proposé), ou pas de rôle budgétaire (le Sénat ne compte que pour la destitution) ?
4. **Sauvegarde** : accepter qu'une partie en cours sous V3 ne puisse pas être reprise en V4, les high scores étant conservés (proposé), ou écrire une migration (+2 h) ?
5. **5e thème de campagne « Santé et retraites »** (Ret 4, Pop 2, Moy 2 ; crédibilité = progression du pilier Social), pour donner un thème aux retraités : l'ajouter (+1 h) ou garder 4 thèmes ?
