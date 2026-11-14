# Mega Président

Jeu de gestion au tour par tour pour **Sega Megadrive / Genesis** : vous incarnez le président de la République française, le temps d'un ou deux mandats.

Gouvernez, faites voter vos lois, gérez les crises, survivez au Parlement, et tentez de vous faire réélire.

> 🚧 Projet en phase de conception. Voir [docs/REGLES.md](docs/REGLES.md) et [docs/BACKLOG.md](docs/BACKLOG.md).

## Cible technique

- Megadrive d'origine (PAL ou NTSC), sans Mega-CD, via une cartouche flash (testé sur EverDrive MD V3).
- High scores sauvegardés dans la SRAM de la cartouche.
- Développé en C avec [SGDK](https://github.com/Stephane-D/SGDK).

## Compilation (prévue)

Prérequis : Docker.

```sh
make        # compile out/rom.bin via l'image ghcr.io/stephane-d/sgdk
make run    # compile puis lance l'émulateur BlastEm
```

## Univers

Tous les partis, personnalités et événements du jeu sont **fictifs**.
