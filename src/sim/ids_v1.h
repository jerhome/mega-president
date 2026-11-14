#ifndef SIM_IDS_V1_H
#define SIM_IDS_V1_H

// Identifiants V1 partagés entre simulation, contenu et écrans
// (docs/CONTRAT_V1.md). Ne pas renuméroter : le contenu s'y réfère.

enum { PM_TECHNOCRATE, PM_RASSEMBLEUR, PM_REFORMATEUR, PM_COUNT };

enum { DIFF_FACILE, DIFF_NORMAL, DIFF_DIFFICILE, DIFF_COUNT };

enum { THEME_EMPLOI, THEME_SECURITE, THEME_POUVOIR_ACHAT, THEME_ECOLOGIE, THEME_COUNT };

// Orientation du budget annuel (V1 simplifiée : une décision par an).
enum { BUDGET_RIGUEUR, BUDGET_EQUILIBRE, BUDGET_RELANCE, BUDGET_COUNT };

#endif
