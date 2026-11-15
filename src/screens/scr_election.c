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

// Libellé d'un candidat : « Vous » ou son nom.
static const char *cand_title(u8 cand)
{
    return cand == CAND_JOUEUR ? "Vous" : cand_name[cand];
}

static const char *cand_sigle(u8 cand)
{
    return cand == CAND_JOUEUR ? grp_sigle[g_game.country.player_group] : cand_party[cand];
}

static GueuleId cand_gueule(u8 cand)
{
    return cand == CAND_JOUEUR ? GUEULE_PRES(g_game.country.orientation) : GUEULE_CAND(cand);
}

// Premier tour : deux lignes par candidat. Rang, nom et parti (à droite) ;
// puis jauge et score (« Génération Climat » ne tient pas entre le nom et le score).
static void draw_row(u16 y, u8 rank, u8 cand, s16 score, s16 gmax)
{
    char num[14];
    bool me = (cand == CAND_JOUEUR);
    UiStyle st = me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;

    ui_number_draw(1, y, rank + 1, 0, UI_STYLE_DIM);
    ui_text_draw_n(3, y, cand_title(cand), 17, 0, st);
    ui_text_right(39, y, cand_sigle(cand), UI_STYLE_DIM);
    ui_format_number(num, score, 1);
    strcat(num, " %");
    ui_gauge_draw(3, y + 1, 29, score, gmax, me ? UI_GAUGE_GOLD : UI_GAUGE_NEUTRAL);
    ui_text_right(39, y + 1, num, st);
    if (rank < 2)
        ui_icon_draw(1, y + 1, UI_ICON_CHECK);
}

// Second tour : deux gueules face à face, noms, partis et scores.
#define FACE_Y 4
static void draw_finalist(u8 side, u8 cand, s16 score, Expr e)
{
    char num[14];
    u16 x = side ? 25 : 4;               // gueule 10 x 12, centrée sur sa colonne de texte
    u16 cx = side ? 22 : 1;              // colonne de texte : 17 cases (noms jusqu'à 16)
    bool me = (cand == CAND_JOUEUR);
    UiStyle st = me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;

    ui_medallion(x, FACE_Y, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(cand_gueule(cand), e, x, FACE_Y, side ? PAL3 : PAL2);
    ui_text_center(cx, FACE_Y + 12, 17, cand_title(cand), st);
    ui_text_center(cx, FACE_Y + 13, 17, cand_sigle(cand), UI_STYLE_DIM);
    ui_format_number(num, score, 1);
    strcat(num, " %");
    ui_text_center(cx, FACE_Y + 15, 17, num, st);
}

static void election_draw(void)
{
    const ElectionResult *e = &g_game.election;
    u8 i;

    if (!dirty)
        return;
    dirty = FALSE;
    art_reset();

    if (phase == PH_ROUND1) {
        ui_window_title(0, 0, 40, 28, "Présidentielle · premier tour", FALSE);
        ui_text_center(1, 4, 38, "Les deux premiers vont au second tour", UI_STYLE_DIM);
        for (i = 0; i < ELECTION_CANDIDATES; i++)
            draw_row(6 + i * 4, i, e->cand[order[i]], e->round1[order[i]], 500);
        ui_footer(1, 26, 38, "A : second tour");
        return;
    }

    ui_window_title(0, 0, 40, 28, "Présidentielle · second tour", FALSE);
    {
        bool verdict = (phase == PH_VERDICT);
        u8 w = e->round2[0] >= e->round2[1] ? 0 : 1;   // vainqueur
        for (i = 0; i < 2; i++) {
            Expr ex = !verdict ? EXPR_NEUTRE : (i == w ? EXPR_CONTENT : EXPR_GENE);
            draw_finalist(i, e->finalist[i], e->round2[i], ex);
        }
        ui_text_center(14, FACE_Y + 5, 11, "contre", UI_STYLE_DIM);
        // Duel : part du premier finaliste sur la barre (or si c'est le joueur).
        ui_gauge_draw(1, FACE_Y + 17, 38, e->round2[0], e->round2[0] + e->round2[1],
                      e->finalist[0] == CAND_JOUEUR ? UI_GAUGE_GOLD : UI_GAUGE_NEUTRAL);
        if (!player_in_round2 && !verdict)   // le verdict le redit
            ui_text_center(1, FACE_Y + 18, 38, "Vous n'êtes pas qualifié(e).", UI_STYLE_ALERT);

        if (!verdict) {
            ui_footer(1, 26, 38, "A : résultat");
        } else {
            const char *l1, *l2;
            UiStyle st = e->elected ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT;
            music_stop();   // la fanfare (ou le thème de défaite) joue seule
            sfx_play(e->elected ? SFX_ELECTION_WIN : SFX_ELECTION_LOSE);
            if (e->elected) {
                l1 = "RÉÉLECTION !";
                l2 = "Le pays vous renouvelle sa confiance.";
            } else if (!player_in_round2) {
                l1 = "DÉFAITE";
                l2 = "Éliminé dès le premier tour.";
            } else {
                l1 = "DÉFAITE";
                l2 = "Battu(e) au second tour.";
            }
            ui_text_center(1, 23, 38, l1, st);
            ui_text_center(1, 24, 38, l2, UI_STYLE_TEXT);
            ui_footer(1, 26, 38, e->elected ? "A : second mandat" : "A : bilan du mandat");
        }
    }
}

static void election_exit(void)
{
    art_reset();
}

const Screen scr_election = { election_init, election_update, election_draw, election_exit };
