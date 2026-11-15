// Mega Président — high scores et saisie des initiales (tâches 1.15, 1.16).
#include <genesis.h>
#include "screens.h"
#include "scr_scores.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "save/sram.h"
#include "sim/sim_state.h"

enum { PH_VIEW = 0, PH_ENTRY };

static ScoreBoard board;
static u8 phase;
static u8 tab;                 // tableau affiché
static bool dirty;
static s8 hilite[SCORES_TABLES];   // rang à mettre en évidence (-1 : aucun)
static char ini[3] = { 'A', 'A', 'A' };   // mémorisées d'une partie à l'autre
static u8 pos;

// Offre en attente (scores_offer)
static bool pending;
static u16 off_score[SCORES_TABLES];
static u8 off_active[SCORES_TABLES];
static u8 off_orient;
static u16 off_year;

static const char *const orient_names[ORIENT_COUNT] = { "Gauche", "Centre", "Droite" };
static const char *const tab_names[SCORES_TABLES] = { "1er MANDAT", "CUMUL 10 ANS" };

void scores_offer(u16 score_mandat1, u16 score_cumul, u8 two_mandates, u8 orientation, u16 year)
{
    off_score[SCORE_TABLE_MANDAT] = score_mandat1;
    off_score[SCORE_TABLE_CUMUL] = score_cumul;
    off_active[SCORE_TABLE_MANDAT] = 1;
    off_active[SCORE_TABLE_CUMUL] = two_mandates ? 1 : 0;
    off_orient = orientation;
    off_year = year;
    pending = TRUE;
}

static void scores_init(void)
{
    u8 t;
    sram_scores_load(&board);
#if defined(MP_DEBUG_SCREEN) && MP_DEBUG_SCREEN == 14
    // ROM de démonstration : tableau rempli (en RAM seulement, jamais sauvegardé).
    for (t = 0; t < SCORES_COUNT; t++) {
        ScoreEntry *e = &board.t[0].e[t];
        e->ini[0] = (char)('A' + t); e->ini[1] = 'W'; e->ini[2] = 'M';
        e->score = (u16)(1980 - t * 137);
        e->orient = (u8)(t % ORIENT_COUNT);
        e->year = (u16)(2032 + (t % 2) * 5);
    }
#endif
    // Reste de l'écran de fin : efface les tuiles du plan A, sans nouveau fondu.
    ui_clear_rect(0, 0, UI_COLS, UI_ROWS);
    hilite[0] = hilite[1] = -1;
    tab = SCORE_TABLE_MANDAT;
    phase = PH_VIEW;
    pos = 0;
    dirty = TRUE;
    if (pending) {
        for (t = 0; t < SCORES_TABLES; t++)
            if (off_active[t] && score_is_high(&board.t[t], off_score[t])) {
                if (phase == PH_VIEW)
                    tab = t;
                phase = PH_ENTRY;
            }
        if (phase == PH_VIEW)
            pending = FALSE;
    }
}

static void validate_entry(void)
{
    u8 t;
    for (t = 0; t < SCORES_TABLES; t++)
        if (off_active[t])
            hilite[t] = score_table_insert(&board.t[t], ini, off_score[t], off_orient, off_year);
    sram_scores_save(&board);
    pending = FALSE;
    phase = PH_VIEW;
    tab = (hilite[SCORE_TABLE_MANDAT] >= 0) ? SCORE_TABLE_MANDAT : SCORE_TABLE_CUMUL;
    dirty = TRUE;
}

static void scores_update(void)
{
    u16 p = input_pressed();
    u16 r = input_repeat();

    if (phase == PH_ENTRY) {
        if (r & INPUT_UP) {
            ini[pos] = (ini[pos] >= 'Z') ? 'A' : (char)(ini[pos] + 1);
            dirty = TRUE;
        }
        if (r & INPUT_DOWN) {
            ini[pos] = (ini[pos] <= 'A') ? 'Z' : (char)(ini[pos] - 1);
            dirty = TRUE;
        }
        if (p & INPUT_RIGHT) {
            pos = (pos + 1) % 3;
            dirty = TRUE;
        }
        if (p & INPUT_LEFT) {
            pos = (pos + 2) % 3;
            dirty = TRUE;
        }
        if (p & (INPUT_A | INPUT_START))
            validate_entry();
        return;
    }

    if (p & (INPUT_LEFT | INPUT_RIGHT)) {
        tab ^= 1;
        dirty = TRUE;
    }
    if (p & (INPUT_A | INPUT_B | INPUT_C | INPUT_START))
        screen_change(SCREEN_TITLE);
}

static void draw_entry(void)
{
    char buf[2] = { 0, 0 };
    u8 i;
    u16 x;

    ui_window_title(0, 0, 40, 28, "Nouveau record !", FALSE);
    ui_text_center(0, 4, UI_COLS, tab_names[tab], UI_STYLE_DIM);
    x = 14 + ui_text_draw(14, 6, "Score : ", UI_STYLE_DIM);
    ui_icon_draw(12, 6, UI_ICON_CP);
    ui_number_draw(x, 6, off_score[tab], 0, UI_STYLE_HIGHLIGHT);
    ui_separator(8, 8, 24);
    ui_text_center(0, 10, UI_COLS, "Vos initiales", UI_STYLE_TEXT);
    for (i = 0; i < 3; i++) {
        buf[0] = ini[i];
        ui_text_draw(17 + i * 2, 13, buf, i == pos ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
        if (i == pos) {
            ui_icon_draw(17 + i * 2, 12, UI_ICON_ARROW_UP);
            ui_icon_draw(17 + i * 2, 14, UI_ICON_ARROW_DOWN);
        }
    }
    ui_text_center(0, 22, UI_COLS, "Haut/Bas : lettre   G/D : position", UI_STYLE_DIM);
    ui_footer(1, 26, 38, "A : valider");
}

static void draw_view(void)
{
    const ScoreTable *t = &board.t[tab];
    char buf[14];
    char line[4];
    u8 i;
    u16 x;

    ui_window_title(0, 0, 40, 28, "Meilleurs scores", FALSE);
    // Rangs aux rangées 5 à 23 (une sur deux), pied de page en 26 : rien ne se chevauche.
    ui_text_center(0, 3, UI_COLS, tab_names[tab], UI_STYLE_HIGHLIGHT);
    ui_text_draw(2, 3, "←", UI_STYLE_DIM);
    ui_text_draw(37, 3, "→", UI_STYLE_DIM);
    // En-têtes calés sur le début (ou la fin, pour les nombres) de chaque colonne.
    ui_text_right(6, 4, "Rang", UI_STYLE_DIM);
    ui_text_draw(11, 4, "Nom", UI_STYLE_DIM);
    ui_text_right(22, 4, "Score", UI_STYLE_DIM);
    ui_text_draw(24, 4, "Orient.", UI_STYLE_DIM);
    ui_text_draw(33, 4, "Année", UI_STYLE_DIM);

    for (i = 0; i < SCORES_COUNT; i++) {
        const ScoreEntry *e = &t->e[i];
        u16 y = 5 + i * 2;
        UiStyle st = (hilite[tab] == (s8)i) ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;

        ui_format_number(buf, i + 1, 0);
        x = 4 + (i < 9 ? 1 : 0);
        if (e->score == 0) {
            ui_text_draw(x, y, buf, UI_STYLE_DIM);
            ui_text_draw(11, y, "---", UI_STYLE_DIM);
            continue;
        }
        ui_text_draw(x, y, buf, st);
        if (i == 0) ui_icon_draw(2, y, UI_ICON_CP);
        line[0] = e->ini[0]; line[1] = e->ini[1]; line[2] = e->ini[2]; line[3] = 0;
        ui_text_draw(11, y, line, st);
        ui_format_number(buf, e->score, 0);
        ui_text_draw(17 + 5 - ui_text_len(buf), y, buf, st);
        ui_text_draw(24, y, orient_names[e->orient < ORIENT_COUNT ? e->orient : 0], st);
        ui_format_number(buf, e->year, 0);
        ui_text_draw(33, y, buf, st);
    }
    ui_footer(1, 26, 38, "Gauche/Droite : tableau   A : retour");
}

static void scores_draw(void)
{
    if (!dirty)
        return;
    dirty = FALSE;
    ui_clear();
    if (phase == PH_ENTRY)
        draw_entry();
    else
        draw_view();
}

const Screen scr_scores = { scores_init, scores_update, scores_draw, NULL };
