# Cœur de simulation — formules et constantes

> Référence pour l'équilibrage. Code : `src/sim/`. Unités internes : `docs/CONTRAT_V0.md` §2.
> `tpy` = tours par an (4 en V0). Les divisions par `tpy` ramènent les taux annuels ou trimestriels au tour.

## Fichiers

| Fichier | Rôle |
|---|---|
| `sim_state.h` | struct `Country`, énumérations, bornes |
| `sim.h` / `sim.c` | `sim_init`, `sim_end_turn`, `sim_propose_law`, popularité, CP |
| `effects.*` | clés d'effets, conversion et bornage (`sim_apply_effects`, réutilisé par les événements) |
| `laws.*` | catalogue `law_defs`, file des lois en attente |
| `economy.c` | croissance, chômage, dette, tension |
| `opinion.c` | opinion des 3 électorats, état de grâce |
| `parliament.*` | votes, relations |
| `election.*` | présidentielle, score de mandat |

## Départ (`sim_init`)

| Grandeur | Valeur |
|---|---|
| Opinion (pop./moy./ais.) | gauche 58/53/42 %, centre 52/56/52 %, droite 45/54/60 % (± 1,5) |
| Croissance | 1,20 % ± 0,20 |
| Chômage | 7,50 % ± 0,30 |
| Dette | 110 % |
| Piliers | 50 ± 2 |
| Tension / scandale / défiance | 20 / 0 / 10 |
| CP | 5 |
| État de grâce | +12 points d'opinion cible |
| Relations | parti du joueur 80, voisins 55, distance 2 : 30, distance 3 : 15 |

## Fin de tour (`sim_end_turn`)

Ordre : 1. lois arrivées à maturité → 2. économie → 3. opinion → 4. Parlement → 5. CP.
Les événements sont appliqués **avant** par l'appelant (`sim_apply_effects`).

### Économie

- **Tendance** = 1,10 % + (Économie − 50) × 0,02 + (Technologie − 50) × 0,01 (en points, piliers sur 100).
- **Croissance** : `g += (tendance − g) / tpy + aléa(±0,25)`, bornée à ±8 %.
- **Chômage (Okun)** : `u −= (g − 1,10) / (2·tpy)` (−0,5 point par an et par point de croissance au-dessus du potentiel), plus un rappel `(7,50 − u) / (10·tpy)`. Borné à 2–25 %.
- **Dette** : variation annuelle = déficit − dette × (g + 2 % d'inflation) ; déficit = 4,5 points + (u − 7,5) × 1. Calcul en centièmes avec report du reste (`debt_acc`). Bornée à 0–300 %. Sans action : ≈ +1 point par an.
- **Tension** : tend (1/tpy de l'écart) vers 20 + (u − 7,5) × 4 − (Social − 50) × 0,4.

### Opinion (par électorat)

Cible (en points de %) :

```
33 + affinité(orientation, électorat)
 − (u − 7,5) × k_u      k_u : pop. 5, moy. 4, ais. 3
 + (g − 1,1) × k_g      k_g : pop. 1, moy. 2, ais. 3
 + 0,4 × moyenne pondérée des (pilier − 50)
 − 0,2 × (tension − 20)
 + état de grâce
```

| Affinité | pop. | moy. | ais. |
|---|---|---|---|
| gauche | +3 | 0 | −4 |
| centre | −1 | +2 | +1 |
| droite | −4 | 0 | +4 |

| Poids des piliers (/16) | Soc | Éco | Tech | Écol | Séc |
|---|---|---|---|---|---|
| populaires | 6 | 3 | 1 | 2 | 4 |
| moyennes | 3 | 4 | 2 | 3 | 4 |
| aisées | 1 | 7 | 3 | 2 | 3 |

- **Inertie** : `opinion += (cible − opinion) / tpy` (1/4 de l'écart par trimestre), + aléa ±0,8 point.
- **État de grâce** : +12 points au départ, −8 par an (nul après 6 trimestres).
- Les effets immédiats des lois et des événements sur l'opinion sont donc **transitoires** ; les piliers et l'économie sont **durables**.
- **Popularité** = (4 × pop. + 4 × moy. + 2 × ais.) / 10.

### Parlement

- **Vote** : un groupe vote pour si `relation + affinité ≥ 75`, avec `affinité = 50 − |orientation loi − orientation groupe| / 2`. Adoption si ≥ 289 voix. Vote déterministe (`parl_vote` sert aussi d'aperçu).
- **Après adoption** : relation de chaque groupe += affinité / 10 (−5 à +5).
- **Après rejet** : groupes qui ont voté pour −2 ; défiance +5 ; opinion −1 point partout ; CP perdu.
- **Dérive par trimestre** : chaque relation se rapproche de 1 point de sa valeur de départ + (popularité − 45) / 2,5.

### Capital politique (par trimestre)

3 + 1 si popularité ≥ 45 % + 1 si ≥ 55 % + 1 si les groupes à relation ≥ 50 totalisent ≥ 289 sièges. Plafond 15.

## Lois V0 (`laws.c`)

Valeurs en unités lisibles. Délai en trimestres (= tours en V0, ×3 en mode mensuel).

| Id | CP | Délai | Orient. | Immédiat | Différé |
|---|---|---|---|---|---|
| LOI_SMIC | 3 | 2 | −60 | POP_POPULAIRES +4, POP_AISEES −3 | SOCIAL +8, ECONOMIE −3, CHOMAGE +2, TENSION −6 |
| LOI_IS | 3 | 3 | +50 | POP_AISEES +4, POP_POPULAIRES −3 | ECONOMIE +8, CROISSANCE +3, CHOMAGE −2, DETTE +2, SOCIAL −3, TENSION +3 |
| LOI_RENOV | 4 | 3 | −30 | POP_MOYENNES +2 | ECOLOGIE +10, CHOMAGE −2, CROISSANCE +2, DETTE +2, SOCIAL +2 |
| LOI_POLICE | 2 | 2 | +40 | POP_MOYENNES +2, POP_AISEES +1, POP_POPULAIRES −1 | SECURITE +10, DETTE +1, SOCIAL −1 |
| LOI_CIR | 3 | 4 | +20 | POP_AISEES +1 | TECHNOLOGIE +12, ECONOMIE +4, CROISSANCE +2, DETTE +1 |

Une loi adoptée ne peut plus être proposée. Une loi rejetée peut l'être de nouveau.

## Présidentielle (`sim_election`)

Adversaires (contrat §5) : gauche ou centre → tous sauf CAND_CENTRE ; droite → tous sauf CAND_DROITE.
Orientation du joueur : gauche −40, centre 0, droite +40.

- **Bilan** = (chômage de départ − chômage final) / 8 + progression moyenne des piliers / 3, borné à ±15 points (unités internes).
- **1er tour, joueur** = 6 % + popularité / 2 + bilan + aléa ±2,5, borné à 4–60 %.
- **1er tour, adversaires** : se partagent le reste au prorata de poids (GR 110, ÉCO 90, CENTRE 150, DROITE 160, NAT 230), multipliés par (distance au joueur + 40) / 120 si distance < 80 (un candidat proche est affaibli), + aléa ±20. Le reste d'arrondi va au plus fort : somme exacte de 1000.
- **2nd tour, joueur** = 50 % + (popularité − 47) × 0,6 + bilan / 2 + front républicain (+3 contre NAT, +2 contre GR) + aléa ±5, borné à 15–85 %. Élu si > 50 %.
- **Joueur éliminé** : duel entre les deux adversaires (écart du 1er tour / 2 + front républicain).

## Score de mandat (`sim_mandate_score`)

popularité (dixièmes) + Σ progression des piliers (dixièmes) / 2 + (chômage départ − final, centièmes) / 2 + (dette départ − finale, dixièmes) / 2 + 500 si réélu. Minimum 0.
Soit : popularité × 10, piliers × 5 par point, 50 par point de chômage en moins, 5 par point de dette évité.

## Partie automatique (tests, 400 graines × 20 tours, sans événements)

| Orientation | Joueur | Popularité finale | Chômage | Dette | Lois | Réélu |
|---|---|---|---|---|---|---|
| gauche | actif | 40,8 % | 6,77 % | 116,9 % | 5,0 | 62 % |
| centre | actif | 38,5 % | 7,20 % | 116,1 % | 3,0 | 40 % |
| droite | actif | 40,0 % | 6,73 % | 116,4 % | 5,0 | 57 % |
| toutes | passif | ≈ 33 % | 7,48 % | 114,9 % | 0 | 0–5 % |

Le joueur « actif » propose, dans un ordre de préférence, chaque loi abordable dont l'aperçu du vote est favorable.
