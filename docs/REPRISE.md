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
- 🔄 U2 widgets (src/ui) et R1 intégrité des tables (vignettes des 6 événements V4, tests des répliques, `_Static_assert`, loi en attente non reproposable au 2nd mandat) : agents lancés le 16/11 vers 00 h 30 ; résultats peut-être perdus après un `/clear` → vérifier les branches `worktree-agent-*` (merge-base sur v4.1-dev, sinon cherry-pick) et relancer si rien.
- À suivre : U3 kit d'écran, puis E1-E6 (écrans), R2 sauvegarde, R3 horloges + correctif C2 du charabia, U4 nettoyage, R6 `-Os`, optimisation de l'hémicycle (P7).

## Pièges
- Les worktrees d'agents partent de `main` (V3) : chaque brief commence par `git merge v4.1-dev` (ou v4-dev).
- BlastEm : tuer par PID ; `tools/shots.sh` capture sur un Xvfb privé (fiable).

## Décisions du 16/11
- Second mandat : une loi en attente s'applique normalement mais ne peut pas être reproposée (DECISIONS.md).
