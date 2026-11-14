#include "sim/institutions.h"
#include "sim/sim.h"

// Institutions V1. Constantes : docs/SIM.md.

#define LEG_REF        420   // score (dixièmes de %) pour lequel les sièges restent stables
#define LEG_K_PARTY    25    // sensibilité du parti du joueur (pour mille par dixième / 10)
#define LEG_K_ALLY     10    // groupes voisins
#define LEG_K_OPP      (-15) // opposition
#define LEG_NOISE      60    // aléa ±6 %
#define LEG_DEFIANCE   15    // défiance après des législatives (nouvelle assemblée)

#define CENSURE_OPP_REL   50   // un groupe est « d'opposition » sous cette relation
#define CENSURE_VOTE_REL  40   // un groupe vote la censure sous ce seuil (+ défiance, scandale)
#define CENSURE_POP       3    // popularité perdue si le gouvernement tombe
#define CENSURE_DEF_AFTER 30   // défiance après la chute du gouvernement
#define CENSURE_DEF_FAIL  10   // défiance retombée après une motion rejetée

#define IMPEACH_VOTE      70   // seuil du score de destitution d'un groupe
#define IMPEACH_RELIEF    20   // scandale retombé si le président survit
#define IMPEACH_COOLDOWN  4    // trimestres avant une nouvelle procédure

u8 sim_in_bloc(const Country *c, u8 g)
{
    return (u8)(sim_abs16((s16)((s16)g - c->player_group)) <= 1);
}

u16 sim_bloc_seats(const Country *c)
{
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (sim_in_bloc(c, g)) s += c->seats[g];
    return s;
}

u8 sim_has_majority(const Country *c)
{
    return (u8)(sim_bloc_seats(c) >= SEATS_MAJORITY);
}

void sim_legislatives(Country *c, LegislativeResult *out)
{
    LegislativeResult tmp;
    LegislativeResult *r = out ? out : &tmp;
    s32 w[GRP_COUNT], wsum = 0, rem[GRP_COUNT];
    u16 given = 0;
    u8 g;
    // Score électoral : popularité (3/4) et électorat-cœur du joueur (1/4).
    // ELEC_* et ORIENT_* sont alignés : gauche -> populaires, centre -> moyennes,
    // droite -> aisées.
    s32 score = (3 * (s32)sim_popularity(c) + c->opinion[c->orientation]) / 4;

    r->bloc_before = sim_bloc_seats(c);
    for (g = 0; g < GRP_COUNT; g++) {
        s16 dist = sim_abs16((s16)((s16)g - c->player_group));
        s32 k = dist == 0 ? LEG_K_PARTY : (dist == 1 ? LEG_K_ALLY : LEG_K_OPP);
        s32 f = 1000 + (score - LEG_REF) * k / 10 + rng_range(&c->rng, -LEG_NOISE, LEG_NOISE);
        if (f < 300) f = 300;
        if (f > 2000) f = 2000;
        // Scrutin majoritaire : effet amplifié (poids × f²).
        w[g] = (s32)group_seats_start[g] * f / 1000 * f / 1000;
        if (w[g] < 5) w[g] = 5;
        wsum += w[g];
        r->seats_before[g] = c->seats[g];
    }
    // Plus forts restes
    for (g = 0; g < GRP_COUNT; g++) {
        s32 q = w[g] * SEATS_TOTAL;
        c->seats[g] = (u16)(q / wsum);
        rem[g] = q % wsum;
        given = (u16)(given + c->seats[g]);
    }
    while (given < SEATS_TOTAL) {
        u8 best = 0;
        for (g = 1; g < GRP_COUNT; g++) if (rem[g] > rem[best]) best = g;
        c->seats[best]++;
        rem[best] = -1;
        given++;
    }
    for (g = 0; g < GRP_COUNT; g++) {
        r->seats_after[g] = c->seats[g];
        // Nouvelle assemblée : les relations se rapprochent de leur base.
        c->relation[g] = (u8)(((u16)c->relation[g] + c->relation_base[g] + 1) / 2);
    }
    r->bloc_after = sim_bloc_seats(c);
    r->majority = sim_has_majority(c);
    r->won = (u8)(r->majority || r->bloc_after >= r->bloc_before);
    c->gauges[JAUGE_DEFIANCE] = LEG_DEFIANCE;
    c->dissolve_turn = (u8)(c->turn + 1);
}

u8 sim_can_dissolve(const Country *c)
{
    if (c->status != STATUS_PLAYING || sim_mandate_over(c)) return 0;
    if (c->impeach != IMPEACH_NONE) return 0;
    if (c->dissolve_turn == 0) return 1;
    return (u8)((u16)c->turn + 1 - c->dissolve_turn >= c->turns_per_year);
}

u8 sim_dissolve(Country *c, LegislativeResult *out)
{
    if (!sim_can_dissolve(c)) return 0;
    sim_legislatives(c, out);
    return 1;
}

// --- Censure ---

u16 sim_opposition_seats(const Country *c)
{
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (g != c->player_group && c->relation[g] < CENSURE_OPP_REL) s += c->seats[g];
    return s;
}

u8 sim_censure_threat(const Country *c)
{
    return (u8)(c->gauges[JAUGE_DEFIANCE] >= CENSURE_DEFIANCE
                && sim_opposition_seats(c) >= SEATS_MAJORITY);
}

u16 sim_censure_votes(const Country *c)
{
    s16 lim = (s16)(CENSURE_VOTE_REL + ((s16)c->gauges[JAUGE_DEFIANCE] - CENSURE_DEFIANCE) / 3
                    + c->gauges[JAUGE_SCANDALE] / 10);
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (g != c->player_group && (s16)c->relation[g] < lim) s += c->seats[g];
    return s;
}

// --- Destitution ---

u8 sim_impeach_pending(const Country *c)
{
    return (u8)(c->impeach == IMPEACH_DEFENSE);
}

u16 sim_impeach_votes(const Country *c)
{
    s32 common = ((s32)c->gauges[JAUGE_SCANDALE] - IMPEACH_SCANDALE)
               + ((s32)c->gauges[JAUGE_DEFIANCE] - IMPEACH_DEFIANCE) / 2
               - ((s32)sim_popularity(c) - 400) / 10;
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++) {
        if (g == c->player_group) continue;
        if (100 - (s32)c->relation[g] + common >= IMPEACH_VOTE) s += c->seats[g];
    }
    return s;
}

u8 sim_defense_available(const Country *c, u8 d)
{
    if (!sim_impeach_pending(c) || d >= DEFENSE_COUNT) return 0;
    if (c->impeach_def & (1u << d)) return 0;
    if (d == DEFENSE_TV) return (u8)(c->cp >= DEFENSE_TV_CP);
    if (d == DEFENSE_NEGOCIER) return (u8)(c->cp >= DEFENSE_NEGOCIER_CP);
    return 1;
}

u8 sim_defend(Country *c, u8 d, u8 group)
{
    if (!sim_defense_available(c, d)) return 0;
    switch (d) {
    case DEFENSE_TV:
        c->cp = (u8)(c->cp - DEFENSE_TV_CP);
        sim_gauge_add(&c->gauges[JAUGE_SCANDALE], -10);
        sim_apply_effect(c, EFF_POP_TOUS, 1);
        break;
    case DEFENSE_SACRIFICE:
        sim_gauge_add(&c->gauges[JAUGE_SCANDALE], -15);
        sim_apply_effect(c, EFF_DEFIANCE, -5);
        sim_apply_effect(c, EFF_POP_TOUS, -1);
        sim_apply_effect(c, EFF_REL_PARTI, -5);
        break;
    default:   // DEFENSE_NEGOCIER
        if (group >= GRP_COUNT || group == c->player_group) return 0;
        c->cp = (u8)(c->cp - DEFENSE_NEGOCIER_CP);
        sim_apply_effect(c, (u8)(EFF_REL_G1 + group), 15);
        break;
    }
    c->impeach_def |= (u8)(1u << d);
    return 1;
}

static u8 next_pm(u8 pm)
{
    if (pm >= PM_COUNT) return PM_RASSEMBLEUR;
    return (u8)((pm + 1) % PM_COUNT);
}

void inst_end_turn_quarter(Country *c, u8 *censure, u16 *censure_votes, u8 *new_pm,
                           u8 *impeach, u16 *impeach_votes)
{
    // 1. Destitution : vote à la fin du trimestre de défense.
    if (c->impeach == IMPEACH_DEFENSE) {
        u16 v = sim_impeach_votes(c);
        *impeach_votes = v;
        c->impeach = IMPEACH_NONE;
        c->impeach_def = 0;
        if (v >= IMPEACH_MAJORITY) {
            c->status = STATUS_DESTITUE;
            *impeach = IMP_EV_VOTEE;
            return;
        }
        *impeach = IMP_EV_REJETEE;
        sim_gauge_add(&c->gauges[JAUGE_SCANDALE], -IMPEACH_RELIEF);
        c->impeach_cool = IMPEACH_COOLDOWN;
        return;
    }
    if (c->impeach_cool) c->impeach_cool--;
    else if (c->gauges[JAUGE_SCANDALE] >= IMPEACH_SCANDALE
             && c->gauges[JAUGE_DEFIANCE] >= IMPEACH_DEFIANCE
             && !sim_mandate_over(c)) {
        c->impeach = IMPEACH_DEFENSE;
        c->impeach_def = 0;
        *impeach = IMP_EV_ENGAGEE;
        *impeach_votes = sim_impeach_votes(c);
        return;
    }

    // 2. Motion de censure.
    if (sim_censure_threat(c)) {
        u16 v = sim_censure_votes(c);
        *censure_votes = v;
        if (v >= SEATS_MAJORITY) {
            u8 pm = next_pm(c->pm);
            *censure = CENSURE_ADOPTEE;
            c->censures++;
            sim_change_pm(c, pm);
            *new_pm = pm;
            sim_apply_effect(c, EFF_POP_TOUS, -CENSURE_POP);
            c->gauges[JAUGE_DEFIANCE] = CENSURE_DEF_AFTER;
        } else {
            *censure = CENSURE_REJETEE;
            sim_apply_effect(c, EFF_DEFIANCE, -CENSURE_DEF_FAIL);
        }
    }
}
