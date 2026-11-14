#ifndef SIM_ACTIONS_H
#define SIM_ACTIONS_H

// Actions V2 du président : référendum (2.06) et remaniement (2.07).

#include "sim/sim_state.h"

// --- Référendum (REGLES §5, §8) ---
#define REFERENDUM_CP        6    // coût en capital politique
#define REFERENDUM_POP_WIN   4    // points d'opinion (tous électorats) si « oui »
#define REFERENDUM_POP_LOSS  4    // points d'opinion perdus si « non »
#define REFERENDUM_DEF_WIN   3    // défiance en moins si « oui »
#define REFERENDUM_DEF_LOSS  10   // défiance en plus si « non »

enum { RFD_OK = 0, RFD_ERR_ID, RFD_ERR_CP, RFD_ERR_USED, RFD_ERR_ADOPTED, RFD_ERR_FULL,
       RFD_ERR_STATE };

typedef struct {
    s16 yes_elec[ELEC_COUNT];   // « oui » par électorat (dixièmes de %)
    s16 yes;                    // « oui » national (dixièmes de %), majorité à 500
    u8  adopted;                // 1 si la loi est adoptée par le peuple
} RefResult;

// RFD_OK si le référendum sur cette loi est possible (au plus un par mandat, loi
// non adoptée, 6 CP), sinon RFD_ERR_*. Possible aussi en cohabitation, sur toute loi.
u8 sim_referendum_check(const Country *c, u8 law_id);
// Aperçu déterministe (sans aléa, sans effet) : out->yes est le « oui » attendu.
void sim_referendum_preview(const Country *c, u8 law_id, RefResult *out);
// Organise le référendum (consomme du hasard). Succès : loi adoptée sans vote du
// Parlement, popularité +4, défiance −3. Échec : popularité −4, défiance +10.
u8 sim_referendum(Country *c, u8 law_id, RefResult *out);

// --- Remaniement (REGLES §5) ---
#define RESHUFFLE_CP      3    // coût en capital politique
#define RESHUFFLE_REL     2    // relation + avec chaque groupe
#define RESHUFFLE_GRACE   30   // état de grâce (dixièmes de %), décroît de 6 par trimestre
#define RESHUFFLE_POP     1    // points d'opinion immédiats (tous électorats)

enum { RSH_OK = 0, RSH_ERR_CP, RSH_ERR_YEAR, RSH_ERR_COHAB, RSH_ERR_PM, RSH_ERR_STATE };

// RSH_OK si le remaniement vers ce profil est possible (hors cohabitation, au
// plus un par an, profil valide et différent de l'actuel, 3 CP).
u8 sim_reshuffle_check(const Country *c, u8 pm);
// Remaniement global : change de Premier ministre et rafraîchit relations et popularité.
u8 sim_reshuffle(Country *c, u8 pm);

#endif
