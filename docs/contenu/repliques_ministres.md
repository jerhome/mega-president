# Répliques des ministres et descriptions des thèmes de campagne

Commande de `REVUE_DA_V4.md` (P1-1.2 et P2-5). Tout est fictif.
Limites : réplique 32 caractères au plus (accents et ponctuation compris,
sans les guillemets que l'écran ajoute) ; descriptions 2 lignes de 38 au plus.
Relecture humour : agent `vannes` souhaitée.

## 1. Répliques (7 personnages × 4 humeurs)

Humeurs : content = `EXPR_CONTENT`, neutre = `NEUTRE`, gêné = `GENE`,
furieux = `FURIEUX`. Le PM parle sans marque de genre (il peut être
Delaunay, Deschamps, Lestrade, Carrel, Lacombe ou Montclar).

| Personnage | Nom affiché | Fonction | Content | Neutre | Gêné | Furieux |
|---|---|---|---|---|---|---|
| Économie | Fontaine | ministre de l'Économie | Les marchés vont adorer. | Les chiffres tiennent. En gros. | Bercy fera... avec. | Ma calculatrice a fumé, là. |
| Intérieur | Fermont | ministre de l'Intérieur | Les képis approuvent. Enfin. | Je surveille. Tout le monde. | Pas prévu dans mon dossier. | On me cherche. On me trouvera. |
| Santé | Varenne | ministre de la Santé | Ça, c'est un bon diagnostic. | Je note. Ordonnance à suivre. | Les urgences n'aimeront pas. | J'ai cassé mon stylo. Encore. |
| Éducation | Marchal | ministre de l'Éducation | Voilà qui mérite un bon point. | Peut mieux faire. Voyons. | Un peu hors sujet, tout ça. | Copie à revoir. Entièrement. |
| Écologie | Kervella | ministre de l'Écologie | Génial ! Je sors les graines ! | OK. Je note. Je grignote. | Le climat n'attendra pas, hein. | Vous piétinez mes graines ! |
| Affaires étrangères | Valcourt | Affaires étrangères | Nos alliés applaudiront. | Je dirai non. Avec le sourire. | Disons... que c'est délicat. | Je passe à l'anglais. Sec. |
| Premier ministre | Matignon | Matignon | Matignon vous suit. De près. | On en reparle en réunion. | Je vais consulter. Longuement. | On n'a pas été prévenu. Encore. |

(Pour le PM, la colonne « Nom affiché » vaut « Matignon » et la fonction
est vide : à l'écran « Matignon » seul suffit. Pour la cohabitation,
`TXT_COHAB_PM_*_NOM` reste utilisable pour le nom ; les répliques sont
les mêmes.)

### Chaînes à copier (ordre : content, neutre, gêné, furieux)

```
ECONOMIE  "Les marchés vont adorer."
          "Les chiffres tiennent. En gros."
          "Bercy fera... avec."
          "Ma calculatrice a fumé, là."
INTERIEUR "Les képis approuvent. Enfin."
          "Je surveille. Tout le monde."
          "Pas prévu dans mon dossier."
          "On me cherche. On me trouvera."
SANTE     "Ça, c'est un bon diagnostic."
          "Je note. Ordonnance à suivre."
          "Les urgences n'aimeront pas."
          "J'ai cassé mon stylo. Encore."
EDUCATION "Voilà qui mérite un bon point."
          "Peut mieux faire. Voyons."
          "Un peu hors sujet, tout ça."
          "Copie à revoir. Entièrement."
ECOLOGIE  "Génial ! Je sors les graines !"
          "OK. Je note. Je grignote."
          "Le climat n'attendra pas, hein."
          "Vous piétinez mes graines !"
AFF_ETR   "Nos alliés applaudiront."
          "Je dirai non. Avec le sourire."
          "Disons... que c'est délicat."
          "Je passe à l'anglais. Sec."
PM        "Matignon vous suit. De près."
          "On en reparle en réunion."
          "Je vais consulter. Longuement."
          "On n'a pas été prévenu. Encore."
```

Note : le « ... » (U+2026) est un seul caractère ; vérifier qu'il existe
dans la police, sinon utiliser « ... » (la longueur reste < 32).

## 2. Descriptions des thèmes de campagne

À ajouter : `TXT_THEME_EMPLOI_DESC`, `TXT_THEME_SECURITE_DESC`,
`TXT_THEME_POUVOIR_ACHAT_DESC`, `TXT_THEME_ECOLOGIE_DESC`
(`TXT_THEME_SANTE_DESC` existe déjà). Cohérentes avec `theme_weight`.

```
EMPLOI         "Chômage, salaires, apprentissage.\nCible jeunes et classes populaires."
SECURITE       "Police, justice, tranquillité. Parle\nsurtout aux retraités. Képis inclus."
POUVOIR_ACHAT  "Prix, carburant, chèque et caddie.\nCible populaires et classes moyennes."
ECOLOGIE       "Climat, transports, sobriété. Cible\njeunes et cadres. Venez à vélo."
```
