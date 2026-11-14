#ifndef CONTENT_WORLD_H
#define CONTENT_WORLD_H

// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "sim/sim_state.h"
#include "sim/election.h"

// Textes UTF-8 : groupes parlementaires (GRP_G1..) et candidats (CAND_*).
extern const char *const grp_name[GRP_COUNT];
extern const char *const grp_sigle[GRP_COUNT];
extern const char *const grp_desc[GRP_COUNT];
extern const char *const cand_name[CAND_POOL_COUNT];
extern const char *const cand_party[CAND_POOL_COUNT];
extern const char *const cand_portrait[CAND_POOL_COUNT];

#endif
