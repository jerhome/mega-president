# Répliques des ministres et descriptions des thèmes de campagne

Commande de `REVUE_DA_V4.md` (P1-1.2 et P2-5). Tout est fictif.
Limites : réplique 32 caractères au plus (accents et ponctuation compris,
sans les guillemets que l'écran ajoute) ; descriptions 2 lignes de 38 au plus.
Relecture humour : faite par l'agent `vannes` (15/11).

## 1. Répliques (7 personnages × 4 humeurs)

Humeurs : content = `EXPR_CONTENT`, neutre = `NEUTRE`, gêné = `GENE`,
furieux = `FURIEUX`. Le PM parle sans marque de genre (il peut être
Delaunay, Deschamps, Lestrade, Carrel, Lacombe ou Montclar).

| Personnage | Nom affiché | Fonction | Content | Neutre | Gêné | Furieux |
|---|---|---|---|---|---|---|
| Économie | Fontaine | ministre de l'Économie | Les actionnaires vont adorer. | Les chiffres tiennent. En gros. | Ça déborde du tableur. | Ma calculatrice a fumé, là. |
| Intérieur | Fermont | ministre de l'Intérieur | Enfin une idée. Je la fiche. | Je surveille. Vous aussi. | Mes indics ne m'ont rien dit. | On me cherche. On me trouvera. |
| Santé | Varenne | ministre de la Santé | Je reprends un café. De joie. | Je note. Ordonnance à suivre. | Les urgences, elles, attendent. | J'ai cassé mon stylo. Encore. |
| Éducation | Marchal | ministre de l'Éducation | Voilà qui mérite un bon point. | Peut mieux faire. Voyons. | Hors sujet. Et deux fautes. | Zéro. Et je souligne en rouge. |
| Écologie | Kervella | ministre de l'Écologie | Génial ! Je sors les graines ! | OK. Je note. Je grignote. | Ça sent un peu le gazole, non ? | Je m'enchaîne à mon bureau ! |
| Affaires étrangères | Valcourt | Affaires étrangères | Champagne dans les ambassades. | Je dirai non. Avec le sourire. | Disons... que c'est délicat. | Je passe à l'anglais. Sec. |
| Premier ministre | Matignon | Matignon | Matignon vous suit. De près. | On fera une réunion de cadrage. | Je vais consulter. Longuement. | Ma démission est déjà tapée. |

(Pour le PM, la colonne « Nom affiché » vaut « Matignon » et la fonction
est vide : à l'écran « Matignon » seul suffit. Pour la cohabitation,
`TXT_COHAB_PM_*_NOM` reste utilisable pour le nom ; les répliques sont
les mêmes.)

### Chaînes à copier (ordre : content, neutre, gêné, furieux)

```
ECONOMIE  "Les actionnaires vont adorer."
          "Les chiffres tiennent. En gros."
          "Ça déborde du tableur."
          "Ma calculatrice a fumé, là."
INTERIEUR "Enfin une idée. Je la fiche."
          "Je surveille. Vous aussi."
          "Mes indics ne m'ont rien dit."
          "On me cherche. On me trouvera."
SANTE     "Je reprends un café. De joie."
          "Je note. Ordonnance à suivre."
          "Les urgences, elles, attendent."
          "J'ai cassé mon stylo. Encore."
EDUCATION "Voilà qui mérite un bon point."
          "Peut mieux faire. Voyons."
          "Hors sujet. Et deux fautes."
          "Zéro. Et je souligne en rouge."
ECOLOGIE  "Génial ! Je sors les graines !"
          "OK. Je note. Je grignote."
          "Ça sent un peu le gazole, non ?"
          "Je m'enchaîne à mon bureau !"
AFF_ETR   "Champagne dans les ambassades."
          "Je dirai non. Avec le sourire."
          "Disons... que c'est délicat."
          "Je passe à l'anglais. Sec."
PM        "Matignon vous suit. De près."
          "On fera une réunion de cadrage."
          "Je vais consulter. Longuement."
          "Ma démission est déjà tapée."
```

Note : points de suspension en trois points ASCII (« ... »), jamais
U+2026. Plus longue réplique : 31 caractères. Répliques du PM sans
marque de genre (relu par `vannes`).

## 2. Descriptions des thèmes de campagne

À ajouter : `TXT_THEME_EMPLOI_DESC`, `TXT_THEME_SECURITE_DESC`,
`TXT_THEME_POUVOIR_ACHAT_DESC`, `TXT_THEME_ECOLOGIE_DESC`
(`TXT_THEME_SANTE_DESC` existe déjà). Cohérentes avec `theme_weight`.

```
EMPLOI         "Chômage, salaires, apprentissage.\nJeunes et populaires. Promis, juré."
SECURITE       "Police, justice, tranquillité. Parle\nsurtout aux retraités. Képis inclus."
POUVOIR_ACHAT  "Prix, plein, caddie, fins de mois.\nCible populaires et classes moyennes."
ECOLOGIE       "Climat, transports, sobriété. Cible\njeunes et cadres. Garez le SUV loin."
```

## 3. À reporter dans content_texts.c

Relecture `vannes` des chaînes de cohabitation. Idée directrice :
remplacer « Le Premier ministre » par « Matignon », comme le nom
affiché à l'écran événement. On évite ainsi tout accord de genre
(le PM de cohabitation peut être Sophie Carrel) et on gagne de la
place pour une chute. Lignes de 38 caractères au plus, vérifiées.

```
TXT_COHAB_BUDGET_GOUV
  "Matignon impose son budget. Votre avis\na été noté, puis classé :"
TXT_COHAB_PLAN_GAUCHE
  "Matignon impose un plan social et de\nrelance : dépenses en hausse, et les\nentreprises passent à la caisse."
TXT_COHAB_PLAN_CENTRE
  "Matignon impose un budget de\nreconduction : rien ne bouge et tout\nle monde râle. L'équilibre, quoi."
TXT_COHAB_PLAN_DROITE
  "Matignon impose la rigueur : moins de\ndépenses, moins d'impôts pour les\nentreprises. Ça ruissellera, promis."
TXT_COHAB_DEFENSE
  "Défense : seule ligne qui vous reste,\nà un cran près de celle de Matignon."
```

Les trois `TXT_COHAB_PM_*_DESC` (Elle/Il, accordés au nom) et
`TXT_COHAB_DEBUT`, `TXT_COHAB_FIN`, `TXT_COHAB_AIDE` : bons, inchangés.
