# Cœur de simulation — formules et constantes

> Référence pour l'équilibrage. Code : `src/sim/`. Unités internes : `docs/CONTRAT_V0.md` §2.
> `tpy` = tours par an (4 : 20 tours ; 12 : mode 60 tours, V2 — voir « Mode 60 tours »).
> Les variations annuelles sont ramenées au tour par `sim_step` (reste reporté d'un tour à l'autre, pas de perte par troncature).

## Fichiers

| Fichier | Rôle |
|---|---|
| `sim_state.h` | struct `Country` (≈ 164 octets sur 68000 avec alignement 2 ; 172 sur PC ; 126 en V1), énumérations, bornes |
| `sim.h` / `sim.c` | `sim_init_ex`, `sim_end_turn`, lois, 49.3, télévision, PM, jauges, CP, 2nd mandat, scores |
| `effects.*` | clés d'effets, conversion et bornage (`sim_apply_effect`) |
| `laws.*` | catalogue `law_defs` (40 lois, masque `LawMask` de 64 bits : `law_mask_get/set`), coût effectif, file des lois en attente |
| `economy.c` | croissance, chômage, dette, tension |
| `opinion.c` | opinion des 3 électorats, état de grâce, usure |
| `parliament.*` | votes, relations |
| `budget.*` | budget annuel (rigueur / équilibre / relance) |
| `institutions.*` | législatives, dissolution, majorité relative, censure, destitution |
| `election.*` | présidentielle, thème de campagne, score de mandat |
| `senate.*` | V2 : Sénat (348 sièges), renouvellement, voix de destitution du Sénat |
| `cohab.*` | V2 : cohabitation (PM d'opposition, lois de souveraineté, lois du gouvernement, budget imposé) |
| `actions.*` | V2 : référendum, remaniement |
| `regions.*` | V2 : popularité par région (13 régions, profils en ROM) |

## Conversion des clés d'effets (V1)

Comme le contrat §3, sauf :

| Clé | 1 unité = | Raison |
|---|---|---|
| `DETTE` | **0,5 point de PIB** (5 en interne) | les événements faisaient dériver la dette de +36 points en 5 ans |
| `SCANDALE` | **2 points de jauge** | les valeurs du contenu (+2 à +6) ne permettaient jamais d'atteindre le seuil de destitution |
| `REL_PARTI` | 1 point de relation avec le parti du joueur (G2, G3 ou G4) | nouvelle clé |

## Départ (`sim_init_ex`)

| Grandeur | Facile | Normal | Difficile |
|---|---|---|---|
| Croissance (± 0,20) | 1,70 % | 1,20 % | 0,40 % |
| Chômage (± 0,30) | 7,00 % | 7,50 % | 8,70 % |
| Dette | 100 % | 110 % | 120 % |
| Opinion (décalage) | +2 | 0 | −2 |
| Relations (décalage) | +5 | 0 | −5 |
| CP | 5 | 4 | 3 |
| Tension / défiance | 15 / 5 | 20 / 10 | 32 / 20 |

- Opinion de base (pop./moy./ais.) : gauche 58/53/42 %, centre 50/54/50 %, droite 46/55/61 % (± 1,5).
- Relations : parti 80, voisins 55, distance 2 : 30, distance 3 : 15 (+ décalage de difficulté).
- Sièges (G1…G5) :

| | gauche | centre | droite |
|---|---|---|---|
| Facile | 85 175 150 100 67 | 65 150 195 110 57 | 60 125 150 155 87 |
| Normal | 75 140 160 120 82 | idem | idem |
| Difficile | 70 115 95 170 127 | 115 110 105 70 177 | 120 170 90 105 92 |

En Difficile, le **bloc présidentiel** (parti + groupes voisins) a moins de 289 sièges : majorité relative dès le départ.

### Premier ministre

| PM | Effet |
|---|---|
| Technocrate | tendance de croissance +0,30 point ; opinion cible −2,5 points |
| Rassembleur | relations +5 à la nomination ; relations « naturelles » +10 (dérive) |
| Réformateur | coût des lois −1 CP (minimum 1) |

`sim_init` (V0) = `sim_init_ex` sans PM (`PM_NONE`), difficulté Normal.

## Fin de tour (`sim_end_turn`)

Ordre : 1. lois arrivées à maturité → 2. budget (piliers, au trimestre) → 3. économie → 4. opinion → 5. Parlement → 6. jauges (au trimestre) → 7. clôture budgétaire (fin d'année) → 8. CP (au trimestre) → 9. institutions : destitution puis censure (au trimestre) → 10. (V2) renouvellement du Sénat. V2 : l'étape 5b (au trimestre) fait proposer ses lois au gouvernement de cohabitation et érode l'état de grâce du remaniement ; l'étape 7 laisse le PM d'opposition décider du budget.
Les événements sont appliqués **avant** par l'appelant. Une partie terminée (destitution) ne bouge plus.

### Économie

- **Tendance** = 1,10 % + (Économie − 50) × 0,02 + (Technologie − 50) × 0,01 + budget + PM.
- **Croissance** : `g += (tendance − g) / tpy + aléa(±0,25)`, bornée à ±8 %.
- **Chômage (Okun)** : variation annuelle `−(g − 1,10) / 2 + (7,50 − u) / 10`. Borné à 2–25 %.
- **Dette** : variation annuelle = déficit − dette × (g + 2 %) ; déficit = 4,5 + (u − 7,5) + budget − (dette − 110) / 20 (consolidation spontanée : −1 point de déficit par 20 points de dette au-dessus de 110 %). Bornée à 0–300 %.
- **Tension** : tend vers 20 + (u − 7,5) × 4 − (Social − 50) × 0,4 + budget.

### Opinion (par électorat)

```
33 + affinité(orientation, électorat)
 − (u − 7,5) × k_u      k_u : pop. 5, moy. 4, ais. 3
 + (g − 1,1) × k_g      k_g : pop. 1, moy. 2, ais. 3
 + 0,4 × moyenne pondérée des (pilier − 50)
 − 0,2 × (tension − 20)
 + état de grâce
 − 0,12 × scandale
 − 0,05 × (défiance − 30) si défiance > 30
 + budget (voir plus bas)
 − 2,5 si PM Technocrate
 − usure : 2nd mandat, de −2 à −8 points (linéaire sur le mandat)
```

| Affinité | pop. | moy. | ais. |
|---|---|---|---|
| gauche | +3 | 0 | −4 |
| centre | −1,5 | +1,5 | +0,5 |
| droite | −4 | 0 | +4 |

Poids des piliers (/16) : populaires 6/3/1/2/4, moyennes 3/4/2/3/4, aisées 1/7/3/2/3 (Soc/Éco/Tech/Écol/Séc). V2 : 6e pilier International, poids faible hors somme (populaires 1, moyennes 1, aisées 2). `OPN_BASE` = 336 (330 en V1) : recalé après l'ajout du pilier international pour garder la réélection d'un joueur actif à ≈ 71 / 55 / 25 % (facile / normal / difficile).

- **Inertie** : `opinion += (cible − opinion) / tpy` + aléa ±0,8 point.
- **État de grâce** : +12 points au début du 1er mandat, +4 au début du 2nd, −8 par an.
- **Popularité** = (4 × pop. + 4 × moy. + 2 × ais.) / 10.

### Jauges (au trimestre)

- **Scandale** : −1 par trimestre. Affaire spontanée : probabilité (4 + scandale / 6) %, +6 à +16.
- **Défiance** : tend de 2 points par trimestre vers 10 + scandale × 3/4. Sources : loi rejetée +5, budget rejeté +3, 49.3 +15 (loi) ou +8 (budget), après des législatives : 15.
- **Usure télévisuelle** : −1 par trimestre.

### Parlement

- **Score de vote** d'un groupe = relation + affinité + 2 × max(0, affinité − 30), avec affinité = 50 − |orientation loi − orientation groupe| / 2. Hors parti du joueur : − (défiance − 20) / 4 si défiance > 20, − scandale / 10.
- **Seuil** : 75 ; +6 pour les groupes hors parti du joueur en **majorité relative**.
  Une loi très marquée dans le sens d'un groupe (écart ≤ 20) passe même avec une relation basse (≥ 15 à affinité 40).
- **Adoption** si ≥ 289 voix. **Après adoption** : relation += affinité / 10. **Après rejet** : soutiens −2, défiance +5, opinion −1, CP perdu.
- **Dérive par trimestre** : relation → base + (popularité − 45) / 2,5 (+10 avec le Rassembleur), de 1 point.

### Capital politique (par trimestre)

1 + 1 si popularité ≥ 50 % + 1 si les groupes à relation ≥ 50 totalisent ≥ 289 sièges. Plafond 15.

| Action | Coût |
|---|---|
| Loi | 3 à 6 CP (−1 avec le Réformateur) |
| 49.3 sur une loi | coût de la loi + 4 CP ; une fois par année du mandat ; défiance +15, opposition −3, opinion −1 |
| 49.3 sur le budget | 4 CP, illimité ; défiance +8, opposition −1 |
| Intervention télévisée | 2 CP, une par tour ; échec (15 + 20 × usure) %, plafonné à 90 % ; succès +3 points d'opinion, flop −2 ; usure +1 par intervention |
| Défense : discours | 2 CP : scandale −10, opinion +1 |
| Défense : sacrifier un ministre | 0 CP : scandale −15, défiance −5, opinion −1, relation du parti −5 |
| Défense : négocier | 3 CP : relation +15 avec un groupe choisi |

## Budget annuel (`budget.c`)

Présenté au 4e trimestre (`sim_budget_due`), voté par l'Assemblée (`sim_budget_vote`) ou passé en 49.3 (`sim_budget_force`). À la fin de l'année, sans budget adopté : **reconduction = équilibre**. Le budget adopté s'applique toute l'année suivante.

| | Rigueur | Équilibre | Relance |
|---|---|---|---|
| Orientation propre (vote : + orientation du joueur / 2) | +40 | 0 | −40 |
| Déficit (points de PIB par an) | −1,5 | 0 | +1,5 |
| Tendance de croissance | −0,30 | 0 | +0,30 |
| Tension (cible) | +6 | 0 | −5 |
| Opinion cible pop./moy./ais. (points) | −2 / −0,5 / +1,5 | 0 | +2 / +1 / −1,5 |
| Piliers par trimestre Soc/Éco/Tech/Écol/Séc (points) | −0,5 / +0,2 / 0 / −0,1 / 0 | 0 | +0,5 / −0,1 / +0,2 / +0,2 / +0,1 |

## Institutions (`institutions.c`)

- **Bloc présidentiel** = parti du joueur + groupes voisins. **Majorité absolue** si bloc ≥ 289 sièges, sinon **majorité relative** (pas de cohabitation en V1).
- **Législatives** : score = (3 × popularité + opinion de l'électorat-cœur) / 4 (gauche → populaires, centre → moyennes, droite → aisées). Facteur f (pour mille) = 1000 + (score − 42 %) × k, k = 2,5 (parti), 1 (voisins), −1,5 (opposition), aléa ±6 %, borné à 0,3–2. Poids = sièges Normal × f² (scrutin majoritaire), répartition au plus fort reste. Ensuite : relations à mi-chemin de leur base, défiance 15. Organisées par `sim_dissolve` (au plus une fois par an, pas l'année qui suit des législatives) et automatiquement au début du 2nd mandat.
- **Censure** (au trimestre) : motion si défiance ≥ 60 et opposition (groupes hors parti à relation < 50) ≥ 289 sièges. Votent la motion les groupes hors parti dont la relation < 40 + (défiance − 60) / 3 + scandale / 10. Adoptée (≥ 289) : nouveau PM nommé d'office (suivant dans la liste, modifiable par `sim_change_pm`), opinion −3, défiance 30. Rejetée : défiance −10.
- **Destitution** : procédure engagée au trimestre si scandale ≥ 70 et défiance ≥ 60 (sauf au dernier tour du mandat, et pendant 4 trimestres après une procédure rejetée). Un trimestre de défense, puis vote : un groupe hors parti vote pour si 100 − relation + (scandale − 70) + (défiance − 60) / 2 − (popularité − 40 %) ≥ 70. Destitué si ≥ 385 voix (2/3) : `STATUS_DESTITUE`, fin de partie sans score. Sinon : scandale −20.

## Lois (`laws.c`)

Valeurs en unités lisibles (DETTE : 1 = 0,5 point). Délai en trimestres.

| Id | CP | Délai | Orient. | Immédiat | Différé |
|---|---|---|---|---|---|
| LOI_SMIC | 4 | 2 | −60 | POP_POPULAIRES +4, POP_AISEES −3 | SOCIAL +8, ECONOMIE −3, CHOMAGE +2, TENSION −6 |
| LOI_IS | 4 | 3 | +50 | POP_AISEES +4, POP_POPULAIRES −3 | ECONOMIE +8, CROISSANCE +3, CHOMAGE −2, DETTE +2, SOCIAL −3, TENSION +3 |
| LOI_RENOV | 5 | 3 | −30 | POP_MOYENNES +2 | ECOLOGIE +10, CHOMAGE −2, CROISSANCE +2, DETTE +2, SOCIAL +2 |
| LOI_POLICE | 3 | 2 | +40 | POP_MOYENNES +2, POP_AISEES +1, POP_POPULAIRES −1 | SECURITE +10, DETTE +1, SOCIAL −1 |
| LOI_CIR | 4 | 4 | +20 | POP_AISEES +1 | TECHNOLOGIE +12, ECONOMIE +4, CROISSANCE +2, DETTE +1 |
| LOI_RETRAITES | 6 | 4 | +40 | POP_POPULAIRES −4, POP_MOYENNES −2, TENSION +8 | DETTE −6, ECONOMIE +5, CROISSANCE +1, SOCIAL −5 |
| LOI_ISF | 4 | 2 | −60 | POP_POPULAIRES +3, POP_AISEES −5 | DETTE −3, SOCIAL +3, ECONOMIE −3, CROISSANCE −1 |
| LOI_HOPITAL | 5 | 3 | −30 | POP_POPULAIRES +2, POP_MOYENNES +2 | SOCIAL +8, SECURITE +2, TENSION −4, DETTE +3 |
| LOI_ECOLE | 4 | 4 | −20 | POP_MOYENNES +2 | TECHNOLOGIE +6, SOCIAL +4, ECONOMIE +2, DETTE +3 |
| LOI_ENERGIE | 6 | 4 | +20 | POP_MOYENNES +1, POP_AISEES +1 | ECOLOGIE +6, ECONOMIE +4, TECHNOLOGIE +4, CROISSANCE +1, DETTE +4 |
| LOI_TRANSPORTS | 5 | 3 | −40 | POP_POPULAIRES +3, POP_MOYENNES +1 | ECOLOGIE +7, SOCIAL +3, TENSION −3, DETTE +3 |
| LOI_IMMIGRATION | 4 | 2 | +70 | POP_MOYENNES +2, POP_AISEES +1, POP_POPULAIRES −1 | SECURITE +6, SOCIAL −4, ECONOMIE −2, TENSION +4 |
| LOI_CHOMAGE | 5 | 3 | +50 | POP_POPULAIRES −3, POP_AISEES +2, TENSION +5 | CHOMAGE −3, ECONOMIE +3, DETTE −3, SOCIAL −5 |
| LOI_NUMERIQUE | 3 | 2 | −20 | POP_POPULAIRES +2, POP_AISEES −1 | DETTE −3, SOCIAL +2, TECHNOLOGIE −2 |
| LOI_LOGEMENT | 5 | 3 | −30 | POP_POPULAIRES +3 | SOCIAL +6, CHOMAGE −1, CROISSANCE +1, ECOLOGIE −1, DETTE +3 |

Une loi adoptée ne peut plus être proposée pendant le mandat (le catalogue redevient disponible au 2nd mandat). Une loi rejetée peut l'être de nouveau.

## Présidentielle (`sim_election`)

- **Popularité de campagne** = popularité avec, par électorat, le bonus du thème : (2 + crédibilité) × poids / 4 points. Poids pop./moy./ais. : emploi 4/3/1, sécurité 2/3/4, pouvoir d'achat 4/4/0, écologie 1/4/3. Crédibilité (bornée à −2..+3 points) : emploi = baisse du chômage × 2 ; sécurité, écologie = progression du pilier / 3 ; pouvoir d'achat = progression du pilier Social / 3, −1 en rigueur.
- **Bilan** = (chômage de départ − final) / 8 + progression moyenne des piliers / 3, borné à ±15 points.
- **1er tour, joueur** = 6 % + popularité de campagne / 2 + bilan + aléa ±2,5, borné à 4–60 %.
- **1er tour, adversaires** : poids GR 110, ÉCO 90, CENTRE 150, DROITE 160, **NAT 150** (230 en V0), × (distance + 40) / 120 si distance < 80, + aléa ±20.
- **2nd tour, joueur** = 50 % + (popularité de campagne − 51) × 0,8 + bilan / 2 − (score de l'adversaire au 1er tour − 20) / 2 + front républicain (+2 contre NAT ou GR) + aléa ±8, borné à 15–85 %.

## Scores

- **Score de mandat** (`sim_mandate_score`) : popularité × 10 + piliers × 5 par point + 50 par point de chômage en moins + 5 par point de dette évité + 500 si réélu. Minimum 0.
- **2nd mandat** : `sim_start_second_mandate` mémorise le score du 1er mandat (avec +500) et remet les valeurs de référence à zéro. `sim_score_total` = score du 1er mandat + score du 2nd mandat (sans bonus de réélection).

## V2 (tâches 2.04, 2.05, 2.06, 2.07, 2.15)

Champs ajoutés à `Country` : `events_done_hi`, `senate[5]`, `cohab`, `cohab_group`, `pm_prev`, `referendum_done`, `reshuffle_year`, `grace_pm`, `gov_laws`, `cohabitations` (+ 22 octets). Aucun changement de comportement hors de ces mécanismes : les statistiques V1 (20 tours) sont inchangées.

### Capacité d'événements : 64

`Country.events_done` (ids 0-31) et `events_done_hi` (ids 32-63) ; accès par `events_done_get/set`. `EVENT_CAPACITY` = 64 (`sim/events.h`) ; une assertion de compilation vérifie `EVT_COUNT <= 64`. **`MAX_EVENTS` dans `tools/content/gen_content.py` (ligne 24, valeur 32) doit passer à 64** (non modifié : hors périmètre).

### Sénat (`senate.c`)

- 348 sièges, groupes G1-G5. Départ = moyenne de la couleur de l'Assemblée de départ (donc selon difficulté et orientation) et d'une composition de fond plus centriste et droitière (22/58/92/118/58). Exemple Normal : 34/71/94/95/54 (bloc présidentiel : 199 à 260 sièges selon l'orientation ; Facile gauche 210 contre Difficile gauche 171).
- **Renouvellement par moitié** au début des années 3, 6 et 9 de la partie (3 ans). La moitié basse de chaque groupe reste en place ; l'autre moitié est redistribuée au plus fort reste avec le poids sièges × f², f = 1000 + (score − 42 %) × k / 10, k = 8 (parti), 3 (voisins), −5 (opposition), aléa ±2,5 %, borné à 0,5-1,6. Inertie forte (≈ un tiers de la sensibilité des législatives). L'aléa vient d'un générateur dérivé : le flux de `c->rng` n'est pas consommé.
- **Destitution** : vote aux deux tiers de l'Assemblée (385) **ET** du Sénat (232). Un sénateur d'un groupe hors parti vote pour si 100 − relation_s + (scandale − 70) + (défiance − 60) / 2 − (popularité − 40 %) ≥ 70, avec relation_s = (relation + relation de base) / 2 (jugement sur la durée).
- Aperçus : `sim_senate_bloc_seats`, `sim_senate_has_majority`, `sim_senate_support_seats`, `sim_impeach_senate_votes`, `sim_impeach_would_pass`, `sim_senate_renewal_due/in`.

### Cohabitation (`cohab.c`)

- **Début** : `sim_legislatives` (dissolution ou début du 2nd mandat) ne donne pas la victoire au bloc présidentiel (`won` = 0 : pas de majorité et moins de sièges qu'avant). Le PM devient `PM_OPPOSITION` (4, hors `PM_COUNT`) ; le camp (`sim_cohab_camp`) est celui du groupe d'opposition le plus nombreux hors du bloc. Le PM du joueur est mémorisé (`pm_prev`) et rétabli à la fin.
- **Fin** : à la première législative où `won` = 1 (majorité, ou sièges gagnés). La dissolution reste possible (au plus une par an).
- **Lois** : le joueur ne propose que les lois de souveraineté : (champ `sovereign` de `LawDef` : 15 lois, dont POLICE, IMMIGRATION, ENERGIE, CIR et 11 lois V2) ; les autres donnent `LAW_ERR_COHAB`. Le 49.3 suit la même règle.
- **Gouvernement** : chaque trimestre, 50 % de chances qu'il fasse adopter, sans CP, la loi non souveraine non adoptée la plus proche de l'orientation de son groupe (aperçu : `sim_cohab_next_law`) ; effets différés complets, effets d'opinion immédiats divisés par 2. Compte rendu : `TurnReport.gov_law`.
- **Budget** : `sim_budget_due` = 0 ; en fin d'année le PM impose relance (camp gauche), équilibre (centre) ou rigueur (droite) : `TurnReport.budget_gov`.
- **Popularité** : cible d'opinion sous 42 % : écart réduit d'un tiers (exposition aux 2/3) ; au-dessus, inchangée.
- Autres : pas de motion de censure (`sim_censure_threat` = 0), pas de remaniement ni de `sim_change_pm`. Le référendum reste possible sur toute loi.
- Fréquence (joueur automatique « dissol » : dissout dès qu'il est sous 42 % de popularité) : 0 à 4 cohabitations pour 100 parties ; très rare pour un joueur compétent (il gagne ses législatives).

### Référendum (`actions.c`)

6 CP, au plus un par mandat (remis à zéro au 2nd mandat), sur une loi non adoptée (file non pleine). « Oui » par électorat (‰) = 470 + (affinité − 30) × 5 + effet immédiat de la loi pour l'électorat × 6 + (popularité − 45 %) × 0,6 + aléa (±4 points, commun), avec affinité = 50 − |orientation loi − orientation électorat| / 2, électorats à −40 / 0 / +40. Total pondéré 40/40/20 ; **adoptée si ≥ 50 %**. Succès : loi adoptée sans vote (relations inchangées), opinion +4, défiance −3. Échec : opinion −4, défiance +10. Aperçu sans aléa : `sim_referendum_preview` (environ 58 % de succès sur des lois tirées au hasard).

### Remaniement (`actions.c`)

3 CP, au plus un par an, hors cohabitation : change de PM (profil différent, effet de `sim_change_pm`, dont le +5 du Rassembleur), relations +2, opinion +1 point, état de grâce `grace_pm` = +3 points d'opinion cible, −0,6 point par trimestre (5 trimestres). Les ministres un par un restent hors périmètre.

### Mode 60 tours (2.15)

`turns_per_year = 12` : tous les flux (économie, opinion, dette, tension) passent par `sim_step` (reste reporté) ; bruits ramenés à ≈ 1/√3 ; lois : délai × 3 ; CP, jauges, Parlement, institutions, érosion de l'état de grâce du remaniement au trimestre (`sim_quarter_tick`, tous les 3 tours) ; budget présenté pendant les 3 derniers mois de l'année ; événements : 33 % par tour (≈ 1 tour sur 3). La calibration n'a demandé aucun changement de constante : l'écart avec le mode 20 tours est resté ≤ 5 points (voir les tableaux).

## Pilier international et régions (V2, 2.08 et 2.01)

**Pilier international** : `PIL_INTERNATIONAL` (index 5, 0-1000, départ ≈ 500), clé d'effet `EFF_INTERNATIONAL` (« INTERNATIONAL », 1 unité = 1 point de pilier). `PIL_COUNT` = 5 reste le nombre de piliers « V1 » affichés par les écrans ; `PIL_TOTAL` = 6 dimensionne `pillars` / `pillars_start` et les boucles de la simulation (opinion, score, progression, budget). Il entre dans le score et la progression moyenne comme les autres piliers. Lois : aide au développement +8, alliance +6, LPM +3, service national +2, taxe carbone +2, exportations d'armes −4, immigration −2. Événements : sommet européen, crise diplomatique, sommet climat, Nobel, espionnage, classement, événement sportif, agences de notation (ajouts de « INTERNATIONAL ±n » seulement).

**Régions** (`regions.c`) : 13 régions, `REG_*` dans le même ordre que `REGION_*` de `src/ui/ui_map.h` (vérifié par `tests/test_regions.c`).
`sim_region_popularity(c, r)` = Σ poids(r, électorat) × opinion(électorat) / 100 + `c->region_gap[r]` (dixièmes de %, `s8`, borné ±10 points). L'écart cible vaut Σ (sensibilité − 3) × (pilier − 500) / 50 − (sensibilité chômage − 3) × (chômage − 7,5 %) / 50 + effets des lois adoptées (table `region_laws`, ex. agriculture productive +1,5 point en Bretagne, énergie +1,5 en Centre-Val de Loire, transports +1,5 en Île-de-France). `sim_region_update` (après l'opinion, à chaque tour) comble 1/6 de l'écart par trimestre. Les élections ignorent les régions. Profils (poids Pop/Moy/Aisées ; sensibilités Soc/Éco/Tech/Écol/Séc/Intl ; chômage) : voir `region_profiles`.

## Partie automatique (tests, 400 graines × 20 tours, événements, 3 PM à parts égales)

Joueur **actif** : budget selon l'orientation (gauche relance, centre équilibre, droite rigueur si la dette dérive), 49.3 si le vote échoue sur une des 4 lois préférées (défiance < 35, CP suffisant), télévision avec modération (usure nulle, CP ≥ 4, popularité < 38 % ou fin de mandat), dissolution en majorité relative si popularité ≥ 47 %, se défend contre la destitution, choisit le meilleur thème ; événements au hasard. **Passif** : ne fait rien (budget reconduit), événements au hasard. **Scand.** : actif qui choisit le plus de scandale et ne se défend pas. **1erch** : passif qui prend toujours le 1er choix des événements.

| Diff. | Orient. | Joueur | Pop. % | Chôm. | Dette | Lois | Réélu % | Destit. % | Score 1 | Cumul |
|---|---|---|---|---|---|---|---|---|---|---|
| facile | gauche | actif | 48,5 | 6,04 | 114,2 | 11,2 | 73 | 0 | 1309 | 2532 |
| facile | centre | actif | 46,0 | 6,13 | 108,0 | 11,0 | 72 | 0 | 1266 | 2510 |
| facile | droite | actif | 43,6 | 6,03 | 105,2 | 10,7 | 66 | 0 | 1157 | 2351 |
| normal | gauche | actif | 45,1 | 6,59 | 123,4 | 10,5 | 54 | 0 | 1160 | 2439 |
| normal | centre | actif | 43,6 | 6,53 | 118,0 | 10,1 | 61 | 0 | 1168 | 2442 |
| normal | droite | actif | 40,9 | 6,45 | 114,7 | 10,2 | 51 | 0 | 1043 | 2313 |
| difficile | gauche | actif | 39,7 | 7,64 | 134,3 | 9,7 | 26 | 0 | 948 | 2342 |
| difficile | centre | actif | 37,4 | 7,52 | 128,6 | 9,9 | 29 | 0 | 946 | 2296 |
| difficile | droite | actif | 35,3 | 7,50 | 125,7 | 9,6 | 21 | 0 | 833 | 2227 |
| normal | toutes | passif | 33,6–34,7 | 7,15 | 119,6 | 0 | 0 | 0 | ≈ 400 | — |
| normal | toutes | scand. | 38,5–43,8 | 6,5 | 118–128 | 10,7 | 40–52 | 0,5–1 | ≈ 1050 | ≈ 2210 |
| normal | toutes | 1erch | 35,7–36,9 | 7,30 | 123,5 | 0 | 0 | 0 | ≈ 415 | — |

Réélection moyenne du joueur actif : Facile 70 %, Normal 55 %, Difficile 26 %.

### Joueur automatique V2 (tests/test_sim_v1.c) et mode 60 tours

Le joueur « actif » utilise désormais le référendum (loi préférée refusée par l'Assemblée, « oui » attendu ≥ 50 %) et le remaniement (PM Technocrate remplacé par le Rassembleur si popularité < 40 % et CP abondants) : ils se déclenchent rarement (≈ 300 référendums et 420 remaniements pour 3 600 parties ; les chiffres de l'actif sont identiques à ceux de la V1). Variantes (Normal) : « refer » dépense 6 CP en référendum dès le tiers du mandat (réélection −2 à −5 points par rapport à l'actif : pas de stratégie dominante) ; « dissol » dissout dès 42 % de popularité.

| Diff. | Orient. | Joueur | Pop. % | Chôm. | Dette | Lois | Réélu % | Destit. % | Score 1 | Cumul |
|---|---|---|---|---|---|---|---|---|---|---|
| normal | gauche | refer | 44,8 | 6,76 | 123,9 | 10,3 | 51 | 0 | 1126 | 2389 |
| normal | centre | refer | 43,5 | 6,62 | 117,9 | 10,1 | 59 | 0 | 1151 | 2415 |
| normal | droite | refer | 40,6 | 6,61 | 115,2 | 10,1 | 46 | 0 | 1002 | 2290 |
| normal | gauche | dissol | 44,9 | 6,60 | 123,6 | 10,6 | 50 | 0 | 1137 | 2420 |
| normal | centre | dissol | 44,1 | 6,46 | 118,2 | 9,9 | 60 | 0 | 1153 | 2431 |
| normal | droite | dissol | 40,9 | 6,45 | 114,7 | 10,2 | 51 | 0 | 1047 | 2319 |

Mode 60 tours comparé au mode 20 tours (joueur actif, 400 graines) :

| Diff. | Orient. | Pop. 20 | Pop. 60 | Chôm. 20 | Chôm. 60 | Dette 20 | Dette 60 | Réélu 20 | Réélu 60 | Destit. 20 / 60 |
|---|---|---|---|---|---|---|---|---|---|---|
| facile | gauche | 48,5 | 48,7 | 6,04 | 5,94 | 114,2 | 113,7 | 73 | 76 | 0 / 0 |
| facile | centre | 46,0 | 46,5 | 6,13 | 6,07 | 108,0 | 107,6 | 72 | 77 | 0 / 0 |
| facile | droite | 43,6 | 44,1 | 6,03 | 6,02 | 105,2 | 105,3 | 66 | 63 | 0 / 0 |
| normal | gauche | 45,1 | 45,0 | 6,59 | 6,57 | 123,4 | 123,4 | 54 | 52 | 0 / 0 |
| normal | centre | 43,6 | 43,6 | 6,53 | 6,51 | 118,0 | 117,6 | 61 | 58 | 0 / 0 |
| normal | droite | 40,9 | 41,2 | 6,45 | 6,47 | 114,7 | 114,8 | 51 | 52 | 0 / 0 |
| difficile | gauche | 39,7 | 39,6 | 7,64 | 7,64 | 134,3 | 134,9 | 26 | 26 | 0 / 0 |
| difficile | centre | 37,4 | 37,4 | 7,52 | 7,54 | 128,6 | 128,7 | 29 | 27 | 0 / 0 |
| difficile | droite | 35,3 | 35,4 | 7,50 | 7,56 | 125,7 | 126,2 | 21 | 22 | 0 / 0 |

Le bruit d'échantillonnage de la réélection (400 graines) est d'environ ±2,5 points par ligne : sur d'autres graines, l'écart reste dans ±6, sans biais net (≈ −2 points en Difficile). Pour que le joueur automatique se comporte de la même façon dans les deux modes, la fin de mandat (télévision des 3 derniers trimestres) est exprimée en trimestres.
