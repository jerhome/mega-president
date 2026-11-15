#include "sim/senate.h"
#include "sim/sim.h"

// Sénat V2. Constantes : docs/SIM.md.

// Composition « de fond » du Sénat (plutôt centre et droite), somme = 348.
static const u16 senate_base[GRP_COUNT] = { 22, 58, 92, 118, 58 };

#define SEN_K_PARTY   8     // sensibilité du renouvellement (pour mille par dixième / 10),
#define SEN_K_ALLY    3     // environ le tiers de celle des législatives
#define SEN_K_OPP     (-5)
#define SEN_REF       420   // score pour lequel les sièges renouvelés restent stables
#define SEN_NOISE     25    // aléa ±2,5 %
#define SEN_BIAS      0     // biais institutionnel du vote de destitution

// Répartit total sièges au prorata des poids (plus forts restes).
static void distribute(u16 *out, const s32 *w, u16 total)
{
    s32 wsum = 0, rem[GRP_COUNT];
    u16 given = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++) wsum += w[g];
    for (g = 0; g < GRP_COUNT; g++) {
        s32 q = w[g] * total;
        out[g] = (u16)(q / wsum);
        rem[g] = q % wsum;
        given = (u16)(given + out[g]);
    }
    while (given < total) {
        u8 best = 0;
        for (g = 1; g < GRP_COUNT; g++) if (rem[g] > rem[best]) best = g;
        out[best]++;
        rem[best] = -1;
        given++;
    }
}

void sim_senate_init(Country *c)
{
    s32 w[GRP_COUNT];
    u8 g;
    // Moyenne de la couleur de l'Assemblée de départ (qui dépend de la difficulté
    // et de l'orientation) et de la composition de fond du Sénat.
    for (g = 0; g < GRP_COUNT; g++)
        w[g] = (s32)c->seats[g] * 1000 / SEATS_TOTAL + (s32)senate_base[g] * 1000 / SENATE_SEATS;
    distribute(c->senate, w, SENATE_SEATS);
}

u16 sim_senate_bloc_seats(const Country *c)
{
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (sim_in_bloc(c, g)) s = (u16)(s + c->senate[g]);
    return s;
}

u8 sim_senate_has_majority(const Country *c)
{
    return (u8)(sim_senate_bloc_seats(c) > SENATE_SEATS / 2);
}

u16 sim_senate_support_seats(const Country *c)
{
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (c->relation[g] >= 50) s = (u16)(s + c->senate[g]);
    return s;
}

// Les sénateurs jugent sur la durée : relation moyennée avec la relation de base.
u16 sim_impeach_senate_votes(const Country *c)
{
    s32 common = ((s32)c->gauges[JAUGE_SCANDALE] - IMPEACH_SCANDALE)
               + ((s32)c->gauges[JAUGE_DEFIANCE] - IMPEACH_DEFIANCE) / 2
               - ((s32)sim_popularity(c) - 400) / 10 + SEN_BIAS;
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++) {
        s32 rel;
        if (g == c->player_group) continue;
        rel = ((s32)c->relation[g] + c->relation_base[g]) / 2;
        if (100 - rel + common >= 70) s = (u16)(s + c->senate[g]);
    }
    return s;
}

u8 sim_impeach_would_pass(const Country *c)
{
    return (u8)(sim_impeach_votes(c) >= IMPEACH_MAJORITY
                && sim_impeach_senate_votes(c) >= SENATE_IMPEACH_MAJORITY);
}

// Année de la partie (0-9) du tour courant ; renouvellement aux années 3, 6, 9.
static u8 game_year(const Country *c)
{
    return (u8)(SIM_MANDATE_YEARS * c->mandate + sim_year(c));
}

u8 sim_senate_renewal_due(const Country *c)
{
    u8 gy;
    if (c->turn == 0 || (c->turn % c->turns_per_year) != 0) return 0;
    gy = game_year(c);
    return (u8)(gy > 0 && (gy % SENATE_RENEW_YEARS) == 0);
}

u8 sim_senate_renewal_in(const Country *c)
{
    u8 base = (u8)(SIM_MANDATE_YEARS * c->mandate);
    u8 y;
    for (y = (u8)(sim_year(c) + 1); y < SIM_MANDATE_YEARS; y++)
        if (((base + y) % SENATE_RENEW_YEARS) == 0)
            return (u8)(y * c->turns_per_year - c->turn);
    return 0;
}

void sim_senate_renew(Country *c, u16 *before)
{
    s32 w[GRP_COUNT];
    u16 keep[GRP_COUNT], add[GRP_COUNT], pool = SENATE_SEATS;
    Rng noise;
    s32 score = (3 * (s32)sim_popularity(c) + sim_core_opinion(c)) / 4;
    u8 g;

    // Aléa tiré d'un générateur dérivé : le flux de c->rng n'est pas consommé.
    rng_seed(&noise, c->rng.state ^ 0x5E4A7E00u ^ ((u32)c->turn << 8));
    for (g = 0; g < GRP_COUNT; g++) {
        s16 dist = sim_abs16((s16)((s16)g - c->player_group));
        s32 k = dist == 0 ? SEN_K_PARTY : (dist == 1 ? SEN_K_ALLY : SEN_K_OPP);
        s32 f = 1000 + (score - SEN_REF) * k / 10 + rng_range(&noise, -SEN_NOISE, SEN_NOISE);
        if (f < 500) f = 500;
        if (f > 1600) f = 1600;
        if (before) before[g] = c->senate[g];
        keep[g] = (u16)(c->senate[g] / 2);       // la moitié non renouvelée reste en place
        pool = (u16)(pool - keep[g]);
        w[g] = (s32)c->senate[g] * f / 1000 * f / 1000;
        if (w[g] < 1) w[g] = 1;
    }
    distribute(add, w, pool);
    for (g = 0; g < GRP_COUNT; g++) c->senate[g] = (u16)(keep[g] + add[g]);
}
