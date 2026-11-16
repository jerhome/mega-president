# Écran Options — spécification DA (tâche 3.19)

Demande du joueur : « fais un bon settings, réfléchis bien ». Ce document est prêt pour un
`md-dev`. Il s'appuie sur `CHARTE_V3.md` (composants, palettes, grille), `REVUE_DA_V3.md` § 6 et 7
(visages animés, voix) et sur l'API XGM2 de SGDK 2.09, **vérifiée dans le conteneur**
(`inc/snd/xgm2.h` et `src/snd/xgm2.c`).

Principe directeur : **peu de réglages, tous utiles, tous à effet immédiat.** Un écran d'options
de console se juge en dix secondes. Six réglages et trois actions de données, pas de ligne
« pour faire riche ».

---

## 0. Ce que XGM2 permet vraiment (vérifié)

| Besoin | API | Verdict |
|---|---|---|
| Volume de la musique FM | `XGM2_setFMVolume(0..100)` : table logarithmique, pas de 0,75 dB (atténuation YM2612 de 0 à 127) | **Oui**, en crans |
| Volume de la musique PSG | `XGM2_setPSGVolume(0..100)` : pas de 2 dB (atténuation 0 à 15) | **Oui**, en crans |
| Volume des percussions de la musique (PCM canal 1) | aucune | **Non** : les fondus et les volumes ne touchent que FM et PSG |
| Volume des bruitages (PCM canal 2) et de la voix (canal 3) | aucune : `XGM2_playPCMEx` n'a pas de paramètre de volume | **Non** : marche/arrêt seulement (ou copies atténuées précalculées, voir § 1.2) |
| Volume mémorisé entre deux morceaux | `fmVol`/`psgVol` sont stockés ; `XGM2_fadeIn` remonte jusqu'à eux ; `restoreVolume` les rétablit à chaque `XGM2_play*` | **Oui**, le réglage survit aux changements de morceau |

Deux pièges à connaître :
1. **Volume changé pendant un fondu** : `doFade()` calcule sa cible au départ. Si le joueur change le
   volume pendant le fondu d'entrée de 250 ms, la dernière image du fondu réécrit l'ancien volume.
   Parade : `music_set_volume()` mémorise une demande, et `music_update()` la réapplique dès que
   `XGM2_isProcessingFade()` repasse à FALSE.
2. **Les percussions PCM ne baissent pas avec la musique.** À 100 %, rien ne change. À 25 %
   (−20 dB sur FM et PSG), la batterie resterait à plein volume et dominerait tout. Voir § 1.1.

---

## 1. Liste finale des réglages

### 1.1 Son

| Ligne | Valeurs | Défaut | Effet immédiat dans l'écran |
|---|---|---|---|
| **Musique** | Coupée · 25 % · 50 % · 75 % · 100 % (5 crans) | 100 % | le morceau en cours change de volume en direct |
| **Bruitages** | Tous · Essentiels · Aucun | Tous | un bruitage d'exemple (`SFX_OK`) quand on change de valeur ; le tic du curseur disparaît aussitôt en « Essentiels » |
| **Voix** | Oui · Non | Oui | passer à « Oui » joue « For sure ! » (canal 3, libre hors démarrage) |

**Crans de musique** : les pourcentages sont des **volumes perçus** (le volume perçu est divisé par deux
tous les −10 dB), pas des valeurs brutes de l'API. Correspondance à reproduire telle quelle :

| Cran | Libellé | Atténuation visée | `XGM2_setFMVolume` | `XGM2_setPSGVolume` |
|---|---|---|---|---|
| 0 | Coupée | silence | — : `music_set_enabled(FALSE)` | — |
| 1 | 25 % | −20 dB | 31 (27 pas × 0,75 = −20,3 dB) | 13 (10 pas × 2 = −20 dB) |
| 2 | 50 % | −10 dB | 46 (13 pas, −9,8 dB) | 35 (5 pas, −10 dB) |
| 3 | 75 % | −4,5 dB | 61 (6 pas, −4,5 dB) | 61 (2 pas, −4 dB) |
| 4 | 100 % | 0 dB | 100 | 100 |

FM et PSG reçoivent des valeurs différentes pour baisser **du même nombre de décibels** : avec une même
valeur, le PSG (basses et arpèges des thèmes d'année) baisserait beaucoup moins vite que la FM et
déséquilibrerait le mixage.

**Percussions aux crans 1 et 2** (le seul vrai problème technique de l'écran) :
- *Essai prioritaire (30 min)* : aux crans 1 et 2, occuper le canal PCM 1 avec un échantillon de
  silence de 256 octets joué **en boucle à la priorité 15** (`XGM2_playPCMEx(silence, 256,
  SOUND_PCM_CH1, 15, FALSE, TRUE)`). Si la musique joue ses percussions à la priorité 7 (valeur de
  `xgm2tool` à vérifier), elles ne remplacent plus le silence : la batterie se tait, la mélodie reste.
  On relâche le canal (`XGM2_stopPCM(SOUND_PCM_CH1)`) aux crans 3 et 4. À 25 et 50 %, une musique
  sans batterie reste agréable : c'est un « fond » discret, ce que cherche le joueur qui baisse le son.
- *Repli si la musique joue à la priorité 15* : supprimer le cran 25 % (crans Coupée · 50 · 75 · 100)
  et accepter une batterie un peu en avant à 50 % (−10 dB sur le reste). À écouter sur console
  avant de trancher ; le repli ne change ni l'écran ni la SRAM (le cran 1 est alors sauté).

**Bruitages « Essentiels »** : coupe `SFX_MOVE`, `SFX_OK` et `SFX_CANCEL` (les clics de menu, entendus
des milliers de fois), garde `SFX_ERROR`, `SFX_ADOPTED`, `SFX_REJECTED`, `SFX_EVENT` et les deux
jingles d'élection (les sons qui **informent**). C'est le vrai besoin derrière « baisser les
bruitages », sans ROM supplémentaire.
Rejeté : un volume des bruitages en crans par copies atténuées des échantillons (−6 dB). Coût
≈ +80 Ko de ROM par cran pour un gain faible ; à reconsidérer seulement si le test console montre
des bruitages trop forts par rapport à la musique (on corrigerait alors le mixage d'origine dans
`tools/music/pcm.py`, pas avec une option).

**Voix** : en V3, la ligne commande seulement « For sure ! » à l'allumage. En V4 (4.18), la même ligne
coupe le charabia des personnages (2 syllabes accueillantes) : aucun changement d'écran ni de format.
Si la V4 veut une valeur intermédiaire, elle l'ajoute (la SRAM stocke un octet).

### 1.2 Affichage

| Ligne | Valeurs | Défaut | Effet immédiat |
|---|---|---|---|
| **Visages** | Animés · Figés | Animés | l'huissier (§ 2) cligne ou se fige aussitôt |
| **Transitions** | Douces · Rapides · Aucune | Douces | l'écran rejoue son apparition douce avec la nouvelle durée |
| **Couleurs** | Standard · Daltonisme | Standard | les flèches d'exemple de l'aide changent de couleur |

- **Visages** (tâche 3.18) : « Figés » arrête `ui_gueule_anim_update()` (clignement, regard, bouche) et
  remet chaque gueule animée sur l'image de base de son expression. Les changements d'expression
  restent, sans clignement de transition. Le libellé « Animés / Figés » remplace « Visages animés :
  oui / non » : plus court (il tient dans la colonne) et plus parlant.
- **Transitions** : « Douces » = durées de la charte (120 / 220 / 150 ms) ; « Rapides » = moitié
  (60 / 110 / 75 ms) ; « Aucune » = palettes posées directement, sans `PAL_fade`. La coupure de
  l'affichage pendant l'init et le premier dessin est **conservée** dans les trois cas (sinon on verrait
  l'écran se construire). La manette reste bloquée pendant un fondu ; en « Aucune », il n'y a plus de
  blocage.
- **Couleurs** : seul réglage d'accessibilité retenu (justification au § 1.5). En « Daltonisme » :
  - PAL0 indices 6 et 7 (jauge A et reflet, flèches « favorables ») : vert (1,5,2)/(4,7,3) →
    **turquoise (0,4,6)/(3,7,7)**. Le turquoise reste distinct du rouge (6,1,1) pour les deutéranopes et
    les protanopes, et distinct du bleu de la jauge B (1,3,7) par sa luminosité.
  - Rampe de la carte des régions (`ui_map_set_region_level`) : rouge → jaune → vert devient
    **orangé (7,2,0) → ivoire (7,7,5) → bleu (1,3,7)**, rampe divergente lisible par tous.
  - Hémicycle, gueules, illustrations : inchangés (couleurs identifiées par leur position et leur légende).

### 1.3 Données

| Ligne | Action | Confirmation | Grisée si |
|---|---|---|---|
| **Effacer les records** | vide les 2 classements (`scores_reset` + `sram_scores_save`) | **double** | aucun record |
| **Effacer la partie** | `sram_game_erase()` | **double** | aucune sauvegarde, **ou écran ouvert depuis une partie** |
| **Réglages d'origine** | remet les 6 réglages à leur défaut et les applique | simple | tous les réglages sont déjà par défaut |

Effacer la sauvegarde pendant une partie n'a pas de sens (la partie en mémoire se ressauvegarde au
trimestre suivant) et serait dangereux : la ligne est grisée, l'aide explique pourquoi.

### 1.4 Pistes écartées (avec raison)

| Piste | Verdict | Raison |
|---|---|---|
| Marge TV (overscan) | **Rejetée** | Il faudrait une deuxième mise en page de tous les écrans. Le défilement du plan ne crée pas de marge, il décale le contenu. Le mode 240 lignes n'existe qu'en PAL. La vraie réponse est une **règle de charte**, déjà presque respectée : 38 colonnes utiles, et rien d'indispensable sur les rangées 0 et 27 (le bandeau de la rangée 0 est décoratif, son titre est en rangée 1). L'écran Options laisse lui-même la rangée 27 vide. |
| Confirmation des actions coûteuses (marche/arrêt) | **Rejetée** | Les confirmations existent déjà (49.3, dissolution, référendum, remaniement, retour au menu). Elles sont rares (quelques-unes par an de jeu) et protègent des actions irréversibles. Les désactiver ne fait gagner qu'une pression, au prix d'une dissolution accidentelle. |
| Afficher les aides et conseils | **Rejetée** | Le jeu n'a pas de conseils spontanés : l'aide est à la demande (C). Il n'y a rien à couper. |
| Vitesse de défilement du texte | **Rejetée en V3** | Aucun texte ne s'écrit lettre à lettre aujourd'hui. À rouvrir si un effet « machine à écrire » arrive. |
| Contraste du texte atténué | **Rejetée** | Le gris bleuté (5,6,7) sur bleu roi (1,2,4) est lisible, même en composite. Il sert aussi à griser les lignes indisponibles : l'éclaircir écraserait la hiérarchie titre / valeur / libellé / grisé. Si un test TV montre un problème, on corrige la charte pour tout le monde, pas avec une option. |
| Groupe « Jeu » | **Supprimé** | Une fois les trois pistes ci-dessus écartées, il resterait un groupe vide. Trois groupes bien remplis valent mieux que quatre dont un creux. |

### 1.5 Accessibilité : avis coût/utilité

- **Daltonisme : oui.** Environ 8 % des hommes confondent le rouge et le vert. Or la **carte des régions**
  est entièrement codée rouge → vert et les **flèches de tendance** du journal disent « bon / mauvais »
  seulement par la couleur (la direction ne suffit pas : une baisse du chômage est verte). Coût ≈ 1 h
  (2 entrées de palette, une rampe). C'est le meilleur rapport coût/utilité de tout l'écran.
- **Bruitages « Essentiels » et Transitions « Aucune »** profitent aussi aux joueurs sensibles au bruit
  répétitif ou aux clignotements, sans réglage dédié.
- Contraste et taille du texte : non (voir § 1.4 ; la police 8×8 à traits de 2 px est déjà le bon choix).

---

## 2. Maquette de l'écran (40 × 28 tuiles)

### 2.1 Une gueule ? Oui : l'huissier de l'Élysée

Un écran d'options est d'habitude le plus triste d'un jeu. Ici, il a un hôte : **Firmin Tisserand,
huissier de l'Élysée**, personnage fictif, sans fonction politique, qui « tient les boutons du
Palais ». Il coûte une gueule (≈ 120 tuiles en VRAM, ≈ 4 Ko de ROM) et rend l'écran mémorable.
Avec les visages animés, il cligne des yeux dans son médaillon : c'est aussi la démonstration en
direct du réglage « Visages ».

**Fiche pour `tools/art/` (gueule V3, 10 × 12 tuiles, PAL2, mêmes règles que les 30 autres)** :
- Sexagénaire impassible, **crâne dégarni** avec une couronne de cheveux gris plaqués, **grosse
  moustache blanche en guidon**, sourcils broussailleux qui débordent, nez fort, joues un peu
  couperosées.
- **Habit noir à queue-de-pie**, plastron et nœud papillon blancs, et surtout la **chaîne d'argent
  à médaille** posée sur les épaules : c'est la silhouette qu'on reconnaît d'un coup d'œil.
- Clin d'œil « service technique » : un **petit tournevis** qui dépasse de la poche de poitrine, à la
  place de la pochette. Une seule touche d'humour graphique, pas plus.
- Palette (15 couleurs) : noir, 3 gris de l'habit, 3 tons de peau, rose des joues, blanc et ivoire du
  plastron, 2 argents de la chaîne, gris acier du tournevis, contour, fond.
- Expressions : **neutre** (impassible, regard droit : l'expression par défaut), **content** (un coin de
  moustache qui se relève, un œil plissé : acquiescement), **gêné** (regard de côté, goutte de sueur :
  confirmations d'effacement), **furieux** (sourcils froncés ; générée pour la cohérence du système,
  pas utilisée sur cet écran).
- Emploi : neutre ; **content pendant 1,5 s après chaque changement de valeur**, avec retour au neutre
  par le clignement de transition (3.18) ; **gêné** pendant les boîtes « Effacer… ».
- Nom à vérifier : aucune personnalité réelle connue sous ce nom dans la vie publique ; le prénom
  désuet fait le personnage.

Sans gueule (lot A seul, § 7), la zone de droite reste vide et la colonne des valeurs glisse vers la
droite (§ 2.3). C'est propre, mais plus froid : je recommande le lot B.

### 2.2 Maquette (avec l'huissier)

```
     0         1         2         3
     0123456789012345678901234567890123456789
 0   ╭━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╮   bandeau ui_banner(0,0,40,"OPTIONS")
 1   ┃               OPTIONS                ┃   titre or, centré
 2   ╰━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╯   filet doré
 3   ╭──────────────────────────────────────╮   ui_window(0,3,40,16)
 4   │ ♪ SON ─────────────────────  ╔════════╗│  rubrique or + filet (x1-27) │ médaillon x29-38, y4-15
 5   │ ▶ Musique     ◀ ▰▰▰▱  75 % ▶ ║        ║│  ligne sélectionnée : barre x1-27, libellé or
 6   │   Bruitages      Essentiels  ║        ║│  libellé gris bleuté, valeur ivoire centrée x16-25
 7   │   Voix              Oui      ║ GUEULE ║│
 8   │                              ║ HUIS-  ║│
 9   │ ◉ AFFICHAGE ───────────────  ║ SIER   ║│
10   │   Visages          Animés    ║        ║│
11   │   Transitions      Douces    ║        ║│
12   │   Couleurs        Standard   ║        ║│
13   │                              ║        ║│
14   │ ▣ DONNÉES ─────────────────  ║        ║│
15   │   Effacer les records        ╚════════╝│
16   │   Effacer la partie          Tisserand │  légende x29-38 : nom ivoire, centré ; ligne grisée si indisponible
17   │   Réglages d'origine          huissier │  « huissier » gris bleuté, centré
18   ╰──────────────────────────────────────╯
19                                              rangée d'air (fond d'écran)
20   ╭──────────────────────────────────────╮   ui_window(0,20,40,7)
21   │ Volume de la musique, effet immédiat.│   aide de la ligne : ivoire, 1 à 2 lignes (21-22)
22   │                                      │
23   │  « La Garde républicaine vous salue. »│  réplique de l'huissier : gris bleuté, centrée
24   │                                      │
25   │   ◀▶ Gauche/Droite : régler  Ⓑ retour│   ui_footer, pastilles
26   ╰──────────────────────────────────────╯
27                                              rangée vide : marge de sécurité TV
```

### 2.3 Règles de mise en page

| Élément | Position | Style |
|---|---|---|
| Bandeau | `ui_banner(0, 0, 40, "OPTIONS", FALSE)` | titre or centré (charte § 4) |
| Fenêtre des réglages | `ui_window(0, 3, 40, 16)`, rangées 3-18 | — |
| Rubriques | rangées 4, 9, 14 : icône de groupe x1, titre x3 en `UI_STYLE_HIGHLIGHT` (MAJUSCULES), puis `ui_separator` jusqu'à x27 | or + filet |
| Lignes de réglage | rangées 5-7, 10-12, 15-17 ; curseur x1, libellé x3-13 | `UI_STYLE_DIM`, or (`HIGHLIGHT`) sur la ligne sélectionnée |
| Barre de sélection | x1-27, la même que `ui_list_draw` (arrondie, reflet) | — |
| Zone de valeur | x16-25 (10 colonnes), valeur centrée | `UI_STYLE_TEXT` (ivoire) |
| Flèches ◀ ▶ | x15 et x26, **seulement sur la ligne sélectionnée** ; la flèche disparaît en butée (pas de bouclage) | icônes or |
| Volume de musique | jauge or de 4 tuiles (x16-19, `ui_gauge_draw`, valeur = cran × 25) + « 75 % » (x21-25) ; cran 0 : « Coupée » centré, sans jauge | jauge or, texte ivoire |
| Lignes de données | libellé x3-26, pas de valeur ; grisées en `UI_STYLE_DIM` sans barre possible (le curseur les saute) | — |
| Médaillon | `ui_medallion(29, 4, 10, 12)` + `ui_gueule_draw(GUEULE_HUISSIER, …, 29, 4, PAL2)` | charte § 6 |
| Légende | rangée 16 « Tisserand » (ivoire), rangée 17 « huissier » (gris bleuté), centrées sur x29-38 | — |
| Fenêtre d'aide | `ui_window(0, 20, 40, 7)`, rangées 20-26 | — |
| Aide de la ligne | rangées 21-22, x1-38, `ui_text_wrap` | ivoire |
| Réplique | rangée 23, centrée, entre « » | gris bleuté |
| Pied | rangée 25 : `ui_footer(1, 25, 38, …)` | pastilles |
| Rangée 27 | vide | marge TV |

**Variante sans gueule (lot A)** : rubriques et filets jusqu'à x38, zone de valeur x26-35, flèches x25
et x36, barre x1-38. Rien d'autre ne bouge.

**Nouvelles icônes (`tools/ui/make_ui.py`, 5 tuiles, après `UI_ICON_DOWN_GOOD`)** : `UI_ICON_ARROW_LEFT`
et `UI_ICON_ARROW_RIGHT` (triangles or, obtenus par rotation de `ARROW_UP`/`ARROW_DOWN`), `UI_ICON_NOTE`
(croche, ivoire), `UI_ICON_EYE` (œil, ivoire), `UI_ICON_CART` (cartouche Megadrive vue de face, gris et
noir ; c'est là que les données sont rangées). Budget VRAM : 32 → 37 icônes, +5 tuiles.

### 2.4 Textes (aide et réplique, 38 colonnes au plus)

| Ligne | Aide (ivoire) | Réplique de l'huissier (gris bleuté) |
|---|---|---|
| Musique | Volume de la musique, effet immédiat. | « La Garde républicaine vous salue. » ; à « Coupée » : « Le silence. Comme après un 49.3. » |
| Bruitages | Essentiels : sans les clics de menu, avec les sons qui informent (votes, événements, erreurs). | « Le parquet craque. C'est d'époque. » |
| Voix | « For sure ! » au démarrage (et les voix des personnages dans une version future). | « On ne coupe pas la parole. Si ? » |
| Visages | Clignements et regards des portraits. | « Ici, les portraits vous épient. » |
| Transitions | Durée des fondus entre les écrans. | « Tout est dans la mise en scène. » |
| Couleurs | rangée 21 : « Carte et jauges lisibles par tous. » ; rangée 22 : « ▲ favorable   ▼ défavorable » (icônes `UP_GOOD`, `DOWN_BAD`) | « Chacun sa couleur. Même le centre. » |
| Effacer les records | Vide les deux classements (1er mandat et 10 ans). | « L'Histoire retiendra. Ou pas. » |
| Effacer la partie | Supprime le mandat sauvegardé. Grisée : « Aucune partie sauvegardée. » ou « Impossible pendant une partie. » | « Comme une promesse électorale. » |
| Réglages d'origine | Remet le son et l'affichage comme au premier jour. | « On revient à la Constitution. » |

La ligne « Couleurs » occupe 2 lignes d'aide : la seconde montre les deux icônes d'exemple, dont la
couleur change en direct. **Longueur** : chaque réplique tient en 38 colonnes guillemets et espaces
insécables compris (vérifié pour toutes) ; une aide de plus de 38 colonnes passe sur la rangée 22.

Pieds de page : lignes de réglage « Gauche/Droite : régler   B : retour » ; lignes de données
« A : effacer   B : retour » (« A : rétablir » pour la dernière ligne).

**Boîtes de confirmation** (`ui_msgbox_open`, choix dans l'ordre habituel action puis « Annuler »,
mais **sélection initiale sur « Annuler »** ; voir l'API, § 5) :

| Étape | Titre | Texte | Choix |
|---|---|---|---|
| Records 1 | Effacer les meilleurs scores ? | Les deux classements seront vidés. | Effacer · Annuler |
| Records 2 | Vraiment ? | C'est définitif. Même le Conseil constitutionnel n'y pourra rien. | Oui, effacer · Annuler |
| Partie 1 | Effacer la partie sauvegardée ? | Le mandat en cours sera perdu. Les meilleurs scores ne changent pas. | Effacer · Annuler |
| Partie 2 | Vraiment ? | Aucun recours possible. Pas même un référendum. | Oui, effacer · Annuler |
| Origine | Rétablir les réglages d'origine ? | Musique, bruitages, voix et affichage reprennent leurs valeurs par défaut. | Rétablir · Annuler |
| Après effacement | — | C'est fait. Je n'ai rien vu, rien entendu. | OK |

---

## 3. Ergonomie

- **Haut/Bas** : ligne précédente ou suivante. Le curseur saute les rubriques et les lignes grisées. Il ne
  boucle pas (comme les autres listes du jeu). Répétition de la manette habituelle (330 ms puis 100 ms).
  `SFX_MOVE` à chaque déplacement (donc muet en « Essentiels » : le réglage se démontre tout seul).
- **Gauche/Droite** : valeur précédente ou suivante, **appliquée aussitôt** (`settings_set` puis
  `settings_apply`). Sans effet en butée, sans bruitage d'erreur. La répétition est active sur la ligne
  Musique uniquement (on balaie les crans en maintenant).
- Retours sonores : Musique, aucun (la musique est l'exemple) ; Bruitages, `SFX_OK` joué **après**
  l'application (donc rien en « Aucun ») ; Voix, « For sure ! » au passage à « Oui » ; les autres
  lignes, `SFX_MOVE`.
- **A** : sur une ligne de données, ouvre la confirmation ; sur une ligne de réglage, passe à la valeur
  suivante (confort du joueur qui n'utilise que A).
- **B** ou **START** : retour à l'écran appelant, avec écriture en SRAM si un réglage a changé.
- **C** : sans effet (l'aide est déjà à l'écran).
- **Musique de l'écran** : aucune musique propre. `music_for_screen(SCREEN_OPTIONS)` ne fait rien et le
  morceau de l'écran appelant continue (thème du titre ou de l'année). On règle donc le volume sur la
  musique qu'on entend vraiment en jouant.

---

## 4. Accès

### 4.1 Depuis l'écran titre (manette 3 boutons)

Les quatre boutons sont pris (START et A : nouvelle partie, B : continuer, C : meilleurs scores), et les
directions ne font rien. Ajouter un bouton caché (« Bas : options ») serait introuvable. **Décision :
la phase « Appuyez sur START » devient un petit menu d'accueil de 4 lignes, et les raccourcis
existants restent tous valables.**

```
17            Élu avec 51 %.                    accroche (inchangée)
18   Les 49 autres vous attendent dehors.
19
20          ▶ Nouvelle partie        [START]    liste x9-30, barre de sélection
21            Continuer la partie      [B]      grisée (gris bleuté) sans sauvegarde
22            Meilleurs scores         [C]
23            Options
24    ──────────────────────────────────────    filet (inchangé)
25          <auteur> - <version>                (inchangé)
26          Développé avec Claude               (inchangé)
```

- START : nouvelle partie, B : continuer, C : meilleurs scores, **comme avant, depuis n'importe quelle
  ligne**. Haut/Bas déplace la barre, A valide la ligne. La sélection initiale est « Nouvelle
  partie », donc **A seul garde exactement son effet actuel**. Rien n'est cassé.
- Les pastilles START, B et C sont dessinées à droite des lignes (icônes `BTN_*`, x29-30 ; START tient
  sur 2 tuiles si son icône est double, sinon x30). Elles **enseignent** les raccourcis.
- Le clignotement à 500 ms est conservé, sur la pastille START seulement : le clin d'œil arcade
  reste, sans faire clignoter une ligne entière.
- Cette liste remplace les rangées 20 (« START : nouvelle partie » clignotant) et 22 (pied de boutons).
- Retour des options : écran titre, phase menu d'accueil, barre sur « Options ».

### 4.2 Depuis le menu de jeu

- `menu_build()` (`scr_game.c`) : ligne **« Options »** juste **avant** « Menu principal (partie
  sauvegardée) », toujours disponible, action `A_OPTIONS`.
- `options_open(SCREEN_GAME)`, sur le modèle de `map_open` et `help_open`.
- Au retour, la barre du menu doit revenir sur la ligne « Options », et non en tête : `game_init()` remet
  aujourd'hui `menu_sel` à 0. Ajouter un drapeau de retour (`game_menu_return_to(A_OPTIONS)` ou
  équivalent) lu par `game_init`.
- Depuis la partie, « Effacer la partie » est grisée (§ 1.3).

---

## 5. Format SRAM

Les scores occupent les octets 0 à 168 (`SCORES_BLOB_SIZE` = 5 + 2 × 82 = 169). L'accroche du titre est
à l'octet 254, la partie commence à 256. **Bloc des réglages : 16 octets, de 224 à 239**
(`SRAM_SETTINGS_OFFSET 224`). Les octets 169 à 223 restent libres pour une évolution des scores, et
240 à 253 pour la suite.

| Octet (relatif) | Contenu |
|---|---|
| 0-1 | signature `'O' 'P'` |
| 2 | version du format : `SETTINGS_VERSION 1` |
| 3 | nombre de valeurs stockées : `SET_COUNT` (6 en V3) |
| 4-11 | valeurs, un octet par réglage, dans l'ordre de `SettingId` (octets inutilisés à 0) |
| 12-13 | réservés, 0 |
| 14-15 | somme de contrôle u16 gros-boutiste : somme des octets 0 à 13, puis XOR 0x5AA5 (une SRAM remplie de 0 ne donne pas une somme valide) |

Lecture (`settings_deserialize`) :
- Signature ou somme fausse (SRAM vierge à 0xFF, à 0x00, ou corrompue) : **tous les défauts**.
- Version plus récente que celle du code : on lit les valeurs connues et on les borne.
- **Chaque valeur hors bornes reprend son défaut, seule** : une valeur folle ne réinitialise pas les
  autres.
- Le nombre de valeurs (octet 3) permet d'ajouter des réglages en V4 sans changer de version : un
  réglage absent prend son défaut.

Écriture : en sortie d'écran si quelque chose a changé, et après « Réglages d'origine ». Jamais à
chaque appui. L'octet 254 n'est ni lu ni écrit par ce code.

---

## 6. API à exposer

### 6.1 Cœur portable — `src/save/settings.h` / `settings.c` (sans SGDK, testé sur PC)

```c
typedef enum {
    SET_MUSIC = 0,  // 0 coupée, 1..4 = 25/50/75/100 %   (défaut 4)
    SET_SFX,        // 0 tous, 1 essentiels, 2 aucun      (défaut 0)
    SET_VOICE,      // 0 non, 1 oui                       (défaut 1)
    SET_FACES,      // 0 figés, 1 animés                  (défaut 1)
    SET_FADES,      // 0 douces, 1 rapides, 2 aucune      (défaut 0)
    SET_COLORS,     // 0 standard, 1 daltonisme           (défaut 0)
    SET_COUNT
} SettingId;

#define SETTINGS_BLOB_SIZE 16
u8   settings_get(SettingId id);
void settings_set(SettingId id, u8 v);     // borne la valeur, marque « modifié »
u8   settings_count(SettingId id);         // nombre de valeurs possibles
u8   settings_default(SettingId id);
void settings_reset(void);                 // tous les défauts
bool settings_all_default(void);
bool settings_dirty(void);  void settings_clear_dirty(void);
u16  settings_serialize(u8 *buf);          // SETTINGS_BLOB_SIZE octets
u8   settings_deserialize(const u8 *buf, u16 len); // 0 sain, 1 valeurs corrigées, 2 tout réinitialisé ; buf NULL : défauts
```
RAM : 6 octets de valeurs + 1 drapeau.

### 6.2 Console

- `src/save/sram.c/.h` : `sram_settings_load()`, `sram_settings_save()`, `SRAM_SETTINGS_OFFSET 224`.
- `src/save/settings_apply.c/.h` : `settings_apply(SettingId)` et `settings_apply_all()`, seuls
  appelants des fonctions ci-dessous.
- `src/audio/music.c/.h` : `void music_set_volume(u8 cran)` (0 = `music_set_enabled(FALSE)`, 1..4 =
  table du § 1.1 ; demande réappliquée à la fin d'un fondu en cours ; silence de priorité 15 sur le canal 1
  aux crans 1-2 si l'essai du § 1.1 est concluant).
- `src/audio/sfx.c/.h` : `void sfx_set_mode(u8 mode)` (`SFX_MODE_ALL`, `SFX_MODE_ESSENTIAL`,
  `SFX_MODE_OFF`) ; `sfx_set_enabled` reste comme raccourci.
- `src/audio/voice.c/.h` : `void voice_set_enabled(bool)` (`voice_play_intro_once` renvoie FALSE si
  coupée) et `void voice_preview(void)` (« For sure ! » à la demande, canal 3).
- `src/ui/ui.c/.h` : `void ui_set_fade_speed(u8 s)` (0 : durées de la charte, 1 : moitié, 2 : aucune) ;
  `void ui_set_colorblind(bool)` (réécrit PAL0 6-7 après chaque `PAL_setPalette(PAL0, …)` des
  lignes 50 et 160 de `ui.c` et prévient `ui_map`) ; `void ui_soft_replay(void)` (rejoue l'apparition
  douce, pour l'aperçu des transitions).
- `src/ui/ui_map.c` : seconde rampe selon le mode couleurs.
- `src/ui/ui_art.c/.h` (API de 3.18) : `void ui_gueule_anim_enable(bool)`.
- `src/ui/ui_widgets.c/.h` : `void ui_msgbox_set_sel(u16 i)` (sélection initiale, à appeler juste après
  `ui_msgbox_open`) et `void ui_selbar_draw(x, y, w, bool on)` (la barre de `ui_list_draw`, exposée pour
  une liste à trous).
- `src/screens/scr_options.c` + `options_open(ScreenId return_to)` ; `SCREEN_OPTIONS` dans
  `ScreenId`, enregistré dans `main.c`.
- **Démarrage** : dans `main()` (`main.c`, avant `screen_run()`), juste après `sfx_init()` et `music_init()` et
  **avant** l'init du premier écran : `sram_settings_load()` puis `settings_apply_all()`. Sinon « For
  sure ! » partirait avant la lecture du réglage Voix.

Le cœur de simulation (`src/sim/`) n'est pas touché. Les réglages ne font pas partie de la sauvegarde de
partie : ils appartiennent à la console, pas au mandat.

---

## 7. Critères d'acceptation

1. `make check` vert ; nouveau test PC `tests/test_settings.c` : défauts sur un bloc à 0xFF, à 0x00 et
   `NULL` ; aller-retour sérialisation ; somme fausse ⇒ défauts ; une valeur hors bornes ⇒ elle seule
   reprend son défaut ; octet 3 < `SET_COUNT` ⇒ réglages manquants par défaut.
2. SRAM : après un effacement des records ou un changement de réglage, l'octet 254 et les octets 256 et
   suivants sont **identiques** (vidage SRAM avant/après dans BlastEm).
3. Musique : les 5 crans s'entendent nettement différents ; le volume se règle **pendant** un fondu
   d'entrée sans revenir à l'ancien ; le réglage survit à un changement de morceau (titre → année 1
   → journal) ; « Coupée » puis « 100 % » relance le bon morceau. Batterie traitée selon le § 1.1
   (essai noté dans `DECISIONS.md`).
4. Bruitages « Essentiels » : plus de tic au curseur ni de clic à la validation, mais l'arpège d'une loi
   adoptée et le jingle d'événement restent. « Aucun » : silence complet.
5. Voix « Non » : pas de « For sure ! » au démarrage suivant (réglage relu en SRAM), la musique du titre
   démarre sans attendre.
6. Visages « Figés » : aucune animation sur le tableau de bord, le journal et les événements ; les gueules
   restent sur l'image de base de leur expression.
7. Transitions : les trois valeurs mesurées au chronomètre de BlastEm, identiques en PAL et en NTSC ; en
   « Aucune », aucun écran n'apparaît en cours de construction.
8. Couleurs « Daltonisme » : carte, jauge de popularité et flèches de tendance conformes au § 1.2 ;
   capture relue par le DA (simulation de deutéranopie sur la capture, par exemple avec un filtre
   Python dans le scratchpad).
9. Titre : START, A (sans bouger), B et C font exactement ce qu'ils faisaient ; « Options » s'atteint
   par Bas ×3 puis A ; « Continuer » grisée sans sauvegarde.
10. Jeu : ligne « Options » avant « Menu principal » ; retour sur cette ligne ; « Effacer la partie »
    grisée.
11. Double confirmation pour les deux effacements, **sélection initiale sur « Annuler »** à chaque
    étape ; B annule à chaque étape.
12. Mise en page : captures PAL et NTSC (ROM de démonstration, charte § 9) relues par le DA ; aucun
    texte au-delà de la colonne 38 ; rangée 27 vide ; médaillon à x29, y4 ; flèches seulement sur la ligne
    sélectionnée.
13. Budgets : VRAM +5 tuiles d'icônes (+ gueule de l'huissier ≈ 120, déjà comprise dans la marge des
    illustrations) ; RAM statique +16 octets au plus ; ROM +6 Ko au plus (lot B compris).

---

## 8. Chiffrage

| Lot | Contenu | Durée |
|---|---|---|
| **A1** | `settings.c/.h` + test PC + SRAM + démarrage | 1 h |
| **A2** | Application : volume musique (table, fondu, essai batterie), mode bruitages, voix, transitions, daltonisme (palette + rampe), visages | 2,5 h |
| **A3** | `scr_options.c` : liste à trous, valeurs, jauge, aides, confirmations, retour ; 5 icônes | 2,5 h |
| **A4** | Accès : menu d'accueil du titre, ligne du menu de jeu, retour sur la ligne | 1 h |
| **A5** | Captures PAL/NTSC, relecture DA, retouches | 0,5 h |
| **B** | Gueule de l'huissier (fiche § 2.1, `tools/art/`, 4 expressions + images d'animation 3.18), planche relue, intégration | 2 h |
| | **Total A** | **≈ 7,5 h** |
| | **Total A + B (recommandé)** | **≈ 9,5 h** |

Le backlog prévoit 4 h : c'est insuffisant pour un écran « soigné ». **Version courte à 4,5 h** si le
temps presse : A1 + A2 sans daltonisme ni essai batterie (musique en Coupée · 50 · 75 · 100) + A3 sans
icônes de groupe + A4, sans huissier. Je déconseille de couper le daltonisme (1 h pour 8 % des joueurs)
et l'accès depuis le titre (sans lui, on ne peut pas baisser le son avant de jouer).

Ordre conseillé : A1 → A2 → A4 → A3 → B (l'huissier peut être dessiné en parallèle par un second agent
dès le début, dans `tools/art/` seulement : fichiers disjoints).

## 9. Points soumis au joueur

1. **L'huissier Firmin Tisserand** comme hôte de l'écran (lot B, +2 h) : recommandé.
2. **Menu d'accueil sur l'écran titre** (4 lignes, raccourcis conservés) : nécessaire pour l'accès sans
   bouton libre ; il remplace le « START : nouvelle partie » clignotant par une liste dont la pastille
   START clignote.
3. Groupe « Jeu » supprimé (rien d'utile à y mettre aujourd'hui).
