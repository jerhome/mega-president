#ifndef SIM_BUDGET_H
#define SIM_BUDGET_H

// Budget annuel détaillé (V4, SPEC_V4 §2) : un plan de 6 lignes (BUD_*) à 5 crans
// (-2..+2) ; 3 préréglages (BUDGET_RIGUEUR / EQUILIBRE / RELANCE) en raccourci.
// Présenté au 4e trimestre ; voté par l'Assemblée ou passé en 49.3, puis en
// vigueur jusqu'à la fin de l'année suivante (Country.budget_lvl).
// Sans budget adopté à la fin de l'année : reconduction = les dépenses reculent
// d'un cran vers 0 (services votés), les impôts ne changent pas.
// Si le Sénat n'est pas acquis au bloc présidentiel, il amende le plan adopté
// (une ligne recule d'un cran) ; la lecture définitive (1 CP) le rétablit.
//
// Les anciennes fonctions à un identifiant de préréglage (sim_budget_vote, ...)
// ne servent qu'aux tests PC : voir tests/budget_compat.h.

#include "sim/sim_state.h"
#include "sim/parliament.h"

#define BUDGET_493_CP 4
#define BUDGET_SENATE_CP 1   // lecture définitive : annule l'amendement du Sénat

enum { BUDGET_OK = 0, BUDGET_ERR_NOT_DUE, BUDGET_ERR_ID, BUDGET_ERR_CP,
       BUDGET_ERR_NO_AMEND };

// Plan de budget : cran (-2..+2) de chaque ligne. Pour les impôts, + = hausse.
typedef struct { s8 lvl[BUD_LINES]; } BudgetPlan;

// 1 si le budget de l'année doit être présenté maintenant (4e trimestre,
// budget de l'année pas encore adopté, partie en cours, hors cohabitation).
u8 sim_budget_due(const Country *c);

// --- Plans et préréglages ---
// Plan d'un préréglage (BUDGET_*) ; id invalide : Équilibre.
void sim_budget_preset_plan(u8 preset, BudgetPlan *out);
// Préréglage dont le plan est exactement celui-ci, sinon BUDGET_PERSO.
u8   sim_budget_plan_preset(const BudgetPlan *p);
// Plan en vigueur du pays.
void sim_budget_current_plan(const Country *c, BudgetPlan *out);
// Borne tous les crans à -2..+2.
void sim_budget_plan_clamp(BudgetPlan *p);

// --- Aperçus d'un plan (aucun effet de bord) ---
// Orientation du vote (-100 gauche ... +100 droite) : orientation du joueur / 2 + somme des lignes.
s8  sim_plan_orientation(const Country *c, const BudgetPlan *p);
// Déficit du plan par rapport à l'équilibre (dixièmes de point de PIB par an), linéaire.
s16 sim_plan_deficit(const BudgetPlan *p);
// Effet sur la tendance de croissance (centièmes de point), linéaire.
s16 sim_plan_growth(const BudgetPlan *p);
// Effet sur la cible de tension (points), linéaire.
s16 sim_plan_tension(const BudgetPlan *p);
// Effet sur la cible d'opinion d'un électorat (dixièmes de %) : lignes
// (asymétrie -5 -2 0 +2 +3, /2) et sérieux budgétaire (-déficit x k_s / 10).
s16 sim_plan_opinion(const BudgetPlan *p, u8 electorate);
// Effet d'un seul cran d'une ligne sur un électorat (sans sérieux budgétaire) : aide.
s16 sim_budget_line_opinion(u8 line, s8 lvl, u8 electorate);
// Effet sur un pilier par trimestre (dixièmes de point), asymétrique.
s16 sim_plan_pillar(const BudgetPlan *p, u8 pillar);
// Pénalité de vote d'un budget « insincère » (déficit > 1,5 point) : points de score
// retirés à chaque groupe hors du parti du joueur, 0 sinon.
s16 sim_plan_insincere_malus(const BudgetPlan *p);
// Aperçu du vote de l'Assemblée sur ce plan.
void sim_budget_plan_preview(const Country *c, const BudgetPlan *p, VoteResult *out);

// Ligne que le Sénat amenderait dans ce plan (ligne + 1), 0 si aucune ou si le Sénat
// ne l'amende pas (bloc présidentiel majoritaire au Sénat, ou cohabitation).
u8 sim_budget_senate_amend_line(const Country *c, const BudgetPlan *p);

// --- Actions ---
// Vote du plan. BUDGET_OK : vote effectué (voir out->adopted) ; adopté, il entre en
// vigueur (éventuellement amendé par le Sénat). Rejeté : défiance +3 ; on peut
// retenter, passer en 49.3, ou laisser la reconduction s'appliquer en fin d'année.
u8 sim_budget_plan_vote(Country *c, const BudgetPlan *p, VoteResult *out);
// 49.3 sur le budget : 4 CP, défiance +8, illimité. Le plan passe tel quel
// (mais le Sénat peut l'amender).
u8 sim_budget_plan_force(Country *c, const BudgetPlan *p);
// Ligne amendée par le Sénat dans le budget en vigueur (ligne + 1), 0 si aucune.
u8 sim_budget_amended_line(const Country *c);
// Lecture définitive (1 CP) : rétablit le plan voulu par le président.
// BUDGET_OK, BUDGET_ERR_NO_AMEND ou BUDGET_ERR_CP.
u8 sim_budget_definitive_reading(Country *c);

// --- Reconduction et cohabitation ---
// Services votés : chaque dépense recule d'un cran vers 0, les impôts ne changent pas.
void sim_budget_reconduct_plan(BudgetPlan *p);
// Plan que le Premier ministre d'opposition impose (camp du PM), ligne Sécurité
// ajustée de Country.cohab_def_adj (-1..+1) : c'est le plan de fin d'année.
void sim_cohab_plan(const Country *c, BudgetPlan *out);
// Choix du président sur la ligne Sécurité et défense (domaine réservé) : -1, 0 ou +1
// cran autour du plan du PM, sans coût en CP.
void sim_cohab_set_defense(Country *c, s8 adjust);

// Interne (fin de tour) : dérive des piliers, une fois par trimestre.
void budget_quarter_update(Country *c);
// Interne : met en vigueur un plan (met à jour Country.budget_lvl et .budget).
void budget_set_current(Country *c, const BudgetPlan *p);
// Interne : clôture de l'année budgétaire (reconduction, plan du PM). Renvoie
// la ligne amendée par le Sénat dans l'année (ligne + 1) avant effacement.
u8 budget_year_close(Country *c, u8 *reconduit, u8 *gov);

#endif
