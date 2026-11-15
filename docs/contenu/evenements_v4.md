# Événements V4

Suite de evenements_v2.md, même format strict. Six événements ciblés sur les électorats
ajoutés par la V4 : retraités (pensions, EHPAD, fracture numérique) et jeunes (affectation
post-bac, chômage des jeunes, marche pour le climat). Tout est fictif.

## EVT_PENSIONS
Condition : DETTE > 100
Probabilité : moyenne
Titre : Le gel des pensions
Vignette : bourse
Texte : Bercy propose de geler les pensions
  pour « un an, peut-être deux ». Le
  Conseil des retraités rappelle qu'il
  vote, lui, à chaque élection. Le
  ministre du Budget parle d'« effort
  partagé », surtout par les autres.
Choix 1 : Geler les pensions
  Effets : DETTE -3, POP_RETRAITES -5, POP_POPULAIRES -1, TENSION +3
  Journal : Pensions gelées : les retraités
    comptent, eux aussi, les centimes.
Choix 2 : Indexer sur l'inflation
  Effets : DETTE +3, SOCIAL +2, POP_RETRAITES +4, POP_JEUNES -2
  Journal : Pensions indexées. Les jeunes
    paieront, ils l'ont bien compris.
Choix 3 : Geler seulement les plus hautes
  Effets : DETTE -1, POP_RETRAITES -1, POP_POPULAIRES +1, POP_CADRES -2
  Journal : Seules les grosses pensions gèlent.
    Elles ont déjà de bons gants.

## EVT_EHPAD
Condition : SOCIAL < 50
Probabilité : moyenne
Titre : Scandale en maison de retraite
Vignette : sante
Texte : Un livre-enquête révèle des repas à
  1,80 euro et des soins chronométrés
  dans les résidences du groupe
  Sérénia-Vie, filiale d'Omnicorp
  France. Chaîne Info 24 ouvre son
  journal avec les photos.
Choix 1 : Plan grand âge
  Effets : SOCIAL +4, DETTE +3, POP_RETRAITES +4, POP_POPULAIRES +1, TENSION -2
  Journal : Plan grand âge : des bras, des lits,
    et le dessert n'est plus en option.
Choix 2 : Contrôles et sanctions
  Effets : SOCIAL +2, SCANDALE -2, POP_RETRAITES +2, ECONOMIE -1, POP_CADRES -1
  Journal : Inspections surprises : le menu
    s'améliore d'un euro vingt.
Choix 3 : Laisser le groupe s'expliquer
  Effets : SCANDALE +4, POP_RETRAITES -4, POP_POPULAIRES -2, REL_PARTI +1
  Journal : Le groupe « regrette ». Les familles
    aussi, mais avec moins de communiqués.

## EVT_FRACTURE_NUM
Condition : TECHNOLOGIE > 45
Probabilité : faible
Titre : Fracture numérique
Vignette : numerique
Texte : Les impôts, la carte vitale et les
  billets de train ne se gèrent plus
  qu'en ligne. Des milliers de seniors
  font la queue au guichet de la mairie
  pour imprimer un code que le site
  leur a envoyé par SMS.
Choix 1 : Un guichet humain par canton
  Effets : SOCIAL +3, DETTE +2, POP_RETRAITES +4, POP_JEUNES +1, CHOMAGE -1
  Journal : Des guichets humains rouvrent. On y
    trouve même des chaises.
Choix 2 : Formation au numérique
  Effets : TECHNOLOGIE +2, DETTE +1, POP_RETRAITES +1, POP_JEUNES +2
  Journal : Les jeunes initient leurs aînés.
    Le mot de passe reste « 1234 ».
Choix 3 : Tout dématérialiser
  Effets : TECHNOLOGIE +2, DETTE -2, POP_RETRAITES -4, POP_JEUNES +1, POP_POPULAIRES -2
  Journal : Tout est en ligne. Les aînés aussi,
    dans la file d'attente.

## EVT_AFFECTATION
Condition : aucune
Probabilité : moyenne
Titre : L'algorithme du bac
Vignette : education
Texte : La plateforme d'affectation Admission+
  publie ses résultats : 60 000
  bacheliers sur liste d'attente, dont
  beaucoup en position 11 482. Les
  lycéens découvrent la patience, et
  les parents la hotline.
Choix 1 : Ouvrir des places
  Effets : SOCIAL +2, DETTE +3, TECHNOLOGIE +1, POP_JEUNES +4, POP_MOYENNES +1
  Journal : Des places s'ouvrent à l'université.
    Il manque encore des amphis.
Choix 2 : Rendre l'algorithme public
  Effets : DEFIANCE -2, SCANDALE -1, POP_JEUNES +2, POP_CADRES -1
  Journal : Le code est publié. Personne ne
    l'a lu, mais c'est rassurant.
Choix 3 : Défendre le système
  Effets : TENSION +3, POP_JEUNES -4, POP_MOYENNES -1, POP_RETRAITES +1
  Journal : « Le système fonctionne. » La liste
    d'attente, aussi.

## EVT_CHOMAGE_JEUNES
Condition : CHOMAGE > 7
Probabilité : moyenne
Titre : Les jeunes sans emploi
Vignette : industrie
Texte : Un jeune actif sur cinq est au
  chômage, et les stages non payés
  s'allongent. Le Syndicat des
  étudiants parle de « génération
  sacrifiée », l'UPE de « génération
  à former ».
Choix 1 : Contrat d'insertion jeunes
  Effets : CHOMAGE -2, DETTE +3, SOCIAL +2, POP_JEUNES +4, POP_RETRAITES -1
  Journal : Contrat d'insertion lancé. Pour une
    fois, le premier emploi est payé.
Choix 2 : Alléger les charges
  Effets : CHOMAGE -1, ECONOMIE +2, DETTE +2, POP_JEUNES +1, POP_CADRES +2
  Journal : Charges allégées pour embaucher les
    jeunes. Les embauches, elles, tardent.
Choix 3 : Compter sur la reprise
  Effets : TENSION +3, POP_JEUNES -5, POP_POPULAIRES -2, POP_RETRAITES +1
  Journal : « La reprise viendra. » Les jeunes
    n'ont pas dit à quelle date.

## EVT_MARCHE_CLIMAT
Condition : ECOLOGIE < 50
Probabilité : moyenne
Titre : La marche pour le climat
Vignette : pollution
Texte : Cent mille jeunes défilent de
  Bastille à République, pancartes en
  carton recyclé : « Vous aurez 40
  degrés, nous aurons des dettes ». Les
  retraités trouvent le slogan
  plutôt bien tourné.
Choix 1 : Recevoir les organisateurs
  Effets : ECOLOGIE +2, TENSION -3, POP_JEUNES +4, POP_RETRAITES -1
  Journal : Les organisateurs sont reçus à
    l'Élysée, gourdes à la main.
Choix 2 : Annoncer un plan climat
  Effets : ECOLOGIE +4, DETTE +3, POP_JEUNES +3, POP_CADRES -1, POP_MOYENNES +1
  Journal : Plan climat annoncé. Les objectifs
    sont pour 2035. Les fonds, eux, non.
Choix 3 : Saluer l'engagement
  Effets : SCANDALE +2, TENSION +2, POP_JEUNES -4, POP_RETRAITES +1
  Journal : Le président « salue l'engagement ».
    Il n'a pas dit lequel.
