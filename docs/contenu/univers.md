# Univers fictif de Mega Président

> Tout est fictif : partis, personnes, entreprises, médias, syndicats.
> Toute ressemblance avec des acteurs réels serait fortuite.

## 1. Groupes parlementaires

| Id | Nom complet | Sigle | Description |
|---|---|---|---|
| G1 | Union des Gauches Radicales | UGR | Rupture avec le marché, SMIC et services publics d'abord. |
| G2 | Rassemblement Social et Vert | RSV | Sociaux-démocrates et écologistes, réformistes mais têtus. |
| G3 | Mouvement du Juste Milieu | MJM | Le centre : ni trop, ni pas assez, et surtout pas de vagues. |
| G4 | Union pour la Tradition Libérale | UTL | Droite d'ordre et d'entreprise : moins d'impôts, plus de képis. |
| G5 | Ligue Souveraine | LSO | Droite nationaliste : frontières, autorité et colère. |

## 2. Parti du joueur

Le parti du joueur est le groupe correspondant à son orientation :

| Orientation | Groupe | Parti du président |
|---|---|---|
| Gauche | G2 | Rassemblement Social et Vert (RSV) |
| Centre | G3 | Mouvement du Juste Milieu (MJM) |
| Droite | G4 | Union pour la Tradition Libérale (UTL) |

## 3. Candidats à la présidentielle

Les 4 adversaires du joueur au premier tour. Le parti du joueur
n'a pas de candidat : c'est le joueur lui-même.

| Identifiant | Candidat | Parti | Portrait |
|---|---|---|---|
| CAND_GAUCHE_RADICALE | Camille Rouvière | UGR | Ancienne inspectrice du travail, voix rauque, programme en 140 mesures dont 139 sont populaires. |
| CAND_ECOLO | Noémie Vasseur | Génération Climat | Climatologue devenue élue, prêche la sobriété depuis son vélo cargo. |
| CAND_DROITE | Édouard Valmont | UTL | Notable de province, ancien ministre du Budget, sourire de banquier. |
| CAND_NATIONALISTE | Gaspard Lenoir | LSO | Tribun de plateaux télé, promet de tout régler en un quinquennat et un slogan. |
| CAND_CENTRE (réserve) | Hélène Marchetti | MJM | Maire d'une ville moyenne, championne du compromis, n'a jamais fini une phrase par un point d'exclamation. |

Choix des 4 adversaires selon l'orientation du joueur (le parti du
joueur n'a jamais de candidat contre lui) :

| Orientation du joueur | Adversaires |
|---|---|
| Gauche (RSV) | Rouvière, Vasseur, Valmont, Lenoir |
| Centre (MJM) | Rouvière, Vasseur, Valmont, Lenoir |
| Droite (UTL) | Rouvière, Vasseur, **Marchetti** (remplace Valmont), Lenoir |

Génération Climat n'a pas de groupe à l'Assemblée.

## 4. Noms récurrents pour le journal

| Rôle | Nom | Note |
|---|---|---|
| Média 1 | Le Quotidien du Matin | Presse papier, sérieux et un peu pompeux. |
| Média 2 | Chaîne Info 24 | Info en continu, bandeau rouge permanent. |
| Syndicat 1 | Union Syndicale des Travailleurs (UST) | Gros syndicat, défilés et sandwichs merguez. |
| Syndicat 2 | Fédération Démocratique des Salariés (FDS) | Syndicat réformiste, négocie et signe. |
| Organisation patronale | Union Patronale des Entreprises (UPE) | Parle d'une seule voix : « baissez nos charges ». |
| Grande entreprise | Omnicorp France | Multinationale, siège à La Défense, valeurs « agiles ». |

## 5. Casting des gueules (V3)

> Fiches pour le dessin de caricatures (pixel art, esprit presse et
> BD franco-belge). Tout est fictif : aucun trait ne doit rappeler une
> personne réelle. Les traits sont volontairement exagérés. Les fiches
> des 14 personnages existants reprennent le dessin V2 (voir
> `tools/art/portraits.py`) en l'accentuant.
> Légende : **Caractère** en une phrase, **Traits** (silhouette et
> signes distinctifs), **Expr.** expressions typiques.

### 5.1 Les trois présidents (avatar du joueur)

**PRES_GAUCHE** : Antoine Verdier. Président, RSV.
Caractère : chaleureux, têtu, promet beaucoup et tient la moitié.
Traits :
- Tête ovale et large, chevelure brune bouclée en nuage.
- Barbe courte poivre et sel, sourcils épais en broussaille.
- Col de chemise ouvert, pas de cravate, manches retroussées.
- Carrure d'ours, épaules tombantes, mains toujours en mouvement.
Expr. : neutre (regard franc) ; ravi (grand sourire, yeux plissés) ; furieux (poing levé, sourcils en V).

**PRES_CENTRE** : Laurent Delmas. Président, MJM.
Caractère : lisse, avenant, évite toute aspérité, y compris les siennes.
Traits :
- Visage en rectangle étroit, front haut, raie nette sur le côté.
- Sourire de publicité, dents très blanches et très régulières.
- Cravate bleue impeccable, costume ajusté, silhouette mince.
- Sourcils fins, symétriques, jamais froncés.
Expr. : neutre (sourire poli) ; ravi (sourire à 32 dents) ; furieux (sourire figé, œil qui tressaute).

**PRES_DROITE** : Henri de Saint-Aubin. Président, UTL.
Caractère : autoritaire, courtois, sait ce qui est bon pour les autres.
Traits :
- Mâchoire carrée, menton fendu, cou de taureau.
- Cheveux gris plaqués en arrière, tempes argentées.
- Sourcils gris horizontaux, regard de commandant.
- Cravate rouge, costume sombre, port de tête très droit.
Expr. : neutre (regard froid) ; ravi (coin de lèvre qui monte) ; furieux (mâchoire serrée, veine au front).

### 5.2 Les Premiers ministres

**PM_TECHNOCRATE** : Pierre Delaunay. Premier ministre technocrate.
Caractère : parle en tableaux, croit que tout se règle en annexe.
Traits :
- Crâne dégarni et luisant, quelques mèches rabattues.
- Lunettes rectangulaires fines, regard de comptable.
- Nez long et pointu, bouche en trait, lèvres fines.
- Costume gris souris, dossier sous le bras, dos voûté.
Expr. : neutre (impassible) ; ravi (un millimètre de sourire) ; furieux (lunettes embuées, sourcil qui frémit).

**PM_RASSEMBLEUR** : Marianne Deschamps. Première ministre rassembleuse.
Caractère : écoute tout le monde, conclut rarement, adorée des couloirs.
Traits :
- Carré blond volumineux, effet casque de cheveux.
- Visage rond, joues roses, sourire large et maternel.
- Veste rouge vif, gros collier de perles de bois.
- Corps rond, bras ouverts comme pour une accolade.
Expr. : neutre (sourire doux) ; ravi (rire, yeux fermés) ; furieux (lèvres pincées, mains sur les hanches).

**PM_REFORMATEUR** : Julien Lestrade. Premier ministre réformateur.
Caractère : jeune, pressé, a déjà une loi pour demain matin.
Traits :
- Tête fine et anguleuse, banane gominée en vague haute.
- Grands yeux cernés, sourcils très mobiles, dents de lapin.
- Chemise blanche sans cravate, manches roulées, goutte de sueur.
- Silhouette maigre penchée en avant, toujours en marche.
Expr. : neutre (regard braqué sur l'horloge) ; ravi (sourire de gamin) ; furieux (cheveux qui se défont, bras en l'air).

### 5.3 Les Premiers ministres de cohabitation

**COHAB_GAUCHE** : Sophie Carrel. PM d'opposition de gauche, UGR.
Caractère : ex-syndicaliste, voix de stentor, négocie en tapant du poing.
Traits :
- Cheveux courts poivre et sel, en pétard.
- Visage carré, rides d'expression profondes, regard direct.
- Foulard rouge noué au cou, veste en toile.
- Poings sur la table, épaules larges, tête rentrée.
Expr. : neutre (mâchoire ferme) ; ravi (rire franc) ; furieux (bouche ouverte, index pointé).

**COHAB_CENTRE** : Bernard Lacombe. PM d'opposition du centre, MJM.
Caractère : gestionnaire prudent, ne signe qu'en trois exemplaires.
Traits :
- Crâne chauve à couronne de cheveux gris, bajoues.
- Moustache grise touffue, sourcils tombants et inquiets.
- Petit corps rond, gilet de laine sous le veston.
- Mains jointes sur le ventre, épaules haussées.
Expr. : neutre (air soucieux) ; ravi (soulagement visible) ; furieux (moustache qui frétille, front plissé).

**COHAB_DROITE** : Philippe de Montclar. PM d'opposition de droite, UTL.
Caractère : notable libéral, ironique, croit à la main invisible et à la sienne.
Traits :
- Visage long, menton conquérant, grand nez aquilin.
- Monocle sur l'œil droit, moustache fine cirée.
- Nœud papillon à pois, pochette, canne de bois.
- Silhouette haute et raide, menton levé.
Expr. : neutre (dédain tranquille) ; ravi (sourire en coin) ; furieux (monocle qui tombe, narines dilatées).

### 5.4 Les candidats à la présidentielle

**CAND_GAUCHE_RADICALE** : Camille Rouvière. UGR.
Caractère : ancienne inspectrice du travail, voix rauque, 140 mesures.
Traits :
- Coupe courte rousse, mèche en crête, visage triangulaire.
- Pommettes saillantes, regard perçant, rides de colère.
- Veste de cuir sur chemise ouverte.
- Menton en avant, bouche toujours en plein discours.
Expr. : neutre (regard fixe) ; ravi (sourire de tribune) ; furieux (bouche grande ouverte, veines du cou).

**CAND_ECOLO** : Noémie Vasseur. Génération Climat.
Caractère : climatologue, sobre, culpabilise avec douceur.
Traits :
- Casque de vélo vert vissé sur la tête, longue tresse blonde.
- Taches de rousseur partout, grands yeux ronds, nez retroussé.
- Pull en laine épais, sac à dos, pince à vélo au pantalon.
- Silhouette fine, légèrement penchée comme sur un guidon.
Expr. : neutre (regard grave) ; ravi (grand sourire, joues roses) ; furieux (casque de travers, sourcils froncés).

**CAND_DROITE** : Édouard Valmont. UTL.
Caractère : notable de province, ancien ministre du Budget, aimable et glacé.
Traits :
- Visage rond comme une lune, double menton étagé.
- Sourire de banquier, petites dents, petits yeux plissés.
- Cheveux blancs courts, lunettes demi-lune sur le nez.
- Costume trois pièces, chaîne de montre, ventre en proue.
Expr. : neutre (sourire commercial) ; ravi (menton qui tremble de joie) ; furieux (sourire qui se crispe, rougeur).

**CAND_NATIONALISTE** : Gaspard Lenoir. LSO.
Caractère : tribun de plateaux télé, un slogan par problème.
Traits :
- Cheveux noirs gominés en arrière, front bas, teint hâlé.
- Mâchoire de boxeur, bouche immense, dents carnassières.
- Sourcils noirs épais en accent circonflexe.
- Costume noir, chemise ouverte, doigt tendu, torse bombé.
Expr. : neutre (regard de défi) ; ravi (rictus de vainqueur) ; furieux (bouche grande ouverte, postillons).

**CAND_CENTRE** : Hélène Marchetti. MJM (réserve).
Caractère : maire d'une ville moyenne, championne du compromis.
Traits :
- Chignon brun serré, visage ovale, joues pleines.
- Sourire conciliant, sourcils levés en point d'interrogation.
- Écharpe tricolore de maire, tailleur beige.
- Mains ouvertes, paumes vers le haut, corps tourné de côté.
Expr. : neutre (sourire d'écoute) ; ravi (sourire élargi) ; furieux (sourire qui hésite, un sourcil bas).

### 5.5 Les ministres

**MIN_ECONOMIE** : Gilles Fontaine. Ministre de l'Économie.
Caractère : chiffres en main, jamais de bonnes nouvelles avant 18 h.
Traits :
- Visage de hibou : lunettes rondes épaisses, yeux énormes.
- Cheveux roux clairsemés, oreilles décollées, menton fuyant.
- Trois stylos dans la poche de poitrine, cravate à rayures.
- Petit et sec, serre une calculatrice sur son cœur.
Expr. : neutre (clignement) ; ravi (yeux brillants, chiffres en vue) ; furieux (lunettes de travers, tic à l'œil).

**MIN_INTERIEUR** : Régis Fermont. Ministre de l'Intérieur. *(Gueule de démonstration V3 déjà dessinée.)*
Caractère : ordre et fermeté, soupçonne tout le monde, y compris ses collègues.
Traits :
- Tête de bouledogue en parpaing, mâchoire plus large que le crâne.
- Brosse rase grise, petits yeux soupçonneux et cernés.
- Nez de boxeur, cicatrice au menton, cou de taureau.
- Costume bleu nuit, cravate serrée, cocarde au revers.
Expr. : neutre (visage fermé) ; ravi (rictus satisfait) ; furieux (veine sur la tempe, dents serrées) ; gêné (goutte de sueur).

**MIN_SANTE** : Odile Varenne. Ministre de la Santé.
Caractère : ancienne médecin hospitalière, fatiguée, lucide, drôle.
Traits :
- Cheveux gris-blond en chignon bas, un stylo piqué dedans.
- Cernes violets, rides de rire, lunettes relevées sur le front.
- Blouse blanche sous le blazer, stéthoscope autour du cou.
- Corps menu, épaules affaissées, tasse de café en main.
Expr. : neutre (regard las) ; ravi (grand rire fatigué) ; furieux (stylo cassé, regard noir).

**MIN_EDUCATION** : Étienne Marchal. Ministre de l'Éducation.
Caractère : ancien professeur de lettres, corrige tout, même les discours.
Traits :
- Cheveux blancs en pointes, hirsute, sourcils broussailleux.
- Longue barbe poivre et sel, petites lunettes rondes.
- Veste en velours côtelé à pièces aux coudes, écharpe.
- Grand, maigre, stylo rouge derrière l'oreille.
Expr. : neutre (sourcil levé) ; ravi (yeux pétillants) ; furieux (stylo rouge brandi, bouche pincée).

**MIN_ECOLOGIE** : Maëlle Kervella. Ministre de l'Écologie.
Caractère : jeune, enthousiaste, grignote des graines en réunion.
Traits :
- Coupe asymétrique, mèche verte, côté rasé.
- Visage rond, joues rosies, grand sourire, anneau au nez.
- Pull large aux manches trop longues, bottes en caoutchouc.
- Petit gabarit, gourde en inox, posture bondissante.
Expr. : neutre (sourire en attente) ; ravi (bras en l'air) ; furieux (bouche en O, mèche dressée).

**MIN_AFFAIRES_ETR** : Louis de Valcourt. Ministre des Affaires étrangères.
Caractère : diplomate raffiné, dit non en trois langues avec le sourire.
Traits :
- Très grand, très mince, visage allongé et pâle.
- Cheveux argentés en vagues, long nez fin, pommettes creuses.
- Écharpe de soie, costume bleu nuit, œillet à la boutonnière.
- Port de héron, regard détaché.
Expr. : neutre (sourire de salon) ; ravi (sourcil complice) ; furieux (lèvre pincée, glace dans l'œil).

### 5.6 Les chefs de groupe parlementaire

**CHEF_UGR** : Yannick Brossard. Chef du groupe UGR.
Caractère : tribun d'estrade, remonte l'hémicycle à la voix.
Traits :
- Tête ronde et chauve, barbe grise de bûcheron.
- Sourcils en broussaille, bouche large.
- Chemise à carreaux rouges, sans cravate, bretelles.
- Ventre généreux, bras écartés, poing levé.
Expr. : neutre (sourire en coin) ; ravi (rire d'ogre) ; furieux (bouche ouverte, veines du front).

**CHEF_RSV** : Claire Aubert. Cheffe du groupe RSV.
Caractère : négociatrice patiente, pose les amendements comme des briques.
Traits :
- Cheveux courts bruns à frange droite, lunettes rouges larges.
- Visage rond, fossettes, sourire en biais.
- Pull col roulé vert, écharpe tricotée main.
- Corps solide et compact, carnet de notes ouvert.
Expr. : neutre (regard calme) ; ravi (fossettes) ; furieux (lunettes baissées, regard par-dessus).

**CHEF_MJM** : Gérard Poulain. Chef du groupe MJM.
Caractère : ni pour, ni contre, bien au contraire.
Traits :
- Visage lunaire, calvitie, favoris blonds en côtelettes.
- Petit nez rond rouge, joues rebondies, regard fuyant.
- Costume beige, cravate pâle, ventre rond.
- Mains qui se frottent, haussements d'épaules.
Expr. : neutre (regard fuyant) ; ravi (sourire gêné) ; furieux (sueur au front, lèvres tremblantes).

**CHEF_UTL** : Aymeric Dubreuil-Castagnac. Président du groupe UTL. *(Gueule de démonstration V3 déjà dessinée.)*
Caractère : grand bourgeois, ironie glacée, mémoire des budgets.
Traits :
- Visage en lame de couteau, haute banane argentée laquée.
- Long nez tombant, menton en avant avec fossette.
- Paupières lourdes, un sourcil toujours levé.
- Costume croisé, cravate rouge, pochette.
Expr. : neutre (regard qui toise) ; ravi (sourire en coin) ; furieux (narines pincées) ; gêné (goutte de sueur).

**CHEF_LSO** : Maxime Corbeau. Chef du groupe LSO.
Caractère : jeune loup, ironique, fait du bruit pour faire du bruit.
Traits :
- Visage taillé à la serpe, joues creuses, nez en bec.
- Cheveux noirs en brosse, tempes rasées, petite barbiche.
- Bomber noir sur chemise blanche, pin's au revers.
- Mince, épaules en avant, mains dans les poches.
Expr. : neutre (regard de côté) ; ravi (sourire carnassier) ; furieux (dents serrées, regard noir).

### 5.7 Presse, Sénat, syndicat, patronat

**JOUR_CHAINE_INFO** : Karine Lopez. Présentatrice de Chaîne Info 24.
Caractère : tout est « urgent » et « dernière minute », même la météo.
Traits :
- Brushing blond roux gonflé, immobile même sous la tempête.
- Maquillage appuyé, grands yeux ouverts, sourcils arqués.
- Sourire large et figé, dents éclatantes.
- Micro à la main, veste claire, bandeau rouge sous le menton.
Expr. : neutre (sérieux de plateau) ; ravi (yeux écarquillés) ; furieux (sourire figé, voix qui monte).

**JOUR_EDITORIALISTE** : Hubert Lenglet. Éditorialiste du Quotidien du Matin.
Caractère : pompeux, sentencieux, déteste les mots simples.
Traits :
- Crâne dégarni, longues mèches blanches sur les côtés.
- Gros nez, monture d'écaille, bajoues.
- Nœud papillon bordeaux, veste en tweed, journal sous le bras.
- Menton sur la poitrine, sourcil levé de condescendance.
Expr. : neutre (regard par-dessus les lunettes) ; ravi (sourire pincé) ; furieux (plume brandie, cheveux hérissés).

**SENAT_PRESIDENT** : Gaston Mirebeau. Président du Sénat.
Caractère : doyen respecté, lent, a vu passer onze gouvernements.
Traits :
- Très âgé, visage ridé comme une carte, oreilles immenses.
- Cheveux blancs en couronne, grosses lunettes, nez bourgeonnant.
- Costume gris froissé, canne, écharpe tricolore tombante.
- Silhouette voûtée, petite taille, paupières lourdes.
Expr. : neutre (somnolence digne) ; ravi (sourire édenté) ; furieux (canne qui frappe le sol).

**SYND_UST** : Nadia Benali. Secrétaire générale de l'UST.
Caractère : franche, solide, mène les défilés et partage les merguez.
Traits :
- Cheveux noirs en queue de cheval haute, mèches grises.
- Visage en cœur, regard vif, sourire large, grosses boucles d'oreille.
- Gilet de syndicat rouge, mégaphone à la main.
- Corps robuste, mains puissantes, allure de tête de cortège.
Expr. : neutre (regard franc) ; ravi (rire sonore) ; furieux (mégaphone levé, cri).

**PATRON_UPE** : Charles-Henri Dumoulin. Président de l'UPE.
Caractère : bronzé, mielleux, répète : « baissez nos charges ».
Traits :
- Teint orange, dents trop blanches, sourire d'agent immobilier.
- Cheveux argent gominés, favoris, sourcils épilés.
- Chemise rose, gilet sans manches, montre énorme.
- Carrure de golfeur, mains lissant sa cravate.
Expr. : neutre (sourire de vendeur) ; ravi (sourire de contrat signé) ; furieux (sourire serré, nuque rouge).

### 5.8 Récapitulatif (30 personnages)

| Id | Nom | Rôle |
|---|---|---|
| PRES_GAUCHE | Antoine Verdier | Président RSV |
| PRES_CENTRE | Laurent Delmas | Président MJM |
| PRES_DROITE | Henri de Saint-Aubin | Président UTL |
| PM_TECHNOCRATE | Pierre Delaunay | PM technocrate |
| PM_RASSEMBLEUR | Marianne Deschamps | PM rassembleuse |
| PM_REFORMATEUR | Julien Lestrade | PM réformateur |
| COHAB_GAUCHE | Sophie Carrel | PM cohabitation gauche |
| COHAB_CENTRE | Bernard Lacombe | PM cohabitation centre |
| COHAB_DROITE | Philippe de Montclar | PM cohabitation droite |
| CAND_GAUCHE_RADICALE | Camille Rouvière | Candidate UGR |
| CAND_ECOLO | Noémie Vasseur | Candidate Génération Climat |
| CAND_DROITE | Édouard Valmont | Candidat UTL |
| CAND_NATIONALISTE | Gaspard Lenoir | Candidat LSO |
| CAND_CENTRE | Hélène Marchetti | Candidate MJM |
| MIN_ECONOMIE | Gilles Fontaine | Ministre de l'Économie |
| MIN_INTERIEUR | Régis Fermont | Ministre de l'Intérieur |
| MIN_SANTE | Odile Varenne | Ministre de la Santé |
| MIN_EDUCATION | Étienne Marchal | Ministre de l'Éducation |
| MIN_ECOLOGIE | Maëlle Kervella | Ministre de l'Écologie |
| MIN_AFFAIRES_ETR | Louis de Valcourt | Ministre des Affaires étrangères |
| CHEF_UGR | Yannick Brossard | Chef de groupe UGR |
| CHEF_RSV | Claire Aubert | Cheffe de groupe RSV |
| CHEF_MJM | Gérard Poulain | Chef de groupe MJM |
| CHEF_UTL | Aymeric Dubreuil-Castagnac | Chef de groupe UTL |
| CHEF_LSO | Maxime Corbeau | Chef de groupe LSO |
| JOUR_CHAINE_INFO | Karine Lopez | Présentatrice Chaîne Info 24 |
| JOUR_EDITORIALISTE | Hubert Lenglet | Éditorialiste, Quotidien du Matin |
| SENAT_PRESIDENT | Gaston Mirebeau | Président du Sénat |
| SYND_UST | Nadia Benali | Secrétaire générale de l'UST |
| PATRON_UPE | Charles-Henri Dumoulin | Président de l'UPE |
