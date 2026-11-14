# Contrat technique V2

> Complète CONTRAT_V0 et CONTRAT_V1. Toute modification passe par la session principale.

## 1. Vague 1 (simulation, sans décision du joueur)

| Propriétaire | Fichiers | Tâches |
|---|---|---|
| Agent **simulation V2** (`md-dev`) | `src/sim/` (sauf fichiers générés), `tests/test_sim*.c`, `tests/test_main.c`, `docs/SIM.md`, `docs/CONTRAT_V2.md` §2 | 2.04 cohabitation, 2.05 Sénat, 2.06 référendum, 2.07 remaniement, 2.15 mode 60 tours ; capacité d'événements portée à 64 |
| Session principale | le reste | intégration, écrans ensuite |

## 2. Clés de textes V2

Le rédacteur écrit ces clés dans `docs/contenu/textes_v2.md` (même format que `textes.md` : `## TXT_CLE` puis le texte). Colonnes : **L** = nombre de lignes maximum, **C** = colonnes maximum par ligne (38 pour un texte d'écran). Les textes à valeur variable (nom de loi, nombre de sièges, score) sont complétés par l'écran : le texte fixe doit alors laisser la place (indiquée). Le convertisseur (`tools/content/gen_content.py`, liste des clés obligatoires et `MAX_EVENTS` à passer à 64) est à étendre par l'agent contenu.

### Cohabitation

| Clé | Usage | L | C |
|---|---|---|---|
| `TXT_COHAB_TITRE` | Étiquette du tableau de bord et titre de l'écran | 1 | 24 |
| `TXT_COHAB_DEBUT` | Journal : législatives perdues, la cohabitation commence | 3 | 38 |
| `TXT_COHAB_FIN` | Journal : législatives gagnées, fin de la cohabitation | 3 | 38 |
| `TXT_COHAB_AIDE` | Aide : ce que le président peut encore faire (lois de souveraineté, référendum, dissolution) et ce qu'il ne peut plus faire | 6 | 38 |
| `TXT_COHAB_LOI_REFUSEE` | Message quand on choisit une loi non souveraine (`LAW_ERR_COHAB`) | 2 | 38 |
| `TXT_COHAB_SOUVERAINE` | Étiquette d'une loi de souveraineté dans la liste des lois | 1 | 12 |
| `TXT_COHAB_LOI_GOUV` | Journal : le gouvernement a fait adopter une loi (le nom de la loi suit à la ligne) | 1 | 38 |
| `TXT_COHAB_BUDGET_GOUV` | Journal : le Premier ministre impose le budget (le libellé `TXT_BUD_*` suit) | 2 | 38 |
| `TXT_COHAB_PM_GAUCHE_NOM`, `TXT_COHAB_PM_CENTRE_NOM`, `TXT_COHAB_PM_DROITE_NOM` | Nom (personne fictive) du Premier ministre d'opposition selon son camp (`sim_cohab_camp`) | 1 | 24 |
| `TXT_COHAB_PM_GAUCHE_DESC`, `TXT_COHAB_PM_CENTRE_DESC`, `TXT_COHAB_PM_DROITE_DESC` | Description du PM d'opposition (ligne politique, budget qu'il impose) | 3 | 38 |

### Sénat

| Clé | Usage | L | C |
|---|---|---|---|
| `TXT_SENAT_TITRE` | Titre de l'écran / de la section « Sénat » | 1 | 24 |
| `TXT_SENAT_AIDE` | Aide : 348 sièges, renouvellement par moitié tous les 3 ans, rôle dans la destitution | 5 | 38 |
| `TXT_SENAT_RENOUVELE_GAIN` | Journal : renouvellement favorable au bloc présidentiel | 3 | 38 |
| `TXT_SENAT_RENOUVELE_PERTE` | Journal : renouvellement défavorable | 3 | 38 |
| `TXT_SENAT_PROCHAIN` | Libellé « Prochain renouvellement » (le nombre de tours est ajouté par l'écran) | 1 | 28 |
| `TXT_SENAT_VOIX` | Libellé « Voix au Sénat » (aperçu de la destitution) | 1 | 24 |
| `TXT_DESTITUTION_SENAT_REJETEE` | Journal : l'Assemblée a voté la destitution mais le Sénat l'a refusée (président sauvé) | 3 | 38 |
| `TXT_DESTITUTION_SENAT_VOTEE` | Journal : l'Assemblée et le Sénat votent la destitution (complète `TXT_DESTITUTION_VOTEE`) | 3 | 38 |

### Référendum

| Clé | Usage | L | C |
|---|---|---|---|
| `TXT_ACT_REFERENDUM` | Libellé de l'action dans le menu (le coût en CP est ajouté par l'écran) | 1 | 24 |
| `TXT_REF_TITRE` | Titre de l'écran | 1 | 24 |
| `TXT_REF_AIDE` | Aide : une fois par mandat, 6 CP, loi non adoptée, succès et échec | 5 | 38 |
| `TXT_REF_CONFIRME` | Confirmation avant de lancer le référendum | 2 | 38 |
| `TXT_REF_DEJA` | Message : référendum déjà utilisé ce mandat (`RFD_ERR_USED`) | 2 | 38 |
| `TXT_REF_ANNONCE` | Écran de suspense avant le résultat : « Le peuple est appelé aux urnes » | 3 | 38 |
| `TXT_REF_OUI` | Journal et écran de résultat : la loi est adoptée par le peuple | 3 | 38 |
| `TXT_REF_NON` | Journal et écran de résultat : le peuple rejette la loi | 3 | 38 |
| `TXT_REF_LABEL_OUI`, `TXT_REF_LABEL_NON` | Libellés des barres de résultat (« OUI » / « NON ») | 1 | 8 |

### Remaniement

| Clé | Usage | L | C |
|---|---|---|---|
| `TXT_ACT_REMANIEMENT` | Libellé de l'action dans le menu (le coût en CP est ajouté par l'écran) | 1 | 24 |
| `TXT_RSH_TITRE` | Titre de l'écran de choix du nouveau PM | 1 | 24 |
| `TXT_RSH_AIDE` | Aide : 3 CP, une fois par an, nouveau profil, relations et popularité rafraîchies, impossible en cohabitation | 4 | 38 |
| `TXT_RSH_CONFIRME` | Confirmation (le nom du PM est ajouté par l'écran) | 2 | 38 |
| `TXT_RSH_FAIT` | Journal : remaniement ministériel, état de grâce | 3 | 38 |
| `TXT_RSH_ANNEE` | Message : déjà un remaniement cette année (`RSH_ERR_YEAR`) | 2 | 38 |
| `TXT_RSH_COHAB` | Message : impossible en cohabitation (`RSH_ERR_COHAB`) | 2 | 38 |

### Mode 60 tours

| Clé | Usage | L | C |
|---|---|---|---|
| `TXT_DUREE_TITRE` | Titre du choix de durée sur l'écran titre | 1 | 24 |
| `TXT_DUREE_20` | « 20 tours (1 tour = 1 trimestre) » | 1 | 38 |
| `TXT_DUREE_60` | « 60 tours (1 tour = 1 mois) » | 1 | 38 |
| `TXT_DUREE_20_DESC`, `TXT_DUREE_60_DESC` | Description : rythme, événements (60 tours : environ 1 tour sur 3) | 2 | 38 |
| `TXT_MOIS_1` … `TXT_MOIS_12` | Noms des mois pour l'en-tête du tableau de bord et du journal en mode 60 tours | 1 | 12 |
| `TXT_TOUR_MOIS` | Libellé « Mois » / « Trimestre » de l'en-tête (« Mois %d » reste composé par l'écran) | 1 | 12 |

Les clés V1 déjà existantes (`TXT_BUD_*`, `TXT_PM_*`, `TXT_DESTITUTION_*`, noms des groupes dans `content_world`) sont réutilisées telles quelles.

## 3. Décisions du joueur (14/11)
- **Pixel art (2.02)** : réalisé par Claude, par programme (scripts Python reproductibles).
  - Style **caricatural, façon BD**.
  - Pas de priorité imposée : on suit l'ordre écran titre, portraits, vignettes d'événements, décors.
  - Tous les personnages sont fictifs.
- **Musique (2.03)** : les 3 compositions originales sont conservées.
- **Carte des régions (2.01)** : style simple et remplaçable, alimentée par la popularité régionale.
