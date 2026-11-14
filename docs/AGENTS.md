# Mega Président — Agents proposés

> **Statut : proposition, rien n'est validé.**
> Ces agents sont propres au projet ; une fois validés, ils seront placés dans `.claude/agents/`.
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
