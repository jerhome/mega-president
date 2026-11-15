// Mega Président — ministres de l'écran événement : nom, fonction, répliques.
// Textes de docs/contenu/repliques_ministres.md (relecture humour : agent vannes).
// Données constantes en ROM ; C portable, sans SGDK.
#ifndef CONTENT_REPLIQUES_H
#define CONTENT_REPLIQUES_H

// Personnages : 0 Économie, 1 Intérieur, 2 Santé, 3 Éducation, 4 Écologie,
// 5 Affaires étrangères (même ordre que GUEULE_MIN), 6 Premier ministre.
#define REP_PERSOS   7
#define REP_PM       6

// Humeurs, dans l'ordre du fichier de textes.
enum { REP_CONTENT, REP_NEUTRE, REP_GENE, REP_FURIEUX, REP_HUMEURS };

extern const char *const rep_nom[REP_PERSOS];        // nom de famille (PM : remplacé par le nom du PM en place)
extern const char *const rep_fonction[REP_PERSOS];   // PM : « Matignon »
extern const char *const rep_texte[REP_PERSOS][REP_HUMEURS];

#endif
