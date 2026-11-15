# Lois V2

25 lois supplémentaires, format de lois_v1.md. Les lignes Orientation, Souveraineté et Effets suggérés
sont indicatives : les valeurs réelles sont dans src/sim/laws.c. Tout est fictif.

## LOI_SURPROFITS
Orientation : -55
Souveraineté : non
Nom : Taxe superprofits
Description :
  Taxe exceptionnelle sur les profits
  hors normes des grands groupes. Les
  dividendes découvrent la solidarité.
Journal si adoptée : Superprofits : la taxe tombe
Journal si rejetée : Superprofits : les bilans respirent
Effets suggérés : DETTE -3, POP_POPULAIRES +3, POP_AISEES -3, ECONOMIE -2, REL_PARTI +2, Coût CP 4, Délai 2

## LOI_SEMAINE4J
Orientation : -60
Souveraineté : non
Nom : Semaine de 4 jours
Description :
  Passage progressif à 32 heures sans
  perte de salaire. Le vendredi est
  un sujet de négociation nationale.
Journal si adoptée : Semaine de 4 jours : vendredi chômé
Journal si rejetée : Semaine de 4 jours : on rouvre lundi
Effets suggérés : POP_POPULAIRES +4, POP_MOYENNES +1, CROISSANCE -2, CHOMAGE -2, SOCIAL +3, Coût CP 5, Délai 3

## LOI_CANTINE
Orientation : -35
Souveraineté : non
Nom : Cantine gratuite
Description :
  Un repas gratuit à l'école pour tous
  les enfants. La purée devient une
  ligne budgétaire très surveillée.
Journal si adoptée : Cantine gratuite : purée offerte
Journal si rejetée : Cantine : le repas reste payant
Effets suggérés : POP_POPULAIRES +3, POP_MOYENNES +2, SOCIAL +3, DETTE +2, Coût CP 3, Délai 2

## LOI_VOLS_COURTS
Orientation : -45
Souveraineté : non
Nom : Fin des vols courts
Description :
  Interdit l'avion quand le train met
  moins de 4 heures. Les valises
  apprennent à voyager en wagon.
Journal si adoptée : Vols courts : cap sur le rail
Journal si rejetée : Vols courts : l'avion reste roi
Effets suggérés : ECOLOGIE +4, ECONOMIE -1, POP_AISEES -2, POP_MOYENNES +1, REL_PARTI +2, Coût CP 3, Délai 2

## LOI_DESERTS_MED
Orientation : -25
Souveraineté : non
Nom : Déserts médicaux
Description :
  Régule l'installation des médecins
  finance des centres de santé. Le
  docteur se rapproche du patient.
Journal si adoptée : Déserts médicaux : des docteurs
Journal si rejetée : Déserts médicaux : la salle d'attente
Effets suggérés : SOCIAL +3, POP_POPULAIRES +2, POP_MOYENNES +2, TENSION -2, DETTE +1, Coût CP 3, Délai 3

## LOI_RIC
Orientation : -30
Souveraineté : oui
Nom : Référendum citoyen
Description :
  Un million de signatures suffisent à
  soumettre une loi au vote. Le peuple
  s'invite, l'agenda aussi.
Journal si adoptée : RIC adopté : le peuple tient le stylo
Journal si rejetée : RIC rejeté : la pétition attendra
Effets suggérés : POP_TOUS +2, DEFIANCE -4, TENSION +2, REL_PARTI +1, Coût CP 5, Délai 4

## LOI_PROPORTIONNELLE
Orientation : -35
Souveraineté : oui
Nom : Proportionnelle
Description :
  Les sièges suivent enfin les voix.
  Les coalitions deviennent la règle,
  les calculettes aussi.
Journal si adoptée : Proportionnelle : chaque voix compte
Journal si rejetée : Proportionnelle : scrutin inchangé
Effets suggérés : DEFIANCE -3, TENSION -2, POP_TOUS +1, REL_PARTI -2, Coût CP 6, Délai 4

## LOI_AIDE_DEV
Orientation : -30
Souveraineté : oui
Nom : Aide au développement
Description :
  Porte l'aide publique au 0,7 % du
  revenu national. La solidarité
  internationale entre dans le budget.
Journal si adoptée : Aide au développement : cap sur 0,7 %
Journal si rejetée : Aide au développement : on rogne
Effets suggérés : DETTE +2, DEFIANCE -2, POP_POPULAIRES +1, POP_AISEES -1, REL_PARTI +2, Coût CP 3, Délai 2

## LOI_POLICE_PROX
Orientation : -30
Souveraineté : oui
Nom : Police de proximité
Description :
  Des agents à pied dans les quartiers
  formés au dialogue. Le rond-point
  retrouve un visage connu.
Journal si adoptée : Proximité : retour du gardien de paix
Journal si rejetée : Police de proximité : pas de budget
Effets suggérés : SECURITE +2, TENSION -4, POP_POPULAIRES +2, DETTE +1, Coût CP 3, Délai 3

## LOI_ENCADREMENT
Orientation : -40
Souveraineté : non
Nom : Encadrement des loyers
Description :
  Plafonne les loyers dans les grandes
  villes. Les annonces immobilières
  rédigent des romans plus courts.
Journal si adoptée : Loyers encadrés : le plafond tient
Journal si rejetée : Loyers : la hausse continue
Effets suggérés : POP_POPULAIRES +3, POP_MOYENNES +1, POP_AISEES -2, ECONOMIE -1, SOCIAL +2, Coût CP 3, Délai 2

## LOI_BIO_CANTINE
Orientation : -25
Souveraineté : non
Nom : Conversion au bio
Description :
  Aides à la conversion et 50 % de bio
  dans les cantines publiques. Les
  champs sourient, les rendements non.
Journal si adoptée : Bio : les champs changent de couleur
Journal si rejetée : Bio : le pesticide garde sa place
Effets suggérés : ECOLOGIE +3, SOCIAL +1, POP_MOYENNES +1, DETTE +1, REL_PARTI +1, Coût CP 3, Délai 3

## LOI_CULTURE
Orientation : -20
Souveraineté : non
Nom : Pass culture pour tous
Description :
  Un chèque culture annuel pour chaque
  jeune et plus de moyens pour les
  théâtres. Molière est ravi.
Journal si adoptée : Pass culture : place aux spectacles
Journal si rejetée : Pass culture : rideau sur le projet
Effets suggérés : POP_MOYENNES +2, POP_POPULAIRES +1, TECHNOLOGIE +1, DETTE +1, Coût CP 2, Délai 2

## LOI_FLAT_TAX
Orientation : +60
Souveraineté : non
Nom : Flat tax
Description :
  Un taux unique de 25 % sur le
  capital. La simplicité séduit les
  uns, inquiète les autres.
Journal si adoptée : Flat tax : un taux pour les gouverner
Journal si rejetée : Flat tax : le barème reste complexe
Effets suggérés : ECONOMIE +3, CROISSANCE +2, DETTE +2, POP_AISEES +3, POP_POPULAIRES -3, Coût CP 4, Délai 2

## LOI_LICENCIEMENT
Orientation : +55
Souveraineté : non
Nom : Licenciement assoupli
Description :
  Plafonne les indemnités et simplifie
  les procédures. Les employeurs
  embauchent, les syndicats défilent.
Journal si adoptée : Licenciement assoupli : on embauche ?
Journal si rejetée : Licenciement : le code reste épais
Effets suggérés : CHOMAGE -3, CROISSANCE +2, POP_POPULAIRES -3, POP_AISEES +2, TENSION +4, Coût CP 5, Délai 3

## LOI_UNIFORME
Orientation : +40
Souveraineté : non
Nom : Uniforme à l'école
Description :
  Tenue unique du primaire au collège.
  Le débat sur la longueur des jupes
  est enfin clos.
Journal si adoptée : Uniforme : tout le monde en bleu
Journal si rejetée : Uniforme : le style reste libre
Effets suggérés : POP_MOYENNES +2, POP_AISEES +1, TENSION +2, SOCIAL -1, REL_PARTI +1, Coût CP 2, Délai 1

## LOI_VIDEO
Orientation : +60
Souveraineté : oui
Nom : Vidéosurveillance
Description :
  Caméras dans les villes de plus
  de 10 000 habitants. Le pigeon est
  filmé sous tous les angles.
Journal si adoptée : Vidéo : tous sous l'objectif
Journal si rejetée : Vidéo : les caméras attendent
Effets suggérés : SECURITE +3, TENSION +3, DEFIANCE +2, POP_AISEES +2, DETTE +1, Coût CP 3, Délai 2

## LOI_PARLEMENTAIRES
Orientation : +35
Souveraineté : oui
Nom : Moins de parlementaires
Description :
  Réduit d'un tiers les députés et
  les sénateurs. Les sièges fondent,
  les ego aussi, peut-être.
Journal si adoptée : Parlementaires : un tiers en moins
Journal si rejetée : Parlementaires : hémicycle plein
Effets suggérés : DETTE -1, DEFIANCE -2, POP_TOUS +1, REL_PARTI -3, Coût CP 5, Délai 4

## LOI_LPM
Orientation : +50
Souveraineté : oui
Nom : Loi de programmation
Description :
  Hausse massive du budget des armées
  sur sept ans. Les chars arrivent, la
  facture aussi.
Journal si adoptée : Armées : le budget monte en grade
Journal si rejetée : Armées : la programmation est reportée
Effets suggérés : SECURITE +3, TECHNOLOGIE +2, DETTE +3, POP_AISEES +1, REL_PARTI +2, Coût CP 4, Délai 4

## LOI_SERVICE_MIL
Orientation : +55
Souveraineté : oui
Nom : Service national
Description :
  Service obligatoire de six mois pour
  tous les jeunes. Le sac à dos est de
  retour, le réveil sonne à six h.
Journal si adoptée : Service national : retour du sac à dos
Journal si rejetée : Service national : on respire
Effets suggérés : SECURITE +2, SOCIAL +1, POP_MOYENNES +1, POP_POPULAIRES -1, DETTE +2, Coût CP 5, Délai 4

## LOI_ALLIANCE
Orientation : +30
Souveraineté : oui
Nom : Alliance militaire
Description :
  Renforce la défense commune avec
  nos alliés et accueille leurs bases.
  La diplomatie parle bas, en kaki.
Journal si adoptée : Alliance : on resserre les rangs
Journal si rejetée : Alliance : on garde nos distances
Effets suggérés : SECURITE +2, DEFIANCE +1, ECONOMIE +1, POP_AISEES +1, REL_PARTI +1, Coût CP 3, Délai 2

## LOI_EXPORT_ARMES
Orientation : +45
Souveraineté : oui
Nom : Exportations d'armes
Description :
  Facilite les contrats d'armement
  les pays partenaires. La balance
  commerciale applaudit, on tousse.
Journal si adoptée : Armes : les contrats décollent
Journal si rejetée : Armes : les exportations sont gelées
Effets suggérés : ECONOMIE +2, CROISSANCE +1, DEFIANCE +2, POP_POPULAIRES -1, DETTE -1, Coût CP 3, Délai 2

## LOI_AGRI_PRODUCT
Orientation : +35
Souveraineté : non
Nom : Agriculture productive
Description :
  Assouplit les normes et soutient les
  grandes exploitations exportatrices.
  Le rendement est roi, la haie part.
Journal si adoptée : Agriculture : les normes se desserrent
Journal si rejetée : Agriculture : les normes restent
Effets suggérés : ECONOMIE +2, CROISSANCE +1, ECOLOGIE -3, POP_MOYENNES +1, TENSION +1, Coût CP 3, Délai 2

## LOI_CARBONE
Orientation : -5
Souveraineté : non
Nom : Taxe carbone
Description :
  Un prix du carbone progressif,
  reversé aux ménages par chèque.
  La pompe devient pédagogue.
Journal si adoptée : Taxe carbone : la pompe se paie
Journal si rejetée : Taxe carbone : le plein reste bas
Effets suggérés : ECOLOGIE +3, DETTE -1, POP_POPULAIRES -2, POP_AISEES -1, TENSION +3, Coût CP 4, Délai 2

## LOI_CYBER
Orientation : +10
Souveraineté : non
Nom : Cybersécurité nationale
Description :
  Un plan pour protéger les mairies
  et hôpitaux des pirates. Le mot de
  passe '1234' est enfin interdit.
Journal si adoptée : Cybersécurité : le pare-feu national
Journal si rejetée : Cybersécurité : les pirates ravis
Effets suggérés : TECHNOLOGIE +3, SECURITE +1, DETTE +1, POP_MOYENNES +1, Coût CP 2, Délai 2

## LOI_PEINES_PLANCHERS
Orientation : +65
Souveraineté : oui
Nom : Peines planchers
Description :
  Des peines minimales pour les
  récidivistes. Les juges y voient une
  liberté en moins, cellules en plus.
Journal si adoptée : Peines planchers : barème durci
Journal si rejetée : Peines planchers : la balance penche
Effets suggérés : SECURITE +3, TENSION +3, POP_AISEES +2, POP_POPULAIRES +1, DETTE +1, REL_PARTI +1, Coût CP 4, Délai 2
