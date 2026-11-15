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
- 🔄 Lancés le 16/11 vers 00 h 45-01 h (résultats peut-être perdus après un `/clear` → vérifier les branches `worktree-agent-*`, merge-base sur v4.1-dev, sinon relancer) :
  - U4 nettoyage partiel (code mort, Z80 chargé une fois, commentaires, CLAUDE.md ; pas screen.c/input.c/ui_map/ui_widgets/ui_hemi/voice.c) ;
  - R3 horloges sur getTick, carte et boîtes en ms, correctif C2 du charabia (pas ui_hemi.c) ;
  - QA intermédiaire de v4.1-dev (qa-rom, comparaison des captures v4-dev / v4.1-dev).
- À suivre : E1-E6 (U3 fait ; de préférence après les corrections du test console V4, qui toucheront les écrans) (écrans), fin de R2 (`budget_seen`, `opinion_prev`, instantané du rapport : avec les lots E), fin de U4 (screen.c/main.c après R3), U4 nettoyage, R6 `-Os`. C1 (`q[48]` de scr_event.c) dans E5.

## Notifications
- Dès qu'une version est prête à pousser : PushNotification au joueur, puis attendre son accord (mémoire feedback-notif-push).

## Pièges
- Les worktrees d'agents partent de `main` (V3) : chaque brief commence par `git merge v4.1-dev` (ou v4-dev).
- BlastEm : tuer par PID ; `tools/shots.sh` capture sur un Xvfb privé (fiable).

## Décisions du 16/11
- Second mandat : une loi en attente s'applique normalement mais ne peut pas être reproposée (DECISIONS.md).
