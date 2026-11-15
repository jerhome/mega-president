# Événements V0

Format strict ; clés d'effets du contrat §3 uniquement.

## EVT_CANICULE
Condition : aucune
Probabilité : moyenne
Titre : Canicule sur le pays
Texte : Quarante degrés à Lyon, les rivières
  s'assèchent et les maisons de retraite
  manquent de ventilateurs. Chaîne
  Info 24 titre « Il fait chaud ».
  Les agriculteurs réclament de l'aide.
Choix 1 : Plan d'urgence canicule
  Effets : SOCIAL +3, DETTE +2, POP_POPULAIRES +2, POP_RETRAITES +3
  Journal : Canicule : l'État brumise
    La facture fait bouillir Bercy.
Choix 2 : Appeler à la sobriété
  Effets : ECOLOGIE +3, POP_MOYENNES -2, POP_POPULAIRES -2, POP_RETRAITES -2
  Journal : Le président conseille de boire.
    On attendait autre chose.
Choix 3 : Aider surtout les agriculteurs
  Effets : ECONOMIE +2, ECOLOGIE -3, POP_MOYENNES +2
  Journal : Aide aux champs : les nappes se
    demandent qui va payer.

## EVT_GREVE_GENERALE
Condition : TENSION > 70
Probabilité : forte
Titre : Grève générale
Texte : L'UST appelle à la grève générale :
  trains à l'arrêt, écoles fermées,
  raffineries bloquées. La FDS propose
  de négocier, l'UPE de « tenir bon ».
  Les cortèges : nombreux pour les
  syndicats, rares pour la préfecture.
Choix 1 : Ouvrir des négociations
  Effets : SOCIAL +4, TENSION -10, DETTE +2, POP_CADRES -3
  Journal : Le président reçoit les syndicats.
    Le patronat boude le buffet.
Choix 2 : Tenir bon
  Effets : TENSION +5, ECONOMIE +2, POP_POPULAIRES -4, POP_RETRAITES +1
  Journal : Pas de recul : la rue gronde,
    les marchés applaudissent.
Choix 3 : Réquisitionner les raffineries
  Effets : TENSION +3, SECURITE +2, POP_MOYENNES +2, POP_POPULAIRES -3, POP_JEUNES -1
  Journal : Raffineries réquisitionnées : pleins
    faits, esprits pas calmés.

## EVT_CRISE_FINANCIERE
Condition : aucune
Probabilité : faible
Titre : Secousse sur les marchés
Texte : Une banque d'affaires étrangère
  vacille et la Bourse de Paris perd
  6 % en une séance. Les mêmes qui
  réclamaient moins d'État appellent
  l'État à l'aide.
Choix 1 : Garantir les dépôts
  Effets : ECONOMIE +3, DETTE +4, POP_CADRES +2, POP_POPULAIRES -2, POP_RETRAITES +1
  Journal : L'État vole au secours des banques.
    La liberté d'entreprise, version État.
Choix 2 : Laisser le marché se purger
  Effets : ECONOMIE -3, CHOMAGE +3, POP_CADRES -3, POP_RETRAITES -2, POP_JEUNES -2
  Journal : L'État ne bouge pas : la main
    invisible ne rend pas la monnaie.
Choix 3 : Taxer les profits bancaires
  Effets : DETTE -2, POP_POPULAIRES +3, POP_CADRES -4, CROISSANCE -1
  Journal : Taxe sur les banques : elles
    menacent de déménager, comme toujours.

## EVT_SOMMET
Condition : aucune
Probabilité : moyenne
Titre : Sommet européen
Texte : Les chefs d'État se réunissent à
  Bruxelles pour une nuit de négociation
  sur le budget commun. Le café est
  mauvais, les compromis aussi.
  Votre voix compte, mais combien de
  fois pouvez-vous dire « non » ?
Choix 1 : Défendre l'intérêt national
  Effets : POP_MOYENNES +3, ECONOMIE -2, CP -1, INTERNATIONAL -4
  Journal : Le président claque la porte.
    Applaudi à Paris, froid ailleurs.
Choix 2 : Chercher le compromis
  Effets : ECONOMIE +3, POP_MOYENNES -2, CP +1, INTERNATIONAL +4
  Journal : Accord à l'aube : tout le monde a
    perdu un peu, personne ne crie.
Choix 3 : Proposer un plan climat commun
  Effets : ECOLOGIE +3, TECHNOLOGIE +2, DETTE +2, INTERNATIONAL +3
  Journal : Plan climat européen : ambitieux,
    surtout sur le papier.

## EVT_EMEUTES
Condition : TENSION > 50
Probabilité : moyenne
Titre : Nuits d'émeutes
Texte : Après une bavure, des quartiers
  s'embrasent. Voitures brûlées,
  vitrines brisées, appels au calme sur
  Chaîne Info 24. Les LSO exigent l'état
  d'urgence, l'UGR une enquête
  indépendante.
Choix 1 : Déployer les forces de l'ordre
  Effets : SECURITE +3, TENSION -5, POP_POPULAIRES -3, REL_G5 +4, POP_RETRAITES +2, POP_JEUNES -3
  Journal : Calme rétabli à coups de boucliers.
    Les causes sont restées sur place.
Choix 2 : Lancer une enquête indépendante
  Effets : SOCIAL +2, TENSION -3, SECURITE -2, POP_MOYENNES -3, POP_JEUNES +1
  Journal : Enquête indépendante : la justice
    avance, la droite aboie.
Choix 3 : Plan de rattrapage des quartiers
  Effets : SOCIAL +3, DETTE +3, POP_POPULAIRES +2, POP_MOYENNES -2, POP_JEUNES +2, POP_RETRAITES -2
  Journal : Un énième plan pour les quartiers.
    On croise les doigts.

## EVT_USINE_FERMEE
Condition : aucune
Probabilité : forte
Titre : Fermeture de l'usine
Texte : Omnicorp France annonce la fermeture
  de son usine de Montbrison : 800
  emplois supprimés, et un communiqué
  qui évoque une « réorganisation
  agile des actifs ». Les actionnaires
  se disent « peinés ».
Choix 1 : Nationaliser temporairement
  Effets : CHOMAGE -2, DETTE +3, POP_POPULAIRES +3, POP_CADRES -3, POP_JEUNES +1
  Journal : L'État reprend l'usine. Omnicorp,
    elle, garde ses dividendes.
Choix 2 : Négocier un plan social
  Effets : SOCIAL +2, DETTE +1, TENSION -2, POP_CADRES -1
  Journal : Plan social signé : 800 familles
    sans pot de départ.
Choix 3 : Ne pas intervenir
  Effets : CHOMAGE +2, ECONOMIE +2, POP_POPULAIRES -3, TENSION +3, POP_JEUNES -2
  Journal : L'État s'abstient. Omnicorp se dit
    « sensible au dialogue ».

## EVT_DECOUVERTE
Condition : aucune
Probabilité : faible
Titre : Percée scientifique
Texte : Un laboratoire public annonce un
  procédé de stockage d'énergie
  révolutionnaire. Les investisseurs
  accourent, les chercheurs rappellent
  qu'ils sont payés au SMIC de la
  recherche.
Choix 1 : Financer le laboratoire
  Effets : TECHNOLOGIE +4, DETTE +2, POP_MOYENNES +2, POP_JEUNES +2
  Journal : Fonds publics pour la découverte.
    Les chercheurs n'y croient pas.
Choix 2 : Céder le brevet à un industriel
  Effets : ECONOMIE +3, CROISSANCE +1, POP_CADRES +2, POP_POPULAIRES -2
  Journal : Brevet vendu à Omnicorp : la science
    est publique, les profits moins.
Choix 3 : Laisser faire le labo
  Effets : TECHNOLOGIE +2, ECOLOGIE +1
  Journal : Le labo s'en sort seul, comme
    toujours. L'État félicite, ému.

## EVT_POLLUTION
Condition : aucune
Probabilité : moyenne
Titre : Rivière polluée
Texte : Une usine chimique déverse des
  produits toxiques dans un fleuve.
  Poissons morts, eau du robinet
  suspecte, élus locaux en gilet jaune.
  L'exploitant évoque « un incident
  technique ».
Choix 1 : Sanctions et dépollution
  Effets : ECOLOGIE +4, DETTE +1, ECONOMIE -2, POP_CADRES -2, POP_JEUNES +1
  Journal : L'usine paie la dépollution.
    Elle trouve ça peu agile.
Choix 2 : Amende symbolique
  Effets : ECONOMIE +2, ECOLOGIE -3, POP_MOYENNES -3, POP_JEUNES -2
  Journal : Amende minime : le fleuve garde
    le pH de la facture.
Choix 3 : Création d'une agence de contrôle
  Effets : ECOLOGIE +2, DETTE +2, SCANDALE -2
  Journal : Nouvelle agence de contrôle : le
    fleuve a des experts à son chevet.

## EVT_DINER_LOBBY
Condition : aucune
Probabilité : moyenne
Titre : Le dîner qui fait jaser
Texte : Le Quotidien du Matin révèle un
  dîner discret entre un ministre et les
  dirigeants de l'UPE. Au menu : homard
  et assouplissement fiscal. Le ministre
  assure avoir « payé son homard ».
Choix 1 : Soutenir le ministre
  Effets : SCANDALE +5, POP_MOYENNES -2, REL_PARTI +3, POP_JEUNES -2, POP_RETRAITES -1
  Journal : Le président défend son ministre.
    Le homard a bon dos.
Choix 2 : Limoger le ministre
  Effets : SCANDALE -3, CP -1, POP_POPULAIRES +2, POP_CADRES -2, POP_JEUNES +1
  Journal : Ministre limogé : l'UPE y voit un
    excès de zèle, le peuple un geste.
Choix 3 : Promettre une loi sur le lobbying
  Effets : SCANDALE -1, CP -1, POP_TOUS +1
  Journal : Une loi sur les lobbies en projet.
    Les lobbies relisent le texte.

## EVT_COMMUNICATION
Condition : aucune
Probabilité : forte
Titre : Gaffe à la télévision
Texte : Le président est surpris par un
  micro ouvert en disant quelque chose
  qui ne se dit pas. Chaîne Info 24
  passe l'extrait en boucle, avec
  musique dramatique.
Choix 1 : S'excuser publiquement
  Effets : POP_TOUS +1, CP -1, SCANDALE -1
  Journal : Le président s'excuse, tête basse.
    Chaîne Info 24 diffuse quand même.
Choix 2 : Tout nier en bloc
  Effets : SCANDALE +4, POP_MOYENNES -2, POP_POPULAIRES -2, POP_JEUNES -1
  Journal : « Sorti de son contexte » : Internet
    a retrouvé le contexte.
Choix 3 : Retourner l'affaire en blague
  Effets : POP_MOYENNES +2, POP_CADRES -2, SCANDALE +2
  Journal : Le président plaisante de sa gaffe.
    Les communicants prient en silence.

## Récapitulatif

| Id | Thème | Condition |
|---|---|---|
| EVT_CANICULE | Écologie | aucune |
| EVT_GREVE_GENERALE | Social | TENSION > 70 |
| EVT_CRISE_FINANCIERE | Économie | aucune |
| EVT_SOMMET | International | aucune |
| EVT_EMEUTES | Sécurité | TENSION > 50 |
| EVT_USINE_FERMEE | Économie, social | aucune |
| EVT_DECOUVERTE | Technologie | aucune |
| EVT_POLLUTION | Écologie | aucune |
| EVT_DINER_LOBBY | Fait divers politique | aucune |
| EVT_COMMUNICATION | Fait divers politique | aucune |
