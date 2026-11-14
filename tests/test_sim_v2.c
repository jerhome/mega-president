#include "test.h"
#include "sim/sim.h"
#include "sim/events.h"

// Tests V2 : capacité d'événements (64), Sénat (2.05), cohabitation (2.04),
// référendum (2.06), remaniement (2.07).

static void new_game(Country *c, u8 orient, u8 diff, u32 seed)
{
    SimSetup s;
    s.orientation = orient;
    s.pm = PM_TECHNOCRATE;
    s.difficulty = diff;
    s.turns_per_year = 4;
    s.seed = seed;
    sim_init_ex(c, &s);
}

static void set_all_opinions(Country *c, s16 v)
{
    int e;
    for (e = 0; e < ELEC_COUNT; e++) c->opinion[e] = v;
}

// --- Événements : capacité 64 ----------------------------------------------

static void test_event_capacity(void)
{
    Country c;
    u8 id;
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 1);
    CHECK_EQ(EVENT_CAPACITY, 64);
    CHECK(EVT_COUNT <= EVENT_CAPACITY);
    for (id = 0; id < 64; id++) CHECK(!events_done_get(&c, id));
    events_done_set(&c, 5);
    events_done_set(&c, 31);
    events_done_set(&c, 32);
    events_done_set(&c, 63);
    for (id = 0; id < 64; id++)
        CHECK_EQ(events_done_get(&c, id), (id == 5 || id == 31 || id == 32 || id == 63));
    CHECK_EQ(c.events_done, (1u << 5) | (1u << 31));
    CHECK_EQ(c.events_done_hi, 1u | (1u << 31));
    // Le second mandat remet le compteur à zéro.
    sim_start_second_mandate(&c, 0);
    CHECK_EQ(c.events_done, 0);
    CHECK_EQ(c.events_done_hi, 0);
}

// --- Sénat -----------------------------------------------------------------

static u16 senate_sum(const Country *c)
{
    u16 s = 0;
    int g;
    for (g = 0; g < GRP_COUNT; g++) s = (u16)(s + c->senate[g]);
    return s;
}

static void test_senate(void)
{
    Country c;
    int d, o, g, t;
    TurnReport r;

    for (d = 0; d < DIFF_COUNT; d++)
        for (o = 0; o < ORIENT_COUNT; o++) {
            new_game(&c, (u8)o, (u8)d, 7);
            CHECK_EQ(senate_sum(&c), SENATE_SEATS);
            for (g = 0; g < GRP_COUNT; g++) CHECK(c.senate[g] > 0);
        }
    // La difficulté change le Sénat : le bloc du joueur est plus fort en Facile.
    {
        Country f, h;
        new_game(&f, ORIENT_GAUCHE, DIFF_FACILE, 7);
        new_game(&h, ORIENT_GAUCHE, DIFF_DIFFICILE, 7);
        CHECK(sim_senate_bloc_seats(&f) > sim_senate_bloc_seats(&h));
        CHECK(sim_senate_support_seats(&f) >= sim_senate_support_seats(&h));
    }
    // Plus stable que l'Assemblée : l'écart entre difficultés est plus petit (en %).
    {
        Country f, h;
        s32 ds, da;
        new_game(&f, ORIENT_CENTRE, DIFF_FACILE, 7);
        new_game(&h, ORIENT_CENTRE, DIFF_DIFFICILE, 7);
        ds = (s32)sim_senate_bloc_seats(&f) * 1000 / SENATE_SEATS
           - (s32)sim_senate_bloc_seats(&h) * 1000 / SENATE_SEATS;
        da = (s32)sim_bloc_seats(&f) * 1000 / SEATS_TOTAL - (s32)sim_bloc_seats(&h) * 1000 / SEATS_TOTAL;
        CHECK(ds >= 0 && ds < da);
    }

    // Renouvellement : années 3, 6 et 9 de la partie (aux tours 12, 4 et 24).
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 3);
    {
        int renewed = 0, first = -1;
        for (t = 0; t < 20; t++) {
            sim_end_turn(&c, &r);
            CHECK_EQ(senate_sum(&c), SENATE_SEATS);
            if (r.senate_renewed) { renewed++; if (first < 0) first = c.turn; }
            if (c.status != STATUS_PLAYING) break;
        }
        CHECK_EQ(renewed, 1);
        CHECK_EQ(first, 12);
        CHECK_EQ(sim_senate_renewal_in(&c), 0);
    }
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 3);
    CHECK_EQ(sim_senate_renewal_in(&c), 12);
    c.turn = 12;
    CHECK(sim_senate_renewal_due(&c));
    c.turn = 11;
    CHECK(!sim_senate_renewal_due(&c));
    // 2nd mandat : renouvellements à l'année 1 (6e année) et 4 (9e année).
    c.mandate = 1;
    c.turn = 0;
    CHECK_EQ(sim_senate_renewal_in(&c), 4);
    c.turn = 4;
    CHECK(sim_senate_renewal_due(&c));
    c.turn = 16;
    CHECK(sim_senate_renewal_due(&c));
    c.turn = 8;
    CHECK(!sim_senate_renewal_due(&c));

    // Inertie forte : la moitié n'est pas renouvelée, et la popularité oriente le reste.
    {
        Country hi, lo;
        u16 before[GRP_COUNT];
        new_game(&hi, ORIENT_CENTRE, DIFF_NORMAL, 9);
        lo = hi;
        set_all_opinions(&hi, 750);
        set_all_opinions(&lo, 200);
        sim_senate_renew(&hi, before);
        sim_senate_renew(&lo, 0);
        CHECK_EQ(senate_sum(&hi), SENATE_SEATS);
        CHECK_EQ(senate_sum(&lo), SENATE_SEATS);
        for (g = 0; g < GRP_COUNT; g++) {
            int dh = (int)hi.senate[g] - before[g];
            CHECK(dh >= -(int)(before[g] / 2) - 1 && dh <= (int)(before[g] / 2) + 60);
            CHECK(hi.senate[g] >= before[g] / 2);
            CHECK(lo.senate[g] >= before[g] / 2);
        }
        CHECK(sim_senate_bloc_seats(&hi) > sim_senate_bloc_seats(&lo));
        // Variation maximale du bloc : bien inférieure à celle d'une dissolution.
        CHECK(sim_senate_bloc_seats(&hi) - sim_senate_bloc_seats(&lo) < 120);
    }

    // Destitution : deux tiers de l'Assemblée ET du Sénat.
    {
        Country a;
        int ok_both = 0;
        new_game(&a, ORIENT_GAUCHE, DIFF_NORMAL, 5);
        a.gauges[JAUGE_SCANDALE] = 100;
        a.gauges[JAUGE_DEFIANCE] = 100;
        for (g = 0; g < GRP_COUNT; g++) a.relation[g] = 0;
        CHECK(sim_impeach_votes(&a) >= IMPEACH_MAJORITY);
        CHECK(sim_impeach_senate_votes(&a) >= SENATE_IMPEACH_MAJORITY);
        CHECK(sim_impeach_would_pass(&a));
        // Sénat protecteur : le parti du joueur y domine (> 1/3 des sièges).
        a.senate[GRP_G2] = 200;
        a.senate[GRP_G1] = 30; a.senate[GRP_G3] = 50; a.senate[GRP_G4] = 40; a.senate[GRP_G5] = 28;
        CHECK_EQ(senate_sum(&a), SENATE_SEATS);
        CHECK(sim_impeach_senate_votes(&a) < SENATE_IMPEACH_MAJORITY);
        CHECK(!sim_impeach_would_pass(&a));
        // Le vote du trimestre applique la règle : procédure en cours, fin de trimestre.
        a.impeach = IMPEACH_DEFENSE;
        sim_end_turn(&a, &r);
        CHECK_EQ(a.status, STATUS_PLAYING);
        CHECK_EQ(r.impeach, IMP_EV_REJETEE);
        CHECK(r.impeach_votes >= IMPEACH_MAJORITY);
        CHECK(r.impeach_senate_votes < SENATE_IMPEACH_MAJORITY);
        // Même situation avec un Sénat hostile : destitué.
        new_game(&a, ORIENT_GAUCHE, DIFF_NORMAL, 5);
        a.gauges[JAUGE_SCANDALE] = 100;
        a.gauges[JAUGE_DEFIANCE] = 100;
        for (g = 0; g < GRP_COUNT; g++) a.relation[g] = 0;
        a.relation_base[GRP_G3] = a.relation_base[GRP_G4] = a.relation_base[GRP_G5] = 0;
        a.impeach = IMPEACH_DEFENSE;
        a.turn = 3;
        sim_end_turn(&a, &r);
        ok_both = (a.status == STATUS_DESTITUE) && r.impeach == IMP_EV_VOTEE;
        CHECK(ok_both);
        CHECK(r.impeach_senate_votes >= SENATE_IMPEACH_MAJORITY);
    }
}

// --- Cohabitation ----------------------------------------------------------

// Avance de n tours sans action (les tests ne dépendent pas du détail du hasard).
static void run_turns(Country *c, int n, TurnReport *r)
{
    int i;
    for (i = 0; i < n && c->status == STATUS_PLAYING && !sim_mandate_over(c); i++)
        sim_end_turn(c, r);
}

// Premier état de cohabitation obtenu par une dissolution perdue (graines successives).
static int find_cohab(Country *c, u8 orient)
{
    u32 seed;
    for (seed = 1; seed < 200; seed++) {
        LegislativeResult lr;
        new_game(c, orient, DIFF_NORMAL, seed);
        c->turn = 8;
        set_all_opinions(c, 250);
        if (sim_dissolve(c, &lr) && lr.cohab_started) return 1;
    }
    return 0;
}

static void test_cohabitation(void)
{
    Country c;
    TurnReport r;
    LegislativeResult lr;
    int o, ok = 1, i;

    // Les lois de souveraineté : police, immigration, énergie, recherche.
    CHECK(sim_law_sovereign(LOI_POLICE) && sim_law_sovereign(LOI_IMMIGRATION));
    CHECK(sim_law_sovereign(LOI_ENERGIE) && sim_law_sovereign(LOI_CIR));
    CHECK(!sim_law_sovereign(LOI_SMIC) && !sim_law_sovereign(LOI_RETRAITES));
    CHECK(!sim_law_sovereign(LOI_COUNT));

    // Hors cohabitation : rien ne change.
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 4);
    CHECK(!sim_cohabitation(&c));
    CHECK_EQ(sim_cohab_next_law(&c), LOI_COUNT);
    CHECK_EQ(law_can_propose(&c, LOI_SMIC), LAW_OK);

    for (o = 0; o < ORIENT_COUNT; o++) ok &= find_cohab(&c, (u8)o);
    CHECK(ok);

    // Début de cohabitation (centre).
    ok = find_cohab(&c, ORIENT_CENTRE);
    CHECK(ok);
    CHECK(sim_cohabitation(&c));
    CHECK_EQ(c.pm, PM_OPPOSITION);
    CHECK(c.pm >= PM_COUNT);
    CHECK_EQ(c.pm_prev, PM_TECHNOCRATE);
    CHECK(!sim_in_bloc(&c, c.cohab_group));
    CHECK(sim_cohab_camp(&c) == ORIENT_GAUCHE || sim_cohab_camp(&c) == ORIENT_DROITE);
    CHECK_EQ(c.cohabitations, 1);
    CHECK(!sim_has_majority(&c));

    // Le joueur ne propose que des lois de souveraineté.
    c.cp = 15;
    CHECK_EQ(law_can_propose(&c, LOI_SMIC), LAW_ERR_COHAB);
    CHECK_EQ(sim_can_force_law(&c, LOI_HOPITAL), LAW_ERR_COHAB);
    CHECK_EQ(law_can_propose(&c, LOI_POLICE), LAW_OK);
    {
        VoteResult v;
        u8 cp = c.cp;
        CHECK_EQ(sim_propose_law(&c, LOI_RETRAITES, &v), LAW_ERR_COHAB);
        CHECK_EQ(c.cp, cp);
        CHECK_EQ(sim_propose_law(&c, LOI_POLICE, &v), LAW_OK);
        CHECK(c.cp < cp);
    }
    // Pas de remaniement, pas de changement de PM, pas de censure, pas de vote du budget.
    CHECK(!sim_change_pm(&c, PM_RASSEMBLEUR));
    CHECK_EQ(c.pm, PM_OPPOSITION);
    CHECK_EQ(sim_reshuffle_check(&c, PM_RASSEMBLEUR), RSH_ERR_COHAB);
    c.gauges[JAUGE_DEFIANCE] = 90;
    c.relation[GRP_G1] = c.relation[GRP_G2] = c.relation[GRP_G4] = c.relation[GRP_G5] = 0;
    CHECK(!sim_censure_threat(&c));
    c.gauges[JAUGE_DEFIANCE] = 20;
    c.turn = 3;
    CHECK(!sim_budget_due(&c));
    {
        VoteResult v;
        CHECK_EQ(sim_budget_vote(&c, BUDGET_RELANCE, &v), BUDGET_ERR_NOT_DUE);
        CHECK_EQ(sim_budget_force(&c, BUDGET_RELANCE), BUDGET_ERR_NOT_DUE);
    }
    // Budget décidé par le Premier ministre selon son camp.
    CHECK(sim_cohab_budget(&c) == BUDGET_RELANCE || sim_cohab_budget(&c) == BUDGET_RIGUEUR);
    CHECK_EQ(sim_cohab_budget(&c), sim_cohab_camp(&c) == ORIENT_GAUCHE ? BUDGET_RELANCE : BUDGET_RIGUEUR);

    // Le référendum reste possible sur une loi non souveraine.
    c.cp = 15;
    CHECK_EQ(sim_referendum_check(&c, LOI_SMIC), RFD_OK);

    // Le gouvernement propose des lois non souveraines, avec effets ; budget imposé.
    {
        int gov = 0, budgets = 0, sov_gov = 0;
        u16 mask_before;
        find_cohab(&c, ORIENT_CENTRE);
        c.turn = 8;
        c.laws_adopted = 0;
        c.n_pending = 0;
        mask_before = c.laws_adopted;
        for (i = 0; i < 12; i++) {
            sim_end_turn(&c, &r);
            if (c.status != STATUS_PLAYING) break;
            if (r.gov_law) {
                gov++;
                CHECK(r.gov_law - 1 < LOI_COUNT);
                if (sim_law_sovereign((u8)(r.gov_law - 1))) sov_gov++;
                CHECK(law_is_adopted(&c, (u8)(r.gov_law - 1)));
            }
            if (r.budget_gov) { budgets++; CHECK_EQ(c.budget, sim_cohab_budget(&c)); }
            CHECK(!r.budget_reconduit || !r.budget_gov);
            if (sim_mandate_over(&c)) break;
        }
        (void)mask_before;
        CHECK(gov >= 1);
        CHECK_EQ(sov_gov, 0);
        CHECK(budgets >= 1);
        CHECK(sim_cohabitation(&c));   // pas de fin sans législatives
        CHECK_EQ(c.gov_laws, gov);
    }

    // Effet d'opinion d'une loi du gouvernement : moitié moins que celle du président.
    {
        Country a, b;
        new_game(&a, ORIENT_CENTRE, DIFF_NORMAL, 4);
        b = a;
        sim_law_adopt(&a, LOI_SMIC, 0);   // POP_POPULAIRES +4, POP_AISEES -3
        sim_law_adopt(&b, LOI_SMIC, 1);
        CHECK_EQ(a.opinion[ELEC_POPULAIRES] - b.opinion[ELEC_POPULAIRES], 20);
    }

    // Popularité moins exposée aux échecs : cible relevée sous le seuil, intacte au-dessus.
    {
        Country a, b;
        new_game(&a, ORIENT_CENTRE, DIFF_DIFFICILE, 4);
        a.pm = PM_NONE;          // sans effet de profil de PM (le Technocrate pèse sur l'opinion)
        a.unemployment = 1500;   // chômage très élevé : cible très basse
        b = a;
        b.cohab = 1;
        b.pm = PM_OPPOSITION;
        CHECK(sim_opinion_target(&a, ELEC_MOYENNES) < COHAB_OPINION_REF);
        CHECK(sim_opinion_target(&b, ELEC_MOYENNES) > sim_opinion_target(&a, ELEC_MOYENNES));
        {
            s16 ta = sim_opinion_target(&a, ELEC_MOYENNES);
            s16 tb = sim_opinion_target(&b, ELEC_MOYENNES);
            // exposition aux deux tiers de l'écart (à 1 point près d'arrondi)
            CHECK(sim_abs16((s16)((COHAB_OPINION_REF - tb) * 3 - (COHAB_OPINION_REF - ta) * 2)) <= 3);
        }
        a.unemployment = 300;
        b = a;
        b.cohab = 1;
        b.pm = PM_OPPOSITION;
        a.growth = b.growth = 300;
        a.budget = b.budget = BUDGET_RELANCE;
        CHECK_EQ(sim_opinion_target(&a, ELEC_MOYENNES) >= COHAB_OPINION_REF, 1);
        CHECK_EQ(sim_opinion_target(&a, ELEC_MOYENNES), sim_opinion_target(&b, ELEC_MOYENNES));
    }

    // Fin de cohabitation : législatives gagnées.
    {
        int ended = 0;
        u32 seed;
        for (seed = 1; seed < 60 && !ended; seed++) {
            find_cohab(&c, ORIENT_DROITE);
            c.pm_prev = PM_REFORMATEUR;
            run_turns(&c, 6, &r);   // plus d'un an depuis les législatives
            set_all_opinions(&c, 800);
            c.rng.state ^= (u32)seed * 7919u;
            if (sim_dissolve(&c, &lr) && lr.cohab_ended) {
                ended = 1;
                CHECK(lr.won);
                CHECK(!sim_cohabitation(&c));
                CHECK_EQ(c.pm, PM_REFORMATEUR);
                CHECK(!lr.cohab_started);
                CHECK_EQ(law_can_propose(&c, LOI_SMIC), LAW_OK);
                CHECK(sim_budget_due(&c) || sim_quarter(&c) != 3);
            }
        }
        CHECK(ended);
    }

    // Une dissolution gagnée hors cohabitation n'en déclenche pas.
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 12);
    c.turn = 8;
    set_all_opinions(&c, 800);
    CHECK(sim_dissolve(&c, &lr));
    CHECK(!lr.cohab_started && !lr.cohab_ended);
    CHECK(!sim_cohabitation(&c));
}

// --- Référendum ------------------------------------------------------------

static void test_referendum(void)
{
    Country c, a;
    RefResult r, p;
    u32 seed;
    int wins = 0, losses = 0, hi = 0, lo = 0, n = 0;

    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 21);
    c.cp = 15;
    CHECK_EQ(sim_referendum_check(&c, LOI_COUNT), RFD_ERR_ID);
    c.cp = 5;
    CHECK_EQ(sim_referendum_check(&c, LOI_SMIC), RFD_ERR_CP);
    c.cp = 6;
    CHECK_EQ(sim_referendum_check(&c, LOI_SMIC), RFD_OK);
    c.laws_adopted = (u16)(1u << LOI_SMIC);
    CHECK_EQ(sim_referendum_check(&c, LOI_SMIC), RFD_ERR_ADOPTED);
    c.laws_adopted = 0;
    c.n_pending = SIM_MAX_PENDING;
    CHECK_EQ(sim_referendum_check(&c, LOI_SMIC), RFD_ERR_FULL);
    c.n_pending = 0;
    {
        u8 cp = c.cp;
        CHECK_EQ(sim_referendum(&c, LOI_COUNT, &r), RFD_ERR_ID);
        CHECK_EQ(c.cp, cp);
    }

    // Aperçu : sans effet, déterministe, borné, cohérent avec la loi.
    a = c;
    sim_referendum_preview(&c, LOI_SMIC, &p);
    CHECK_EQ(c.cp, a.cp);
    CHECK_EQ(c.rng.state, a.rng.state);
    CHECK(p.yes >= 50 && p.yes <= 950);
    // Les classes populaires approuvent plus le SMIC que les aisées.
    CHECK(p.yes_elec[ELEC_POPULAIRES] > p.yes_elec[ELEC_AISEES]);
    sim_referendum_preview(&c, LOI_IS, &p);
    CHECK(p.yes_elec[ELEC_AISEES] > p.yes_elec[ELEC_POPULAIRES]);
    sim_referendum_preview(&c, LOI_COUNT, &p);
    CHECK_EQ(p.yes, 0);

    // Succès ou échec selon l'aléa ; effets, une fois par mandat.
    for (seed = 1; seed <= 400; seed++) {
        Country b;
        s16 pop0;
        u8 def0;
        u8 law = (u8)(seed % LOI_COUNT);
        new_game(&b, (u8)(seed % 3), DIFF_NORMAL, seed);
        b.cp = 10;
        pop0 = sim_popularity(&b);
        def0 = b.gauges[JAUGE_DEFIANCE];
        CHECK_EQ(sim_referendum(&b, law, &r), RFD_OK);
        n++;
        CHECK_EQ(b.cp, 4);
        CHECK(b.referendum_done);
        CHECK_EQ(sim_referendum_check(&b, (u8)((law + 1) % LOI_COUNT)), RFD_ERR_USED);
        if (r.adopted) {
            wins++;
            CHECK(law_is_adopted(&b, law));
            CHECK_EQ(b.n_pending, 1);
            CHECK(sim_popularity(&b) >= pop0 + 2 * REFERENDUM_POP_WIN / 2 - 6);   // loi : effets immédiats
            CHECK(b.gauges[JAUGE_DEFIANCE] <= def0);
            CHECK(r.yes >= 500);
        } else {
            losses++;
            CHECK(!law_is_adopted(&b, law));
            CHECK_EQ(b.n_pending, 0);
            CHECK(sim_popularity(&b) <= pop0 - REFERENDUM_POP_LOSS * 10 + 1);
            CHECK_EQ(b.gauges[JAUGE_DEFIANCE], def0 + REFERENDUM_DEF_LOSS);
            CHECK(r.yes < 500);
        }
        // Aucun vote du Parlement : les relations ne bougent pas à l'adoption.
        // (seulement par la dérive, absente ici)
    }
    CHECK(wins > 40 && losses > 40);
    printf("  Référendum : %d succès, %d échecs sur %d (lois et orientations variées)\n", wins, losses, n);

    // Un joueur populaire gagne plus souvent qu'un joueur impopulaire.
    for (seed = 1; seed <= 300; seed++) {
        Country b;
        new_game(&b, ORIENT_CENTRE, DIFF_NORMAL, seed);
        b.cp = 10;
        set_all_opinions(&b, 600);
        if (sim_referendum(&b, LOI_HOPITAL, &r) == RFD_OK && r.adopted) hi++;
        new_game(&b, ORIENT_CENTRE, DIFF_NORMAL, seed);
        b.cp = 10;
        set_all_opinions(&b, 280);
        if (sim_referendum(&b, LOI_HOPITAL, &r) == RFD_OK && r.adopted) lo++;
    }
    CHECK(hi > lo + 60);

    // Le 2nd mandat rend un nouveau référendum possible.
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 22);
    c.cp = 12;
    sim_referendum(&c, LOI_SMIC, &r);
    CHECK_EQ(sim_referendum_check(&c, LOI_ISF), RFD_ERR_USED);
    sim_start_second_mandate(&c, 0);
    c.cp = 12;
    CHECK_EQ(sim_referendum_check(&c, LOI_ISF), RFD_OK);
}

// --- Remaniement -----------------------------------------------------------

static void test_reshuffle(void)
{
    Country c;
    TurnReport r;
    u8 rel[GRP_COUNT];
    int g;
    s16 pop;

    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 30);
    c.cp = 10;
    CHECK_EQ(sim_reshuffle_check(&c, PM_TECHNOCRATE), RSH_ERR_PM);   // déjà en poste
    CHECK_EQ(sim_reshuffle_check(&c, PM_COUNT), RSH_ERR_PM);
    CHECK_EQ(sim_reshuffle_check(&c, PM_NONE), RSH_ERR_PM);
    c.cp = 2;
    CHECK_EQ(sim_reshuffle_check(&c, PM_REFORMATEUR), RSH_ERR_CP);
    c.cp = 10;
    for (g = 0; g < GRP_COUNT; g++) rel[g] = c.relation[g];
    pop = sim_popularity(&c);
    CHECK_EQ(sim_reshuffle(&c, PM_RASSEMBLEUR), RSH_OK);
    CHECK_EQ(c.pm, PM_RASSEMBLEUR);
    CHECK_EQ(c.cp, 10 - RESHUFFLE_CP);
    for (g = 0; g < GRP_COUNT; g++)   // relations : +5 (Rassembleur) + 2 (remaniement)
        CHECK_EQ(c.relation[g], rel[g] + 5 + RESHUFFLE_REL);
    CHECK_EQ(c.grace_pm, RESHUFFLE_GRACE);
    CHECK_EQ(sim_popularity(&c), pop + RESHUFFLE_POP * 10);
    // Au plus un par an.
    CHECK_EQ(sim_reshuffle_check(&c, PM_REFORMATEUR), RSH_ERR_YEAR);
    CHECK_EQ(sim_reshuffle(&c, PM_REFORMATEUR), RSH_ERR_YEAR);
    CHECK_EQ(c.pm, PM_RASSEMBLEUR);
    // L'état de grâce s'érode par trimestre (6 points).
    sim_end_turn(&c, &r);
    CHECK_EQ(c.grace_pm, RESHUFFLE_GRACE - 6);
    // État de grâce : la cible d'opinion est relevée.
    {
        Country a = c;
        a.grace_pm = 0;
        CHECK_EQ(sim_opinion_target(&c, ELEC_MOYENNES) - sim_opinion_target(&a, ELEC_MOYENNES), c.grace_pm);
    }
    sim_end_turn(&c, &r); sim_end_turn(&c, &r); sim_end_turn(&c, &r);   // fin de l'année
    CHECK_EQ(c.grace_pm, 6);
    CHECK_EQ(sim_year(&c), 1);
    c.cp = 10;
    CHECK_EQ(sim_reshuffle_check(&c, PM_REFORMATEUR), RSH_OK);
    sim_end_turn(&c, &r); sim_end_turn(&c, &r);
    CHECK_EQ(c.grace_pm, 0);
    // Mode 60 tours : l'érosion se fait au trimestre (tous les 3 tours).
    {
        Country m;
        SimSetup s;
        s.orientation = ORIENT_CENTRE; s.pm = PM_TECHNOCRATE; s.difficulty = DIFF_NORMAL;
        s.turns_per_year = 12; s.seed = 5;
        sim_init_ex(&m, &s);
        m.cp = 10;
        CHECK_EQ(sim_reshuffle(&m, PM_REFORMATEUR), RSH_OK);
        sim_end_turn(&m, 0); sim_end_turn(&m, 0); sim_end_turn(&m, 0);
        CHECK_EQ(m.grace_pm, RESHUFFLE_GRACE - 6);
    }
}

// --- Compatibilité de la taille de l'état ----------------------------------

static void test_country_size(void)
{
    // Country doit rester compact (RAM Megadrive) : borne large, la taille réelle
    // est reportée dans docs/SIM.md.
    printf("  sizeof(Country) = %u octets (PC)\n", (unsigned)sizeof(Country));
    CHECK(sizeof(Country) <= 220);
}

void test_sim_v2(void)
{
    test_event_capacity();
    test_senate();
    test_cohabitation();
    test_referendum();
    test_reshuffle();
    test_country_size();
}
