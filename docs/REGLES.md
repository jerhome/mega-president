# Mega Président — Règles du jeu

> **Statut : validé le 2026-11-14, mis à jour pour la V4 le 2026-11-15.**
> Les valeurs chiffrées sont calées par le simulateur (tâche 4.14) ; formules exactes dans [SIM.md](SIM.md).
> Le périmètre de chaque version est détaillé dans [BACKLOG.md](BACKLOG.md).
>
> | Version | Apport |
> |---|---|
> | V1, V2 | Jeu de base, Parlement, cohabitation, Sénat, référendum, régions |
> | **V3** | Apparence (interface, visages animés, musique) et écran **Options** |
> | **V4** | **5 électorats**, **budget détaillé**, 5e thème de campagne, sauvegarde versionnée, voix des personnages (en cours) |
>
> Les passages marqués **(V3)** ou **(V4)** sont nouveaux dans ces versions.

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

| Catégorie | V1 | Ajouts V2 / V4 |
|---|---|---|
| Popularité | Globale (0–100 %), moyenne pondérée de 3 électorats | V2 : popularité par région. **V4 : 5 électorats** (voir ci-dessous) |
| Économie | Croissance, chômage, dette | Déficit, lié au budget détaillé (V4) |
| Piliers (0–100, départ ≈ 50) | Social, Économie, Technologie, Écologie, Sécurité | V2 : + Rayonnement international |
| Jauges de danger | Tension sociale, Scandale, Défiance du Parlement | — |

Une même décision plaît rarement à tout le monde : une réforme peut plaire aux cadres et fâcher les classes populaires.

### Les 5 électorats (V4)

Catégories exclusives, par âge pour les non-actifs et par classe pour les actifs. Le poids est la part des suffrages exprimés (l'abstention est déjà déduite).

| Électorat | Abrév. | Qui | Poids |
|---|---|---|---|
| Classes populaires | Pop | ouvriers et employés actifs | 26 |
| Classes moyennes | Moy | professions intermédiaires, artisans, agriculteurs | 22 |
| Cadres | Cad | cadres, libéraux, chefs d'entreprise | 12 |
| Retraités | Ret | retraités, quel que soit leur métier | 28 |
| Jeunes | Jeu | 18-29 ans : études, jobs, chômage | 12 |

**Popularité** = (26 Pop + 22 Moy + 12 Cad + 28 Ret + 12 Jeu) / 100. Chaque électorat vise une cible d'opinion, qu'il rejoint par inertie (1/tour en mode 20 tours). La cible dépend de :

- l'**orientation** du joueur (affinité propre à chaque électorat : la gauche plaît aux populaires et aux jeunes, la droite aux cadres) ;
- le **chômage** (les jeunes et les populaires y sont les plus sensibles) et la **croissance** (surtout les cadres) ;
- les **piliers**, avec un poids propre à chacun : le Social pour les populaires et les retraités, la Sécurité pour les retraités, l'Écologie pour les jeunes, l'Économie pour les cadres ;
- la tension sociale, le scandale, la défiance du Parlement, l'état de grâce et, au 2nd mandat, l'usure ;
- le **budget** voté (§ 7) et le « sérieux budgétaire », surtout apprécié des cadres ;
- les **effets immédiats** des lois et des événements, qui visent des électorats précis (la réforme des retraites fâche surtout les retraités).

Le tableau de bord affiche les 5 valeurs en entiers, colorées selon la tendance. Le bouton C ouvre le détail (jauges, variation, phrase de contexte).

Les électorats pèsent aussi dans les **législatives** et le renouvellement du Sénat : on regarde l'électorat-cœur de l'orientation (gauche : Pop et Jeu ; centre : Moy et Ret ; droite : Cad et Ret), avec la popularité.

## 5. Le capital politique (CP)

C'est la ressource principale.

- **Départ** : 5 CP en facile, 4 en normal, 3 en difficile.
- **Gain** : 1 CP par trimestre, +1 si la popularité atteint 50 %, +1 avec la majorité absolue, soit 1 à 3 CP. Le stock est plafonné à 15. *(Réglage du 14/11 : la base était de 3 CP, trop abondante.)*
- **Moment du gain** : à la fin du trimestre, affiché dans le rapport. En mode 60 tours (1 tour = 1 mois), il tombe tous les 3 mois ; les autres mois rapportent 0. Le tableau de bord indique le prochain apport (« +2 CP/tour », « +2 CP dans 2 mois »).
- **Dépenses** :

| Action | Coût |
|---|---|
| Proposer une loi | 2 à 6 CP (1 de moins avec le PM Réformateur) |
| Forcer une loi (49.3) | coût de la loi + 4 CP |
| Budget au 49.3 | 4 CP |
| Lecture définitive du budget, contre le Sénat (V4) | 1 CP |
| Intervention télévisée (popularité +) | 2 CP |
| Référendum (V2) | 6 CP |
| Remaniement (V2) | 3 CP |
| Défense contre la destitution : discours / négociation | 2 / 3 CP |

Un CP dépensé pour une loi rejetée est perdu. Les événements peuvent aussi en faire gagner ou perdre.

## 6. Les lois

Le catalogue compte 15 lois en V1 et plus de 40 en V2. Exemples : réforme des retraites, plan de transition énergétique, baisse de l'impôt sur le revenu, programmation de la recherche, réforme de la police.

Chaque loi a :

- **un coût** en CP et en budget ;
- **une orientation**, qui détermine quels groupes la soutiennent ;
- **des effets différés** : elle met de 1 à 4 trimestres à produire ses effets. Une réforme lancée juste avant l'élection n'a pas le temps de payer.

Une loi est **adoptée avec 289 voix sur 577**. En cas d'échec, le CP est perdu et la défiance augmente.

## 7. Le budget

Une fois par an, l'Assemblée vote la loi de finances.

Dépenser plus améliore les piliers mais alourdit la dette. Taxer plus réduit la dette mais coûte en popularité et en croissance.

### Budget détaillé (V4)

**6 lignes**, chacune sur **5 crans** : −2, −1, 0 (reconduction des crédits), +1, +2. Pour les impôts, + signifie une hausse.

| # | Ligne | Domaine |
|---|---|---|
| 0 | Social et retraites | santé, pensions, minima |
| 1 | Éducation et recherche | école, université, recherche |
| 2 | Sécurité et défense | police, justice, armées |
| 3 | Transition et réseaux | énergie, transports |
| 4 | Impôts des ménages | revenu, TVA, CSG |
| 5 | Impôts des entreprises et du capital | sociétés, patrimoine |

Chaque ligne agit sur le déficit, la croissance, la tension, les piliers, les 5 électorats et le vote des groupes. Exemples : le Social plaît aux populaires et aux retraités, l'Éducation aux jeunes, la Sécurité aux retraités, les impôts des entreprises fâchent les cadres. L'effet d'un cran est **asymétrique** : monter à +2 rapporte 1,5 fois un cran, descendre à −2 coûte 2,5 fois (une coupe franche fait très mal). Pour les impôts, c'est l'inverse : une forte hausse fait très mal, une forte baisse rapporte peu.

**Préréglages** (première ligne de l'écran, validation en deux pressions) :

| Préréglage | Social | Éduc. | Sécu. | Trans. | Ménages | Entr. |
|---|---|---|---|---|---|---|
| Rigueur | −1 | 0 | 0 | −1 | +1 | 0 |
| Équilibre | 0 | 0 | 0 | 0 | 0 | 0 |
| Relance | +1 | +1 | 0 | +1 | 0 | 0 |

Dès qu'une ligne change, le préréglage devient « Personnalisé ». L'écran montre en direct le déficit, la croissance, la tension, l'effet sur chaque électorat (`++` à `--`) et le **vote prévu** sur 577 sièges.

**Garde-fous** (pas de plan miracle) :

- **Sérieux budgétaire** : un déficit pèse sur l'opinion, surtout chez les cadres (puis les retraités et les classes moyennes) ;
- **Défiance des marchés** : le coût du déficit croît plus vite que le déficit (le carré). Un plan très déficitaire s'effondre dans l'opinion ;
- **Rappel des piliers** : un plan constant fait monter un pilier jusqu'à un palier (quelques points), pas indéfiniment ;
- **Coût de la dette** : au-delà de 3 points de dette gagnés depuis le début du mandat, l'opinion baisse ;
- **Prime de risque** : au-dessus de 120 % de dette et de 1 point de déficit, la croissance en pâtit ;
- **Budget insincère** : au-dessus de 1,5 point de déficit, les groupes hors du parti du joueur votent plus durement.

**Vote et 49.3** : le plan est voté comme une loi (orientation = celle du joueur et des lignes). Le **49.3** le fait passer sans vote : 4 CP, défiance +8, autant de fois qu'on veut.

**Reconduction** : sans budget adopté en fin d'année, chaque dépense recule d'un cran vers 0 (« services votés ») et les impôts ne changent pas. Impossible d'imposer un plan extrême une fois puis de le laisser courir.

**Cohabitation** : le Premier ministre d'opposition impose son plan (gauche : `+1 +1 0 +1 0 +1`, centre : tout à 0, droite : `−1 0 +1 −1 0 −1`). Le président règle seulement la **ligne Sécurité et défense**, à ±1 cran, sans CP. L'écran s'ouvre en lecture seule, titre « Budget du gouvernement ».

**Sénat** : si le bloc présidentiel n'y a pas la majorité (rare, surtout en difficile) et hors cohabitation, le Sénat **amende** le budget adopté, par vote comme par 49.3 : la ligne la plus contraire à son orientation recule d'un cran. Le journal l'annonce. Le président peut demander la **lecture définitive** pour **1 CP** : l'Assemblée a le dernier mot et son plan est rétabli.

Calendrier : présentation au 4e trimestre (les 3 derniers mois en mode 60 tours), application toute l'année suivante. Historique : en V1, 4 postes et un niveau d'impôt global ; la V4 remplace ce choix par le budget détaillé.

## 8. Le Parlement

- **Assemblée nationale** : 577 sièges répartis entre 5 groupes fictifs. Chaque groupe a une **relation** avec le président (0–100), qui évolue selon les lois et les choix.
- **49.3** : force l'adoption sans vote. Illimité pour le budget, limité à une loi par année du mandat. Augmente fortement la défiance.
- **Motion de censure** : si la défiance est élevée et que l'opposition réunit 289 sièges, le gouvernement tombe. Il faut nommer un nouveau Premier ministre et la popularité baisse. Le président, lui, reste en place.
- **Dissolution** : au plus une fois par an. Elle déclenche des législatives anticipées, où les sièges sont redistribués selon la popularité.
- **Majorité perdue** :
  - V1 : la majorité devient relative, donc les votes sont plus difficiles ;
  - V2 : **cohabitation**, avec un Premier ministre d'opposition ; le président ne garde la main que sur les affaires étrangères, la défense (dont la ligne Sécurité et défense du budget, V4) et le référendum.
- **Sénat** : V2. Il compte pour la destitution et est renouvelé par moitié tous les 3 ans. **V4** : il peut aussi amender le budget (§ 7).
- **Référendum** (V2, 6 CP, un par mandat) : porte sur une loi non adoptée. **V4** : chacun des 5 électorats vote « oui » ou « non » selon sa proximité avec la loi (les jeunes et les populaires penchent à gauche, les cadres et les retraités à droite), puis le total est pondéré par les poids du § 4. Adopté à 50 % : la loi passe sans vote, opinion +4, défiance −3. Rejeté : opinion −4, défiance +10. L'écran affiche les 5 « oui » sur 5 lignes.

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

Au dernier tour du mandat, le joueur choisit un **thème de campagne**, qui donne un bonus auprès de certains électorats. **V4 : 5 thèmes**, avec le poids de chacun sur les 5 électorats :

| Thème | Pop | Moy | Cad | Ret | Jeu | Crédibilité (bilan du mandat) |
|---|---|---|---|---|---|---|
| Emploi | 4 | 3 | 1 | 0 | 4 | baisse du chômage |
| Sécurité | 2 | 3 | 2 | 4 | 0 | pilier Sécurité |
| Pouvoir d'achat | 4 | 4 | 0 | 3 | 2 | pilier Social ; −1 si budget Social < 0 ou impôts des ménages en hausse |
| Écologie | 1 | 3 | 3 | 0 | 4 | pilier Écologie |
| **Santé et retraites** (V4) | 2 | 2 | 0 | 4 | 0 | pilier Social |

Un thème n'a d'effet que s'il est crédible : promettre l'écologie après cinq ans de recul du pilier ne rapporte presque rien.

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

## 14. Options (V3)

Écran accessible depuis le menu d'accueil du titre et le menu de jeu. L'huissier de l'Élysée, Firmin Tisserand, y commente chaque ligne. Les réglages sont **appliqués aussitôt** et gardés dans la cartouche ; ils appartiennent à la console, pas à la partie.

| Groupe | Réglage | Valeurs (défaut en gras) |
|---|---|---|
| Son | Musique | Coupée, 25, 50, 75, **100 %** |
| Son | Bruitages | **Tous**, Essentiels (sans les clics de menu), Aucun |
| Son | Voix | **Oui**, Non |
| Affichage | Visages | **Animés**, Figés |
| Affichage | Transitions | **Douces**, Rapides, Aucune |
| Affichage | Couleurs | **Standard**, Daltonisme (carte et flèches lisibles par tous) |
| Données | Effacer les records | double confirmation |
| Données | Effacer la partie | double confirmation |
| Données | Réglages d'origine | confirmation simple |

Les lignes de données sans objet restent visibles, grisées et inactives, avec la raison dans l'aide : « Aucune partie sauvegardée », « Impossible pendant une partie ».

## 15. Sauvegarde (V4)

- La partie en cours est sauvegardée dans la cartouche (SRAM), à côté des records.
- **Une partie sauvegardée par une version précédente est ignorée** : le titre affiche qu'elle vient d'une ancienne version et propose une nouvelle partie. Aucune migration (décision du joueur, 15/11).
- Les **records** (deux tableaux) et les **réglages** sont **conservés** d'une version à l'autre.

## 16. Voix des personnages (V4, en cours, tâche 4.18)

Les personnages « parlent » en charabia, comme dans les Sims : **2 syllabes accueillantes** par réplique, avec un timbre propre à chacun. Les lèvres suivent, puis seuls les yeux bougent. Le réglage **Voix** des Options coupe ce charabia, ainsi que le « For sure ! » du démarrage.
