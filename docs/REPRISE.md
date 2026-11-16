# Point de reprise (16/11, soir)

À lire en premier après un `/clear`. Puis recréer la tâche planifiée de suivi (mémoire feedback-suivi-planifie).

## Branches
- `v4-dev` : **V4 figée pour le test console du joueur** (version affichée v4.0). ROM : `~/mega-president-roms/mega-president-v4.0-test.bin` ; liste à tester : `docs/TEST_CONSOLE_V4.md` (copie à côté de la ROM). Reste la tâche 4.17 : corriger les retours du joueur sur v4-dev, étiquette v4.0, release GitHub. **Pas de push sans accord.**
- `v4.1-dev` : V4.1 en cours (relecture et factorisation, sans assembleur), plan dans `docs/AUDIT_V4_1.md` § 6. Toute modification issue de l'audit va en V4.1, jamais en V4 (demande du joueur).
- Après chaque correction sur v4-dev : `git merge v4-dev` dans v4.1-dev (worktree de travail : scratchpad `v41`, sinon `git worktree add`). Vérifier `git merge-base --is-ancestor v4-dev v4.1-dev`.
- `main` = V3 publiée (release v3.0 sur GitHub le 15/11).
- **Pré-release `v4.1-pre`** (16/11, demande du joueur) : état de v4.1-dev, version affichée « v4.1-pre », étiquette et branche v4.1-dev poussées, release GitHub marquée préversion avec la ROM. ROM locale : `out/mega-president-v4.1-pre.bin` (et `out/rom.bin`). Le joueur la teste : ses retours vont dans v4.1-dev. À la livraison finale : version affichée v4.1.

## V4.1 : état des lots (AUDIT_V4_1 § 6)
- ✅ U1 StrBuf (`src/util/strbuf`), fusionné dans v4.1-dev.
- ✅ R4 mesures (`make rom-perf`, `docs/MESURES_V4_1.md`) : seul l'hémicycle perd des images (20-25 ms) ; titre avec sauvegarde, options et destitution pas encore mesurés.
- ✅ R1 intégrité des tables : vignettes générées (les 6 événements V4 ont la bonne image et le bon ministre), tests des répliques, `_Static_assert`, loi en attente non reproposable au 2nd mandat. Fusionné dans v4.1-dev (139 814 vérifications).
- ✅ U2 widgets (src/ui, documentés dans src/ui/README.md « Widgets réutilisables » ; `ui_confirm_update` : 1 = oui, 0 = non), fusionné dans v4.1-dev.
- ✅ U3 kit d'écran (scr_kit.[ch], pm_name/pillar_name bornés dans scr_txt.h, art_reset_quiet), fusionné dans v4.1-dev.
- ✅ R2 sauvegarde (format v3 : empreinte, `game_sanitize`, 2 emplacements, Fletcher différé ; `Game` dans `src/game/game_state.h`). Une sauvegarde V4 (v2) sera refusée par la V4.1 (message « ancienne version »). Fusionné.
- ✅ P7 hémicycle : 20-25 ms → 9-10 ms par image (rendu identique). Le compteur d'images perdues `L` reste élevé partout : à étudier (boucle entière). Fusionné.
- ✅ R3 horloges sur getTick (input_frame_ticks réel, plafonné à 300 ms), carte et boîtes en ms, palette carte en DMA_QUEUE, C2 corrigé. Fusionné.
- ✅ U4 partiel : code mort retiré (fonctions de test dans `tests/budget_compat.c`), Z80 chargé une fois (sfx_init), commentaires et CLAUDE.md. (le reste du code mort : fin de U4). Fusionné.
- ✅ E1 réduit : remaniement, carte, aide migrés (démos 22-25 ajoutées). Aide et redessins du remaniement sans fondu (I3) : à faire valider par la DA. Pas de bug daltonien sur la carte (en-tête corrigé). Fusionné.
- ✅ QA intermédiaire v4.1-dev : 0 bloquant ; 34/38 captures identiques à v4 (écarts = accroche aléatoire) ; sauvegarde et partie scriptée OK en PAL/NTSC ; 1 gênant : tuiles parasites 1-2 images pendant la construction de l'hémicycle (correctif lancé). UI_HEMI_SENATE inutilisé. Pied de page à passer en v4.1 à la livraison.
- ✅ Hémicycle : la carte de tuiles n'est posée qu'une fois toutes les tuiles envoyées (apparition d'un bloc ≈ 7 images après l'entrée), M ≈ 9-10 ms. Fusionné.
- ✅ E2 : tableau de bord partie 1 (kit_header, ui_stat_row, menu borné, ui_confirm, liste des lois, « file pleine », pm_name) ; rendu identique hors curseur ; menu et lois sans fondu interne. Fusionné.
- ✅ Fin de U4 : code mort de src/ui et voice_preview supprimés, screen.c découplé (`screen_set_enter_hook`, inits et réglages dans main.c), compteur L : artefact corrigé (première image comptée à part, `E<n>`), vraies pertes de 2-4 images sur les redessins lourds après un appui (budget, électorats, titre) ; `M` sous-estime ces dessins, voir `P<n>`. Fusionné.
- ✅ Revue DA V4.1 (REVUE_DA_V4.md, section « V4.1 : changements visuels ») : redessins nets généralisés sauf résultats payés en CP ; hémicycle d'un bloc OK s'il apparaît éteint ; « Faible » reste rouge ; 6 vignettes validées.
- ✅ Corrections DA V41-1 à V41-5 : allumage du vote rétabli (`ui_pal_set`, testé PAL en transitions normales/Rapides/Aucune ; NTSC non testé), première image atténuée dès `ui_clear`, fondu sur le résultat du remaniement, daltonisme (« Faible » rouge), vignette santé décalée. Fusionné.
- ✅ Aide corrigée (6 piliers, 1 CP/trimestre + 2 de bonus, 60 tours, 49.3, cohabitation, START) + REGLES.md § START. Commité sur v4.1-dev.
- ✅ E3 : fin du tableau de bord (StrBuf, sb_arrow, kit_group_seats), compte rendu (game_calendar_year), second mandat, game_turn_label ; démos 26 et 27 ; fondus selon la règle DA (set_mode_quiet / page_begin). Rendu identique sur 8 démos ; ROM +1 Ko (gain attendu avec E4-E6 et R6). Fusionné.
- ✅ R6 : `src/screens/*.c` et `game.c` en -Os via `tools/sgdk_os.mk` (`make SCREENS_OS=0` pour revenir en -O3) : code -54 Ko (_etext 0x5cee4 → 0x4f690), fluidité identique. ⚠️ Changer d'option ne recompile pas les .o existants : supprimer `out/src/screens` et `out/src/game` avant. Fusionné.
- 🔄 En cours (16/11 soir) : 3 agents. (1) md-dev E4 (referendum, impeach, end, scores) en worktree ; (2) qa-rom sur v4.1-pre (NTSC, démos 5/20/21/25, pied de page, sauvegarde V4 refusée), rapport dans sa réponse seulement ; (3) md-dev fin de R4 (mesures titre avec sauvegarde, options, destitution → MESURES_V4_1.md) en worktree. Fusionner E4 et R4 dans v4.1-dev après `make check`.
- À suivre, après les retours du test console (ces écrans peuvent changer) : E5 (event + C1 `q[48]`, campaign, election), E6 (options, budget + cache P3 + fin de R2 : `budget_seen`, `opinion_prev`, instantané du rapport), scr_elec ; puis version affichée v4.1, QA V4.1 complète (NTSC, démos 5/20/21/25, retours nets), revue DA finale, test console, PushNotification, release.

## Notifications
- Dès qu'une version est prête à pousser : PushNotification au joueur, puis attendre son accord (mémoire feedback-notif-push).

## Pièges
- Les worktrees d'agents partent de `main` (V3) : chaque brief commence par `git merge v4.1-dev` (ou v4-dev).
- BlastEm : tuer par PID ; `tools/shots.sh` capture sur un Xvfb privé (fiable).

## Décisions du 16/11
- Second mandat : une loi en attente s'applique normalement mais ne peut pas être reproposée (DECISIONS.md).
