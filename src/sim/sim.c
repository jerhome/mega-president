#include "sim/sim.h"

// Opinion de départ par orientation (lendemain d'élection).
static const s16 opinion_start[ORIENT_COUNT][ELEC_COUNT] = {
    { 580, 530, 420 },   // gauche
    { 500, 540, 500 },   // centre
    { 460, 550, 610 },   // droite
};

// Relation de départ selon la distance au parti du joueur (contrat §4).
static const u8 relation_by_distance[4] = { 80, 55, 30, 15 };

// Sièges de départ par difficulté et orientation (Normal : contrat §4).
// Difficile : le bloc présidentiel (parti + voisins) est sous 289 sièges.
static const u16 seats_start[DIFF_COUNT][ORIENT_COUNT][GRP_COUNT] = {
    {   // Facile
        {  85, 175, 150, 100,  67 },
        {  65, 150, 195, 110,  57 },
        {  60, 125, 150, 155,  87 },
    },
    {   // Normal
        {  75, 140, 160, 120,  82 },
        {  75, 140, 160, 120,  82 },
        {  75, 140, 160, 120,  82 },
    },
    {   // Difficile
        {  70, 115,  95, 170, 127 },
        { 115, 110, 105,  70, 177 },
        { 120, 170,  90, 105,  92 },
    },
};

// Conjoncture et jauges de départ par difficulté.
typedef struct {
    s16 growth, unemployment, debt, opinion;
    s8  relation;
    u8  cp, tension, defiance;
} DiffParams;

static const DiffParams diff_params[DIFF_COUNT] = {
    { 170, 700, 1000,  20,  5, 5, 15,  5 },   // Facile
    { 120, 750, 1100,   0,  0, 4, 20, 10 },   // Normal
    {  40, 870, 1200, -20, -5, 3, 32, 20 },   // Difficile
};

#define GRACE_START    120   // +12 points d'opinion cible au début du 1er mandat
#define GRACE_START2   40    // +4 points au début du 2nd mandat
#define GRACE_PM_DECAY   6   // érosion de l'état de grâce du remaniement, par trimestre
#define RASSEMBLEUR_INIT 5   // PM Rassembleur : relations + 5 à la nomination

void sim_init_ex(Country *c, const SimSetup *s)
{
    u8 i, orientation = s->orientation, diff = s->difficulty, tpy = s->turns_per_year;
    const DiffParams *dp;
    u16 k;
    u8 *raw = (u8 *)c;
    for (k = 0; k < sizeof(Country); k++) raw[k] = 0;   // pas de memset (pas de libc)

    if (orientation >= ORIENT_COUNT) orientation = ORIENT_CENTRE;
    if (diff >= DIFF_COUNT) diff = DIFF_NORMAL;
    if (tpy != 12) tpy = SIM_TURNS_PER_YEAR_V0;
    dp = &diff_params[diff];

    rng_seed(&c->rng, s->seed);
    c->orientation = orientation;
    c->difficulty = diff;
    c->turns_per_year = tpy;
    c->player_group = (u8)(GRP_G2 + orientation);
    c->pm = PM_NONE;
    c->theme = THEME_NONE;
    c->budget = BUDGET_EQUILIBRE;
    c->status = STATUS_PLAYING;

    for (i = 0; i < ELEC_COUNT; i++)
        c->opinion[i] = (s16)(opinion_start[orientation][i] + dp->opinion
                              + rng_range(&c->rng, -15, 15));

    c->growth = (s16)(dp->growth + rng_range(&c->rng, -20, 20));
    c->unemployment = (s16)(dp->unemployment + rng_range(&c->rng, -30, 30));
    c->debt = dp->debt;
    for (i = 0; i < PIL_COUNT; i++)
        c->pillars[i] = (s16)(500 + rng_range(&c->rng, -20, 20));

    for (i = 0; i < PIL_COUNT; i++) c->pillars_start[i] = c->pillars[i];
    c->unemployment_start = c->unemployment;
    c->debt_start = c->debt;

    c->gauges[JAUGE_TENSION] = dp->tension;
    c->gauges[JAUGE_SCANDALE] = 0;
    c->gauges[JAUGE_DEFIANCE] = dp->defiance;
    c->cp = dp->cp;
    c->grace = GRACE_START;

    for (i = 0; i < GRP_COUNT; i++) {
        s16 dist = sim_abs16((s16)((s16)i - c->player_group));
        c->relation_base[i] = (u8)sim_clamp((s32)relation_by_distance[dist] + dp->relation,
                                            0, RELATION_MAX);
        c->relation[i] = c->relation_base[i];
        c->seats[i] = seats_start[diff][orientation][i];
    }
    sim_senate_init(c);
    c->pm_prev = PM_NONE;
    if (s->pm < PM_COUNT) sim_change_pm(c, s->pm);
}

void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year)
{
    SimSetup s;
    s.orientation = orientation;
    s.pm = PM_NONE;
    s.difficulty = DIFF_NORMAL;
    s.turns_per_year = turns_per_year;
    s.seed = seed;
    sim_init_ex(c, &s);
}

s16 sim_popularity(const Country *c)
{
    s32 p = 4 * (s32)c->opinion[ELEC_POPULAIRES] + 4 * (s32)c->opinion[ELEC_MOYENNES]
          + 2 * (s32)c->opinion[ELEC_AISEES];
    return (s16)((p + 5) / 10);
}

u8 sim_mandate_turns(const Country *c)
{
    return (u8)(c->turns_per_year * SIM_MANDATE_YEARS);
}

u8 sim_mandate_over(const Country *c)
{
    return (u8)(c->turn >= sim_mandate_turns(c));
}

u8 sim_election_due(const Country *c)
{
    return (u8)(c->status == STATUS_PLAYING && c->mandate == 0 && sim_mandate_over(c));
}

u8 sim_game_over(const Country *c)
{
    if (c->status != STATUS_PLAYING) return 1;
    return (u8)(c->mandate > 0 && sim_mandate_over(c));
}

u8 sim_quarter_tick(const Country *c)
{
    // En mode mensuel, un trimestre se termine tous les 3 tours.
    if (c->turns_per_year <= 4) return 1;
    return (u8)((c->turn % 3) == 0);
}

u8 sim_year(const Country *c)
{
    return (u8)(c->turn / c->turns_per_year);
}

u8 sim_quarter(const Country *c)
{
    return (u8)((c->turn % c->turns_per_year) * 4 / c->turns_per_year);
}

s16 sim_step(s32 annual, s16 *acc, s16 tpy)
{
    s32 a = (s32)*acc + annual;
    s32 d = a / tpy;
    *acc = (s16)(a - d * tpy);
    return (s16)d;
}

s16 sim_pillar_progress(const Country *c)
{
    s16 sum = 0;
    u8 i;
    for (i = 0; i < PIL_COUNT; i++) sum += (s16)(c->pillars[i] - c->pillars_start[i]);
    return (s16)(sum / PIL_COUNT);
}

u8 sim_law_cost(const Country *c, u8 law_id)
{
    const LawDef *law = law_get(law_id);
    return law ? law_cost(c, law) : 0;
}

void sim_law_adopt(Country *c, u8 law_id, u8 gov)
{
    const LawDef *law = law_get(law_id);
    law_mask_set(&c->laws_adopted, law_id);
    if (gov) {
        // Loi du gouvernement : le président n'en récolte que la moitié de l'opinion.
        u8 i;
        for (i = 0; i < law->n_immediate; i++) {
            u8 k = law->immediate[i].key;
            s8 v = law->immediate[i].value;
            if (k >= EFF_POP_POPULAIRES && k <= EFF_POP_TOUS) v = (s8)(v / 2);
            sim_apply_effect(c, k, v);
        }
    } else {
        sim_apply_effects(c, law->immediate, law->n_immediate);
    }
    c->pending[c->n_pending].law = law_id;
    c->pending[c->n_pending].turns_left = law_delay_turns(c, law);
    c->n_pending++;
}

u8 sim_propose_law(Country *c, u8 law_id, VoteResult *out)
{
    const LawDef *law;
    u8 err = law_can_propose(c, law_id);
    if (err != LAW_OK) return err;
    law = law_get(law_id);

    c->cp = (u8)(c->cp - law_cost(c, law));
    parl_vote(c, law->orientation, out);
    parl_after_vote(c, law->orientation, out);

    if (out->adopted) {
        sim_law_adopt(c, law_id, 0);
    } else {
        // Échec : CP perdu, défiance en hausse, petit coût d'image.
        sim_apply_effect(c, EFF_DEFIANCE, 5);
        sim_apply_effect(c, EFF_POP_TOUS, -1);
    }
    return LAW_OK;
}

// --- 49.3 ---
#define ART49_DEFIANCE  15
#define ART49_REL       3

u8 sim_can_force_law(const Country *c, u8 law_id)
{
    const LawDef *law = law_get(law_id);
    u8 err = law_can_propose(c, law_id);
    if (err != LAW_OK && err != LAW_ERR_CP) return err;
    if (c->art49_year == sim_year(c) + 1) return LAW_ERR_493;
    if (c->cp < law_cost(c, law) + ART49_CP) return LAW_ERR_CP;
    return LAW_OK;
}

u8 sim_force_law(Country *c, u8 law_id)
{
    const LawDef *law;
    VoteResult v;
    u8 g, err = sim_can_force_law(c, law_id);
    if (err != LAW_OK) return err;
    law = law_get(law_id);
    c->cp = (u8)(c->cp - law_cost(c, law) - ART49_CP);
    c->art49_year = (u8)(sim_year(c) + 1);
    // Adoption sans vote : les relations évoluent comme pour une adoption,
    // l'opposition s'irrite en plus.
    v.adopted = 1;
    for (g = 0; g < GRP_COUNT; g++) v.pour[g] = 0;
    parl_after_vote(c, law->orientation, &v);
    for (g = 0; g < GRP_COUNT; g++)
        if (g != c->player_group)
            c->relation[g] = (u8)sim_clamp((s32)c->relation[g] - ART49_REL, 0, RELATION_MAX);
    sim_law_adopt(c, law_id, 0);
    sim_apply_effect(c, EFF_DEFIANCE, ART49_DEFIANCE);
    sim_apply_effect(c, EFF_POP_TOUS, -1);
    return LAW_OK;
}

// --- Intervention télévisée ---
#define TV_FLOP_BASE    15   // % d'échec sans usure
#define TV_FLOP_STEP    20   // % d'échec en plus par point d'usure
#define TV_FLOP_MAX     90
#define TV_GAIN         3    // points d'opinion en cas de succès
#define TV_LOSS         2    // points perdus en cas de flop

u8 sim_tv_flop_chance(const Country *c)
{
    u16 p = (u16)(TV_FLOP_BASE + TV_FLOP_STEP * (u16)c->tv_fatigue);
    return (u8)(p > TV_FLOP_MAX ? TV_FLOP_MAX : p);
}

u8 sim_tv_available(const Country *c)
{
    return (u8)(c->status == STATUS_PLAYING && c->cp >= TV_CP && c->tv_turn != c->turn + 1);
}

u8 sim_tv(Country *c)
{
    u8 flop;
    if (c->tv_turn == c->turn + 1) return TV_ERR_USED;
    if (c->cp < TV_CP || c->status != STATUS_PLAYING) return TV_ERR_CP;
    c->cp = (u8)(c->cp - TV_CP);
    c->tv_turn = (u8)(c->turn + 1);
    flop = (u8)(rng_below(&c->rng, 100) < sim_tv_flop_chance(c));
    if (c->tv_fatigue < 10) c->tv_fatigue++;
    if (flop) {
        sim_apply_effect(c, EFF_POP_TOUS, -TV_LOSS);
        return TV_FLOP;
    }
    sim_apply_effect(c, EFF_POP_TOUS, TV_GAIN);
    return TV_SUCCES;
}

// --- Premier ministre ---
u8 sim_change_pm(Country *c, u8 pm)
{
    u8 g;
    if (pm >= PM_COUNT || pm == c->pm || c->cohab) return 0;
    c->pm = pm;
    if (pm == PM_RASSEMBLEUR)
        for (g = 0; g < GRP_COUNT; g++)
            c->relation[g] = (u8)sim_clamp((s32)c->relation[g] + RASSEMBLEUR_INIT, 0, RELATION_MAX);
    return 1;
}

// --- Capital politique (par trimestre) ---
#define CP_BASE 1

u8 sim_cp_gain(const Country *c)
{
    s16 pop = sim_popularity(c);
    u8 g = CP_BASE;
    if (pop >= 500) g++;
    if (parl_support_seats(c) >= SEATS_MAJORITY) g++;
    return g;
}

// --- Jauges (par trimestre) ---
#define SCANDAL_DECAY      1    // décroissance : 1 point par trimestre
#define DEFIANCE_REF       10   // la défiance tend vers 10 + scandale × 3/4
#define DEFIANCE_DECAY     2    // points par trimestre
#define AFFAIR_PCT         4    // % de chance d'une affaire par trimestre (+ scandale / 6)
#define AFFAIR_MIN         6
#define AFFAIR_MAX         16

static u8 gauges_quarter(Country *c)
{
    u8 affair = 0;
    u8 s = c->gauges[JAUGE_SCANDALE];
    u8 d = c->gauges[JAUGE_DEFIANCE];
    u8 dec = SCANDAL_DECAY;
    u8 ref = (u8)(DEFIANCE_REF + s * 3 / 4);   // les affaires nourrissent la défiance
    if (d > ref + DEFIANCE_DECAY) d = (u8)(d - DEFIANCE_DECAY);
    else if (d + DEFIANCE_DECAY < ref) d = (u8)(d + DEFIANCE_DECAY);
    else d = ref;
    s = (u8)(s > dec ? s - dec : 0);
    c->gauges[JAUGE_SCANDALE] = s;
    c->gauges[JAUGE_DEFIANCE] = d;
    if (rng_below(&c->rng, 100) < (u16)(AFFAIR_PCT + c->gauges[JAUGE_SCANDALE] / 6)) {
        sim_gauge_add(&c->gauges[JAUGE_SCANDALE], rng_range(&c->rng, AFFAIR_MIN, AFFAIR_MAX));
        affair = 1;
    }
    if (c->tv_fatigue) c->tv_fatigue--;
    return affair;
}

void sim_end_turn(Country *c, TurnReport *report)
{
    TurnReport tmp;
    TurnReport *r = report ? report : &tmp;
    u8 year_end;

    r->popularity_before = sim_popularity(c);
    r->censure = CENSURE_NONE;
    r->censure_votes = 0;
    r->new_pm = PM_NONE;
    r->impeach = IMP_EV_NONE;
    r->impeach_votes = 0;
    r->budget_reconduit = 0;
    r->affair = 0;
    r->cp_gain = 0;
    r->n_matured = 0;
    r->impeach_senate_votes = 0;
    r->gov_law = 0;
    r->budget_gov = 0;
    r->senate_renewed = 0;
    if (c->status != STATUS_PLAYING) {
        r->popularity_after = r->popularity_before;
        return;
    }

    year_end = (u8)(((c->turn + 1) % c->turns_per_year) == 0);
    c->turn++;

    // 1. Lois arrivées à maturité
    r->n_matured = law_process_pending(c, r->matured);
    // 2. Budget (piliers, au trimestre)
    if (sim_quarter_tick(c)) budget_quarter_update(c);
    // 3. Économie
    sim_economy_update(c);
    // 4. Opinion
    sim_opinion_update(c);
    // 5. Parlement
    parl_end_turn(c);
    // 5b. V2 : en cohabitation, le gouvernement propose ses lois ; l'état de grâce
    // du remaniement s'érode.
    if (sim_quarter_tick(c)) {
        r->gov_law = cohab_government_quarter(c);
        c->grace_pm = (u8)(c->grace_pm > GRACE_PM_DECAY ? c->grace_pm - GRACE_PM_DECAY : 0);
    }
    // 6. Jauges
    if (sim_quarter_tick(c)) r->affair = gauges_quarter(c);
    // 7. Clôture de l'année budgétaire : sans budget adopté, reconduction.
    if (year_end) {
        if (c->cohab) {
            // Cohabitation : le Premier ministre d'opposition décide du budget.
            c->budget = sim_cohab_budget(c);
            r->budget_gov = 1;
        } else if (!c->budget_done) {
            c->budget = BUDGET_EQUILIBRE;
            r->budget_reconduit = 1;
        }
        c->budget_done = 0;
    }
    // 8. Capital politique (par trimestre)
    if (sim_quarter_tick(c)) {
        u8 before = c->cp;
        sim_apply_effect(c, EFF_CP, (s8)sim_cp_gain(c));
        r->cp_gain = (u8)(c->cp - before);
    }
    // 9. Institutions : destitution, censure (au trimestre)
    if (sim_quarter_tick(c))
        inst_end_turn_quarter(c, &r->censure, &r->censure_votes, &r->new_pm,
                              &r->impeach, &r->impeach_votes, &r->impeach_senate_votes);
    // 10. V2 : renouvellement du Sénat par moitié (début de l'année 3, 6, 9).
    if (c->status == STATUS_PLAYING && sim_senate_renewal_due(c)) {
        sim_senate_renew(c, r->senate_before);
        r->senate_renewed = 1;
    }
    r->popularity_after = sim_popularity(c);
}

// --- Second mandat ---

void sim_set_theme(Country *c, u8 theme)
{
    c->theme = theme < THEME_COUNT ? theme : THEME_NONE;
}

// Électorats sensibles à chaque thème (poids /4) : populaires, moyennes, aisées.
static const u8 theme_weight[THEME_COUNT][ELEC_COUNT] = {
    { 4, 3, 1 },   // emploi
    { 2, 3, 4 },   // sécurité
    { 4, 4, 0 },   // pouvoir d'achat
    { 1, 4, 3 },   // écologie
};

#define THEME_BASE  20   // bonus de base (dixièmes de %) pour un électorat très sensible

static s16 theme_credibility(const Country *c, u8 theme)
{
    s32 cr;
    switch (theme) {
    case THEME_EMPLOI:
        cr = ((s32)c->unemployment_start - c->unemployment) / 5;   // -1 pt : +20
        break;
    case THEME_SECURITE:
        cr = ((s32)c->pillars[PIL_SECURITE] - c->pillars_start[PIL_SECURITE]) / 3;
        break;
    case THEME_POUVOIR_ACHAT:
        cr = ((s32)c->pillars[PIL_SOCIAL] - c->pillars_start[PIL_SOCIAL]) / 3
           + (c->budget == BUDGET_RIGUEUR ? -10 : 0);
        break;
    default:   // THEME_ECOLOGIE
        cr = ((s32)c->pillars[PIL_ECOLOGIE] - c->pillars_start[PIL_ECOLOGIE]) / 3;
        break;
    }
    return sim_clamp(cr, -20, 30);
}

s16 sim_theme_bonus(const Country *c, u8 theme, u8 e)
{
    if (theme >= THEME_COUNT || e >= ELEC_COUNT) return 0;
    return (s16)(((s32)THEME_BASE + theme_credibility(c, theme)) * theme_weight[theme][e] / 4);
}

void sim_start_second_mandate(Country *c, LegislativeResult *out)
{
    u8 i;
    c->score_first = sim_mandate_score(c, 1);
    c->mandate = 1;
    c->turn = 0;
    for (i = 0; i < PIL_COUNT; i++) c->pillars_start[i] = c->pillars[i];
    c->unemployment_start = c->unemployment;
    c->debt_start = c->debt;
    c->grace = GRACE_START2;
    c->theme = THEME_NONE;
    c->events_done = 0;
    c->last_event = 0;
    c->art49_year = 0;
    c->tv_turn = 0;
    c->budget_done = 0;
    c->impeach = IMPEACH_NONE;
    c->impeach_def = 0;
    c->impeach_cool = 0;
    c->events_done_hi = 0;
    c->referendum_done = 0;
    c->reshuffle_year = 0;
    c->grace_pm = 0;
    law_mask_clear(&c->laws_adopted);   // le catalogue redevient disponible au 2nd mandat
    sim_legislatives(c, out);
}

u16 sim_score_first(const Country *c)
{
    return c->score_first;
}

u16 sim_score_total(const Country *c)
{
    u32 t = (u32)c->score_first + sim_mandate_score(c, 0);
    return (u16)(t > 65535u ? 65535u : t);
}
