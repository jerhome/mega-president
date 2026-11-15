// Mega Président — ministres de l'écran événement (voir content_repliques.h).
#include "content/content_repliques.h"

const char *const rep_nom[REP_PERSOS] = {
    "Fontaine", "Fermont", "Varenne", "Marchal", "Kervella", "Valcourt", "Matignon"
};

const char *const rep_fonction[REP_PERSOS] = {
    "ministre de l'Économie", "ministre de l'Intérieur", "ministre de la Santé",
    "ministre de l'Éducation", "ministre de l'Écologie", "Affaires étrangères", "Matignon"
};

const char *const rep_texte[REP_PERSOS][REP_HUMEURS] = {
    { "Les actionnaires vont adorer.", "Les chiffres tiennent. En gros.", "Ça déborde du tableur.", "Ma calculatrice a fumé, là." },
    { "Enfin une idée. Je la fiche.", "Je surveille. Vous aussi.", "Mes indics ne m'ont rien dit.", "On me cherche. On me trouvera." },
    { "Je reprends un café. De joie.", "Je note. Ordonnance à suivre.", "Les urgences, elles, attendent.", "J'ai cassé mon stylo. Encore." },
    { "Voilà qui mérite un bon point.", "Peut mieux faire. Voyons.", "Hors sujet. Et deux fautes.", "Zéro. Et je souligne en rouge." },
    { "Génial ! Je sors les graines !", "OK. Je note. Je grignote.", "Ça sent un peu le gazole, non ?", "Je m'enchaîne à mon bureau !" },
    { "Champagne dans les ambassades.", "Je dirai non. Avec le sourire.", "Disons... que c'est délicat.", "Je passe à l'anglais. Sec." },
    { "Matignon vous suit. De près.", "On fera une réunion de cadrage.", "Je vais consulter. Longuement.", "Ma démission est déjà tapée." }
};
