# Événements V2

Suite de evenements_v1.md, même format strict.
Tendance globale : neutre.

## EVT_AFFAIRE_SUITE
Condition : SCANDALE > 40
Probabilité : forte
Titre : L'affaire rebondit
Texte : Chaîne Info 24 diffuse de nouvelles
  écoutes : les affaires en cours
  touchent désormais le premier cercle.
  Les députés réclament des comptes,
  le bandeau rouge est en surchauffe.
Choix 1 : S'expliquer devant la presse
  Effets : SCANDALE -3, POP_MOYENNES -1, TENSION +2, POP_TOUS +1
  Journal : Le président s'explique. Les
    journalistes en demandent plus.
Choix 2 : Sacrifier un ministre
  Effets : SCANDALE -4, REL_PARTI -5, CP -1, POP_TOUS +1
  Journal : Un ministre part « pour se
    consacrer à sa famille ».
Choix 3 : Dénoncer un complot
  Effets : SCANDALE +4, DEFIANCE +4, POP_POPULAIRES -2
  Journal : « Un complot ! » Personne n'en
    trouve la preuve. Ni le contraire.

## EVT_SENAT_FRONDEUR
Condition : DEFIANCE > 45
Probabilité : moyenne
Titre : Le Sénat fait front
Texte : Le Sénat rejette trois textes en une
  semaine et convoque le gouvernement
  pour une audition fleuve. Le
  président du Palais du Luxembourg
  parle de « contrôle républicain ».
Choix 1 : Négocier avec les sénateurs
  Effets : DEFIANCE -5, REL_G4 +3, REL_G1 -2, CP -1, POP_TOUS +1
  Journal : Le président reçoit les sénateurs.
    Les petits fours calment les esprits.
Choix 2 : Passer outre
  Effets : DEFIANCE +5, SCANDALE +2, CP +1, POP_TOUS +1
  Journal : Le Sénat contourné. Il note, il
    s'en souviendra.
Choix 3 : Annoncer une réforme du Sénat
  Effets : DEFIANCE +1, TENSION +2, POP_POPULAIRES +2
  Journal : Réformer le Sénat : promesse qui
    revient comme les sénatoriales.

## EVT_COUR_COMPTES
Condition : DETTE > 110
Probabilité : moyenne
Titre : Rapport de la Cour des comptes
Texte : La Cour des comptes publie un rapport
  sévère : dérapage de la dette,
  dépenses mal évaluées, niches
  fiscales à la dérive. Les magistrats
  demandent « un effort inédit ».
Choix 1 : Plan d'économies
  Effets : DETTE -3, SOCIAL -3, POP_POPULAIRES -3, TENSION +3, POP_TOUS +1
  Journal : Plan d'économies annoncé. Les
    ministères ressortent les ciseaux.
Choix 2 : Taxer les niches fiscales
  Effets : DETTE -2, ECONOMIE -2, POP_CADRES -3, POP_POPULAIRES +1, POP_TOUS +1
  Journal : Les niches fiscales rabotées. Les
    lobbies sortent leurs avocats.
Choix 3 : Contester le rapport
  Effets : DEFIANCE +3, SCANDALE +2, POP_MOYENNES -1
  Journal : Le gouvernement conteste les
    chiffres. Les chiffres tiennent bon.

## EVT_AGENCE_NOTATION
Condition : CROISSANCE < 1
Probabilité : moyenne
Titre : Dégradation de la note
Texte : Une agence de notation abaisse la
  note de la France d'un cran. Les
  taux d'emprunt montent, les
  éditorialistes aussi. L'agence
  précise qu'elle « n'a rien de
  personnel ».
Choix 1 : Rassurer les marchés
  Effets : ECONOMIE +2, DETTE -1, POP_CADRES +2, POP_POPULAIRES -2, POP_TOUS +1, POP_RETRAITES +1
  Journal : Le président rassure les marchés.
    Les marchés prennent des notes.
Choix 2 : Dénoncer les agences
  Effets : POP_POPULAIRES +3, ECONOMIE -3, DETTE +2, POP_TOUS +1, INTERNATIONAL -2
  Journal : Haro sur les agences : la
    dette, elle, ne s'en émeut pas.
Choix 3 : Un plan de relance ciblé
  Effets : CROISSANCE +2, DETTE +3, CHOMAGE -1
  Journal : Relance ciblée pour regagner la
    confiance. Les taux restent perplexes.

## EVT_SOMMET_CLIMAT
Condition : aucune
Probabilité : moyenne
Titre : Sommet sur le climat
Texte : Le sommet mondial sur le climat
  s'ouvre à Lyon. Cent quatre-vingt
  délégations, des discours émus et
  des avions privés. Noémie Vasseur
  est venue à vélo cargo, forcément.
Choix 1 : Porter un accord ambitieux
  Effets : ECOLOGIE +4, ECONOMIE -2, POP_CADRES -2, POP_MOYENNES +1, POP_TOUS +1, INTERNATIONAL +4
  Journal : Accord ambitieux porté par Paris.
    Les industriels demandent un délai.
Choix 2 : Un accord a minima
  Effets : ECOLOGIE +1, POP_TOUS +1, CP +1, INTERNATIONAL +1, POP_JEUNES +1
  Journal : Accord a minima signé. Chacun
    repart avec sa conclusion.
Choix 3 : Défendre l'industrie d'abord
  Effets : ECONOMIE +3, ECOLOGIE -3, TENSION +2, POP_CADRES +1, INTERNATIONAL -3, POP_JEUNES -3
  Journal : La France défend ses usines. Le
    climat verra plus tard.

## EVT_NOBEL
Condition : aucune
Probabilité : faible
Titre : Prix Nobel français
Texte : Un chercheur français reçoit le prix
  Nobel de physique. Il remercie son
  équipe, son laboratoire et ses
  crédits « enfin renouvelés ». Les
  ministres se bousculent sur la photo.
Choix 1 : Annoncer un plan recherche
  Effets : TECHNOLOGIE +4, DETTE +2, POP_MOYENNES +2, POP_TOUS +1, INTERNATIONAL +2, POP_JEUNES +1
  Journal : Plan recherche annoncé. Le Nobel
    sourit, les labos comptent.
Choix 2 : Fêter sans dépenser
  Effets : POP_TOUS +1, TECHNOLOGIE +1, INTERNATIONAL +1
  Journal : Dîner à l'Élysée, discours ému.
    Les crédits attendront.
Choix 3 : Rappeler les coupes passées
  Effets : TECHNOLOGIE +1, SCANDALE +2, POP_MOYENNES -1
  Journal : Le chercheur rappelle les coupes.
    La photo officielle est moins nette.

## EVT_URGENCES_PEDIATRIQUES
Condition : aucune
Probabilité : moyenne
Titre : Urgences pédiatriques
Texte : Une bronchiolite sature les
  urgences pédiatriques. Des enfants
  sont transférés à 200 kilomètres.
  Les soignants lancent l'alerte,
  les parents, eux, l'ont déjà donnée.
Choix 1 : Plan d'urgence pour l'enfance
  Effets : SOCIAL +3, DETTE +2, POP_POPULAIRES +2, TENSION -2, POP_TOUS +1, POP_JEUNES +1
  Journal : Lits et postes en pédiatrie. Les
    parents respirent, Bercy tousse.
Choix 2 : Réquisitionner des cliniques
  Effets : SOCIAL +2, ECONOMIE -2, POP_MOYENNES +1, POP_CADRES -2, POP_TOUS +1
  Journal : Les cliniques privées réquisitionnées.
    Elles sortent le stylo rouge.
Choix 3 : Miser sur la télémédecine
  Effets : TECHNOLOGIE +2, SOCIAL -2, POP_POPULAIRES -3, TENSION +3, POP_RETRAITES -2
  Journal : Un écran pour les bronchiolites.
    Le thermomètre reste manuel.

## EVT_CLASSES_RURALES
Condition : aucune
Probabilité : moyenne
Titre : Fermeture de classes rurales
Texte : Quarante classes rurales vont fermer
  à la rentrée. Les maires nouent
  leur écharpe aux grilles des écoles,
  les parents occupent les cours de
  récréation. Les vaches regardent.
Choix 1 : Geler les fermetures
  Effets : SOCIAL +2, DETTE +2, POP_POPULAIRES +2, TENSION -3, POP_TOUS +1, POP_RETRAITES +2
  Journal : Fermetures gelées. Les écharpes
    des maires reprennent leur cou.
Choix 2 : Regrouper les écoles
  Effets : ECONOMIE +1, DETTE -1, POP_POPULAIRES -3, TENSION +3, POP_TOUS +1, POP_RETRAITES -2
  Journal : Écoles regroupées, cars scolaires
    en plus. Le village se vide un peu.
Choix 3 : Un plan pour les territoires
  Effets : SOCIAL +1, TECHNOLOGIE +1, DETTE +1, POP_MOYENNES +1
  Journal : Plan ruralité : de la fibre et des
    promesses. L'école viendra après.

## EVT_FESTIVAL
Condition : aucune
Probabilité : faible
Titre : Festival menacé
Texte : Le grand festival d'été d'Avignon-les-
  Pins perd la moitié de sa subvention
  locale. Les intermittents menacent
  de jouer devant les préfectures.
  Le théâtre vit, mais sous perfusion.
Choix 1 : Sauver le festival
  Effets : SOCIAL +1, DETTE +1, POP_MOYENNES +2, POP_CADRES -1, POP_TOUS +1, POP_JEUNES +3
  Journal : Festival sauvé. Le public applaudit,
    la Cour des comptes soupire.
Choix 2 : Le laisser trouver des mécènes
  Effets : ECONOMIE +1, POP_CADRES +1, POP_MOYENNES -2, TENSION +2, POP_TOUS +1, POP_JEUNES -3
  Journal : Le festival cherche un mécène.
    Omnicorp propose un logo géant.
Choix 3 : Lancer un fonds pour la culture
  Effets : SOCIAL +2, DETTE +2, POP_POPULAIRES +1, POP_MOYENNES +1
  Journal : Fonds pour la culture créé. Les
    guichets ouvrent, les salles aussi.

## EVT_LYCEENS
Condition : aucune
Probabilité : moyenne
Titre : Mouvement lycéen
Texte : Les lycéens bloquent leurs
  établissements contre une réforme du
  baccalauréat. Poubelles en barricade,
  slogans au feutre. Les parents
  hésitent entre fierté et inquiétude.
Choix 1 : Retirer la réforme
  Effets : TENSION -5, POP_POPULAIRES +2, ECONOMIE -1, REL_PARTI -2, POP_TOUS +1, POP_JEUNES +4
  Journal : Réforme retirée. Les poubelles
    retrouvent leur trottoir.
Choix 2 : Ouvrir une concertation
  Effets : TENSION -2, SOCIAL +1, CP -1, POP_TOUS +1, POP_JEUNES +2
  Journal : Concertation lancée. Les lycéens
    exigent un ordre du jour clair.
Choix 3 : Faire évacuer les lycées
  Effets : SECURITE +1, TENSION +5, SCANDALE +2, POP_MOYENNES +1, POP_JEUNES -4, POP_RETRAITES +2
  Journal : Lycées évacués. Les images tournent
    en boucle, les cortèges grossissent.

## EVT_GREVE_AIGUILLEURS
Condition : aucune
Probabilité : moyenne
Titre : Contrôleurs aériens en grève
Texte : Les contrôleurs aériens cessent le
  travail un vendredi de départs en
  vacances. Les aéroports ressemblent
  à des gares de campagne. Les valises
  tournent, les avions restent au sol.
Choix 1 : Négocier les effectifs
  Effets : SOCIAL +2, DETTE +2, TENSION -3, POP_CADRES -1, POP_TOUS +1
  Journal : Contrôleurs recrutés. Les avions
    reprennent l'air, Bercy ses calculs.
Choix 2 : Réquisitionner les contrôleurs
  Effets : SECURITE +1, ECONOMIE +2, TENSION +5, POP_POPULAIRES -2, POP_TOUS +1, POP_RETRAITES +1
  Journal : Contrôleurs réquisitionnés. Ils
    contrôlent, mais pas de bon cœur.
Choix 3 : Laisser pourrir le conflit
  Effets : ECONOMIE -3, TENSION +3, POP_MOYENNES -2, SCANDALE +1
  Journal : Le conflit s'enlise. Les touristes
    visitent Roissy en détail.

## EVT_REFORME_INSTITUTIONS
Condition : POPULARITE < 40
Probabilité : moyenne
Titre : Pour une VIe République
Texte : Des cortèges réclament une VIe
  République : référendum d'initiative
  citoyenne, fin du 49.3, mandat non
  renouvelable. Camille Rouvière parle
  de « Constitution à rénover ».
Choix 1 : Convoquer une convention
  Effets : DEFIANCE -4, TENSION -3, CP -2, POP_POPULAIRES +2, POP_TOUS +1, POP_JEUNES +2
  Journal : Convention citoyenne annoncée.
    Cent cinquante tirés au sort.
Choix 2 : Réformer à la marge
  Effets : DEFIANCE -1, POP_MOYENNES +1, REL_G3 +2, POP_TOUS +1
  Journal : Quelques retouches à la Constitution.
    La rue est tenue au courant.
Choix 3 : Refuser tout changement
  Effets : DEFIANCE +4, TENSION +4, SCANDALE +2, REL_PARTI +2
  Journal : « La Constitution tient. » Les
    cortèges, eux aussi.

## EVT_ESPIONNAGE
Condition : SCANDALE > 25
Probabilité : moyenne
Titre : Scandale d'espionnage
Texte : Des journalistes révèlent qu'un
  service de renseignement a surveillé
  des opposants et des reporters.
  Le ministre jure ignorer tout. Les
  micros, eux, n'ont rien dit.
Choix 1 : Ouvrir une enquête
  Effets : SCANDALE -3, SECURITE -2, DEFIANCE -2, POP_MOYENNES +1, POP_TOUS +1, INTERNATIONAL +1
  Journal : Enquête ouverte sur les services.
    Les dossiers sont dans les tiroirs.
Choix 2 : Invoquer le secret défense
  Effets : SCANDALE +4, DEFIANCE +4, SECURITE +1, POP_MOYENNES -2, POP_TOUS +1, INTERNATIONAL -3
  Journal : Secret défense invoqué. Il est
    bien gardé, y compris des juges.
Choix 3 : Réformer le renseignement
  Effets : SECURITE +1, SCANDALE +1, DEFIANCE -3, CP -1
  Journal : Contrôle des services renforcé.
    Les micros seront déclarés.

## EVT_PANNE_ELECTRIQUE
Condition : aucune
Probabilité : faible
Titre : Panne géante d'électricité
Texte : Un incident sur le réseau plonge le
  quart du pays dans le noir. Métros
  bloqués, feux éteints, ascenseurs
  habités. Au bout de six heures,
  les bougies sont en rupture de stock.
Choix 1 : Plan de sécurisation du réseau
  Effets : TECHNOLOGIE +2, SECURITE +2, DETTE +3, POP_MOYENNES +1, POP_TOUS +1, POP_RETRAITES +2
  Journal : Réseau renforcé : des lignes neuves.
    La lumière a un prix.
Choix 2 : Blâmer l'opérateur
  Effets : ECONOMIE -2, POP_POPULAIRES +1, SCANDALE +1, TENSION -1, POP_TOUS +1
  Journal : L'opérateur est sermonné. Il
    promet de rallumer, c'est déjà ça.
Choix 3 : Miser sur la sobriété
  Effets : ECOLOGIE +3, TENSION +2, POP_MOYENNES -2, CROISSANCE -1, POP_RETRAITES -3, POP_JEUNES +1
  Journal : Sobriété prônée. Les citoyens
    éteignent, le président sourit.

## EVT_MAREE_NOIRE
Condition : aucune
Probabilité : faible
Titre : Marée noire
Texte : Un pétrolier s'échoue au large de la
  Bretagne. Les plages se couvrent de
  mazout, les bénévoles de gants jaunes.
  L'armateur, basé dans un paradis
  fiscal, promet de « suivre ça ».
Choix 1 : Faire payer l'armateur
  Effets : ECOLOGIE +3, ECONOMIE -1, POP_POPULAIRES +2, POP_CADRES -1, POP_TOUS +1, POP_JEUNES +2
  Journal : L'armateur est sommé de payer. Ses
    avocats ont déjà une réponse.
Choix 2 : Financer le nettoyage
  Effets : ECOLOGIE +2, DETTE +3, POP_MOYENNES +1, TENSION -2, POP_TOUS +1
  Journal : L'État finance le nettoyage. Les
    plages sont nettes, les caisses moins.
Choix 3 : Minimiser l'accident
  Effets : ECOLOGIE -3, SCANDALE +3, POP_MOYENNES -3, ECONOMIE +1, POP_JEUNES -3
  Journal : « Rien de grave. » Les mouettes
    ne sont pas de cet avis.
