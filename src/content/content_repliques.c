// Mega Président — ministres de l'écran événement (voir content_repliques.h).
#include "content/content_repliques.h"

const char *const rep_nom[REP_PERSOS] = {
    "Fontaine", "Fermont", "Varenne", "Marchal", "Kervella", "Valcourt", "Matignon"
};

const char *const rep_fonction[REP_PERSOS] = {
    "ministre de l'Économie", "ministre de l'Intérieur", "ministre de la Santé",
    "ministre de l'Éducation", "ministre de l'Écologie", "Affaires étrangères", ""
};

const char *const rep_texte[REP_PERSOS][REP_HUMEURS] = {
    { "Les marchés vont adorer.", "Les chiffres tiennent. En gros.",
      "Bercy fera... avec.", "Ma calculatrice a fumé, là." },
    { "Les képis approuvent. Enfin.", "Je surveille. Tout le monde.",
      "Pas prévu dans mon dossier.", "On me cherche. On me trouvera." },
    { "Ça, c'est un bon diagnostic.", "Je note. Ordonnance à suivre.",
      "Les urgences n'aimeront pas.", "J'ai cassé mon stylo. Encore." },
    { "Voilà qui mérite un bon point.", "Peut mieux faire. Voyons.",
      "Un peu hors sujet, tout ça.", "Copie à revoir. Entièrement." },
    { "Génial ! Je sors les graines !", "OK. Je note. Je grignote.",
      "Le climat n'attendra pas, hein.", "Vous piétinez mes graines !" },
    { "Nos alliés applaudiront.", "Je dirai non. Avec le sourire.",
      "Disons... que c'est délicat.", "Je passe à l'anglais. Sec." },
    { "Matignon vous suit. De près.", "On en reparle en réunion.",
      "Je vais consulter. Longuement.", "On n'a pas été prévenu. Encore." }
};
