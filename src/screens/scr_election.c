// Mega Président — écran d'élection présidentielle (tâche 6.3).
#include "audio/music.h"
#include "audio/sfx.h"
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_world.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

enum { PH_ROUND1 = 0, PH_ROUND2, PH_VERDICT };

static u8 phase;
static bool dirty;
static u8 order[ELECTION_CANDIDATES];   // indices dans g_game.election.cand, triés
static bool player_in_round2;

static void election_init(void)
{
    music_play(MUS_ELECTION);
    const ElectionResult *e;
    u8 i, j;

    game_run_election();
    e = &g_game.election;
    for (i = 0; i < ELECTION_CANDIDATES; i++)
        order[i] = i;
    for (i = 1; i < ELECTION_CANDIDATES; i++) {   // tri par score décroissant
        u8 k = order[i];
        for (j = i; j > 0 && e->round1[order[j - 1]] < e->round1[k]; j--)
            order[j] = order[j - 1];
        order[j] = k;
    }
    player_in_round2 = (e->finalist[0] == CAND_JOUEUR || e->finalist[1] == CAND_JOUEUR);
    phase = PH_ROUND1;
    dirty = TRUE;
}

static void election_update(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_START)) {
        if (phase < PH_VERDICT) {
            phase++;
            dirty = TRUE;
        } else {
            game_after_election();
        }
    }
}

// Libellé d'un candidat : « Vous (RSV) » ou « Nom (parti) ».
static void cand_label(char *buf, u8 cand)
{
    if (cand == CAND_JOUEUR) {
        strcpy(buf, "Vous (");
        strcat(buf, grp_sigle[g_game.country.player_group]);
        strcat(buf, ")");
    } else {
        strcpy(buf, cand_name[cand]);
        strcat(buf, " (");
        strcat(buf, cand_party[cand]);
        strcat(buf, ")");
    }
}

// Ligne de résultat : nom, pourcentage, jauge (2 lignes de texte).
static void draw_row(u16 y, u8 cand, s16 score, s16 gmax)
{
    char buf[UI_LINE_BUF];
    char num[14];
    u16 n;
    bool me = (cand == CAND_JOUEUR);
    UiStyle st = me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;

    cand_label(buf, cand);
    ui_text_draw_n(1, y, buf, 28, 0, st);
    n = ui_format_number(num, score, 1);
    ui_text_draw(UI_TEXT_COLS - n - 1, y, num, st);
    ui_text_draw(UI_TEXT_COLS - 1, y, "%", st);
    ui_gauge_draw(1, y + 1, UI_TEXT_COLS, score, gmax, me ? UI_GAUGE_GOLD : UI_GAUGE_NEUTRAL);
}

// Bloc d'un finaliste (second tour) : portrait 7 x 7 à gauche, nom, parti, score et jauge à droite.
// PAL2 est empruntée par les portraits : jauges positive / dorée, jamais NEUTRAL.
static void draw_finalist(u16 y, u8 cand, s16 score)
{
    char buf[UI_LINE_BUF];
    char num[14];
    bool me = (cand == CAND_JOUEUR);
    UiStyle st = me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;

    ui_art_draw(me ? ART_PRES(g_game.country.orientation) : ART_CAND(cand), 1, y);
    ui_text_draw_n(9, y + 1, me ? "Vous" : cand_name[cand], 30, 0, st);
    buf[0] = 0;
    strcat(buf, "(");
    strcat(buf, me ? grp_sigle[g_game.country.player_group] : cand_party[cand]);
    strcat(buf, ")");
    ui_text_draw_n(9, y + 2, buf, 30, 0, UI_STYLE_TEXT);
    ui_format_number(num, score, 1);
    strcat(num, " %");
    ui_text_draw(9, y + 4, num, st);
    ui_gauge_draw(9, y + 5, 30, score, 1000, me ? UI_GAUGE_GOLD : UI_GAUGE_POSITIVE);
}

static void election_draw(void)
{
    const ElectionResult *e = &g_game.election;
    u8 i;

    if (!dirty)
        return;
    dirty = FALSE;
    art_reset();
    ui_window(0, 0, 40, 28);

    if (phase == PH_ROUND1) {
        ui_text_center(0, 1, UI_COLS, "PRÉSIDENTIELLE : PREMIER TOUR", UI_STYLE_HIGHLIGHT);
        for (i = 0; i < ELECTION_CANDIDATES; i++)
            draw_row(4 + i * 3, e->cand[order[i]], e->round1[order[i]], 500);
        ui_text_center(0, 25, UI_COLS, "A : second tour", UI_STYLE_DIM);
    } else {
        ui_text_center(0, 1, UI_COLS, "PRÉSIDENTIELLE : SECOND TOUR", UI_STYLE_HIGHLIGHT);
        for (i = 0; i < 2; i++)
            draw_finalist(3 + i * 7, e->finalist[i], e->round2[i]);
        if (!player_in_round2)
            ui_text_center(0, 17, UI_COLS, "Vous n'êtes pas qualifié(e).", UI_STYLE_TEXT);

        if (phase == PH_ROUND2) {
            ui_text_center(0, 25, UI_COLS, "A : résultat", UI_STYLE_TEXT);
        } else {
            const char *l1, *l2;
            UiStyle st = e->elected ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT;
            ui_window(4, 18, 32, 8);
            music_stop();   // la fanfare (ou le thème de défaite) joue seule
            sfx_play(e->elected ? SFX_ELECTION_WIN : SFX_ELECTION_LOSE);
            if (e->elected) {
                l1 = "RÉÉLECTION !";
                l2 = "Les Français vous renouvellent leur confiance.";
            } else if (!player_in_round2) {
                l1 = "DÉFAITE";
                l2 = "Éliminé dès le premier tour.";
            } else {
                l1 = "DÉFAITE";
                l2 = "Battu(e) au second tour.";
            }
            ui_text_center(4, 19, 32, l1, st);
            ui_text_wrap(6, 21, 28, 2, l2, UI_STYLE_TEXT);
            ui_text_center(4, 24, 32, e->elected ? "A : second mandat" : "A : bilan du mandat", UI_STYLE_TEXT);
        }
    }
}

static void election_exit(void)
{
    art_reset();
}

const Screen scr_election = { election_init, election_update, election_draw, election_exit };
