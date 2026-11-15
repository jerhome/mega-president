# Journal des décisions

Décisions prises en autonomie (ou arbitrages du joueur), à relire. Chacune est réversible sauf mention contraire.

| Date | Décision | Alternative écartée | Raison |
|---|---|---|---|
| 14/11 | Mode 60 tours reporté en V2 | Le garder en V1 | Arbitrage du joueur (vitesse) |
| 14/11 | Budget annuel simplifié : rigueur, équilibre, relance | 4 postes + impôts | Arbitrage du joueur (vitesse) |
| 14/11 | Équilibrage par parties automatiques + 1 test du joueur | Plusieurs sessions de test | Arbitrage du joueur (vitesse) |
| 14/11 | Tests sur vraie console regroupés en fin de projet | Test EverDrive le 15/11 | Demande du joueur |
| 14/11 | Le bouton C ouvre l'aide ; seul A valide | A et C valident | Conforme aux règles (§13) |
| 14/11 | Événements de ministre : effet sur la relation avec le parti du joueur (`REL_PARTI`) | Relation fixe avec l'UTL | Bug : incohérent si le joueur n'est pas de droite |
| 14/11 | Flambée de l'électricité déclenchée par la tension (> 40) | Dette > 100 | Bug : toujours vraie, et sans lien logique |
| 14/11 | Capital politique : 1 CP/trimestre (+1 si popularité ≥ 50 %, +1 si majorité) au lieu de 3 | 3 CP de base (REGLES §5) | Le CP était trop abondant en V0 |
| 14/11 | Catalogue de lois de nouveau disponible au 2nd mandat | Lois adoptées une fois pour toute la partie | Sinon le 2nd mandat n'a presque plus d'actions |
| 14/11 | Conversions : DETTE 1 unité = 0,5 point ; SCANDALE 1 unité = 2 points | Conversions V0 | Dette trop forte ; destitution impossible avec le contenu actuel |
| 14/11 | Destitution rare (≈ 1 % des parties d'un joueur à scandales) | Plus fréquente | Cible « rare mais possible » ; à revoir avec plus d'événements en chaîne |
| 14/11 | Vague 2 des écrans confiée à 2 agents Sonnet, flux (game.c) écrit par la session principale | Un seul agent | Gain de temps, fichiers disjoints |
| 14/11 | V2 commencée par la simulation (cohabitation, Sénat, référendum, remaniement, 60 tours) ; pixel art et musique en attente du joueur | Commencer par la carte ou le pixel art | Ces deux sujets demandent des choix artistiques du joueur |
| 14/11 | Musique V2 provisoire : 3 compositions originales simples sur le PSG (titre, jeu, élection) | Attendre le choix du joueur ; musiques libres de droits | Avancer sans bloquer ; facilement remplaçable par une vraie bande-son (XGM) |
| 14/11 | 4e agent temporaire (sauvegarde, musique et lois en parallèle de la simulation) | Rester à 3 | Autorisé par le joueur pour accélérer ; fichiers disjoints |
| 14/11 | Sauvegarde de partie : struct Game copiée en bloc avec signature, taille et somme de contrôle | Format champ par champ | Simple et robuste ; une sauvegarde d'une autre version est ignorée proprement |
| 14/11 | PM d'opposition de gauche renommée « Sophie Carrel » (au lieu de « Marianne Aubrac ») | Garder le nom proposé | Aubrac évoque des résistants réels ; prénom Marianne déjà pris par une PM |
| 14/11 | 5 électorats (2.09) et budget détaillé (2.10) reportés en V4 | Les faire en V2 | Tableau de bord déjà dense ; la popularité par région (carte) apporte plus ; le budget en un choix a été validé par le joueur |
| 14/11 | Équilibrage V2 : +1 popularité générale sur les choix 1 et 2 des nouveaux événements | Retoucher le cœur de la simulation | Les 15 événements V2 faisaient perdre ~5 points de réélection ; réglage local et réversible |
| 14/11 | Pixel art par programme, style caricatural BD, ordre titre → portraits → événements → décors ; musique provisoire conservée | — | Décisions du joueur |
| 15/11 | Musique par année (2.1.4) : 5 thèmes PSG (1 sol majeur optimiste, 2 fa majeur à basse répétée « machine à écrire », 3 mi mineur syncopé, 4 la mineur lent sans mélodie rapide, 5 ré majeur montant d'un ton toutes les 8 mesures) ; 2nd mandat rebouclé modulo 5 | Un seul thème de jeu | Variété et signature par année ; tableaux ROM de ~2 o/note, +0 RAM |
| 15/11 | Voix « For sure ! » : synthèse SAPI (voix fr-FR) → WAV 8 bits 13,4 kHz (13 Ko), driver Z80 PCM chargé puis déchargé après lecture ; musique de titre différée jusqu'à la fin de la voix | Driver XGM2 / PCM4 | Le plus léger ; PSG piloté par le 68000, non affecté ; aucune allocation dynamique |
| 15/11 | Plantage de la 4e année : tampons de ligne à 80 octets (`UI_LINE_BUF`) + `-fstack-protector-strong` (message d'arrêt explicite au lieu d'un saut aléatoire) | Corriger la seule ligne fautive | Les textes UTF-8 (accents sur 2 octets) rendent toute ligne de 38 colonnes susceptible de dépasser 40 octets ; coût : ~0,5 Ko de RAM statique et un léger surcoût par fonction |
| 15/11 | Version et auteur centralisés dans `src/version.h` (GAME_VERSION à incrémenter à chaque version) | Texte en dur dans scr_title.c | Le joueur veut la version affichée à chaque livraison |
| 15/11 | Retour au menu : sauvegarde de l'état courant du trimestre | Revenir à la sauvegarde du début de trimestre | Ne rien perdre de ce que le joueur a fait ; pas de « triche » par rechargement |
| 15/11 | V3 centrée sur le beau (interface, gueules, musique FM) ; 5 électorats et budget détaillé reportés en V4 | Les inclure en V3 | Décision du joueur |
| 15/11 | V3 : l'interface n'utilise que PAL0/PAL1, PAL2/PAL3 réservées aux illustrations | Palettes partagées comme en V2 | Les gueules et une interface riche ont besoin de couleurs : 64 couleurs à l'écran au total |
| 15/11 | V3 lancée en 3 agents parallèles (interface, gueules en Opus, musique FM) | Tranche de démonstration d'abord | Demande du joueur (« déclenche tout ») ; Opus pour les gueules, tâche la plus exigeante en qualité |
| 15/11 | Gueules V3 validées (style des 3 démonstrations) ; renommages : Camille Roussel → Camille Rouvière, Julien Maréchal → Julien Lestrade ; présidents Antoine Verdier, Laurent Delmas, Henri de Saint-Aubin | Garder les anciens noms | Noms trop proches de personnalités réelles ; décision du joueur |
| 15/11 | V4 : les 5 propositions de SPEC_V4.md validées | Variantes simplifiées | Décision du joueur |
| 15/11 | Interface V3 validée ; hémicycle dynamique + illustrations redessinées ; « plus rien de cheap » | Garder l'hémicycle illustré seul | Demande du joueur (« le jeu est fun, il doit être beau ») |
| 15/11 | Série prévue sur plusieurs plateformes : Mega Président (Megadrive), Super Président (Nintendo), Giga Président (PC/Mac) ; le nom est aussi un pied de nez aux pouvoirs trop grands du président de la Ve République | — | Décision du joueur, notée dans README.md |
| 15/11 | Musique V3 (3.11) : pilote XGM2 de SGDK pour tout le son ; morceaux composés en code (tools/music/, instruments FM définis par leurs opérateurs) → VGM → ressource XGM2 multi-pistes (percussions PCM partagées) ; bruitages en PCM sur le canal 2, voix sur le canal 3 ; thèmes PSG V2 non encore recomposés convertis en VGM PSG | Garder le séquenceur PSG du 68000 à côté de XGM2 | Le 68000 et le Z80 écriraient en même temps dans le PSG (écritures en deux octets corrompues) ; un seul pilote, aucun tas, tempo adapté PAL/NTSC par XGM2 |
| 15/11 | Écran titre : 10 accroches en rotation (compteur en SRAM, octet 254) | Une accroche fixe | Choix du joueur |
