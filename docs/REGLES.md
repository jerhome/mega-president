# Mega Président — Règles du jeu

> **Statut : brouillon de discussion, rien n'est validé.**
> Les valeurs chiffrées sont des points de départ, à affiner lors de l'équilibrage.
> Le périmètre de chaque version (V0 / V1 / V2) est détaillé dans [BACKLOG.md](BACKLOG.md).

## Ton et univers

- Sérieux et réaliste, avec parfois une **pointe d'humour de gauche** dans les événements et le journal.
- **Partis et personnalités entièrement fictifs.**
- Jeu en français.

## 1. But du jeu

Tu es élu président de la République française. Tu gouvernes pendant 5 ans, tu fais progresser le pays, tu restes populaire, puis tu tentes de te faire réélire pour un second mandat.

- **Fin normale** : à la fin du 1er mandat si tu perds la présidentielle, ou à la fin du 2nd si tu as été réélu (pas de 3e mandat, comme dans la Constitution).
- **Défaite** : être **destitué** par le Parlement (article 68).

## 2. Début de partie

Le joueur choisit :

- **son orientation** : gauche, centre ou droite. Elle détermine les groupes parlementaires alliés et opposés ;
- **son Premier ministre**, parmi 3 profils :
  - Technocrate : économie +, popularité − ;
  - Rassembleur : meilleures relations avec les groupes ;
  - Réformateur : lois moins coûteuses ;
- **la difficulté** : Facile (large majorité, conjoncture favorable), Normal, Difficile (majorité relative, crise en cours) ;
- **la durée** : **20 tours** (1 tour = 1 trimestre) ou **60 tours** (1 tour = 1 mois) par mandat.
  En mode 60 tours, les gains et les effets par tour sont divisés par 3, et un événement survient environ 1 tour sur 3 : le rythme global d'un mandat reste comparable.

## 3. Le tour de jeu

1. **Rapport** : un journal résume l'évolution des indicateurs et l'actualité.
2. **Événement(s)** : une situation à trancher, avec 2 ou 3 choix.
3. **Actions** : dépense du capital politique.
4. **Votes** : l'Assemblée vote les lois proposées.
5. **Fin du tour** : les effets s'appliquent, puis l'opinion et le Parlement réagissent.

## 4. Les indicateurs

| Catégorie | V1 | Ajouts V2 |
|---|---|---|
| Popularité | Globale (0–100 %), calculée à partir de **3 électorats** : classes populaires, classes moyennes, classes aisées | 5 électorats, popularité par région |
| Économie | Croissance, chômage, dette | Déficit détaillé |
| Piliers (0–100, départ ≈ 50) | Social, Économie, Technologie, Écologie, Sécurité | + Rayonnement international |
| Jauges de danger | Tension sociale, Scandale, Défiance du Parlement | — |

Une même décision plaît rarement à tout le monde : une réforme peut plaire aux classes aisées et fâcher les classes populaires.

## 5. Le capital politique (CP)

C'est la ressource principale.

- **Gain** : 3 CP par trimestre, plus un bonus de popularité et un bonus de majorité. Le stock est plafonné à 15.
- **Dépenses en V1** :

| Action | Coût |
|---|---|
| Proposer une loi | 2 à 6 CP |
| 49.3 | 4 CP |
| Intervention télévisée (popularité +) | 2 CP |

- **Ajouts V2** : voyage diplomatique, remaniement, référendum.

## 6. Les lois

Le catalogue compte 15 lois en V1 et plus de 40 en V2. Exemples : réforme des retraites, plan de transition énergétique, baisse de l'impôt sur le revenu, programmation de la recherche, réforme de la police.

Chaque loi a :

- **un coût** en CP et en budget ;
- **une orientation**, qui détermine quels groupes la soutiennent ;
- **des effets différés** : elle met de 1 à 4 trimestres à produire ses effets. Une réforme lancée juste avant l'élection n'a pas le temps de payer.

Une loi est **adoptée avec 289 voix sur 577**. En cas d'échec, le CP est perdu et la défiance augmente.

## 7. Le budget

Une fois par an, l'Assemblée vote la loi de finances.

- **V1** : 4 postes de dépenses (social, éducation et recherche, sécurité, écologie) et un niveau d'imposition global.
- **V2** : 6 postes et 3 impôts (impôt sur le revenu, TVA, impôt sur les sociétés).

Dépenser plus améliore les piliers mais alourdit la dette. Taxer plus réduit la dette mais coûte en popularité et en croissance.

## 8. Le Parlement

- **Assemblée nationale** : 577 sièges répartis entre 5 groupes fictifs. Chaque groupe a une **relation** avec le président (0–100), qui évolue selon les lois et les choix.
- **49.3** : force l'adoption sans vote. Illimité pour le budget, limité à une autre loi par session. Augmente fortement la défiance.
- **Motion de censure** : si la défiance est élevée et que l'opposition réunit 289 sièges, le gouvernement tombe. Il faut nommer un nouveau Premier ministre et la popularité baisse. Le président, lui, reste en place.
- **Dissolution** : au plus une fois par an. Elle déclenche des législatives anticipées, où les sièges sont redistribués selon la popularité.
- **Majorité perdue** :
  - V1 : la majorité devient relative, donc les votes sont plus difficiles ;
  - V2 : **cohabitation**, avec un Premier ministre d'opposition ; le président ne garde la main que sur les affaires étrangères, la défense et le référendum.
- **Sénat** : V2 uniquement. Il compte pour la destitution et est renouvelé par moitié tous les 3 ans.

## 9. Les événements

Environ 30 en V1, plus de 100 en V2. Trois types :

- **aléatoires** : crise financière, canicule, découverte scientifique, sommet international… ;
- **conditionnels**, déclenchés par les jauges : tension sociale > 70 → grève générale ; scandale > 50 → révélations dans la presse ;
- **en chaîne** : une affaire mal gérée peut rebondir plusieurs trimestres plus tard.

## 10. La destitution (défaite)

Elle est possible quand **scandale ≥ 70** et **défiance ≥ 60**.

1. L'opposition engage la procédure. Le président dispose d'un trimestre pour se défendre : discours, sacrifier un ministre, négocier avec un groupe (en CP).
2. **Vote** à la majorité des **deux tiers** :
   - V1 : de l'Assemblée seule ;
   - V2 : de l'Assemblée et du Sénat.
3. S'il survit, le président voit le scandale retomber en partie, mais la défiance reste haute.

**Une partie terminée par une destitution n'entre pas dans les high scores.**

## 11. L'élection présidentielle

Au dernier tour du mandat, le joueur choisit un **thème de campagne** (emploi, sécurité, pouvoir d'achat, écologie…), qui donne un bonus auprès de certains électorats.

- **1er tour** contre 4 candidats fictifs. Le score dépend de la popularité, du chômage, de la progression des piliers, du thème de campagne et d'une petite part de hasard.
- **2nd tour** entre les deux premiers.
- **En cas de victoire**, le second mandat commence, suivi de législatives. Le 2nd mandat comporte une **usure du pouvoir** : la popularité s'érode plus vite.

## 12. Score et high scores

**Score d'un mandat** :

- popularité finale × 10 ;
- \+ progression de chaque pilier × 5 ;
- \+ bonus économiques (baisse du chômage, maîtrise de la dette) ;
- \+ 500 en cas de réélection.

**Deux tableaux de 10 entrées**, sauvegardés dans la cartouche (SRAM), avec 3 initiales par entrée :

- **1er mandat** : le score au terme des 5 premières années, que le joueur soit réélu ou non ;
- **2nd mandat** : le **score cumulé sur les 10 ans**.

Les parties terminées par une destitution n'y figurent jamais.

## 13. Commandes (manette 3 boutons)

| Touche | Action |
|---|---|
| Croix directionnelle | Naviguer |
| A | Valider |
| B | Retour |
| C | Aide ou détail d'un indicateur |
| Start | Finir le tour ou ouvrir le menu |
