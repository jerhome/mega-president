# Point de reprise (16/11)

- `v4-dev` : V4 prête pour le test console du joueur (version affichée v4.0). ROM : `~/mega-president-roms/mega-president-v4.0-test.bin`. Reste 4.17 (retours du joueur, étiquette v4.0, release ; pas de push sans accord).
- `v4.1-dev` : V4.1 commencée selon `docs/AUDIT_V4_1.md` (lot U1 StrBuf fusionné ; lot R4 mesures en cours). Fusionner régulièrement v4-dev dans v4.1-dev.
- Piège : les worktrees d'agents partent de `main` ; leur demander de fusionner la bonne branche (v4-dev ou v4.1-dev) avant de travailler.
- Recréer la tâche planifiée de suivi (mémoire feedback-suivi-planifie).
