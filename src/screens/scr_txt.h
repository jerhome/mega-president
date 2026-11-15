// Mega Président — accès par indice aux textes V4 (électorats, lignes du budget).
// Les textes viennent de docs/contenu/textes_v4.md ; ces accesseurs bornent les
// indices (jamais de lecture hors tableau, même si un électorat est ajouté).
#ifndef SCR_TXT_H
#define SCR_TXT_H

#include <genesis.h>
#include "sim/sim_state.h"
#include "sim/budget.h"
#include "content/content_texts.h"
#include "ui/ui.h"

// Variantes d'un électorat (ordre du fichier de textes).
enum { ET_COURT, ET_LONG, ET_ABR, ET_QUI, ET_HAUT, ET_BAS, ET_COUNT };

static inline const char *elec_txt(u8 e, u8 k)
{
    return (e < ELEC_COUNT && k < ET_COUNT) ? txt[TXT_ELEC_0_COURT + e * ET_COUNT + k] : "";
}

// Icône d'un électorat : Pop deux personnes, Moy personnage, Cad usine, Ret calendrier, Jeu puce.
static inline UiIcon elec_icon(u8 e)
{
    static const UiIcon ic[ELEC_COUNT] = { UI_ICON_SOCIAL, UI_ICON_PERSON, UI_ICON_ECONOMY,
                                           UI_ICON_CALENDAR, UI_ICON_TECHNOLOGY };
    return e < ELEC_COUNT ? ic[e] : UI_ICON_BULLET;
}

// Nom de famille (après la dernière espace) : la colonne du médaillon fait 10 cases.
static inline const char *surname(const char *full)
{
    const char *s = full, *last = full;
    for (; *s; s++)
        if (*s == ' ') last = s + 1;
    return last;
}

static inline const char *bl_nom(u8 line)
{
    return line < BUD_LINES ? txt[TXT_BL_0_NOM + line] : "";
}
static inline const char *bl_long(u8 line)
{
    return line < BUD_LINES ? txt[TXT_BL_0_LONG + line] : "";
}
// Cran affiché : -2..+2.
static inline const char *bl_cran(u8 line, s8 lvl)
{
    return (line < BUD_LINES && lvl >= -2 && lvl <= 2)
               ? txt[TXT_BL_0_CRAN_0 + line * 5 + lvl + 2] : "";
}
static inline const char *bl_aide(u8 line)
{
    return line < BUD_LINES ? txt[TXT_BL_0_AIDE + line] : "";
}

// --- Premier ministre, cohabitation, piliers (indices bornés) ---
_Static_assert(TXT_PM_TECHNOCRATE_DESC == TXT_PM_TECHNOCRATE_NOM + 1, "TXT_PM_* NOM puis DESC");
_Static_assert(TXT_PM_RASSEMBLEUR_NOM == TXT_PM_TECHNOCRATE_NOM + 2, "TXT_PM_* contigus");
_Static_assert(TXT_PM_REFORMATEUR_NOM == TXT_PM_TECHNOCRATE_NOM + 4, "TXT_PM_* contigus");

// Nom du Premier ministre ; « - » pour PM_NONE ou un indice hors bornes.
static inline const char *pm_name(u8 pm)
{
    return pm < PM_COUNT ? txt[TXT_PM_TECHNOCRATE_NOM + 2 * pm] : "-";
}
static inline const char *pm_desc(u8 pm)
{
    return pm < PM_COUNT ? txt[TXT_PM_TECHNOCRATE_DESC + 2 * pm] : "";
}
// Nom du Premier ministre de cohabitation (camp 0 gauche, 1 centre, 2 droite).
static inline const char *cohab_pm_name(u8 camp)
{
    return camp < 3 ? txt[TXT_COHAB_PM_GAUCHE_NOM + camp] : "-";
}
// Nom d'un pilier ; court : 13 cases avant une jauge (« Internat. »).
static inline const char *pillar_name(u8 p, bool court)
{
    static const char *const lng[PIL_TOTAL] = { "Social", "Économie", "Technologie",
        "Écologie", "Sécurité", "International" };
    static const char *const sht[PIL_TOTAL] = { "Social", "Économie", "Technologie",
        "Écologie", "Sécurité", "Internat." };
    if (p >= PIL_TOTAL) return "";
    return court ? sht[p] : lng[p];
}

#endif
