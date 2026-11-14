# Cœur de simulation — formules et constantes

> Référence pour l'équilibrage. Code : `src/sim/`. Unités internes : `docs/CONTRAT_V0.md` §2.
> `tpy` = tours par an (4 en V1 ; le mode 12 tours/an est reporté en V2, le code reste compatible).
> Les variations annuelles sont ramenées au tour par `sim_step` (reste reporté d'un tour à l'autre, pas de perte par troncature).

## Fichiers

| Fichier | Rôle |
|---|---|
| `sim_state.h` | struct `Country` (126 octets sur 68000), énumérations, bornes |
| `sim.h` / `sim.c` | `sim_init_ex`, `sim_end_turn`, lois, 49.3, télévision, PM, jauges, CP, 2nd mandat, scores |
| `effects.*` | clés d'effets, conversion et bornage (`sim_apply_effect`) |
| `laws.*` | catalogue `law_defs` (15 lois), coût effectif, file des lois en attente |
| `economy.c` | croissance, chômage, dette, tension |
| `opinion.c` | opinion des 3 électorats, état de grâce, usure |
| `parliament.*` | votes, relations |
| `budget.*` | budget annuel (rigueur / équilibre / relance) |
| `institutions.*` | législatives, dissolution, majorité relative, censure, destitution |
| `election.*` | présidentielle, thème de campagne, score de mandat |

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

Ordre : 1. lois arrivées à maturité → 2. budget (piliers, au trimestre) → 3. économie → 4. opinion → 5. Parlement → 6. jauges (au trimestre) → 7. clôture budgétaire (fin d'année) → 8. CP (au trimestre) → 9. institutions : destitution puis censure (au trimestre).
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

Poids des piliers (/16) : populaires 6/3/1/2/4, moyennes 3/4/2/3/4, aisées 1/7/3/2/3 (Soc/Éco/Tech/Écol/Séc).

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
