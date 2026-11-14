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
| 14/11 | 5 électorats (2.09) et budget détaillé (2.10) reportés en V3 | Les faire en V2 | Tableau de bord déjà dense ; la popularité par région (carte) apporte plus ; le budget en un choix a été validé par le joueur |
| 14/11 | Équilibrage V2 : +1 popularité générale sur les choix 1 et 2 des nouveaux événements | Retoucher le cœur de la simulation | Les 15 événements V2 faisaient perdre ~5 points de réélection ; réglage local et réversible |
