# Mega Président

Jeu de gestion au tour par tour pour **Sega Megadrive / Genesis** : vous incarnez le président de la République française, le temps d'un ou deux mandats.

Gouvernez, faites voter vos lois, gérez les crises, survivez au Parlement, et tentez de vous faire réélire.

> Versions jouables : voir les [releases](https://github.com/jerhome/mega-president/releases) (ROM `.bin` à copier sur une cartouche flash ou à ouvrir dans un émulateur).
> Règles : [docs/REGLES.md](docs/REGLES.md) · Suivi : [docs/BACKLOG.md](docs/BACKLOG.md) · Décisions : [docs/DECISIONS.md](docs/DECISIONS.md).

## Cible technique

- Megadrive d'origine (PAL ou NTSC), sans Mega-CD, via une cartouche flash (prévu pour EverDrive MD V3 ; test sur console à venir).
- High scores sauvegardés dans la SRAM de la cartouche.
- Développé en C avec [SGDK](https://github.com/Stephane-D/SGDK).

## Compilation

Prérequis : Docker.

```sh
make        # compile out/rom.bin via l'image ghcr.io/stephane-d/sgdk
make run    # compile puis lance l'émulateur BlastEm (PAL) ; make run-ntsc pour le NTSC
make test   # tests du cœur de simulation sur PC
make check  # tests + ROM + budgets mémoire
```

## Pourquoi « Mega » Président

Le nom est un double clin d'œil :

- **la console** : *Mega* comme Megadrive. La série est prévue sur d'autres plateformes,
  avec un nom qui grandit à chaque fois : **Super Président** sur Nintendo (Super Nintendo),
  puis **Giga Président** sur PC et Mac ;
- **le régime** : c'est un pied de nez au pouvoir démesuré que la Ve République donne à son
  président. Plus la machine est grosse, plus le président l'est aussi… et plus les
  contre-pouvoirs rapetissent.

## Univers

Tous les partis, personnalités et événements du jeu sont **fictifs**.
