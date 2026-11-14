# Événements V0b

Suite de evenements_v0.md, même format strict.
Tendance globale : légèrement défavorable au joueur.

## EVT_EPIDEMIE
Condition : aucune
Probabilité : moyenne
Titre : Épidémie de grippe
Texte : Les urgences débordent, les brancards
  s'alignent dans les couloirs. Le
  ministère recommande « de ne pas
  tomber malade avant mars ». Les
  soignants, eux, comptent leurs
  heures supplémentaires.
Choix 1 : Plan blanc et recrutements
  Effets : SOCIAL +3, DETTE +3, POP_POPULAIRES +2, ECONOMIE -1
  Journal : Plan blanc déclenché : l'hôpital
    respire, les comptes toussent.
Choix 2 : Campagne de vaccination
  Effets : SOCIAL +1, DETTE +1, POP_MOYENNES -1, CP -1
  Journal : Tous vaccinés ! Reste à trouver
    des infirmières pour piquer.
Choix 3 : Compter sur le système
  Effets : SOCIAL -3, POP_POPULAIRES -4, TENSION +3
  Journal : « Le système tient. » Les
    brancards, moins.

## EVT_GREVE_ENSEIGNANTS
Condition : aucune
Probabilité : moyenne
Titre : Grève des enseignants
Texte : Classes surchargées, postes vacants,
  salaires gelés : l'UST et la FDS
  appellent à fermer les écoles. Les
  parents jonglent, les élèves
  applaudissent discrètement.
Choix 1 : Revaloriser les salaires
  Effets : SOCIAL +3, DETTE +3, TENSION -4, POP_AISEES -2
  Journal : Les profs augmentés. Bercy cherche
    la craie de rechange.
Choix 2 : Promettre des postes plus tard
  Effets : TENSION +3, CP -1, POP_MOYENNES -2
  Journal : Des postes « à la rentrée »,
    promis pour la 4e fois.
Choix 3 : Imposer un service minimum
  Effets : TENSION +5, SECURITE +1, POP_MOYENNES +2, SOCIAL -2
  Journal : Service minimum : l'école garde
    la porte, pas les élèves.

## EVT_CRISE_LAIT
Condition : aucune
Probabilité : moyenne
Titre : Crise du lait
Texte : Les éleveurs déversent leur lait
  devant les préfectures : le prix
  payé ne couvre plus les coûts. La
  grande distribution jure qu'elle
  « soutient les territoires ».
Choix 1 : Prix minimum garanti
  Effets : ECONOMIE -2, DETTE +2, POP_MOYENNES +3, POP_POPULAIRES -2
  Journal : Prix du lait garanti : le yaourt
    prend un euro, le président un point.
Choix 2 : Aide d'urgence aux éleveurs
  Effets : DETTE +3, TENSION -3, ECOLOGIE -1
  Journal : Chèque aux éleveurs : le lait
    est tiré, la facture aussi.
Choix 3 : Laisser jouer le marché
  Effets : ECONOMIE +1, TENSION +4, POP_MOYENNES -4, ECOLOGIE -1
  Journal : Les éleveurs sont seuls face au
    marché. Les vaches aussi.

## EVT_CYBERATTAQUE
Condition : TECHNOLOGIE < 40
Probabilité : moyenne
Titre : Cyberattaque contre l'État
Texte : Des pirates paralysent les serveurs
  d'hôpitaux et de mairies. Les
  dossiers sont chiffrés, la rançon
  réclamée en monnaie virtuelle. Le mot
  de passe d'un ministère était 1234.
Choix 1 : Investir dans la cybersécurité
  Effets : TECHNOLOGIE +3, SECURITE +2, DETTE +3, POP_MOYENNES -1
  Journal : Budget cyber en hausse : le mot de
    passe 1234 est enfin interdit.
Choix 2 : Refuser de payer la rançon
  Effets : SCANDALE +2, SOCIAL -2, POP_POPULAIRES -3
  Journal : Pas de rançon : les services
    attendront, la fierté aussi.
Choix 3 : Payer discrètement
  Effets : SCANDALE +5, DETTE +1, POP_AISEES -1, CP -1
  Journal : Rançon payée en cachette. Chaîne
    Info 24 l'a su avant le trésorier.

## EVT_GEANT_NUMERIQUE
Condition : aucune
Probabilité : faible
Titre : Le géant et ses impôts
Texte : Le géant du numérique Omnicorp
  Cloud paie moins d'impôts qu'une
  boulangerie de quartier. Il menace de
  transférer ses serveurs et ses 2 000
  emplois si l'on touche à son régime.
Choix 1 : Taxer les géants du web
  Effets : DETTE -3, POP_POPULAIRES +3, TECHNOLOGIE -2, CROISSANCE -1
  Journal : Taxe sur le numérique : Omnicorp
    Cloud découvre le mot « impôt ».
Choix 2 : Négocier un accord
  Effets : ECONOMIE +1, SCANDALE +2, POP_POPULAIRES -3, CP -1
  Journal : Accord discret avec le géant : on
    ne dira pas le montant.
Choix 3 : Ne rien changer
  Effets : TECHNOLOGIE +2, POP_AISEES +2, POP_POPULAIRES -4, SCANDALE +2
  Journal : Le géant reste, et ses impôts
    restent à l'étranger.

## EVT_LOGEMENT
Condition : aucune
Probabilité : forte
Titre : Crise du logement
Texte : Loyers en hausse, files de cent
  candidats pour dix mètres carrés,
  tentes sur les trottoirs. Les
  promoteurs promettent de construire
  « dès que ce sera rentable ».
Choix 1 : Encadrer les loyers
  Effets : SOCIAL +3, ECONOMIE -2, POP_POPULAIRES +3, POP_AISEES -3
  Journal : Loyers encadrés : les bailleurs
    découvrent l'inflation des autres.
Choix 2 : Plan de construction massif
  Effets : SOCIAL +2, DETTE +4, CHOMAGE -1, ECOLOGIE -2
  Journal : 300 000 logements promis. Les
    grues, elles, demandent un permis.
Choix 3 : Défiscaliser l'investissement
  Effets : ECONOMIE +2, SOCIAL -3, POP_AISEES +2, POP_POPULAIRES -4
  Journal : Avantage fiscal aux bailleurs : le
    loyer, lui, ne baisse pas.

## EVT_GREVE_RAIL
Condition : aucune
Probabilité : moyenne
Titre : Grève à la CFN
Texte : La Compagnie Ferroviaire Nationale
  est à l'arrêt : un train sur trois
  seulement. La direction parle de
  « réforme nécessaire », les
  cheminots de « réforme nécessaire...
  à Bercy ».
Choix 1 : Céder sur les salaires
  Effets : SOCIAL +2, DETTE +3, TENSION -4, ECONOMIE -1
  Journal : La CFN repart : les billets aussi,
    en hausse.
Choix 2 : Maintenir la réforme
  Effets : ECONOMIE +2, TENSION +5, POP_POPULAIRES -3
  Journal : Réforme maintenue : les quais
    sont pleins, les trains moins.
Choix 3 : Réquisitionner les cheminots
  Effets : SECURITE +1, TENSION +6, POP_POPULAIRES -3, CP -1
  Journal : Cheminots réquisitionnés : les
    trains partent, la colère reste.

## EVT_AFFAIRE_JUDICIAIRE
Condition : SCANDALE > 40
Probabilité : forte
Titre : Un ministre mis en examen
Texte : Un ministre est mis en examen pour
  prise illégale d'intérêts. Il crie à
  l'acharnement, le parquet à
  l'indépendance. Le Quotidien du Matin
  publie les SMS. Tous.
Choix 1 : Exiger sa démission
  Effets : SCANDALE -4, CP -2, POP_POPULAIRES +2, REL_PARTI -2
  Journal : Le ministre démissionne. Il part
    en citant la présomption d'innocence.
Choix 2 : Le maintenir au gouvernement
  Effets : SCANDALE +6, POP_TOUS -2, REL_PARTI +2
  Journal : Ministre maintenu : la présomption
    d'innocence, version gouvernement.
Choix 3 : Réformer le statut des ministres
  Effets : SCANDALE -2, CP -2, DEFIANCE +2, POP_MOYENNES +1
  Journal : Un statut des ministres en vue.
    Certains le relisent à l'avance.

## EVT_CRISE_DIPLO
Condition : aucune
Probabilité : moyenne
Titre : Crise avec un voisin
Texte : Un pays voisin saisit nos chalutiers
  et ferme un tunnel frontalier. Les
  ambassadeurs sont convoqués, les
  drapeaux brûlés, les touristes
  rapatriés. La LSO réclame des
  représailles.
Choix 1 : Répondre fermement
  Effets : SECURITE +2, ECONOMIE -2, POP_MOYENNES +2, REL_G5 +3, INTERNATIONAL -3
  Journal : Fermeté face au voisin : les
    chalutiers, eux, restent à quai.
Choix 2 : Négocier discrètement
  Effets : ECONOMIE +1, CP -1, POP_MOYENNES -3, REL_G5 -3, INTERNATIONAL +3
  Journal : Négociations secrètes : tout se
    sait, surtout les concessions.
Choix 3 : Saisir l'Union européenne
  Effets : CP -1, POP_MOYENNES -1, ECOLOGIE +1, DEFIANCE +1
  Journal : Bruxelles est saisi. Réponse
    attendue d'ici la prochaine crise.

## EVT_PRIX_ELECTRICITE
Condition : TENSION > 40
Probabilité : moyenne
Titre : Flambée de l'électricité
Texte : Le prix de gros de l'électricité
  triple en un hiver. Les factures
  explosent, les boulangers éteignent
  le four. Le fournisseur public
  parle de « tension sur le réseau ».
Choix 1 : Bouclier tarifaire
  Effets : POP_POPULAIRES +3, POP_MOYENNES +2, DETTE +4, ECOLOGIE -1
  Journal : Bouclier sur l'électricité : la
    facture est gelée, la dette chauffe.
Choix 2 : Aide ciblée aux ménages
  Effets : SOCIAL +2, DETTE +2, POP_MOYENNES -2, POP_AISEES -1
  Journal : Chèque énergie ciblé : pour les
    pauvres, un joli courrier.
Choix 3 : Laisser jouer le marché
  Effets : ECONOMIE +1, ECOLOGIE +1, TENSION +4, POP_POPULAIRES -5
  Journal : Factures libres : les ménages
    s'éclairent à la bougie.

## Récapitulatif

| Id | Thème | Condition | Tendance |
|---|---|---|---|
| EVT_EPIDEMIE | Santé | aucune | défavorable |
| EVT_GREVE_ENSEIGNANTS | Éducation | aucune | défavorable |
| EVT_CRISE_LAIT | Agriculture | aucune | défavorable |
| EVT_CYBERATTAQUE | Numérique | TECHNOLOGIE < 40 | défavorable |
| EVT_GEANT_NUMERIQUE | Numérique, fiscalité | aucune | légèrement favorable |
| EVT_LOGEMENT | Logement | aucune | défavorable |
| EVT_GREVE_RAIL | Transports | aucune | défavorable |
| EVT_AFFAIRE_JUDICIAIRE | Scandale | SCANDALE > 40 | défavorable |
| EVT_CRISE_DIPLO | International | aucune | défavorable |
| EVT_PRIX_ELECTRICITE | Énergie | DETTE > 100 | défavorable |
