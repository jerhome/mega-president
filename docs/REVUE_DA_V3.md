# Revue artistique de la V3 (directeur artistique, 15/11)

Revue complète avant le test sur la console du joueur : 30 gueules (4 expressions), écran titre,
15 vignettes, 3 décors, 12 écrans capturés (`design_v3/v31/`, captures de 21 h 30, à jour à
une demi-heure près ; `v32/` était vide). Les captures plus anciennes (`design_v3/v3_*.png`) ont
encore les portraits V2 : elles servent d'historique et ne sont pas notées une par une.

**Priorités** : P1 = visible et gênant ; P2 = finition ; P3 = idée.
**Verdict** : ✅ montrable, ❌ à corriger avant de le montrer.

## Bilan

| Lot | Avant la revue | Après mes retouches |
|---|---|---|
| Gueules (30) | 24 ✅ / 6 ❌ | **30 ✅** (6 corrigées dans `tools/art/`) |
| Vignettes (15) | 14 ✅ / 1 ❌ | **15 ✅** (« FERMÉ » corrigé) |
| Décors (3) et illustration du titre | 4 ✅ | 4 ✅ |
| Écrans (12) | 5 ✅ / 7 ❌ | 5 ✅ / 7 ❌ (corrections dans `src/` : voir « Consignes pour md-dev ») |
| **Total (61)** | **47 ✅ / 14 ❌** | **54 ✅ / 7 ❌** |

Avis général : les gueules sont la grande réussite de la V3. Elles ont un vrai style, sculpté et
éclairé, avec des silhouettes qu'on reconnaît. Les vignettes ont le même ton et l'interface est
cohérente : bleu nuit, or, ivoire. Pour l'instant, les défauts qui comptent sont des défauts de mise en page :
débordements, pied de page collé, médaillon mal dimensionné. Il manque aussi un vrai logo sur l'écran titre.

Planche avant/après de mes retouches : `scratchpad/gueules_v3/planche_retouches_DA.png`.

## 1. Gueules

| # | Personnage | Verdict | Remarques |
|---|---|---|---|
| 1 | Antoine Verdier (pres_gauche) | ✅ | Tignasse en nuage et barbe de prof : la gueule-étendard du jeu. |
| 2 | Laurent Delmas (pres_centre) | ❌ → ✅ | **P1, corrigé** : le visage était fade, sans gag (« gendre idéal » générique) alors que c'est un président qu'on voit tout le temps. Ajouts : sourire de pub plus large, **éclat de dentifrice** en étoile sur les dents (neutre et content), fossettes. |
| 3 | Henri de Saint-Aubin (pres_droite) | ✅ | P2 : le « cou de taureau » ne se lit pas, et le visage est plat entre les tempes. On pourrait élargir le cou de 4 px et creuser les orbites. |
| 4 | Pierre Delaunay (pm_technocrate) | ✅ | Le badge sur la veste est une trouvaille. |
| 5 | Marianne Deschamps (pm_rassembleur) | ✅ | Le rouge aux joues de « furieux » fonctionne bien. |
| 6 | Julien Lestrade (pm_reformateur) | ✅ | La goutte de sueur est présente aussi en neutre : c'est voulu (le réformateur toujours sous pression), je garde ce gag. |
| 7 | Sophie Carrel (cohab_gauche) | ❌ → ✅ | **P1, corrigé** : on la prenait pour un homme. De plus, avec ses cheveux gris en pointes, son foulard rouge et sa veste kaki, elle était la jumelle d'Étienne Marchal. Ajouts : créoles rouges, rouge à lèvres (neutre), cils. |
| 8 | Bernard Lacombe (cohab_centre) | ✅ | |
| 9 | Philippe de Montclar (cohab_droite) | ✅ | Le monocle et la fine moustache : parfait. |
| 10 | Camille Rouvière (cand_gauche_radicale) | ✅ | P2 : en neutre, la bouche ouverte en « O » avec les dents fait « ahurie » plutôt que « tribun ». Il faudrait une bouche serrée avec un coin relevé. |
| 11 | Noémie Vasseur (cand_ecolo) | ✅ | P2 : les taches de rousseur font des points sombres qu'on prend pour de la saleté. Une teinte plus claire ou moins de points. |
| 12 | Hélène Marchetti (cand_centre) | ✅ | P3 : c'est la plus sage du casting. Une idée de gag : l'écharpe tricolore qui pend jusqu'au sol, ou un sourire « photo officielle » figé. |
| 13 | Édouard Valmont (cand_droite) | ✅ | Le double menton étagé est très drôle. |
| 14 | Gaspard Lenoir (cand_nationaliste) | ❌ → ✅ | **P1, corrigé** : son teint brun-orangé le faisait passer pour un personnage non blanc, alors que c'est le candidat d'extrême droite. La lecture était fausse et gênante, et il ressemblait à Nadia Benali. Il a maintenant le teint rougeaud d'un orateur de meeting et des yeux bleu glacier. La mâchoire de boxeur ne change pas. |
| 15 | Gilles Fontaine (min_economie) | ✅ | Le hibou aux grandes oreilles. |
| 16 | Régis Fermont (min_interieur) | ✅ | Le bouledogue en parpaing. |
| 17 | Odile Varenne (min_sante) | ❌ → ✅ | **P1, corrigé** : le stylo violet traversait la joue et ressemblait à un **coquard**. Il est maintenant planté dans le chignon, hors du visage, et les cheveux sont gris-blond au lieu de kaki. P2 restant : avec les lunettes relevées, la masse des cheveux se lit encore comme un casque d'aviateur. Il faudrait dégager le front de 3 px et ajouter deux mèches qui retombent. |
| 18 | Étienne Marchal (min_education) | ❌ → ✅ | **P1, corrigé** : c'était le jumeau de Carrel (voir 7). Il porte maintenant une veste en velours côtelé marron, plus « prof ». P2 : le stylo rouge à l'oreille, fin d'un pixel, se lit comme une entaille. Il faudrait l'épaissir à 2 px avec un capuchon blanc. |
| 19 | Maëlle Kervella (min_ecologie) | ✅ | |
| 20 | Louis de Valcourt (min_affaires_etr) | ✅ | |
| 21 | Yannick Brossard (chef_ugr) | ✅ | Le bûcheron à la chemise à carreaux : excellent. |
| 22 | Claire Aubert (chef_rsv) | ✅ | P2 : en neutre, un point rouge au coin gauche de la bouche fait penser à une blessure. |
| 23 | Gérard Poulain (chef_mjm) | ✅ | P2 : les favoris blonds se lisent comme un casque audio. Il faudrait les foncer et leur donner une texture de poils. |
| 24 | Aymeric Dubreuil-Castagnac (chef_utl) | ❌ → ✅ | **P1, corrigé** : il ressemblait trop à Saint-Aubin, du même parti (cheveux gris, costume marine, cravate rouge). Il porte maintenant un costume anthracite à fines rayures et une cravate prune. |
| 25 | Maxime Corbeau (chef_lso) | ✅ | |
| 26 | Karine Lopez (jour_chaine_info) | ✅ | Une des meilleures : le brushing et le bandeau « 24 ». |
| 27 | Hubert Lenglet (jour_editorialiste) | ✅ | P2 : le modelé est pâteux (taches d'ombre au hasard sur les joues) et la bouche est illisible en neutre. Il faudrait des seuils d'ombre plus francs et un trait de bouche plus marqué. |
| 28 | Gaston Mirebeau (senat_president) | ✅ | |
| 29 | Nadia Benali (synd_ust) | ✅ | |
| 30 | Charles-Henri Dumoulin (patron_upe) | ✅ | P3 : en furieux, la ligne rose sur le cou est sans objet. |

Cohérence du casting : après les retouches, aucune paire n'est plus confondable de loin.
Vigilance : il y a beaucoup de crânes chauves avec une couronne grise (Lacombe, Delaunay,
Poulain, Lenglet, Brossard). Leurs silhouettes restent distinctes (moustache, lunettes,
favoris, barbe), mais une future gueule ne devra plus être chauve.

## 2. Écran titre (illustration)

✅ Composition lisible et drôle : l'Élysée, le président qui salue, la pancarte « UST SMIC + »,
le « GRR ! », le CRS, le « 1 M€ » et la journaliste.
- P2 : le corps du président est une cloche brune sans bras gauche ni épaules : il faut marquer les épaules et la veste.
- P2 : les figurants ont la peau jaune ou verdâtre (« 1 M€ », « GRR ! »). Elle ne va pas avec les gueules : il faudrait prendre la peau rose de la palette.
- P2 : on ne sait pas qui dit « 1 M€ ». La queue de la bulle doit pointer vers le patron.
- Le vrai problème vient de l'écran, pas de l'illustration : voir § 4 (logo, drapeau caché).

## 3. Vignettes et décors

| Vignette | Verdict | Remarques |
|---|---|---|
| Grève | ✅ | |
| Canicule | ✅ | P2 : « 45° » rouge sur orange, peu contrasté. |
| Inondation | ✅ | Le canard et le parapluie : très bien. |
| Scandale | ✅ | P2 : le « E » de SCANDALE touche le bord. |
| Sommet | ✅ | |
| Bourse | ✅ | |
| Santé | ✅ | |
| Pollution | ✅ | P2 : l'usine brune à gauche n'est qu'une masse : il faut des cheminées et un toit en dents de scie. |
| Sécurité | ✅ | |
| Numérique | ✅ | P2 : on ne sait pas ce qu'est l'objet doré sous l'écran. |
| Éducation | ✅ | Accent de « ZÉRO » ajouté. P3 : « ZÉRO VU ! » est obscur ; « 0/20 ! » serait plus clair. |
| Énergie | ✅ | |
| Agriculture | ✅ | |
| Industrie | ❌ → ✅ | **P1, corrigé** : on lisait « FERME » (une ferme !) au lieu de « FERMÉ ». La police 3×5 n'a pas d'accent : je l'ai dessiné à la main. |
| Fête | ✅ | |
| Décor hémicycle | ✅ | |
| Décor urne | ✅ | |
| Bandeau du Quotidien | ✅ | P2 : « EDITION SPECIALE » sans accents (police 3×5). |

## 4. Écrans (captures `v31/`)

| Écran | Verdict | Corrections |
|---|---|---|
| Titre | ❌ | **P1** : le pied de page affiche « v2.1 » sur la V3. **P1** : le titre est un simple bandeau de texte en police d'interface, trop petit pour un écran titre, et il cache le haut du drapeau et le pigeon. Il faut un vrai logo (`A_VALIDER.md`). P2 : le bandeau en rangée 0 est en zone d'overscan. |
| Titre, choix du PM | ❌ | **P1** : sous le médaillon, un morceau de la journaliste (manche blanche) dépasse en rangée 14 et fait penser à une erreur d'affichage. |
| Tableau de bord | ✅ | P2 : « International » colle à sa jauge. P2 : la ligne « Pop 49,2 Moy 54,1 Ais 49,7 » est cryptique (« Ais »). P2 : les piliers n'ont pas de « % » alors que les autres valeurs en ont. |
| Journal (bilan) | ✅ | P2 : la moitié basse est vide (rangées 17 à 24). P2 : la flèche « → » entre les deux popularités se lit mal en rouge. |
| Événement | ❌ | **P1** : la vignette en paysage (8×6 tuiles) est posée dans un médaillon portrait de 10×12 : un grand cadre noir à moitié vide. Le cadre doit épouser l'image, comme le veut la charte. |
| Proposer une loi | ✅ | Propre et hiérarchisé. |
| Assemblée | ❌ | **P1** : « Union pour la Tradition Libérale » déborde jusqu'au cadre. **P1** : « Bloc 420 absolue » ne veut rien dire : le libellé est coupé en deux autour du nombre. P2 : « B retour » est collé à la dernière ligne. |
| Vote (hémicycle) | ✅ | P2 : « Effets dans 2 trimestres. » colle au pied de page. |
| Carte | ❌ | **P1** : la légende dit « Jaune / Rouge / Vert », mais la carte montre du jaune et de l'orange : on ne sait pas ce que veut dire l'orange. P2 : la région choisie ne se voit pas (pas de contour). P2 : pixel parasite jaunâtre en (120, 219). |
| Aide | ✅ | P3 : « high scores » → « meilleurs scores ». |
| Intervention télévisée | ❌ | **P1** : retours à la ligne erratiques (« Français ont » seul sur sa ligne, « audiences » isolé) alors que 27 colonnes sont disponibles. |
| Meilleurs scores | ❌ | **P1** : la ligne « 10 » chevauche le pied de page. P2 : les en-têtes « Score / Orient. » ne sont pas alignés sur les colonnes. |

Captures anciennes (`v3_*.png`), à revérifier sur la ROM actuelle : le journal de l'année 3
affichait « 0,0 → 50,7 » pour tous les piliers (valeurs précédentes à zéro ?). Sur la page
Sénat, tout le paragraphe est en rouge : seul le titre devrait l'être (P2).

**Marges de sécurité TV.** Tous les écrans posent leur bandeau en rangée 0 (y 0-23) et leur
pied de page en rangée 25 ou 26. Sur un téléviseur PAL à tube, les 8 à 12 premières lignes sont
souvent mangées. Je ne déplace rien à l'aveugle : **pendant le test console, le joueur vérifie que le texte du bandeau du haut et le pied de page sont entiers.**
S'ils sont rognés, on descend tous les écrans d'une rangée (consigne P2-6).

## 5. Consignes pour md-dev

Pour chaque consigne : les fichiers, ce qu'on attend et le critère d'acceptation. Toutes se vérifient
par une capture BlastEm (PAL et NTSC) relue par le DA.

**P1-1. Version au pied du titre**. Fichier : `src/version.h`.
Passer `GAME_VERSION` à « v3.0 » au moment de la livraison de la V3.
Critère : le pied de page de l'écran titre affiche « v3.0 ».

**P1-2. Choix du PM sur l'écran titre** (`src/screens/scr_title.c`).
Placer le médaillon du PM en x 29-38, y 3-14 : son bas doit toucher le bord supérieur du
panneau, sans rangée d'illustration visible en dessous. Si une rangée reste visible, effacer
le rectangle d'illustration x 28-39 sur la rangée concernée (`ui_art_erase` ou tuiles de fond).
Critère : aucun morceau de personnage coupé autour du médaillon.

**P1-3. Médaillon des vignettes d'événement** (`src/screens/scr_event.c`, `src/screens/evt_art.c`).
La vignette fait 8×6 tuiles. Utiliser `ui_medallion(x, y, 8, 6)` posé sur la vignette elle-même
(x 30-37). Centrer verticalement sur le bloc de texte (y 5-10 pour un texte de 6 à 8 lignes),
au lieu du cadre de gueule 10×12. Le texte garde ses 27 colonnes, ou passe à 28.
Critère : aucun fond noir dans le cadre, et le filet doré touche l'image sur les 4 côtés.

**P1-4. Assemblée** (`src/screens/scr_game.c`, vue Assemblée).
- Une ligne par groupe : `★/♦`, sigle en or, sièges, jauge de relation, valeur. Le nom complet
  du groupe ne s'affiche que pour la ligne sélectionnée, ou en note sous la liste (≤ 36 colonnes).
- En haut, à gauche « Majorité absolue » et sa valeur en dessous ; à droite « Bloc présidentiel »
  et sa valeur en dessous. Pas de libellé coupé autour d'un nombre.
- Une rangée vide avant le pied de page « B : retour ».
Critère : aucun caractère en colonne 39, et chaque libellé se lit d'un bloc.

**P1-5. Carte** (`src/screens/scr_map.c`, `src/ui/ui_map.c`).
La légende doit nommer exactement les couleurs affichées. Si l'orange représente la popularité
faible, écrire « Orange : faible », ou mieux, passer la teinte faible à un vrai rouge (7,2,1),
pour garder le code habituel rouge/jaune/vert. Pour la région sélectionnée, ajouter un contour
blanc ou clignotant (le clignotement existe déjà : vérifier qu'il est visible hors fondu).
P2 : trouver la tuile qui pose le pixel jaunâtre en (120, 219), rangée 27, colonne 15.
Critère : un joueur sait sans aide ce que veut dire chaque couleur.

**P1-6. Intervention télévisée** (`src/screens/scr_game.c` ou l'écran TV).
Le texte doit couler sur 27 colonnes avec la même fonction de retour à la ligne que l'événement.
Les coupures actuelles (12 à 16 caractères) font penser à une largeur comptée en octets UTF-8
ou à une mauvaise largeur. Critère : aucune ligne coupée alors que le mot suivant tenait.

**P1-7. Meilleurs scores** (`src/screens/scr_scores.c`).
Dix lignes de rang tous les 2 rangs à partir de la rangée 5, donc jusqu'à la rangée 23. Pied de page
en rangée 25 ou 26, avec au moins une rangée vide entre les deux. Aligner les en-têtes sur le
début des colonnes de valeurs. Critère : aucun chevauchement, même avec 10 noms remplis.

**P2-1. Tableau de bord** : libellé « Internat. », ou jauges décalées d'une colonne ;
remplacer « Pop / Moy / Ais » par « Populaire 49 · Moyen 54 · Aisé 50 » sur une ligne de
38 colonnes au plus ; mettre « % » aux piliers ou le retirer partout.
**P2-2. Vote** : une rangée vide entre « Effets dans… » et le pied de page.
**P2-3. Journal, page Sénat** : titre en rouge, corps en ivoire (`UI_STYLE_TEXT`).
**P2-4. Journal, bilan** : utiliser la moitié basse vide, par exemple en remontant le pied de page
ou en y mettant la manchette « à la une » de la page 1.
**P2-5. Titre** : le bandeau « MEGA PRÉSIDENT » en attendant le logo (voir `A_VALIDER.md`).
**P2-6. Overscan** : si le test console montre que la rangée 0 ou la rangée 27 est rognée, décaler
`ui_banner` en rangée 1 sur tous les écrans et vérifier qu'aucun pied de page ne dépasse la rangée 26.

## 6. Animer les visages ? (question du joueur, référence : *Dune*, Cryo 1992)

**Avis de la DA : oui, et c'est le meilleur rapport effet/coût qui reste en V3.** Dans *Dune*,
on ne retient pas une animation précise. On retient que les personnages sont **vivants** : ils
clignent des yeux, leurs lèvres bougent quand ils parlent, ils tournent un peu le regard.
C'est discret, irrégulier, et ça ne vole jamais la vedette au texte. Nos gueules sont des
caricatures fixes et très expressives : un battement de paupières suffit à les faire exister.
On garde l'intention de *Dune*, pas son style (pas de rotoscopie, pas de grands gestes de tête).

### Quelles animations, par ordre d'intérêt
1. **Clignement des yeux** : une image « yeux fermés » de 100 à 130 ms, plus une image
   « mi-clos » de 60 ms pour les grands yeux (Fontaine, Lopez, Lestrade). Intervalle
   **aléatoire de 2,5 à 6 s**, avec parfois un double clignement (1 fois sur 6). Deux gueules
   à l'écran ne clignent jamais ensemble.
2. **Bouche qui parle** pendant qu'un texte s'affiche ou reste « dit » par le personnage :
   2 images (mi-ouverte, ouverte) en alternance avec la bouche de l'expression. Cadence
   **irrégulière de 90 à 160 ms**, petite pause sur les espaces et la ponctuation si on suit le texte.
   Arrêt net à la fin du texte ou à l'appui sur A.
3. **Regard qui se déplace** : les iris vont sur le côté pendant 0,8 à 1,5 s, toutes les 8 à 15 s.
   Très peu cher, et c'est l'effet *Dune* le plus fort par rapport à son coût.
4. **Transition d'expression masquée par un clignement** : quand une gueule change d'expression
   (journal, élection), on passe par « yeux fermés » pendant 80 ms. Le changement paraît voulu
   et ne fait plus de saut.
5. **Petits accessoires en sprites matériels** (optionnel) : la goutte de sueur de « gêné » qui
   glisse de 4 px puis réapparaît (cycle de 1,5 s), l'éclat de dentifrice de Delmas qui
   scintille, la veine de « furieux » qui bat. Un sprite de 8×8 dans la palette de la gueule
   coûte une tuile et aucun redessin du plan.

**Quels écrans**
| Écran | Animations |
|---|---|
| Tableau de bord (PM) | clignement et regard seulement : le joueur y lit des chiffres |
| Journal (président, éditorialiste) | clignement ; transition d'expression par clignement |
| Événement, Intervention TV, Lopez à la télévision | bouche qui parle pendant l'apparition du texte, clignement |
| Élection 2nd tour (deux finalistes) | clignements désynchronisés ; le perdant regarde de côté |
| Remaniement, second mandat, choix du PM | clignement, regard |

**À éviter** : tout mouvement régulier comme un métronome ; deux gueules animées en même temps ;
la bouche qui bouge sans texte ; animer pendant les fondus ou sous une boîte de message ; faire
bouger la tête entière ou les épaules (redessin de tout le portrait, effet « pantin ») ; les
cycles de palette sur la peau. Règle d'or : **si on remarque l'animation, elle est trop forte.**

### Faisabilité Megadrive
- **Principe** : le même que `ui_gueule_expr`. On réécrit seulement un petit rectangle de tuiles
  du plan B (les yeux : environ 5×2 tuiles ; la bouche : environ 3×2 tuiles). Les tuiles sont chargées en VRAM
  avec la gueule, et une image d'animation ne coûte qu'une écriture de 6 à 10 mots de carte de
  tuiles : pas de DMA de tuiles, presque rien par image.
- **Génération (`tools/art/`)** : `gueules.py` sait déjà rendre une expression avec d'autres
  paramètres d'yeux et de bouche (`T.eye(open_=…)`, `look`, `T.mouth(kind=…)`). On ajoute des
  pseudo-expressions dérivées de chaque expression (`blink`, `look`, `talk1`, `talk2`).
  `make_art.py` compare l'image obtenue à l'image de l'expression et extrait le plus petit
  rectangle de tuiles qui change, comme pour les patches actuels. La palette reste celle du
  personnage (15 couleurs, aucune couleur en plus).
- **Coût par personnage** : clignement pour les 4 expressions (~4 × 10 tuiles), regard en
  neutre (~10), parole en neutre et content (2 × 2 × ~6 tuiles), soit **≈ 75 tuiles ≈ 2,4 Ko de ROM**,
  donc ≈ 72 Ko pour les 30 personnages. La ROM fait aujourd'hui 768 Ko sur 2 048 : c'est confortable.
  **En VRAM**, on ne charge que l'animation de l'expression affichée : ≈ 25 tuiles par gueule,
  ≈ 50 pour deux gueules, sur ≈ 1 040 tuiles libres.
- **Cadence** : tout se compte en millisecondes, avec la même horloge que le curseur animé
  (4 × 100 ms). Le rendu est donc identique en PAL et en NTSC. Les intervalles aléatoires viennent
  d'un **petit générateur à part, jamais du `rng` de la simulation**, pour que les parties restent
  reproductibles.
- **RAM** : 2 emplacements (PAL2, PAL3) de ~16 octets chacun.

### Consigne prête pour un md-dev (lot « gueules vivantes »)
1. **`tools/art/gueules.py`, `persos.py`, `make_art.py`** : pour chaque personnage, rendre
   `blink` (yeux fermés) pour les 4 expressions, `blink_half` en option (champ `half_blink=True`
   dans la définition), `look` (iris à droite, neutre) et `talk1`/`talk2` (neutre et content :
   bouche mi-ouverte, puis ouverte, sans changer les dents ni le rouge à lèvres du personnage).
   Extraire pour chacun le rectangle minimal par rapport à son expression. Écrire les PNG
   `gueule_<nom>_<expr>_<anim>.png` et compléter `GueuleDef` dans `ui_art_gueules.h` (rectangle et images).
   Planche de contrôle : `apercu_gueules.py --anim` (toutes les images de tous les personnages)
   pour la relecture DA.
2. **`src/ui/ui_art.c/.h`** : nouvelle API
   `ui_gueule_anim_start(slot, id, expr, x, y, pal)`, `ui_gueule_anim_talk(slot, bool)`,
   `ui_gueule_anim_expr(slot, expr)` (transition par clignement de 80 ms), `ui_gueule_anim_stop(slot)`,
   `ui_gueule_anim_update()` appelée une fois par image par la boucle d'écran. Elle doit être
   suspendue pendant les fondus et les boîtes de message, et ne jamais dépasser une écriture de
   rectangle par gueule et par image.
3. **Intégration** : les écrans du tableau ci-dessus. `ui_gueule_anim_talk(TRUE)` pendant
   l'apparition des textes d'événement et de télévision.
4. **Critères d'acceptation** : planche relue par le DA ; mêmes cadences en PAL et en NTSC
   (vérification au chronomètre sur 20 clignements) ; budget VRAM inchangé à ±60 tuiles près ;
   `make check` vert ; aucune animation visible pendant un fondu.

**Chiffrage** : générateur et planche 4 h, API `ui_art` 3 h, intégration sur 6 écrans 2 h, relecture DA et
retouches 1 h, soit **≈ 10 h** (+ 2 h si on ajoute les accessoires en sprites).
**Version** : je recommande une **V3.1**, lancée juste après le test console de la V3, pour ne
pas retarder la livraison. Si le test console est repoussé, le **clignement seul (≈ 4 h)** peut
entrer en V3 : c'est à lui seul 70 % de l'effet « vivant ».

## 7. Charabia à la Sims (question du joueur : « trois syllabes sans aucun sens, puis seulement les yeux »)

> **Décision du joueur (15/11) : charabia reporté en V4 (tâche 4.18).** En V3, les visages s'animent
> **sans son**, exactement selon le § 6 (tâche 3.18), y compris la bouche qui parle pendant
> l'apparition des textes. Ce § 7 est un **avis pour la V4** : il ne modifie pas le lot V3 en cours.
> L'API `ui_gueule_anim_talk(slot, bool)` du § 6 suffit à brancher le charabia plus tard.

**Avis de la DA : oui, c'est une très bonne idée, et c'est même le complément naturel du § 6.**
Le joueur a vu juste sur un point : une bouche qui remue en silence pendant tout un paragraphe
fait « pantin muet ». Une bouche qui s'agite **pendant un son**, puis se tait pendant qu'on lit,
fait « le personnage a parlé, maintenant je lis ce qu'il a dit ». C'est la convention des Sims,
d'*Animal Crossing* et d'*Undertale*, et elle vieillit très bien. **En V4, quand le son arrivera**, je propose de remplacer la « bouche
qui parle pendant tout l'affichage du texte » du § 6 par : **bouche pendant le charabia, puis
repos, avec seulement le clignement et le regard.** C'est plus juste et moins cher en
animation.

Réponse directe à la question complémentaire (**1 syllabe ou 3 ?**) : **un mélange.** Une
interjection d'une syllabe par défaut, et trois syllabes pour les moments forts. Comparaison au
§ 7.3.

### 7.1 Intérêt
- **Charme et gueules** : le son donne une **voix** à des caricatures qui n'ont que 4 expressions.
  Un « hmpf » grave et sec pour Fermont, un « ouiii ? » flûté et mielleux pour Dumoulin : on
  reconnaît le personnage avant de lire son nom. C'est exactement ce qu'on veut, des gueules mémorables.
- **Humour** : le charabia rend drôles des répliques sérieuses sans toucher au texte. Lenglet qui
  pérore en trois syllabes pompeuses au-dessus d'un éditorial accablant, Poulain qui fait
  « beuh… bah… boh » (ni pour ni contre), Lopez qui mitraille « tatata ! » à la moindre météo. C'est
  bon enfant, et la pointe politique reste dans les textes et le design des voix (le patron
  mielleux, la syndicaliste franche), jamais dans les mots puisqu'il n'y en a pas.
- **Risque d'agacement : réel**, c'est le point à soigner. Un jeu de gestion se joue longtemps,
  et le même son sur le même écran 200 fois devient vite insupportable. Les parades (variété,
  rareté, coupure par A, option de menu) sont détaillées au § 7.5. Avec elles, le risque est maîtrisé.

### 7.2 Design sonore
**Une voix par personnage, construite à partir de 6 familles.** Trente voix entièrement
différentes, ce serait trop long à régler et pas plus lisible à l'oreille. Six familles de
timbre, déclinées par personnage en hauteur, débit, mélodie et voyelles préférées, donnent
30 voix qu'on distingue sans tout refaire.

| Famille | Timbre | Personnages (exemples) | Signature |
|---|---|---|---|
| Ténor chaleureux | moyen-grave, rond, voyelles « o, a » | Verdier, Brossard, Mirebeau (plus lent, plus grave) | mélodie qui monte puis tombe, comme une promesse |
| Lisse | moyen, net, sans aspérité, « é, i » | Delmas, Marchetti, Poulain, Lacombe | mélodie plate, débit régulier, presque synthétique |
| Grave autoritaire | grave, sec, attaques dures « k, t, r » | Saint-Aubin, Fermont, Valmont, Lenoir | syllabes courtes, finale qui tombe, gros « hmpf » |
| Nasal ironique | moyen, nasal, « an, on, in » | Dubreuil-Castagnac, Montclar, Valcourt, Lenglet, Corbeau | finale qui remonte, petit rire d'une syllabe |
| Aigu vif | aigu, rapide, « i, a » | Lestrade, Kervella, Lopez, Vasseur (plus douce) | débit rapide, staccato, finale montante |
| Alto franc | moyen-grave, voix poitrinée, un peu rauque | Carrel, Rouvière, Benali, Varenne (lasse), Deschamps, Aubert | volume et attaque marqués, finale franche |

Cas à part : Delaunay et Fontaine (famille « lisse » mais **monotone**, débit d'énumération
comptable), Marchal (articulé à l'excès, syllabes détachées), Dumoulin (glissando mielleux, voyelles
longues).

**Quand les déclencher**
- **Uniquement à l'apparition d'une réplique attribuée à un personnage visible** : événement,
  conseil du PM, intervention TV, Lopez à la télévision, résultat d'élection, remaniement, cohabitation.
- **Jamais** sur le tableau de bord, les menus, l'Assemblée chiffre par chiffre, ni le journal (le
  journal est un texte écrit, on ne le « dit » pas ; l'éditorialiste peut avoir un petit « hm ! » à
  l'ouverture, au plus).
- **Une fois par boîte de texte**, au début. Rien à la page suivante de la même réplique.
- **Anti-radotage** : si le même personnage a déjà parlé il y a moins de 20 s, on joue seulement
  son interjection courte, ou rien une fois sur deux.
- **A coupe tout** : le son s'arrête et la bouche se ferme net.

**Enchaînement avec la musique et les bruitages**
- Le charabia passe **après** le bruitage d'apparition (jingle d'événement, « ding » TV) : 150 à
  250 ms de décalage, jamais en même temps.
- Pas d'atténuation de la musique pour une syllabe : on mixe le charabia assez fort (pointe à
  -3 dB) et dans le médium (300 Hz à 3 kHz), là où la musique FM laisse de la place. Pour les
  3 syllabes, une **légère baisse de la musique** (`XGM2_setFMVolume` et `XGM2_setPSGVolume`, -25 %
  pendant 0,6 s, remontée en 200 ms) est un plus, à tester sur console et à garder seulement si elle ne
  « pompe » pas à l'oreille.
- Le charabia ne doit jamais tomber pile sur un temps fort de percussion PCM (canal 1) ; en pratique,
  sur une boucle, c'est imperceptible. Ce n'est pas un critère bloquant.

### 7.3 Une syllabe ou trois ? Comparaison
| Critère | 1 syllabe (interjection) | 3 syllabes (charabia) |
|---|---|---|
| Exemples | Fermont « hmpf », Poulain « bah », Kervella « oh ! », Lenglet « hm-hm », Benali « ouais » | Lopez « ta-ta-tam ! », Lenglet « bou-ré-dam », Verdier « mo-na-ba » |
| Charme | Fort et immédiat : c'est un **tic de personnage**, comme dans la BD (« Rogntudju ») | Plus fort encore : on croit entendre une **phrase**, effet Sims complet |
| Agacement | Faible : 0,25 s, ça passe comme une ponctuation | Plus élevé : 0,5 à 0,7 s, et la redite se remarque plus |
| Lèvres | 1 image (bouche ouverte, propre à l'expression), 2 avec une mi-ouverte | 2 images (mi-ouverte, ouverte), alternées sur les syllabes, plus la fermeture sur « b, m, p » |
| ROM (13,3 kHz) | ≈ 3,3 Ko par interjection ; 2 variantes × 30 ≈ **200 Ko** | banque de 6 syllabes ≈ 12 Ko par perso ; × 30 ≈ **360 Ko** |
| ROM (6,65 kHz) | ≈ **100 Ko** | ≈ **180 Ko** |
| Production | ≈ 8 h au total | ≈ 12 h (enchaînement de syllabes, synchronisation plus fine) |

**Recommandation : le mélange.**
- **Par défaut, l'interjection d'une syllabe** (2 variantes par personnage, tirées au hasard) :
  conseils du PM, événements ordinaires, cohabitation, remaniement.
- **Trois syllabes pour les moments forts** : première apparition d'un personnage dans la partie,
  intervention TV, Lopez en direct, résultat d'élection, scandale, motion de censure, fin de mandat.
  C'est rare, donc ça reste un plaisir.
- Les 3 syllabes sont **tirées d'une banque de 6 syllabes** par personnage et enchaînées par le code
  (ordre aléatoire, sans répéter la même deux fois de suite) : 120 combinaisons, donc jamais deux
  fois la même « phrase » d'affilée, pour le prix de 6 échantillons.
- Mélange à **6,65 kHz** : ≈ 100 + 180 = **≈ 280 Ko**. Avec les visages animés (≈ 72 Ko), la ROM
  passe de 768 Ko à ≈ 1,1 Mo sur 2 Mo. **Budget à inscrire dans `tools/budget.sh` : 300 Ko de charabia
  au maximum.**

**Fréquence d'échantillonnage.** Attention, XGM2 ne lit que **deux cadences : 13,3 kHz ou
6,65 kHz** (paramètre `halfRate` de `XGM2_playPCMEx`). Le 8 et le 11 kHz n'existent pas avec ce
pilote. Je recommande **6,65 kHz** : la bande passante (jusqu'à 3,3 kHz) contient les deux premiers
formants, donc le timbre et la mélodie, et le côté « talkie-walkie » rend la parole un peu étouffée,
ce qui est exactement ce qu'on veut d'un charabia. Si l'écoute sur console montre que les voix aiguës
(Lopez, Kervella, Lestrade) deviennent ternes, on passe **ces voix-là seulement** à 13,3 kHz : le
drapeau est choisi par échantillon, ça ne coûte rien en code.

### 7.4 Synchronisation des lèvres
- **Principe** : le générateur exporte, avec chaque échantillon, une **table de temps** (début et fin
  de chaque noyau de voyelle, consonnes labiales « b, m, p »), en millisecondes. Le code ouvre la
  bouche sur les voyelles, la ferme sur les labiales et entre les syllabes, et la remet au repos à la
  fin de l'échantillon. Pas d'analyse du son sur la console, tout est précalculé.
- **Images de bouche** : **2 par expression affichée** (mi-ouverte, ouverte), ce sont les `talk1` et
  `talk2` du § 6. Règle : mi-ouverte sur les 40 premières ms d'une voyelle et sur les voyelles fermées
  (i, u, ou), ouverte sur a, o, é ; fermeture sur les labiales. Pour l'interjection d'une syllabe,
  `talk2` seul suffit, `talk1` en attaque est un bonus.
- On les rend pour **les 4 expressions** (et non plus neutre et content seulement, comme prévu au
  § 6), car « hmpf » se dit souvent avec la tête de « furieux » ou « gêné ». Coût : + 2 × 2 × ~6 tuiles
  par personnage, ≈ + 0,8 Ko, soit ≈ 24 Ko de ROM pour les 30. En VRAM, rien de plus : on ne charge
  que l'expression affichée.
- **Après le son** : bouche au repos, puis seulement le clignement et le regard, comme le propose le
  joueur. Le premier clignement vient 300 à 600 ms après la fin du son : c'est le petit « temps de
  réaction » qui rend le personnage vivant.
- Horloge : la même que le § 6 (millisecondes, `getTick`), donc la synchronisation est identique en
  PAL et en NTSC.

### 7.5 Production
**Outil recommandé : synthèse formantique maison en Python (`tools/voice/charabia.py`)**, plutôt
que SAPI.
- **Pourquoi pas SAPI** : une seule voix française disponible (Hortense), donc 30 personnages
  obtenus en changeant la hauteur, ce qui donne l'effet « disque accéléré ou ralenti » et des
  timbres trop proches. SAPI lit aussi des syllabes inventées comme du français : on risque des mots
  réels involontaires (« bite », « con », un nom propre). Enfin, cela suppose Windows et PowerShell,
  ce qui n'est pas reproductible dans le conteneur. SAPI reste bon pour une **réplique** précise
  (« For sure ! ») ; pour du charabia, il est inadapté.
- **Synthèse maison** : source glottique (impulsions avec légère irrégularité, souffle réglable),
  3 filtres de formants par voyelle (a, é, i, o, ou, an, on), consonnes simples (explosion pour
  p/t/k/b/d, bruit filtré pour f/ch/s, nasale pour m/n), enveloppe et courbe de hauteur par syllabe.
  Chaque personnage est une **fiche de paramètres** (`charabia_persos.py`) : hauteur de base,
  courbe mélodique, débit, souffle, nasalité, vibrato, voyelles et consonnes préférées, plus sa ou ses
  interjections. Graine fixe par personnage : **même entrée, même fichier**, octet pour octet.
- **100 % fictif et propre** : les syllabes sont tirées dans un alphabet inventé, et un **filtre de
  mots interdits** (liste de syllabes et de combinaisons à éviter, mots grossiers et noms réels)
  rejette les tirages douteux. Aucune voix réelle n'est échantillonnée.
- Sortie : PCM 8 bits signé à 6,65 kHz (ou 13,3 kHz), normalisé, pré-emphase légère et fondu de
  5 ms aux bords (pas de clic), comme `make_voice.py`. Fichiers `res/sfx/chb_<perso>_<n>.wav`, plus la
  table des temps en en-tête C générée. Cible `make voice`, et planche d'écoute
  `charabia.py --preview` (un WAV qui enchaîne les 30 personnages) pour la relecture.
- **Coût en ROM** (rappel) : 1 octet par échantillon, soit **≈ 1 000 octets par syllabe de 150 ms à
  6,65 kHz** (2 000 à 13,3 kHz). Mélange recommandé : ≈ 280 Ko à 6,65 kHz, ≈ 560 Ko tout à 13,3 kHz.
- **Canal PCM : le canal 3**. La voix « For sure ! » n'y joue qu'une fois, à l'allumage ; le canal
  est libre ensuite. Le canal 2 reste aux bruitages et le 1 aux percussions, ce qui évite qu'un
  « clic » de menu coupe une syllabe. Le module `voice.c` devient la porte d'entrée unique du canal 3
  (`voice_chb_play(perso, court/long)`, `voice_chb_stop()`), avec une file de 3 syllabes au plus et
  la même prudence que pour « For sure ! » (ne pas interroger le Z80 juste après le lancement).

### 7.6 À éviter
- La bouche qui bouge **sans son**, ou le son sans bouche (personnage hors écran : pas de charabia).
- Un charabia **à chaque écran** ou à chaque page de texte : une fois par réplique, et moins encore
  si le personnage vient de parler.
- Des voix **trop proches de vraies voix** ou des syllabes qui forment des mots : le filtre de mots
  interdits est obligatoire, et aucune imitation d'une personnalité réelle, même par le timbre ou un tic.
- Les **caricatures sonores lourdes** (accent régional moqué, voix de femme « hystérique », voix
  efféminée pour faire rire) : on fait rire avec le caractère du personnage, pas avec un cliché. Les
  femmes ne sont pas toutes aiguës : Carrel et Rouvière sont graves et rauques, c'est dans leurs fiches.
- **Changer la hauteur à l'exécution** pour varier : impossible proprement avec XGM2. La variété vient
  de la banque de syllabes et des variantes précalculées.
- Un volume plus fort que la musique de plus de 3 dB, et des syllabes de plus de 200 ms (on
  « entend » alors une voix ralentie et on cherche du sens).
- Le charabia pendant un fondu, sous une boîte système ou pendant la sauvegarde.
- **Pas d'option pour le couper** : il faut une ligne dans les options, « Voix : toutes / courtes
  seulement / aucune ».

### 7.7 Chiffrage et planning
| Poste | 1 syllabe seule | Mélange recommandé |
|---|---|---|
| Synthétiseur formantique et filtre de mots | 3 h | 3 h |
| 30 fiches de voix et réglage à l'oreille | 2 h | 3 h |
| Code : `voice_chb_*`, file de syllabes, tables de temps, option de menu | 1,5 h | 3 h |
| Synchronisation des lèvres avec `ui_gueule_anim` (+ images de parole pour les 4 expressions) | 0,5 h | 1,5 h |
| Intégration aux écrans, règles de déclenchement | 0,5 h | 1 h |
| Écoute sur console, relecture DA, retouches | 0,5 h | 1 h |
| **Total** | **≈ 8 h** | **≈ 12,5 h** |

**Planning : V4 (tâche 4.18)**, sur la base des visages animés de la V3 (tâche 3.18). Les images
`talk1`/`talk2` et `ui_gueule_anim_talk` existeront déjà ; il restera à rendre les images de parole
pour les 4 expressions, si la V3 ne les a faites qu'en neutre et content, et à piloter la bouche par
la table des temps au lieu du texte. **Recommandation V4 : le mélange (1 syllabe par défaut,
3 pour les moments forts), ≈ 12,5 h**, ou la syllabe seule (≈ 8 h) d'abord si la V4 est chargée :
l'architecture (banque, file, table des temps) accepte les 3 syllabes plus tard sans rien refaire.

## 8. Relecture des animations (tâche 3.18, visages animés sans son)

Relu le 15/11 sur les 30 personnages : 4 expressions × (clignement, regard, parole 1, parole 2),
agrandis ×3 à ×5, et les captures BlastEm (Lopez qui parle à la télévision, Delaunay qui cligne
au tableau de bord). Planche avant/après (hors dépôt) : `/tmp/claude-1000/-home-jerome-claude-mega-president/0a6fa057-997b-490b-8cbe-9637ee148545/scratchpad/gueules_v3/animations_avant_apres_DA.png`
(rangée du haut : avant ; du bas : après).

**Verdict global : ✅ montrable après mes retouches.** L'intention *Dune* est tenue : c'est
discret, la palette de chaque gueule est respectée (aucune couleur en plus) et les rectangles de
tuiles n'ont pas grandi (en-tête `ui_art_gueules.h` inchangé, ROM 896 Ko).

### 8.1 Ce qui va bien
- ✅ **Yeux fermés** : l'arc d'encre vers le bas et l'ombre de paupière lisent bien comme
  « paupière baissée », pas comme un sourire. Les lunettes, le monocle de Montclar, les lunettes
  rouges de Benali et l'ombre à paupières violette de Lopez restent en place : la gueule ne
  « saute » pas. Les expressions qui ont déjà les yeux plissés (content de Brossard, Varenne,
  Kervella, Deschamps, Dumoulin, Mirebeau) n'ont pas de clignement : c'est juste.
- ✅ **Regard de côté** : 2 px vers la droite, avec un reflet qui suit l'iris. C'est l'effet le plus
  vivant pour son coût, très réussi sur les grands yeux (Fontaine, Lestrade, Lopez).
- ✅ **Parole** : les bouches fermées (trait, moue, ondulée) s'ouvrent en petit ovale, puis en bouche
  ouverte avec les dents du haut ; le rouge à lèvres et les barbes restent ceux du personnage.
  La barbe de Verdier et la moustache de Marchal laissent bien voir l'ouverture.
- ✅ **Captures** : la bouche de Lopez à l'antenne et le clignement de Delaunay sont propres,
  sans déchet de tuile autour des rectangles.

### 8.2 Corrigé par la DA (`tools/art/traits.py`, `g_pouvoir.py`, `g_gouv.py`)
1. ❌→✅ **Clignement « apaisé »** : un furieux qui cligne perdait sa colère pendant 120 ms
   (paupières horizontales sous des sourcils froncés), un gêné son inquiétude. Le trait de
   l'œil fermé garde maintenant la moitié de la pente de l'expression (`_closed_eye(…, tilt)`).
   Visible sur Saint-Aubin, Montclar et tous les « furieux » à sourcils froncés.
2. ❌→✅ **« Parole 1 » invisible sur les bouches peu ouvertes** : Delmas, Lopez, Dumoulin
   (sourires de publicité ou de plateau, ouverture < 2,6) refermaient à peine leur bouche :
   4 à 8 pixels changeaient, l'œil ne voyait rien. Les deux images ouvrent maintenant davantage
   (+0,9 puis +2,0) ; les bouches déjà grandes ouvertes gardent l'ancien principe (moitié, puis
   +1,3).
3. ❌→✅ **Lestrade muet** : ses dents de lapin étaient redessinées par-dessus la bouche qui parle
   (4 pixels de différence). Quand il parle, les deux dents pendent maintenant dans l'ouverture,
   sans contour, et la bouche s'ouvre vraiment.
4. ❌→✅ **Étoiles de Fontaine** (content) : elles flottaient sur les paupières fermées et
   restaient au centre quand le regard partait à droite. Elles disparaissent au clignement et
   suivent le regard.

### 8.3 Restes et avis
- ❌ (connu, mineur) **Mi-clos de 60 ms** pour Fontaine, Lopez et Lestrade : non fait. Il adoucit
  le clignement des très grands yeux, mais on ne le remarque qu'au ralenti. **À faire en V3.1** :
  une 5e image `blink_half` (paupière à `open_ * 0,4`) sur les 3 gueules, environ 1 h, 12 tuiles
  par personnage. Pas bloquant pour le test console.
- ⚠ **Barbe de Verdier** : en parlant, la lèvre brune de l'expression neutre disparaît au profit de
  l'ouverture sombre. C'est lisible et acceptable ; je n'y touche pas.
- **Écran événement : faut-il une gueule qui parle ? Avis de la DA : non en V3, et pas sous cette
  forme en V3.1.** Le texte d'un événement est la voix du **narrateur** : c'est une situation,
  pas une réplique. Faire parler un ministre sur ce texte enfreindrait la règle du § 6 (« la
  bouche ne bouge que sur une réplique attribuée »). La vignette (8 × 6 tuiles, PAL3) est
  l'identité de l'événement : on la reconnaît en un coup d'œil, et la remplacer par une tête
  appauvrirait l'écran. Les ajouter côte à côte coûterait 10 colonnes de texte (26 → 16 colonnes,
  10 lignes, illisible pour les événements longs), ≈ 140 tuiles de VRAM et ≈ 6 h (table
  événement → ministre de tutelle, mise en page, PAL2).
  **Ce que je recommande à la place (V4, avec le charabia, ≈ 3 h)** : à la phase « Votre décision »,
  une réplique **attribuée** du ministre de tutelle sous l'écho du *Quotidien* (« Fermont :
  — Bien, Monsieur le Président. »), avec sa gueule en médaillon à la place de la vignette
  pendant 2 s. Correspondance proposée par vignette : canicule, pollution, énergie →
  Kervella (Écologie) ; bourse, industrie, numérique, agriculture → Fontaine (Économie) ;
  sécurité, scandale → Fermont (Intérieur) ; santé → Varenne ; éducation → Marchal ;
  sommet → Valcourt (Affaires étrangères) ; grève, fête → le Premier ministre en place.

## 9. Écran Options (tâche 4.22, relecture de SPEC_OPTIONS.md codée)

Captures relues : `titre_menu`, `options_pal`, `options_ntsc`, `options_daltonisme`,
`options_confirmation`, `jeu_retour_daltonisme`, `carte_daltonisme`, planches `huissier` et
`huissier_anim`, puis capture de contrôle après retouches (ROM de démonstration, DEMO=11).

### 9.1 Verdicts

| Élément | Verdict | Commentaire |
|---|---|---|
| Huissier Firmin Tisserand | ❌ puis ✅ après retouche DA | Avant : la couronne de cheveux était deux plaques argentées verticales, à lire comme un casque audio ; la moustache, une barre blanche plate (un os en travers de la bouche) ; la couperose, des mouchetures (acné) ; la chaîne, une couture pointillée ; la goutte de « gêné » flottait hors du crâne. Corrigé dans `tools/art/g_divers.py` (`huissier`) : deux touffes plaquées au-dessus et en arrière des oreilles, peignées vers l'arrière, oreilles dégagées ; moustache en guidon à deux ailes (raie centrale), branches qui tombent puis remontent ; rougeur nette en hachures sous les pommettes ; chaîne à maillons de 2 px ; coin de moustache relevé de « content » recollé à la pointe ; goutte sur la tempe. 162 tuiles de base, 73 tuiles d'animation. |
| Icônes `ARROW_LEFT`/`RIGHT`, `NOTE`, `EYE` | ✅ | Lisibles, bien calées. |
| Icône `CART` | ❌ puis ✅ | Elle se lisait comme une disquette ou un boîtier gris clair. Redessinée en cartouche Megadrive noire : stries de préhension en haut, étiquette blanche à bandeau bleu, épaulement du connecteur en bas (`tools/ui/make_ui.py`). |
| Mise en page | ✅ | Conforme à la maquette § 2.2 : médaillon x29 y4, légende 16-17, rangée 27 vide, rien au-delà de la colonne 38, flèches seulement sur la ligne active et absentes en butée. Point mineur accepté : les valeurs de 10 lettres (« Essentiels », « Daltonisme ») touchent leurs flèches ; c'est la largeur prévue. La boîte de confirmation remplace le bord bas de la fenêtre des réglages : acceptable (comportement standard de `ui_msgbox`). |
| Répliques | ✅ | Le ton est juste (« Le silence. Comme après un 49.3. », « Chacun sa couleur. Même le centre. », « Je n'ai rien vu, rien entendu. »). Retouche : l'aide de la ligne Voix ne promet plus une fonction future (« et plus tard les voix des personnages ») ; elle dit « « For sure ! » quand la console s'allume. ». La tâche 4.18 la remplacera par « « For sure ! » à l'allumage et le charabia des personnages. ». |
| Daltonisme | ✅ | Simulation deutéranope et protanope (matrices de Machado) sur les couleurs VDP : favorable turquoise (0,4,6) → bleu, défavorable rouge (6,1,1) → brun foncé, sans confusion possible ; carte orangé / jaune / bleu → brun / jaune clair / bleu, trois niveaux distincts par la teinte et la luminosité. Le jaune central gardé à la place de l'ivoire de la spec est le bon choix (le blanc marque la région choisie). Le turquoise et le bleu de la jauge B sont proches pour un deutéranope, mais ils ne portent pas de sens opposé et sont toujours étiquetés. |
| Menu d'accueil du titre | ✅ | 4 lignes, pastilles des raccourcis, « Continuer » grisée sans sauvegarde. |

### 9.2 Lignes « Données » grisées : décision

**Elles deviennent sélectionnables mais inactives.** Une ligne grise que le curseur saute est une
porte fermée sans écriteau : le joueur qui veut effacer sa partie depuis le jeu ne comprend pas
pourquoi rien ne se passe. C'est d'ailleurs le seul endroit où l'aide est vraiment utile. Le coût
est nul en VRAM et en RAM.

### 9.3 Consigne `md-dev` : lignes inactives (src/screens/scr_options.c seulement)

1. `next_row()` ne saute plus les lignes grisées : les 9 lignes sont toutes atteignables (les
   rubriques restent hors liste). `fix_cursor()` devient inutile (le curseur reste où il est après
   un effacement, sur la ligne désormais grisée, ce qui montre le résultat).
2. `draw_row()` : une ligne de données inactive **sélectionnée** reçoit la barre et le curseur, mais
   son libellé reste en `UI_STYLE_DIM` (gris sur la barre : « on est là, mais c'est fermé »).
   Inactive et non sélectionnée : `UI_STYLE_DIM`, comme aujourd'hui.
3. `draw_help()`, ligne inactive : rangée 21 = la raison (ivoire), rangée 23 = réplique (gris bleuté),
   pied « B : retour » seul.

   | Ligne et cause | Raison (rangée 21) | Réplique |
   |---|---|---|
   | Records, aucun record | Aucun record pour l'instant. | « Page blanche. L'Histoire attend. » |
   | Partie, ouvert depuis le jeu | Impossible pendant une partie. | « Pas pendant le mandat, voyons. » |
   | Partie, aucune sauvegarde | Aucune partie sauvegardée. | « Rien à effacer. Tout est en ordre. » |
   | Réglages d'origine, déjà par défaut | Les réglages sont déjà ceux d'origine. | « Tout est d'origine. Même moi. » |

   La cause « depuis le jeu » prime sur « aucune sauvegarde » (tester `return_screen` d'abord).
4. **A** sur une ligne inactive : `SFX_ERROR` (gardé en mode « Essentiels », c'est un son qui
   informe), aucune boîte, l'huissier reste neutre.
5. Critères : depuis le titre sans sauvegarde ni record, Bas depuis « Couleurs » passe sur les trois
   lignes de données, chacune affiche sa raison et sa réplique ; depuis le jeu, « Effacer la partie »
   affiche « Impossible pendant une partie. » ; après « Réglages d'origine » validé, le curseur reste
   sur la ligne, grisée, avec sa raison ; aucun texte au-delà de la colonne 38 ; capture relue par le
   DA.
