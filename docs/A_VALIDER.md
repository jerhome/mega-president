# Décisions à valider par le joueur

> Décisions importantes prises (ou mises en attente) pendant que le joueur est en retrait.
> Les décisions mineures sont dans [DECISIONS.md](DECISIONS.md).
> Format : contexte, options, recommandation, ce qui a été fait en attendant. Le joueur répond par « ok », « non » ou un autre choix ; la ligne passe ensuite dans DECISIONS.md.

| Date | Sujet | Options | Recommandation | En attendant | Réponse |
|---|---|---|---|---|---|
| 15/11 | Logo de l'écran titre (revue DA, P1) : le bandeau de texte « MEGA PRÉSIDENT » en police d'interface est trop petit pour un écran titre et cache le drapeau | (a) logo dessiné par programme dans `tools/art/` : lettres capitales dorées biseautées, contour bleu nuit, « PRÉSIDENT » plus grand que « MEGA », environ 224 × 40 px, posé sur le ciel de l'illustration et dans sa palette (aucune palette en plus) ; (b) garder le bandeau | (a) : c'est la première image du jeu, elle doit « faire cartouche ». ≈ 3 h (DA + md-dev), ≈ 100 tuiles | Bandeau actuel conservé | |
| 15/11 | Visages animés « comme dans Dune » (principe accepté par le joueur) et charabia à la Sims : clignement, regard, transitions d'expression masquées par un clignement ; la bouche ne bouge plus pendant tout le texte mais seulement pendant un charabia fictif (synthèse maison, canal PCM 3), puis seuls les yeux bougent ; détail dans `REVUE_DA_V3.md` § 6 et § 7 | (a) lot V3.1 « gueules vivantes et parlantes » complet après le test console (≈ 21 h : visages 10 h, charabia mélangé 12,5 h, 1 h de recouvrement) ; (b) visages et interjection d'1 syllabe seulement en V3.1 (≈ 17 h), les 3 syllabes en V3.2 ; (c) visages seuls, sans son (≈ 10 h) ; (d) clignement seul en V3 (≈ 4 h), le reste en V3.1 | (a) : interjection d'1 syllabe par défaut, 3 syllabes pour les moments forts, à 6,65 kHz (≈ 280 Ko de ROM, budget 300 Ko), option « Voix » dans le menu ; (b) si on veut livrer plus vite | Rien de codé ; consignes prêtes (§ 6 et § 7) | |
