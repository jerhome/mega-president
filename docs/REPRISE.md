# Point de reprise (16/11, nuit)

À lire en premier après un `/clear`. Puis recréer la tâche planifiée de suivi (mémoire feedback-suivi-planifie).

## Branches
- `v4-dev` : **V4 figée pour le test console du joueur** (version affichée v4.0). ROM : `~/mega-president-roms/mega-president-v4.0-test.bin` ; liste à tester : `docs/TEST_CONSOLE_V4.md` (copie à côté de la ROM). Reste la tâche 4.17 : corriger les retours du joueur sur v4-dev, étiquette v4.0, release GitHub. **Pas de push sans accord.**
- `v4.1-dev` : V4.1 en cours (relecture et factorisation, sans assembleur), plan dans `docs/AUDIT_V4_1.md` § 6. Toute modification issue de l'audit va en V4.1, jamais en V4 (demande du joueur).
- Après chaque correction sur v4-dev : `git merge v4-dev` dans v4.1-dev (worktree de travail : scratchpad `v41`, sinon `git worktree add`). Vérifier `git merge-base --is-ancestor v4-dev v4.1-dev`.
- `main` = V3 publiée (release v3.0 sur GitHub le 15/11).

## V4.1 : état des lots (AUDIT_V4_1 § 6)
- ✅ U1 StrBuf (`src/util/strbuf`), fusionné dans v4.1-dev.
- ✅ R4 mesures (`make rom-perf`, `docs/MESURES_V4_1.md`) : seul l'hémicycle perd des images (20-25 ms) ; titre avec sauvegarde, options et destitution pas encore mesurés.
- ✅ R1 intégrité des tables : vignettes générées (les 6 événements V4 ont la bonne image et le bon ministre), tests des répliques, `_Static_assert`, loi en attente non reproposable au 2nd mandat. Fusionné dans v4.1-dev (139 814 vérifications).
- ✅ U2 widgets (src/ui, documentés dans src/ui/README.md « Widgets réutilisables » ; `ui_confirm_update` : 1 = oui, 0 = non), fusionné dans v4.1-dev.
- ✅ U3 kit d'écran (scr_kit.[ch], pm_name/pillar_name bornés dans scr_txt.h, art_reset_quiet), fusionné dans v4.1-dev.
- ✅ R2 sauvegarde (format v3 : empreinte, `game_sanitize`, 2 emplacements, Fletcher différé ; `Game` dans `src/game/game_state.h`). Une sauvegarde V4 (v2) sera refusée par la V4.1 (message « ancienne version »). Fusionné.
- ✅ P7 hémicycle : 20-25 ms → 9-10 ms par image (rendu identique). Le compteur d'images perdues `L` reste élevé partout : à étudier (boucle entière). Fusionné.
- ✅ R3 horloges sur getTick (input_frame_ticks réel, plafonné à 300 ms), carte et boîtes en ms, palette carte en DMA_QUEUE, C2 corrigé. Fusionné.
- ✅ U4 partiel : code mort retiré (fonctions de test dans `tests/budget_compat.c`), Z80 chargé une fois (sfx_init), commentaires et CLAUDE.md. Restent morts : `ui_art_erase`, `ui_gueule_erase`, `ui_map_clear_region`, `ui_soft_cancel`, `voice_preview`. Bug possible signalé : légende daltonienne de la carte sur PAL2 (aussi en V4), confié à E1. Fusionné.
- ✅ E1 réduit : remaniement, carte, aide migrés (démos 22-25 ajoutées). Aide et redessins du remaniement sans fondu (I3) : à faire valider par la DA. Pas de bug daltonien sur la carte (en-tête corrigé). Fusionné.
- ✅ QA intermédiaire v4.1-dev : 0 bloquant ; 34/38 captures identiques à v4 (écarts = accroche aléatoire) ; sauvegarde et partie scriptée OK en PAL/NTSC ; 1 gênant : tuiles parasites 1-2 images pendant la construction de l'hémicycle (correctif lancé). UI_HEMI_SENATE inutilisé. Pied de page à passer en v4.1 à la livraison.
- ✅ Hémicycle : la carte de tuiles n'est posée qu'une fois toutes les tuiles envoyées (apparition d'un bloc ≈ 7 images après l'entrée), M ≈ 9-10 ms. Fusionné.
- ✅ E2 : tableau de bord partie 1 (kit_header, ui_stat_row, menu borné, ui_confirm, liste des lois, « file pleine », pm_name) ; rendu identique hors curseur ; menu et lois sans fondu interne. Fusionné.
- ✅ Fin de U4 : code mort de src/ui et voice_preview supprimés, screen.c découplé (`screen_set_enter_hook`, inits et réglages dans main.c), compteur L : artefact corrigé (première image comptée à part, `E<n>`), vraies pertes de 2-4 images sur les redessins lourds après un appui (budget, électorats, titre) ; `M` sous-estime ces dessins, voir `P<n>`. Fusionné.
- ✅ Revue DA V4.1 (REVUE_DA_V4.md, section « V4.1 : changements visuels ») : redessins nets généralisés sauf résultats payés en CP ; hémicycle d'un bloc OK s'il apparaît éteint ; « Faible » reste rouge ; 6 vignettes validées.
- ✅ Corrections DA V41-1 à V41-5 : allumage du vote rétabli (`ui_pal_set`, testé PAL en transitions normales/Rapides/Aucune ; NTSC non testé), première image atténuée dès `ui_clear`, fondu sur le résultat du remaniement, daltonisme (« Faible » rouge), vignette santé décalée. Fusionné.
- ✅ Aide corrigée (6 piliers, 1 CP/trimestre + 2 de bonus, 60 tours, 49.3, cohabitation, START) + REGLES.md § START. Commité sur v4.1-dev.
- 🔄 Lancés le 16/11 vers 00 h 45-01 h (résultats peut-être perdus après un `/clear` → vérifier les branches `worktree-agent-*`, merge-base sur v4.1-dev, sinon relancer) :
  - E3 : fin de scr_game.c (+ consigne DA V41-1 pour scr_game : set_mode_quiet) (résultats, vote, Assemblée), scr_report, scr_mandate2, game_turn_label ;
- À suivre (après les retours du test console V4, car ces écrans peuvent changer) : E4-E6, scr_elec (E1 restant), fin de R2 (budget_seen…), R6 `-Os`, puis QA V4.1 complète (NTSC, démos 5/20/21/25) et revue DA finale (U3 fait ; de préférence après les corrections du test console V4, qui toucheront les écrans) (écrans), fin de R2 (`budget_seen`, `opinion_prev`, instantané du rapport : avec les lots E), U4 nettoyage, R6 `-Os`. C1 (`q[48]` de scr_event.c) dans E5.

## Notifications
- Dès qu'une version est prête à pousser : PushNotification au joueur, puis attendre son accord (mémoire feedback-notif-push).

## Pièges
- Les worktrees d'agents partent de `main` (V3) : chaque brief commence par `git merge v4.1-dev` (ou v4-dev).
- BlastEm : tuer par PID ; `tools/shots.sh` capture sur un Xvfb privé (fiable).

## Décisions du 16/11
- Second mandat : une loi en attente s'applique normalement mais ne peut pas être reproposée (DECISIONS.md).
