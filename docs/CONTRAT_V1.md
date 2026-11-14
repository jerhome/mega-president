# Contrat technique V1

> Coordination des agents pour la V1 (vague 1). Complète [CONTRAT_V0.md](CONTRAT_V0.md), dont les règles restent valables (unités, clés d'effets, UTF-8, 38 colonnes).
> Toute modification passe par la session principale.

## 1. Partage des fichiers (vague 1)

| Propriétaire | Fichiers | Tâches |
|---|---|---|
| Agent **règles** (`md-dev`) | `src/sim/` (sauf `events_*` générés et `ids_v1.h`), `tests/test_sim*.c`, `tests/test_main.c`, `docs/SIM.md` | 1.01 à 1.11, données des 10 nouvelles lois (1.12), première passe d'équilibrage (1.20) |
| Agent **sauvegarde et contenu** (`md-dev`) | `src/save/`, `src/screens/scr_scores.c`, `tools/content/`, `src/content/` (généré), `src/sim/events_*` (générés), `tests/test_save.c`, `tests/test_content.c` | 1.15, 1.16, extension du convertisseur |
| Agent **rédacteur** | `docs/contenu/` (nouveaux fichiers uniquement, sauf corrections demandées) | 1.12 (textes), 1.13 (10 événements), textes institutionnels |
| Session principale | `src/main.c`, `src/game/`, `src/screens/` (sauf `scr_scores.c`), `src/sim/ids_v1.h`, `Makefile`, docs de pilotage, git | intégration, vague 2 (écrans) |

Aucun agent ne commite. Si un agent a besoin d'un fichier qui ne lui appartient pas, il le signale dans son rapport.

## 2. Les 10 nouvelles lois

Identifiants à ajouter à l'énumération de `src/sim/laws.h`, **dans cet ordre, après `LOI_CIR`** :

| Id | Loi | Orientation |
|---|---|---|
| LOI_RETRAITES | Report de l'âge de départ à la retraite | +40 |
| LOI_ISF | Rétablissement d'un impôt sur la fortune | −60 |
| LOI_HOPITAL | Plan pour l'hôpital public | −30 |
| LOI_ECOLE | Recrutement d'enseignants | −20 |
| LOI_ENERGIE | Relance du nucléaire et des renouvelables | +20 |
| LOI_TRANSPORTS | Trains du quotidien, tarif unique | −40 |
| LOI_IMMIGRATION | Durcissement des conditions d'entrée | +70 |
| LOI_CHOMAGE | Réforme de l'assurance chômage | +50 |
| LOI_NUMERIQUE | Taxe sur les géants du numérique | −20 |
| LOI_LOGEMENT | Construction de logements sociaux | −30 |

Au total, 15 lois. Le masque `laws_adopted` (u16) suffit.

## 3. Identifiants partagés : `src/sim/ids_v1.h`

`PM_TECHNOCRATE`, `PM_RASSEMBLEUR`, `PM_REFORMATEUR` · `DIFF_FACILE`, `DIFF_NORMAL`, `DIFF_DIFFICILE` · `THEME_EMPLOI`, `THEME_SECURITE`, `THEME_POUVOIR_ACHAT`, `THEME_ECOLOGIE` · `BUDGET_RIGUEUR`, `BUDGET_EQUILIBRE`, `BUDGET_RELANCE` (budget simplifié : une décision par an).

## 4. Textes institutionnels : `docs/contenu/textes.md`

Format, une entrée par clé :

```
## TXT_CLE
(texte, 1 à 3 lignes de 38 colonnes au maximum)
```

Le convertisseur génère `enum { TXT_..., TXT_COUNT }` et `const char *const txt[]` dans `src/content/content_texts.{h,c}`. Une clé de la liste absente du fichier est une erreur.

Clés obligatoires :

| Thème | Clés |
|---|---|
| 49.3 | `TXT_493_UTILISE` (journal après un 49.3) |
| Télévision | `TXT_TV_SUCCES`, `TXT_TV_FLOP` |
| Censure | `TXT_CENSURE_DEPOSEE`, `TXT_CENSURE_ADOPTEE`, `TXT_CENSURE_REJETEE` |
| Gouvernement | `TXT_NOUVEAU_PM` |
| Dissolution | `TXT_DISSOLUTION`, `TXT_LEGISLATIVES_GAGNEES`, `TXT_LEGISLATIVES_PERDUES` |
| Budget | `TXT_BUDGET_ADOPTE`, `TXT_BUDGET_493` |
| Destitution | `TXT_DESTITUTION_PROCEDURE`, `TXT_DESTITUTION_VOTEE`, `TXT_DESTITUTION_REJETEE` |
| Second mandat | `TXT_SECOND_MANDAT`, `TXT_FIN_SECOND_MANDAT` |
| Premiers ministres | `TXT_PM_TECHNOCRATE_NOM`, `TXT_PM_TECHNOCRATE_DESC`, `TXT_PM_RASSEMBLEUR_NOM`, `TXT_PM_RASSEMBLEUR_DESC`, `TXT_PM_REFORMATEUR_NOM`, `TXT_PM_REFORMATEUR_DESC` (le nom est celui d'une personne fictive, 24 caractères au maximum) |
| Difficulté | `TXT_DIFF_FACILE`, `TXT_DIFF_NORMAL`, `TXT_DIFF_DIFFICILE` (une ligne de description chacun) |
| Thèmes de campagne | `TXT_THEME_EMPLOI`, `TXT_THEME_SECURITE`, `TXT_THEME_POUVOIR_ACHAT`, `TXT_THEME_ECOLOGIE` (libellé de 24 caractères au maximum) |
| Budget | `TXT_BUD_RIGUEUR`, `TXT_BUD_EQUILIBRE`, `TXT_BUD_RELANCE` (libellés de 20 caractères au maximum) et `TXT_BUD_RIGUEUR_DESC`, `TXT_BUD_EQUILIBRE_DESC`, `TXT_BUD_RELANCE_DESC` (2 lignes de 38 colonnes au maximum) |

## 5. Lois V1 : `docs/contenu/lois_v1.md`

Même format que `lois_v0.md`. Le convertisseur lit tous les `docs/contenu/lois_*.md`. Chaque `LOI_*` de `laws.h` doit avoir ses textes, et inversement.

## 6. Sauvegarde (SRAM)

- Une zone d'en-tête : signature `MPV1` et version de format.
- **Deux tableaux de 10 entrées** : le 1er mandat, et le cumul sur 10 ans. Chaque entrée contient 3 initiales (A–Z), le score (u16), l'orientation et l'année de fin.
- Un contrôle d'intégrité (somme ou CRC) par tableau. Un tableau corrompu est réinitialisé, sans planter.
- Une partie terminée par une destitution n'est jamais enregistrée.
- L'accès passe par des fonctions SGDK `SRAM_*` encapsulées dans `src/save/`. La logique de classement (insertion, tri) est en C portable, testée sur PC.

## 7. Arbitrages du joueur (14/11)

- Mode 60 tours reporté en V2 : la V1 ne propose que 20 tours.
- Budget simplifié : une décision par an (rigueur, équilibre, relance).
- Bruitages maintenus en V1 (non coupés).
- Équilibrage surtout par parties automatiques, puis un seul test du joueur.
- Nouvelle clé d'effet `REL_PARTI` : relation avec le parti du joueur, quelle que soit son orientation.
