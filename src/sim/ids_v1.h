#ifndef SIM_IDS_V1_H
#define SIM_IDS_V1_H

// Identifiants V1 partagés entre simulation, contenu et écrans
// (docs/CONTRAT_V1.md). Ne pas renuméroter : le contenu s'y réfère.

enum { PM_TECHNOCRATE, PM_RASSEMBLEUR, PM_REFORMATEUR, PM_COUNT };

enum { DIFF_FACILE, DIFF_NORMAL, DIFF_DIFFICILE, DIFF_COUNT };

// V4 : 5e thème « Santé et retraites ».
enum { THEME_EMPLOI, THEME_SECURITE, THEME_POUVOIR_ACHAT, THEME_ECOLOGIE,
       THEME_SANTE_RETRAITES, THEME_COUNT };

// Préréglages du budget annuel. Depuis la V4 le budget est un plan de 6 lignes
// (sim/budget.h) ; ces identifiants sont les 3 préréglages (BUDGET_COUNT) et
// BUDGET_PERSO désigne un plan personnalisé (valeur de Country.budget).
enum { BUDGET_RIGUEUR, BUDGET_EQUILIBRE, BUDGET_RELANCE, BUDGET_COUNT,
       BUDGET_PERSO = BUDGET_COUNT };

// V4 : lignes du budget (4 dépenses, 2 impôts), crans -2..+2 par ligne.
enum { BUD_SOCIAL, BUD_EDUCATION, BUD_SECURITE, BUD_TRANSITION,
       BUD_IMPOT_MENAGES, BUD_IMPOT_ENTREPRISES, BUD_LINES };
#define BUD_LVL_MIN (-2)
#define BUD_LVL_MAX 2

#endif
