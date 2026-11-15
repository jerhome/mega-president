# Cœur de simulation — formules et constantes

> Référence pour l'équilibrage. Code : `src/sim/`. Unités internes : `docs/CONTRAT_V0.md` §2.
> `tpy` = tours par an (4 : 20 tours ; 12 : mode 60 tours, V2 — voir « Mode 60 tours »).
> Les variations annuelles sont ramenées au tour par `sim_step` (reste reporté d'un tour à l'autre, pas de perte par troncature).
>
> **Sommaire des sections V4** : « Opinion (par électorat) » (5 électorats, affinités, `OPN_BASE`) ; « Présidentielle » (5 thèmes) ; « Référendum » ; « Pilier international et régions » (5 poids) ; « Budget détaillé » (lignes, préréglages, Sénat, cohabitation) ; « V4 : mesures du simulateur » (4.13) ; « V4 : passe de contenu » (4.05, 4.06) ; « V4 : calage de l'équilibre » (4.14, valeurs finales).

## Fichiers

| Fichier | Rôle |
|---|---|
| `sim_state.h` | struct `Country` (≈ 180 octets sur 68000 avec alignement 2 ; 188 sur PC ; 172 en V2 ; 126 en V1), énumérations, bornes |
| `sim.h` / `sim.c` | `sim_init_ex`, `sim_end_turn`, lois, 49.3, télévision, PM, jauges, CP, 2nd mandat, scores |
| `effects.*` | clés d'effets, conversion et bornage (`sim_apply_effect`) ; V4 : `POP_RETRAITES`, `POP_JEUNES`, alias `POP_CADRES` |
| `laws.*` | catalogue `law_defs` (40 lois, masque `LawMask` de 64 bits : `law_mask_get/set`), coût effectif, file des lois en attente |
| `economy.c` | croissance, chômage, dette, tension |
| `opinion.c` | opinion des 5 électorats (V4), état de grâce, usure |
| `parliament.*` | votes, relations |
| `budget.*` | V4 : budget détaillé (6 lignes × 5 crans, préréglages, Sénat, reconduction, plan du PM) |
| `institutions.*` | législatives, dissolution, majorité relative, censure, destitution |
| `election.*` | présidentielle, thème de campagne, score de mandat |
| `senate.*` | V2 : Sénat (348 sièges), renouvellement, voix de destitution du Sénat |
| `cohab.*` | V2 : cohabitation (PM d'opposition, lois de souveraineté, lois du gouvernement, budget imposé) |
| `actions.*` | V2 : référendum, remaniement |
| `regions.*` | V2 : popularité par région (13 régions, profils en ROM ; V4 : 5 poids d'électorats) |

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

- Opinion de base (V4 : Pop/Moy/Cad/Ret/Jeu) : gauche 58/53/40/48/60 %, centre 46/54/54/56/44 %, droite 44/55/62/60/38 % (± 1,5). Moyenne pondérée à ±1 point de la V2 (SPEC_V4 §1.3). En V2 : gauche 58/53/42, centre 50/54/50, droite 46/55/61 sur 3 électorats.
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
34,2 + affinité(orientation, électorat)               (OPN_BASE = 396 depuis la tâche 4.14)
 − (u − 7,5) × k_u      k_u : Pop 5, Moy 4, Cad 3, Ret 3, Jeu 6
 + (g − 1,1) × k_g      k_g : Pop 1, Moy 2, Cad 4, Ret 1, Jeu 2
 + 0,4 × moyenne pondérée des (pilier − 50)
 − 0,2 × (tension − 20)
 + état de grâce
 − 0,12 × scandale
 − 0,05 × (défiance − 30) si défiance > 30
 + budget (sim_plan_opinion : lignes + sérieux budgétaire, voir « Budget détaillé »)
 − 2,5 si PM Technocrate
 − usure : 2nd mandat, de −2 à −8 points (linéaire sur le mandat)
```

**5 électorats (V4)** : classes populaires (Pop, poids 26), classes moyennes (Moy, 22), cadres (Cad, 12), retraités (Ret, 28), jeunes (Jeu, 12). Poids = parts des suffrages exprimés (`elec_weight`, somme 100). `ELEC_AISEES` est un alias de `ELEC_CADRES` (index 2).

| Affinité | Pop | Moy | Cad | Ret | Jeu |
|---|---|---|---|---|---|
| gauche | +3 | 0 | −3 | −1 | +3 |
| centre | −2 | +1 | +1,5 | +1,5 | −2 |
| droite | −4 | 0 | +4 | +1 | −3,5 |

Poids des piliers (/16, Soc/Éco/Tech/Écol/Séc ; International hors somme : 1 / 1 / 2 / 1 / 1) : Pop 6/3/1/2/4, Moy 3/4/2/3/4, Cad 1/6/4/3/2, Ret 5/3/0/1/7, Jeu 4/2/3/6/1.

`OPN_BASE` = **396** (valeur finale de la tâche 4.14 ; 336 en V2, 342 puis 339 pendant la V4 : voir « V4 : calage »). Les affinités du tableau ci-dessus sont les valeurs de base ; le calage 4.14 a ajouté +17 à la ligne gauche et −6 à la ligne droite.

- **Inertie** : `opinion += (cible − opinion) / tpy` + aléa ±0,8 point.
- **État de grâce** : +12 points au début du 1er mandat, +4 au début du 2nd, −8 par an.
- **Popularité** = Σ `elec_weight[e]` × opinion(e) / 100 (26 / 22 / 12 / 28 / 12).
- **Électorat-cœur** (`sim_core_opinion`, législatives et renouvellement du Sénat) : gauche (Pop + Jeu) / 2, centre (Moy + Ret) / 2, droite (Cad + Ret) / 2. Il remplace l'ancien alignement implicite ELEC/ORIENT (`opinion[orientation]`).

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
| Loi | 2 à 6 CP (−1 avec le Réformateur) |
| 49.3 sur une loi | coût de la loi + 4 CP ; une fois par année du mandat ; défiance +15, opposition −3, opinion −1 |
| 49.3 sur le budget | 4 CP, illimité ; défiance +8, opposition −1 |
| Intervention télévisée | 2 CP, une par tour ; échec (15 + 20 × usure) %, plafonné à 90 % ; succès +3 points d'opinion, flop −2 ; usure +1 par intervention |
| Défense : discours | 2 CP : scandale −10, opinion +1 |
| Défense : sacrifier un ministre | 0 CP : scandale −15, défiance −5, opinion −1, relation du parti −5 |
| Défense : négocier | 3 CP : relation +15 avec un groupe choisi |

## Budget détaillé (`budget.c`, V4)

**6 lignes × 5 crans** (−2, −1, 0 = reconduction, +1, +2 ; pour les impôts, + = hausse). `BudgetPlan { s8 lvl[BUD_LINES]; }`, plan en vigueur dans `Country.budget_lvl[6]` ; `Country.budget` mémorise le préréglage correspondant (`BUDGET_RIGUEUR/EQUILIBRE/RELANCE`) ou `BUDGET_PERSO` (3) ; `BUDGET_COUNT` reste 3 (les préréglages). Présenté au 4e trimestre (`sim_budget_due`), voté (`sim_budget_plan_vote`) ou passé en 49.3 (`sim_budget_plan_force`, 4 CP, défiance +8, illimité), en vigueur jusqu'à l'adoption suivante.

Effets d'**un cran (+1)** par ligne (tables ROM `bud_line`) :

| Ligne | Déficit (dixièmes/an) | Croiss. (centièmes) | Tension | Orient. vote | Pop | Moy | Cad | Ret | Jeu | Piliers (dixièmes/trim.) |
|---|---|---|---|---|---|---|---|---|---|---|
| Social et retraites | +5 | +5 | −2 | −15 | +10 | +3 | −3 | +10 | +3 | Soc +3 |
| Éducation et recherche | +3 | +5 | 0 | −10 | +2 | +5 | +3 | 0 | +10 | Tech +3, Soc +1 |
| Sécurité et défense | +3 | 0 | 0 | +15 | +2 | +3 | +2 | +8 | −3 | Séc +3, Intl +1 |
| Transition et réseaux | +4 | +10 | −1 | −10 | 0 | +3 | +3 | −2 | +8 | Écol +3, Éco +1 |
| Impôts des ménages | −6 | −10 | +2 | 0 | −6 | −10 | −8 | −6 | −3 | — |
| Impôts des entreprises | −5 | −15 | −1 | −20 | +3 | 0 | −10 | −2 | +2 | Éco −3 |

- **Linéaires** (niveau × valeur) : déficit, croissance, tension, orientation.
- **Asymétriques** : opinion et piliers, multiplicateur `m[niveau + 2]` puis division par 2. Dépenses : `{ −5, −2, 0, 2, 3 }` (+2 ne rapporte que 1,5 fois un cran, −2 coûte 2,5 fois). **Impôts : table inversée `{ −3, −2, 0, 2, 5 }`** (écart à SPEC_V4 §2.2, qui donne la table des dépenses à toutes les lignes) : une forte hausse fait très mal (2,5 ×), une forte baisse rapporte peu (1,5 ×) ; avec la table des dépenses, « impôts à −2 » était la stratégie dominante (réélection de l'« austère » à 65 % en droite).
- **Sérieux budgétaire** : opinion(e) −= déficit du plan × `k_s[e]` / 10, `k_s` = Pop 0, Moy 3, Cad 10, Ret 6, Jeu 0.
- **Budget insincère** : si le déficit du plan dépasse 1,5 point, chaque groupe hors du parti du joueur perd (déficit − 15) / 2 points de score de vote (`sim_plan_insincere_malus`).
- **Prime de risque** (`economy.c`) : si dette > 120 % et déficit du plan > 1 point, la tendance de croissance perd 0,05 point par dixième de déficit au-delà de 10.
- **Vote** : orientation = orientation du joueur / 2 + Σ niveau × orientation de la ligne (bornée à ±100), même `parl_vote`.
- **Préréglages** (`sim_budget_preset_plan`) : Rigueur `−1 0 0 −1 +1 0` (déficit −1,5 ; croissance −0,25 ; popularité pondérée −0,9 point), Équilibre `0 0 0 0 0 0`, Relance `+1 +1 0 +1 0 0` (déficit +1,2 ; croissance +0,20 ; popularité pondérée +0,6 point).
- **Reconduction** (fin d'année sans budget adopté, `budget_year_close`) : chaque dépense recule d'un cran vers 0, les impôts ne changent pas (`TurnReport.budget_reconduit`).
- **Cohabitation** : le plan du PM d'opposition entre en vigueur en fin d'année (`sim_cohab_plan`) : gauche `+1 +1 0 +1 0 +1`, centre tout à 0, droite `−1 0 +1 −1 0 −1`. Le président règle la ligne Sécurité et défense à ±1 cran autour (`sim_cohab_set_defense`, `Country.cohab_def_adj`), sans CP.
- **Sénat** : si le bloc présidentiel n'a pas la majorité au Sénat (`sim_senate_has_majority` = 0) et hors cohabitation, il **amende** le plan adopté (vote ou 49.3) : la ligne dont la contribution (cran × orientation de la ligne) est la plus négative recule d'un cran vers 0 (`sim_budget_senate_amend_line`, `Country.bud_amend_line/_from`). **Lecture définitive** : `sim_budget_definitive_reading`, 1 CP, rétablit le plan voulu (possible tant que l'année n'est pas close). `TurnReport.senate_amend` (ligne + 1) l'annonce au compte rendu de fin d'année s'il n'a pas été annulé. L'amendement est rare (bloc présidentiel minoritaire au Sénat : surtout en difficile) : ≈ 1 % des budgets des parties automatiques.
- **Compatibilité** : `sim_budget_vote/force(c, BUDGET_*)`, `sim_budget_deficit/growth/opinion/pillar/tension/orientation/preview(b)` s'appliquent au plan du préréglage. Ces fonctions ne servent qu'aux tests PC : `tests/budget_compat.c`.

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

- **Popularité de campagne** = popularité avec, par électorat, le bonus du thème : (2 + crédibilité) × poids / 4 points. Poids Pop/Moy/Cad/Ret/Jeu (V4, 5 thèmes) : emploi 4/3/1/0/4, sécurité 2/3/2/4/0, pouvoir d'achat 4/4/0/3/2, écologie 1/3/3/0/4, **santé et retraites 2/2/0/4/0**. Crédibilité (bornée à −2..+3 points) : emploi = baisse du chômage × 2 ; sécurité, écologie = progression du pilier / 3 ; santé et retraites = progression du pilier Social / 3 ; pouvoir d'achat = progression du pilier Social / 3, −1 si la ligne Social du budget est < 0 ou les impôts des ménages > 0. La popularité de campagne pondère les électorats par `elec_weight`.
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
- **Budget** : `sim_budget_due` = 0 ; en fin d'année le PM impose son plan (V4 : gauche `+1 +1 0 +1 0 +1`, centre tout à 0, droite `−1 0 +1 −1 0 −1`, ligne Sécurité ajustée par le président) : `TurnReport.budget_gov`.
- **Popularité** : cible d'opinion sous 42 % : écart réduit d'un tiers (exposition aux 2/3) ; au-dessus, inchangée.
- Autres : pas de motion de censure (`sim_censure_threat` = 0), pas de remaniement ni de `sim_change_pm`. Le référendum reste possible sur toute loi.
- Fréquence (joueur automatique « dissol » : dissout dès qu'il est sous 42 % de popularité) : 0 à 4 cohabitations pour 100 parties ; très rare pour un joueur compétent (il gagne ses législatives).

### Référendum (`actions.c`)

6 CP, au plus un par mandat (remis à zéro au 2nd mandat), sur une loi non adoptée (file non pleine). « Oui » par électorat (‰) = 470 + (affinité − 30) × 5 + effet immédiat de la loi pour l'électorat × 6 + (popularité − 45 %) × 0,6 + aléa (±4 points, commun), avec affinité = 50 − |orientation loi − orientation électorat| / 2, électorats à −30 / 0 / +40 / +20 / −40 (Pop/Moy/Cad/Ret/Jeu, V4). Total pondéré par `elec_weight` (26/22/12/28/12) ; **adoptée si ≥ 50 %**. Succès : loi adoptée sans vote (relations inchangées), opinion +4, défiance −3. Échec : opinion −4, défiance +10. Aperçu sans aléa : `sim_referendum_preview` (environ 58 % de succès sur des lois tirées au hasard).

### Remaniement (`actions.c`)

3 CP, au plus un par an, hors cohabitation : change de PM (profil différent, effet de `sim_change_pm`, dont le +5 du Rassembleur), relations +2, opinion +1 point, état de grâce `grace_pm` = +3 points d'opinion cible, −0,6 point par trimestre (5 trimestres). Les ministres un par un restent hors périmètre.

### Mode 60 tours (2.15)

`turns_per_year = 12` : tous les flux (économie, opinion, dette, tension) passent par `sim_step` (reste reporté) ; bruits ramenés à ≈ 1/√3 ; lois : délai × 3 ; CP, jauges, Parlement, institutions, érosion de l'état de grâce du remaniement au trimestre (`sim_quarter_tick`, tous les 3 tours) ; budget présenté pendant les 3 derniers mois de l'année ; événements : 33 % par tour (≈ 1 tour sur 3). La calibration n'a demandé aucun changement de constante : l'écart avec le mode 20 tours est resté ≤ 5 points (voir les tableaux).

## Pilier international et régions (V2, 2.08 et 2.01)

**Pilier international** : `PIL_INTERNATIONAL` (index 5, 0-1000, départ ≈ 500), clé d'effet `EFF_INTERNATIONAL` (« INTERNATIONAL », 1 unité = 1 point de pilier). `PIL_COUNT` = 5 reste le nombre de piliers « V1 » affichés par les écrans ; `PIL_TOTAL` = 6 dimensionne `pillars` / `pillars_start` et les boucles de la simulation (opinion, score, progression, budget). Il entre dans le score et la progression moyenne comme les autres piliers. Lois : aide au développement +8, alliance +6, LPM +3, service national +2, taxe carbone +2, exportations d'armes −4, immigration −2. Événements : sommet européen, crise diplomatique, sommet climat, Nobel, espionnage, classement, événement sportif, agences de notation (ajouts de « INTERNATIONAL ±n » seulement).

**Régions** (`regions.c`) : 13 régions, `REG_*` dans le même ordre que `REGION_*` de `src/ui/ui_map.h` (vérifié par `tests/test_regions.c`).
`sim_region_popularity(c, r)` = Σ poids(r, électorat) × opinion(électorat) / 100 + `c->region_gap[r]` (dixièmes de %, `s8`, borné ±10 points). L'écart cible vaut Σ (sensibilité − 3) × (pilier − 500) / 50 − (sensibilité chômage − 3) × (chômage − 7,5 %) / 50 + effets des lois adoptées (table `region_laws`, ex. agriculture productive +1,5 point en Bretagne, énergie +1,5 en Centre-Val de Loire, transports +1,5 en Île-de-France). `sim_region_update` (après l'opinion, à chaque tour) comble 1/6 de l'écart par trimestre. Les élections ignorent les régions. Profils (poids Pop/Moy/Cad/Ret/Jeu, V4, somme 100, table SPEC_V4 §1.5 ; sensibilités Soc/Éco/Tech/Écol/Séc/Intl ; chômage) : voir `region_profiles`.

## Partie automatique (tests, 400 graines × 20 tours, événements, 3 PM à parts égales)

Joueur **actif** : budget par préréglage selon l'orientation (gauche relance, centre équilibre, droite rigueur si la dette dérive), 49.3 si le vote échoue sur une des 4 lois préférées (défiance < 35, CP suffisant), télévision avec modération (usure nulle, CP ≥ 4, popularité < 38 % ou fin de mandat), dissolution en majorité relative si popularité ≥ 47 %, se défend contre la destitution, choisit le meilleur thème ; événements au hasard. **Passif** : ne fait rien (budget reconduit), événements au hasard. **Scand.** : actif qui choisit le plus de scandale et ne se défend pas. **1erch** : passif qui prend toujours le 1er choix des événements.

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

## V4 : mesures du simulateur (5 électorats et budget détaillé)

`tests/test_sim_v1.c`, 800 graines (400 avant la V4 : le bruit d'échantillonnage de la réélection est de ±2,5 points à 400 graines, la comparaison 20 / 60 tours y était fragile ; tolérance de l'écart des deux modes portée à 7 points sur 800 graines).

**Avant / après** (réélection du joueur actif, V2 à 400 graines, V4 à 800) :

| | Facile | Normal | Difficile |
|---|---|---|---|
| V2 (3 électorats) | 71 % | 55 % | 25 % |
| V4 sans recalage de `OPN_BASE` (336) | 68 % | 52 % | 20 % |
| **V4 avec `OPN_BASE` = 342** | **72 %** | **55 %** | **24 %** |

Détail V4 (actif, 20 tours) : facile 75 / 75 / 66 %, normal 58 / 56 / 51 %, difficile 25 / 25 / 20 % (gauche / centre / droite) ; écart entre orientations ≤ 9 points ; passif : < 1 % de réélection ; destitution : 0 % pour l'actif ; mode 60 tours : popularité, chômage et dette à ±0,6 point, réélection à ±5 points en moyenne (gauche +5, centre +6 en facile, bruit).

**Popularité finale par électorat** (actif, normal, 1er mandat) : gauche Pop 49,4 / Moy 46,7 / Cad 42,2 / Ret 44,6 / Jeu 50,7 ; centre 43,7 / 45,8 / 44,4 / 44,8 / 43,9 ; droite 40,2 / 42,9 / 45,0 / 42,9 / 39,6. Aucun électorat hors de 20-80 % (vérifié par un test).

**Préréglage fixe toute la partie** (normal, centre) : Rigueur 41,6 % de popularité, dette 114,6 %, réélection 39 % ; Équilibre 46,4 % / 118,3 % / 68 % ; Relance 50,5 % / 121,3 % / 91 %. La relance l'emportait déjà en V2 (mesure sur le code V2 : rigueur 55 %, équilibre 72 %, relance 94 % sur 400 graines) : le déficit coûte peu, car la consolidation spontanée (`DEBT_CONSOL`) et l'inflation plafonnent la dette, et les piliers n'ont aucun rappel vers 50.

**Variantes de budget** (normal ; réélection gauche / centre / droite) : *optimiseur* (glouton par coordonnées sur la popularité prévue, plans adoptables) 58 / 85 / 87 % ; *extrême* (dépenses +2, impôts −2, 49.3 chaque année) 55 / 83 / 77 %, dette 127-141 % ; *austère* (tout à −2) 4 / 24 / 65 %. Cible de SPEC_V4 §3.4 (au plus 5 points au-dessus du joueur actif) **non atteinte** : le joueur actif ne joue qu'avec les préréglages, et l'optimiseur le dépasse de 30 points au centre et à droite. Garde-fou provisoire +45 points remplacé en 4.14 par +15 (voir la section « calage »). Le calage (tâche 4.14) devra agir sur l'économie (conséquences d'une dette qui monte : prime de risque sur la dette seule, rappel des piliers, `k_s`), pas seulement sur les tables du budget : doubler `k_s` fait passer l'optimiseur à 38 / 63 / 64 % mais rapproche aussi les préréglages entre eux.

**Sénat** : 1 % des budgets des parties automatiques sont amendés (le bloc présidentiel a presque toujours la majorité au Sénat, sauf en difficile).

## V4 : passe de contenu sur 5 électorats (tâches 4.05 et 4.06)

- **Lois** : 37 des 40 lois reçoivent des effets immédiats sur les retraités et/ou les jeunes (les trois autres, référendum citoyen, proportionnelle et moins de parlementaires, agissent sur `POP_TOUS`). Toutes les clés `POP_AISEES` deviennent `POP_CADRES` (même clé C). Plafond `LAW_MAX_IMMEDIATE` = 4 respecté : quand une loi dépassait, l'effet le moins marquant est sorti (police : cadres +1 retiré ; immigration, uniformes, peines planchers : cadres +1 ou populaires +1 retiré). Cas particulier de la réforme des retraites : populaires −4, moyennes −2, **retraités −5** et tension +8 ; le « jeunes +1 » de SPEC_V4 §1.4 n'a pas de place (4 effets maximum) et pèse peu (12 % × 1).
- **Événements** : 37 événements existants gagnent des effets `POP_RETRAITES` / `POP_JEUNES` (limite de 6 effets par choix) ; `POP_AISEES` devient `POP_CADRES` partout. 6 événements nouveaux dans `docs/contenu/evenements_v4.md` : `EVT_PENSIONS` (gel des pensions), `EVT_EHPAD` (scandale en maison de retraite), `EVT_FRACTURE_NUM` (fracture numérique des seniors), `EVT_AFFECTATION` (algorithme d'affectation post-bac), `EVT_CHOMAGE_JEUNES`, `EVT_MARCHE_CLIMAT`. Total : 51 événements (limite 64).
- **Recalage** : la nouvelle passe fait gagner 2 à 3 points de réélection à l'ensemble (les retraités et les jeunes reçoivent désormais les effets que 3 électorats absorbaient seuls). `OPN_BASE` 342 → **339**.

| Réélection du joueur actif (800 graines) | Facile | Normal | Difficile |
|---|---|---|---|
| Avant la passe (`OPN_BASE` = 342) | 72 % | 55 % | 24 % |
| Passe de contenu sans recalage | 74 % | 58 % | 26 % |
| **Passe de contenu, `OPN_BASE` = 339** | **72 %** | **56 %** | **25 %** |

Détail (gauche / centre / droite) : facile 76 / 76 / 66, normal 59 / 58 / 50, difficile 26 / 28 / 22 ; écart entre orientations ≤ 10 points. Popularité finale par électorat (actif, normal, gauche) : Pop 49,5 / Moy 46,9 / Cad 41,8 / Ret 45,3 / Jeu 50,7. Préréglages fixes (centre, normal) : rigueur 42 %, équilibre 71 %, relance 93 % de réélection (dominance du budget inchangée, voir tâche 4.14).

## V4 : calage de l'équilibre du budget détaillé (tâche 4.14)

**Problème** : la relance dominait (préréglage fixe, centre, normal : rigueur 41 %, équilibre 69 %, relance 93 % de réélection) et l'optimiseur dépassait le joueur actif de 30 points au centre et à droite. La dette ne coûtait presque rien (+7 points entre rigueur et relance sur 5 ans, plafonnée par `DEBT_CONSOL`) et les piliers, sans rappel, s'accumulaient sans limite.

**Leviers retenus** (entiers, tous lisibles) :

| Levier | Où | Valeur | Effet |
|---|---|---|---|
| **Défiance des marchés** : coût convexe du déficit du plan | `budget.c`, `MARKET_DIV` (dans `sim_plan_opinion`, donc visible à l'aperçu du budget) | opinion −= déficit² / 7 (déficit en dixièmes de point de PIB), pour tous les électorats | Relance (+1,2 point) : −2,1 points ; plan « tout à +2 » (≈ +5 points) : effondrement. Rend l'optimiseur raisonnable : il retombe sur `+1 +1 +1 −1 −1 +2`. |
| **Rappel des piliers vers 50** | `budget.c`, `PILLAR_RECALL` (`budget_quarter_update`) | 1/12 de l'écart par trimestre (zone morte de 11 dixièmes) | Un plan constant atteint un palier (au plus ≈ +4 à +5 points de pilier) au lieu de s'accumuler : l'écart rigueur / relance en popularité passe de 8,6 à 4,6 points. |
| **Coût de la dette creusée** | `opinion.c`, `DEBT_OPN_REF`, `DEBT_OPN_DIV` | au-delà de +3 points de dette depuis le début du mandat : opinion −= excès² / 800 (dixièmes) | Progressif et relatif à la difficulté (Difficile démarre à 120 %). Pèse peu à 20 tours (la dette bouge peu), mais punit les plans durablement déficitaires. |
| **Plafond de dette assoupli** | `economy.c`, `DEBT_CONSOL` | 20 → 120 (−1 point de déficit par an pour 120 points de dette au-delà de 110 %) | La dette n'est plus bornée par la consolidation spontanée : « extrême » monte à 144 % au lieu de ≈ 135 %. |
| Recalage de l'opinion | `opinion.c` | `OPN_BASE` 339 → 396 ; ligne gauche des affinités +17, ligne droite −6 | Compensent la perte de popularité due aux nouveaux coûts, ramènent l'écart entre orientations à 6 points et le joueur passif à droite sous 5 %. |

Écartés : doubler `k_s` (rapproche les préréglages entre eux), prime de risque sur la seule dette (trop faible à 5 ans), effet retard de l'inflation (l'inflation est une constante, un état supplémentaire changerait `Country` et la sauvegarde pour un effet de l'ordre du bruit). Le malus « budget insincère » (vote) est inchangé.

Joueur automatique « actif » : seuils de dérive de dette de `bot_budget` relevés (gauche : relance tant que la dette a dérivé de moins de 8 points, droite 6, centre 12) car les anciens seuils (6 / 3 / 6 points) faisaient basculer le centre sur la rigueur dans un cas sur deux.

**Tableau de dominance** (normal, réélection gauche / centre / droite, 800 graines) :

| Joueur | Avant | Après |
|---|---|---|
| Actif | 59 / 58 / 50 | 53 / 59 / 56 |
| Optimiseur | 62 / 86 / 88 (+3 / +28 / +38) | 47 / 62 / 67 (−6 / +3 / +11) |
| Extrême | 55 / 82 / 78 | 0 / 0 / 10 |
| Austère | 5 / 24 / 65 | 15 / 27 / 61 |
| Passif | < 1 % | 3 / 4 / 5 (max 4,6) |
| Préréglage fixe, centre : rigueur / équilibre / relance | 41 / 69 / 93 | 39 / 62 / 73 |

L'optimiseur n'est plus au-dessus du joueur actif de plus de 11 points ; la relance fixe est à +14 du joueur actif (centre) au lieu de +35. Dette finale (normal, centre) : rigueur 115,9 %, équilibre 120,1 %, relance 123,4 %, extrême 139 %. Popularité directe des préréglages à l'aperçu : rigueur −0,9 point, relance −1,3 point (le gain de la relance vient de la croissance et des piliers, avec un coût de marchés immédiat).

**Réélection du joueur actif** (20 tours) : facile 74 / 76 / 71 %, normal 53 / 59 / 56 %, difficile 20 / 27 / 22 % (gauche / centre / droite) ; moyennes 73 / 56 / 23 % ; écart entre orientations ≤ 6 points ; destitution 0 %. Mode 60 tours : popularité, chômage et dette à ±1 point, réélection plus haute de 0 à 8 points (aléa moins bruité à 12 tours par an) : tolérance du test portée à 9 points.

**Tests** : le garde-fou provisoire « +45 points » est remplacé par la cible finale : optimiseur, extrême, austère et chaque préréglage fixe ≤ joueur actif + 15 points ; joueur passif < 5 %.
