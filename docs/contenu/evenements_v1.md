# Événements V1

Suite de evenements_v0.md et v0b, même format strict.
Tendance globale : à peu près neutre.

## EVT_SECHERESSE
Condition : aucune
Probabilité : moyenne
Titre : Sécheresse historique
Texte : Les nappes phréatiques sont à sec
  dès le mois de juin. Des communes
  sont ravitaillées par camion-citerne
  et le préfet interdit d'arroser les
  pelouses. Le maïs, lui, s'est rendu.
Choix 1 : Restrictions d'eau strictes
  Effets : ECOLOGIE +3, TENSION +3, POP_MOYENNES -2, POP_RETRAITES -1, POP_JEUNES +1
  Journal : Eau rationnée : on se lave vite,
    on arrose peu, on râle beaucoup.
Choix 2 : Aide d'urgence aux exploitants
  Effets : ECONOMIE +2, DETTE +2, POP_MOYENNES +2, ECOLOGIE -2
  Journal : Chèque pour les champs. Les nappes
    n'ont pas reçu de virement.
Choix 3 : Lancer un plan de l'eau
  Effets : ECOLOGIE +3, DETTE +2, POP_POPULAIRES +1, POP_JEUNES +1
  Journal : Plan de l'eau : des tuyaux neufs,
    des promesses pour dans dix ans.

## EVT_CLASSEMENT_EDU
Condition : aucune
Probabilité : faible
Titre : Classement international
Texte : Une étude internationale classe les
  élèves français en dessous de la
  moyenne en mathématiques. Le Quotidien
  du Matin parle de « déclassement »,
  les enseignants de « moyens ».
Choix 1 : Plan de revalorisation
  Effets : SOCIAL +3, DETTE +2, POP_POPULAIRES +2, POP_JEUNES +2
  Journal : Les profs seront mieux payés.
    Les élèves, eux, auront cours.
Choix 2 : Évaluations et soutien ciblé
  Effets : TECHNOLOGIE +2, POP_MOYENNES +2, POP_POPULAIRES -2, POP_JEUNES -2
  Journal : Plus d'évaluations pour mieux
    mesurer. Les cartables s'alourdissent.
Choix 3 : Dénoncer un classement biaisé
  Effets : POP_MOYENNES -2, SCANDALE +3, INTERNATIONAL -2, POP_JEUNES -1
  Journal : Le président critique le classement.
    Le thermomètre est accusé de fièvre.

## EVT_EVENEMENT_SPORTIF
Condition : aucune
Probabilité : faible
Titre : Grand événement sportif
Texte : La France accueille une compétition
  internationale. Stades neufs, hôtels
  complets et métros bondés. Les
  commerçants se frottent les mains,
  les riverains se bouchent l'oreille.
Choix 1 : Soigner l'accueil à tout prix
  Effets : ECONOMIE +3, DETTE +3, POP_TOUS +1, INTERNATIONAL +3
  Journal : Fête réussie, addition salée.
    Le chiffre officiel reste à venir.
Choix 2 : Un événement sobre
  Effets : ECOLOGIE +2, DETTE -1, POP_MOYENNES -1, POP_JEUNES +1
  Journal : Des Jeux sans fard : moins de
    paillettes, moins de gaspillage.
Choix 3 : Réserver des places aux écoles
  Effets : SOCIAL +2, POP_POPULAIRES +3, DETTE +1, POP_JEUNES +2
  Journal : Des tribunes pour les écoliers.
    Les VIP se serrent un peu.

## EVT_JUSTICE_MOYENS
Condition : SCANDALE > 25
Probabilité : moyenne
Titre : Magistrats en colère
Texte : Une tribune de mille magistrats
  dénonce le manque de moyens et les
  pressions politiques. Les audiences
  durent des mois, les dossiers
  s'empilent. Les juges rappellent
  qu'ils sont indépendants.
Choix 1 : Recruter des magistrats
  Effets : SECURITE +2, DETTE +2, SCANDALE -4, POP_MOYENNES +1
  Journal : Des juges en plus : la justice
    cherche encore des greffiers.
Choix 2 : Réformer la procédure
  Effets : ECONOMIE +1, SCANDALE +3, POP_POPULAIRES -2
  Journal : La procédure est simplifiée.
    Les avocats la trouvent compliquée.
Choix 3 : Promettre une loi de moyens
  Effets : SCANDALE -2, POP_POPULAIRES +1, POP_CADRES -1
  Journal : Loi de programmation promise. Les
    magistrats attendent le décret.

## EVT_OUTREMER
Condition : aucune
Probabilité : moyenne
Titre : Colère en outre-mer
Texte : Un archipel d'outre-mer proteste
  contre le prix de la vie : un litre
  de lait coûte le double de Paris.
  Barrages sur les routes, rayons
  vides et élus qui montent à Paris.
Choix 1 : Bouclier prix local
  Effets : SOCIAL +2, DETTE +2, TENSION -4, POP_POPULAIRES +2, POP_JEUNES +1
  Journal : Les prix sont encadrés là-bas.
    Les supermarchés négocient leur marge.
Choix 2 : Plan de développement
  Effets : ECONOMIE +2, CHOMAGE -2, DETTE +2
  Journal : Plan de développement : des routes,
    des ports, des promesses durables.
Choix 3 : Une mission d'enquête
  Effets : TENSION +3, SCANDALE +2, POP_CADRES +1
  Journal : Mission d'enquête envoyée. Les
    barrages, eux, restent sur place.

## EVT_INFLATION_ALIM
Condition : aucune
Probabilité : forte
Titre : Le panier s'allège
Texte : Les prix de l'alimentation ont
  bondi de 12 % en un an. Les pâtes
  deviennent un produit de luxe, et
  la grande distribution jure ne pas
  y être pour grand-chose.
Choix 1 : Chèque alimentaire
  Effets : POP_POPULAIRES +3, DETTE +2, SOCIAL +1, POP_RETRAITES +2, POP_JEUNES +1
  Journal : Chèque alimentaire : le caddie
    respire un peu, la dette moins.
Choix 2 : Plafonner les marges
  Effets : POP_POPULAIRES +2, POP_CADRES -2, ECONOMIE -2, TENSION -2, POP_RETRAITES +1
  Journal : Marges plafonnées. La distribution
    découvre le mot « solidarité ».
Choix 3 : Faire confiance aux filières
  Effets : ECONOMIE +2, POP_POPULAIRES -3, TENSION +3, POP_RETRAITES -3, POP_JEUNES -1
  Journal : « La concurrence jouera. » Le
    panier moyen aussi, à la baisse.

## EVT_LAICITE_ECOLE
Condition : aucune
Probabilité : faible
Titre : Laïcité à l'école
Texte : Un incident dans un collège relance
  le débat sur la laïcité. Les
  enseignants demandent des règles
  claires, les familles de l'apaisement.
  Tout le monde invoque la République.
Choix 1 : Circulaire de fermeté
  Effets : SECURITE +2, TENSION +2, POP_MOYENNES +2, POP_POPULAIRES -2, POP_RETRAITES +2, POP_JEUNES -2
  Journal : Circulaire de fermeté : les
    règles sont claires. Les esprits, non.
Choix 2 : Médiation et formation
  Effets : SOCIAL +2, TENSION -3, DETTE +1, POP_MOYENNES -1
  Journal : Médiateurs dans les collèges.
    L'apaisement prend son temps.
Choix 3 : Réunir un comité de sages
  Effets : SCANDALE -1, TENSION -1, POP_MOYENNES +1
  Journal : Un comité est créé. Il rendra son
    avis après la prochaine rentrée.

## EVT_DESINFORMATION
Condition : DEFIANCE > 40
Probabilité : moyenne
Titre : Rumeur virale
Texte : Un faux document circule en ligne :
  il attribue au gouvernement un projet
  de taxe sur les animaux de compagnie.
  Quinze millions de vues avant midi.
  Le démenti en fera dix mille.
Choix 1 : Démentir sans attendre
  Effets : DEFIANCE -4, POP_MOYENNES +1, TENSION -2, POP_RETRAITES +1
  Journal : Démenti officiel dans l'heure. La
    rumeur, elle, a déjà fait le tour.
Choix 2 : Loi sur les plateformes
  Effets : DEFIANCE -2, TECHNOLOGIE -2, POP_CADRES -2, SECURITE +1
  Journal : Les plateformes sommées de réagir.
    Elles promettent un « dialogue ».
Choix 3 : Ignorer, la rumeur passera
  Effets : DEFIANCE +4, SCANDALE +2, POP_RETRAITES -1
  Journal : Le président laisse dire. Les
    internautes, eux, ne se gênent pas.

## EVT_ACCIDENT_INDUSTRIEL
Condition : aucune
Probabilité : faible
Titre : Incendie industriel
Texte : Un entrepôt de produits chimiques
  prend feu près d'un quartier habité.
  Un nuage noir traverse la ville.
  Les riverains veulent des réponses,
  l'exploitant des avocats.
Choix 1 : Enquête et sanctions
  Effets : ECOLOGIE +3, SECURITE +1, ECONOMIE -2, POP_CADRES -2, POP_JEUNES +1
  Journal : L'exploitant est sanctionné. Le
    nuage s'est dissipé, pas la colère.
Choix 2 : Plan de contrôle des sites
  Effets : SECURITE +2, ECOLOGIE +2, DETTE +2
  Journal : Contrôles renforcés. Trop tard
    pour cet entrepôt, mieux que rien.
Choix 3 : Indemniser et rassurer
  Effets : POP_POPULAIRES +2, DETTE +1, SCANDALE +2, ECOLOGIE -2
  Journal : Les riverains sont indemnisés.
    Les causes restent à éclaircir.

## EVT_VOTE_JEUNES
Condition : POPULARITE < 45
Probabilité : moyenne
Titre : La jeunesse se mobilise
Texte : Des milliers de lycéens et d'étudiants
  défilent pour le climat, l'emploi et
  le logement. Slogans soignés,
  banderoles inventives, abstention.
  Les sondeurs sont aux aguets.
Choix 1 : Recevoir leurs représentants
  Effets : POP_TOUS +1, TENSION -3, SCANDALE -1, POP_JEUNES +4
  Journal : Les jeunes à l'Élysée. Les canapés
    n'en reviennent pas.
Choix 2 : Un plan jeunesse
  Effets : SOCIAL +3, CHOMAGE -2, DETTE +3, POP_POPULAIRES +2, POP_JEUNES +5, POP_RETRAITES -1
  Journal : Plan jeunesse : bourses, emploi et
    logement. L'addition viendra après.
Choix 3 : Ne rien céder
  Effets : TENSION +4, POP_MOYENNES +1, POP_POPULAIRES -3, POP_JEUNES -5, POP_RETRAITES +1
  Journal : Pas de concession. Les cortèges
    reviendront, plus nombreux.

## Récapitulatif

| Id | Thème | Condition | Tendance |
|---|---|---|---|
| EVT_SECHERESSE | Écologie | aucune | neutre |
| EVT_CLASSEMENT_EDU | Éducation | aucune | neutre |
| EVT_EVENEMENT_SPORTIF | Sport, économie | aucune | légèrement favorable |
| EVT_JUSTICE_MOYENS | Justice | SCANDALE > 25 | neutre |
| EVT_OUTREMER | Outre-mer | aucune | neutre |
| EVT_INFLATION_ALIM | Pouvoir d'achat | aucune | légèrement défavorable |
| EVT_LAICITE_ECOLE | Laïcité | aucune | neutre |
| EVT_DESINFORMATION | Numérique | DEFIANCE > 40 | légèrement défavorable |
| EVT_ACCIDENT_INDUSTRIEL | Industrie | aucune | neutre |
| EVT_VOTE_JEUNES | Jeunesse | POPULARITE < 45 | neutre |
