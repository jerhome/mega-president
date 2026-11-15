#ifndef SIM_SIM_H
#define SIM_SIM_H

// API principale du cœur de simulation (tâches 2.1, 2.2, 2.4, V1 1.01-1.11).
// Formules et constantes : docs/SIM.md.

#include "sim/sim_state.h"
#include "sim/effects.h"
#include "sim/laws.h"
#include "sim/parliament.h"
#include "sim/election.h"
#include "sim/budget.h"
#include "sim/institutions.h"
#include "sim/senate.h"
#include "sim/cohab.h"
#include "sim/actions.h"
#include "sim/regions.h"

#define SIM_TURNS_PER_YEAR_V0 4
#define SIM_MANDATE_YEARS     5

// Paramètres d'une nouvelle partie (V1).
typedef struct {
    u8  orientation;      // ORIENT_*
    u8  pm;               // PM_* (PM_NONE : aucun profil)
    u8  difficulty;       // DIFF_*
    u8  turns_per_year;   // 4 (20 tours) ou 12 (60 tours)
    u32 seed;
} SimSetup;

// Nouvelle partie V1.
void sim_init_ex(Country *c, const SimSetup *setup);

// Compatibilité V0 : sim_init_ex sans Premier ministre, difficulté Normal.
void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year);

// Poids électoraux en % des suffrages exprimés : Pop 26, Moy 22, Cad 12, Ret 28, Jeu 12.
extern const u8 elec_weight[ELEC_COUNT];

// Popularité globale (dixièmes de %) : moyenne des électorats pondérée par elec_weight.
s16 sim_popularity(const Country *c);
// Opinion de l'électorat-cœur de l'orientation du joueur (législatives, Sénat) :
// gauche (Pop + Jeu) / 2, centre (Moy + Ret) / 2, droite (Cad + Ret) / 2.
s16 sim_core_opinion(const Country *c);

// Nombre total de tours du mandat (20 ou 60).
u8 sim_mandate_turns(const Country *c);
// 1 quand le mandat en cours est terminé (tours épuisés).
u8 sim_mandate_over(const Country *c);
// 1 si une présidentielle doit avoir lieu : fin du 1er mandat (pas de 3e mandat).
u8 sim_election_due(const Country *c);
// 1 si la partie est finie : destitution, ou fin du 2nd mandat.
u8 sim_game_over(const Country *c);
// 1 si le tour courant termine un trimestre (toujours vrai à 4 tours par an).
u8 sim_quarter_tick(const Country *c);
// Année du mandat en cours (0-4) et trimestre de l'année (0-3) du tour à jouer.
u8 sim_year(const Country *c);
u8 sim_quarter(const Country *c);

// Propose une loi : paie le CP, fait voter l'Assemblée ; si adoptée, applique
// les effets immédiats et met les effets différés en attente.
// Renvoie LAW_OK (vote effectué, voir out->adopted) ou LAW_ERR_*.
u8 sim_propose_law(Country *c, u8 law_id, VoteResult *out);

// --- 49.3 (REGLES §8) ---
#define ART49_CP          4    // coût de la procédure (en plus du coût de la loi)
// LAW_OK si la loi peut être adoptée par 49.3, sinon LAW_ERR_* (dont LAW_ERR_493).
u8 sim_can_force_law(const Country *c, u8 law_id);
// Adopte la loi sans vote : coût de la loi + 4 CP, défiance ++, une fois par an.
u8 sim_force_law(Country *c, u8 law_id);

// --- Intervention télévisée ---
#define TV_CP             2
enum { TV_SUCCES, TV_FLOP, TV_ERR_CP, TV_ERR_USED };
// Probabilité d'échec (en %) de la prochaine intervention (aperçu, sans effet).
u8 sim_tv_flop_chance(const Country *c);
// 1 si une intervention est possible ce tour (CP et pas déjà faite).
u8 sim_tv_available(const Country *c);
// Intervention : 2 CP ; TV_SUCCES (popularité +) ou TV_FLOP (popularité -).
u8 sim_tv(Country *c);

// --- Premier ministre ---
// Change de Premier ministre (après une censure, ou au début de partie).
// Renvoie 1 si appliqué (pm valide et différent de l'actuel). Refusé (0) pendant
// une cohabitation. En cohabitation, c->pm vaut PM_OPPOSITION (>= PM_COUNT).
u8 sim_change_pm(Country *c, u8 pm);
// Adopte une loi : bit « adoptée », effets immédiats, mise en attente des effets
// différés. gov = 1 : loi du gouvernement de cohabitation (effets d'opinion /2).
// Ne contrôle rien (voir law_can_propose). Interne, exposé pour les actions V2.
void sim_law_adopt(Country *c, u8 law_id, u8 gov);
// Coût effectif d'une loi (le Réformateur la rend moins chère).
u8 sim_law_cost(const Country *c, u8 law_id);

// --- Fin de tour ---
enum { CENSURE_NONE, CENSURE_REJETEE, CENSURE_ADOPTEE };
enum { IMP_EV_NONE, IMP_EV_ENGAGEE, IMP_EV_REJETEE, IMP_EV_VOTEE };

// Compte rendu d'une fin de tour (pour le journal).
typedef struct {
    u8  n_matured;
    u8  matured[SIM_MAX_PENDING];   // lois dont les effets se sont appliqués
    u8  cp_gain;
    s16 popularity_before;
    s16 popularity_after;
    // V1
    u8  censure;          // CENSURE_* : motion examinée ce tour
    u16 censure_votes;    // voix pour la motion
    u8  new_pm;           // PM nommé d'office après une censure adoptée (PM_NONE sinon)
    u8  impeach;          // IMP_EV_* : procédure engagée, rejetée ou votée
    u16 impeach_votes;    // voix pour la destitution (vote du tour)
    u8  budget_reconduit; // 1 : année close sans budget adopté, reconduction
    u8  affair;           // 1 : une affaire a éclaté (scandale spontané)
    // V2
    u16 impeach_senate_votes; // voix pour la destitution au Sénat (vote du tour)
    u8  gov_law;          // cohabitation : loi du gouvernement adoptée ce tour (id + 1), 0 sinon
    u8  budget_gov;       // 1 : budget imposé par le Premier ministre d'opposition (fin d'année)
    u8  senate_renewed;   // 1 : le Sénat a été renouvelé par moitié ce tour
    u16 senate_before[GRP_COUNT]; // sièges du Sénat avant renouvellement (si senate_renewed)
    // V4
    u8  senate_amend;     // fin d'année : ligne du budget amendée par le Sénat et non rétablie (ligne + 1), 0 sinon
} TurnReport;

// Fin de tour, dans l'ordre : lois arrivées à maturité, budget, économie,
// opinion, Parlement, jauges, clôture budgétaire, capital politique, puis
// institutions (destitution, censure). Les effets d'événements sont appliqués
// avant par l'appelant (events_choose / sim_apply_effects). report peut être NULL.
void sim_end_turn(Country *c, TurnReport *report);

// --- Second mandat (REGLES §11) ---
// Choisit le thème de campagne (THEME_*) avant la présidentielle.
void sim_set_theme(Country *c, u8 theme);
// Bonus d'un thème auprès d'un électorat (dixièmes de %), aperçu sans effet.
s16 sim_theme_bonus(const Country *c, u8 theme, u8 electorate);
// Démarre le 2nd mandat après une réélection : mémorise le score du 1er mandat,
// remet les compteurs à zéro et organise des législatives (out peut être NULL).
void sim_start_second_mandate(Country *c, LegislativeResult *out);
// Score du 1er mandat (valable une fois le 2nd mandat commencé).
u16 sim_score_first(const Country *c);
// Score cumulé sur 10 ans : 1er mandat + 2nd mandat (en fin de 2nd mandat).
u16 sim_score_total(const Country *c);

// Étapes séparées (exposées pour les tests).
void sim_economy_update(Country *c);
void sim_opinion_update(Country *c);
s16  sim_opinion_target(const Country *c, u8 electorate);
u8   sim_cp_gain(const Country *c);
u8   sim_cp_turns_to_next(const Country *c);   // tours avant le prochain apport de CP (>= 1)
// Avance un reste de calcul : renvoie (acc + annuel) / tpy et garde le reste.
s16  sim_step(s32 annual, s16 *acc, s16 tpy);

// Progression moyenne des piliers depuis le début (dixièmes de point).
s16 sim_pillar_progress(const Country *c);

#endif
