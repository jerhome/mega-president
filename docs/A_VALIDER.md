# Décisions à valider par le joueur

> Décisions importantes prises (ou mises en attente) pendant que le joueur est en retrait.
> Les décisions mineures sont dans [DECISIONS.md](DECISIONS.md).
> Format : contexte, options, recommandation, ce qui a été fait en attendant. Le joueur répond par « ok », « non » ou un autre choix ; la ligne passe ensuite dans DECISIONS.md.

| Date | Sujet | Options | Recommandation | En attendant | Réponse |
|---|---|---|---|---|---|
| 15/11 | Logo de l'écran titre (revue DA, P1) : le bandeau de texte « MEGA PRÉSIDENT » en police d'interface est trop petit pour un écran titre et cache le drapeau | (a) logo dessiné par programme dans `tools/art/` : lettres capitales dorées biseautées, contour bleu nuit, « PRÉSIDENT » plus grand que « MEGA », environ 224 × 40 px, posé sur le ciel de l'illustration et dans sa palette (aucune palette en plus) ; (b) garder le bandeau | (a) : c'est la première image du jeu, elle doit « faire cartouche ». ≈ 3 h (DA + md-dev), ≈ 100 tuiles | Bandeau actuel conservé | |
| 15/11 | Visages animés « comme dans Dune » (principe accepté par le joueur) : clignement, regard, bouche qui parle pendant les textes, transitions d'expression masquées par un clignement ; détail dans `REVUE_DA_V3.md` § 6 | (a) lot V3.1 complet après le test console (≈ 10 h) ; (b) clignement seul en V3 (≈ 4 h), le reste en V3.1 ; (c) tout en V3 | (a), ou (b) si le test console est repoussé | Rien de codé ; consigne md-dev prête | |
