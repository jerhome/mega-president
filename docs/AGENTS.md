# Mega Président — Agents proposés

> **Statut : validé le 2026-11-14** (démarrage avec `md-dev`, `qa-rom`, `redacteur`, en Sonnet par défaut).
> Ces agents sont définis dans `.claude/agents/`. Règle : **3 agents simultanés au maximum** ; un **4e agent temporaire** est permis s'il fait gagner du temps, avec notification mobile au joueur ; modèle Sonnet par défaut, Opus ponctuellement.
> Claude (la session principale) les coordonne : le joueur ne parle qu'à elle.

| Agent | Rôle | Intervient sur | Quand |
|---|---|---|---|
| `md-dev` | Développeur C et SGDK pour la Megadrive : moteur, interface, simulation, SRAM. Connaît les limites du 68000 : pas de calcul en virgule flottante, 64 Ko de mémoire vive, gestion de la mémoire vidéo et des palettes | E0 à E6, V1 | Dès la V0 |
| `qa-rom` | Compile, lance les tests unitaires sur PC, vérifie la mémoire vive et la taille de la ROM, fait un contrôle dans BlastEm et rédige un rapport. **Ne modifie jamais le code** | Tout | Dès la V0 |
| `redacteur` | Écrit les lois et les événements en français dans le fichier de contenu. Garde le ton sérieux avec la pointe d'humour de gauche prévue, n'utilise que des noms fictifs et respecte la limite de 38 colonnes | E4, V1 (contenu) | Dès la tâche 4.2 |
| `game-designer` | Règles, formules, effets des lois, conditions de victoire et de défaite. Rédige des spécifications sans coder | Simulation, institutions | V1 si besoin (Claude tient ce rôle au début) |
| `equilibrage` | Lance le simulateur sur PC, analyse les statistiques et propose des ajustements chiffrés | 1.19, 1.20, 2.14 | V1 |
| `assets` | Graphismes : palettes, images provisoires, contrat de format pour le pixel artiste | 1.5, V2 | V2 |

**Recommandation** : démarrer avec `md-dev`, `qa-rom` et `redacteur`, puis ajouter les autres au besoin pour limiter la consommation de tokens.

## Table d'aiguillage

La session principale aiguille chaque demande selon cette table. En cas de doute ou de chevauchement de fichiers, elle garde la tâche ou la découpe.

| Type de demande | Agent | Fichiers autorisés | Remarque |
|---|---|---|---|
| Règle, formule, institution, équilibrage | `md-dev` (simulation) | `src/sim/`, `tests/test_sim*.c`, `docs/SIM.md` | Aucun texte dans `src/sim` |
| Écran, menu, affichage, ergonomie | `md-dev` (écrans) | `src/screens/<écrans attribués>` | Un agent par groupe d'écrans, sans chevauchement |
| Interface, police, manette, widgets | `md-dev` (interface) | `src/ui/`, `res/`, `tools/font/` | Le contrat graphique est dans `src/ui/README.md` |
| Sauvegarde, high scores | `md-dev` | `src/save/`, `src/screens/scr_scores.c` | |
| Son | `md-dev` | `src/audio/` | |
| Format du contenu, convertisseur | `md-dev` | `tools/content/`, `src/content/` (généré) | |
| Texte, loi, événement, nom, ton | `redacteur` | `docs/contenu/` | Le convertisseur valide les longueurs |
| Vérification, budgets, non-régression | `qa-rom` | lecture seule | Ne modifie jamais le code |
| Flux entre écrans, intégration, `main.c`, Makefile, git, releases | Session principale | `src/game/`, `src/main.c`, `Makefile`, docs de pilotage | |

## Agent de triage ponctuel (prévu)

Il servira **uniquement** pour un gros lot de retours du joueur, par exemple après une session de test.

- **Modèle** : léger (Haiku ou Sonnet).
- **Entrée** : la liste brute des remarques.
- **Sortie** : un tableau donnant pour chaque remarque :
  - la catégorie : bug, équilibrage, texte, ergonomie ou idée V2 ;
  - le fichier probable ;
  - l'agent cible, d'après la table ci-dessus ;
  - la priorité ;
  - une estimation en heures.
- **Usage** : la session principale valide ce tableau avant toute délégation.
