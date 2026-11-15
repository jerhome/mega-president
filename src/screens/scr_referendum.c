// Mega Président — référendum (V2, tâche 2.06) : choix de la loi, aperçu par
// électorat, confirmation, suspense puis résultat. Retour au tableau de bord.
#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_laws.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"
#include "screens/scr_txt.h"

typedef enum { P_PICK, P_CONFIRM, P_SUSPENSE, P_RESULT } Phase;

#define SUSPENSE_MS 1800

static Phase phase;
static bool dirty;          // redessin complet de la phase
static bool detail_dirty;   // aperçu (phase P_PICK)
static UiList list;
static RefResult res;
static u8 chosen;
static s16 pop_before, pop_after;
static u8 def_before, def_after;
static u32 phase_ms;

static void cat_num(char *dst, s32 v, u16 dec)
{
    char b[14];
    ui_format_number(b, v, dec);
    strcat(dst, b);
}

static void draw_header(void)
{
    char buf[16];
    u16 n;
    ui_banner(0, 0, 40, txt[TXT_REF_TITRE], TRUE);
    buf[0] = 0;
    cat_num(buf, g_game.country.cp, 0);
    strcat(buf, "/");
    cat_num(buf, CP_MAX, 0);
    n = ui_text_len(buf);
    ui_text_right(39, 1, " CP", UI_STYLE_DIM);
    ui_text_right(36, 1, buf, UI_STYLE_TEXT);
    ui_icon_draw(34 - n, 1, UI_ICON_CP);
}

static void refresh_disabled(void)
{
    list.disabled = 0;
    list.disabled_hi = 0;
    for (u16 i = 0; i < LOI_COUNT; i++)
        if (sim_referendum_check(&g_game.country, (u8)i) != RFD_OK)
            ui_list_set_disabled(&list, i, TRUE);
}

static void ref_init(void)
{
    ui_list_init(&list, 1, 4, 38, 5, law_name, LOI_COUNT);
    refresh_disabled();
    for (u16 i = 0; i < LOI_COUNT; i++)    // première loi possible
        if (ui_list_enabled(&list, i)) {
            list.sel = (u8)i;
            if (i >= list.rows) list.top = (u8)(i - list.rows + 1);
            break;
        }
    phase = P_PICK;
    dirty = TRUE;
}

static void gauge_row(u16 y, const char *label, s16 yes, UiStyle st)
{
    char buf[14];
    ui_text_draw_n(1, y, label, 18, 18, st);
    ui_gauge_draw(20, y, 11, yes, 1000, yes >= 500 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
    buf[0] = 0;
    cat_num(buf, yes, 1);
    strcat(buf, " %");
    ui_clear_rect(31, y, 8, 1);
    ui_text_right(39, y, buf, yes >= 500 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
}

static void draw_detail(void)
{
    u8 id = list.sel;
    u8 err = sim_referendum_check(&g_game.country, id);
    RefResult pv;

    ui_clear_rect(1, 10, 38, 16);
    ui_text_draw_n(1, 10, law_name[id], 38, 0, UI_STYLE_HIGHLIGHT);
    if (err != RFD_ERR_ADOPTED) {
        sim_referendum_preview(&g_game.country, id, &pv);
        gauge_row(11, "OUI attendu", pv.yes, UI_STYLE_HIGHLIGHT);
        for (u16 e = 0; e < ELEC_COUNT; e++)
            gauge_row(13 + e, elec_txt(e, ET_LONG), pv.yes_elec[e], UI_STYLE_DIM);
    }
    if (err == RFD_ERR_USED)
        ui_text_wrap(1, 19, 38, 2, txt[TXT_REF_DEJA], UI_STYLE_ALERT);
    else if (err == RFD_ERR_CP)
        ui_text_draw(1, 19, "Capital politique insuffisant", UI_STYLE_ALERT);
    else if (err == RFD_ERR_ADOPTED)
        ui_text_draw(1, 19, "Déjà adoptée", UI_STYLE_DIM);
    else if (err != RFD_OK)
        ui_text_draw(1, 19, "Impossible pour l’instant", UI_STYLE_ALERT);
    else
        ui_text_wrap(1, 19, 38, 5, txt[TXT_REF_AIDE], UI_STYLE_DIM);
}

static void go_phase(Phase p)
{
    phase = p;
    dirty = TRUE;
    phase_ms = screen_time_ms();
}

static void launch(void)
{
    Country *c = &g_game.country;
    pop_before = sim_popularity(c);
    def_before = c->gauges[JAUGE_DEFIANCE];
    if (sim_referendum(c, chosen, &res) != RFD_OK) {
        sfx_play(SFX_ERROR);
        go_phase(P_PICK);
        refresh_disabled();
        return;
    }
    pop_after = sim_popularity(c);
    def_after = c->gauges[JAUGE_DEFIANCE];
    law_mask_set(&g_game.laws_tried, chosen);
    if (res.adopted) law_mask_set(&g_game.laws_adopted, chosen);
    go_phase(P_SUSPENSE);
}

static void ref_update(void)
{
    if (dirty) return;
    switch (phase) {
    case P_PICK:
        switch (ui_list_update(&list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&list);
            detail_dirty = TRUE;
            break;
        case UI_LIST_CHOSEN: {
            static const char *const ch[2] = { "Lancer", "Annuler" };
            chosen = list.sel;
            ui_msgbox_open(law_name[chosen], txt[TXT_REF_CONFIRME], ch, 2);
            phase = P_CONFIRM;
            break;
        }
        case UI_LIST_CANCELLED:
            screen_change(SCREEN_GAME);
            break;
        default:
            break;
        }
        break;
    case P_CONFIRM: {
        s16 r = ui_msgbox_update();
        if (r < 0 && (input_pressed() & INPUT_CANCEL)) r = 1;
        if (r < 0) break;
        ui_msgbox_close();
        if (r == 0) launch();
        else go_phase(P_PICK);
        break;
    }
    case P_SUSPENSE:
        if ((input_pressed() & (INPUT_OK | INPUT_START))
            || screen_time_ms() - phase_ms > SUSPENSE_MS) {
            sfx_play(res.adopted ? SFX_ADOPTED : SFX_REJECTED);
            go_phase(P_RESULT);
        }
        break;
    default:
        if (input_pressed() & (INPUT_OK | INPUT_CANCEL | INPUT_START))
            screen_change(SCREEN_GAME);
        break;
    }
}

static void draw_result(void)
{
    char buf[UI_LINE_BUF];
    ui_text_center(1, 4, 38, law_name[chosen], UI_STYLE_HIGHLIGHT);
    ui_text_wrap(1, 6, 38, 3, txt[res.adopted ? TXT_REF_OUI : TXT_REF_NON], UI_STYLE_TEXT);
    ui_text_draw(1, 10, txt[TXT_REF_LABEL_OUI], UI_STYLE_TEXT);
    ui_gauge_draw(10, 10, 20, res.yes, 1000, UI_GAUGE_POSITIVE);
    buf[0] = 0; cat_num(buf, res.yes, 1); strcat(buf, " %");
    ui_text_right(39, 10, buf, UI_STYLE_HIGHLIGHT);
    ui_text_draw(1, 11, txt[TXT_REF_LABEL_NON], UI_STYLE_TEXT);
    ui_gauge_draw(10, 11, 20, 1000 - res.yes, 1000, UI_GAUGE_NEGATIVE);
    buf[0] = 0; cat_num(buf, 1000 - res.yes, 1); strcat(buf, " %");
    ui_text_right(39, 11, buf, UI_STYLE_ALERT);
    ui_text_draw(1, 13, "OUI par électorat", UI_STYLE_HIGHLIGHT);
    ui_separator(19, 13, 20);
    for (u16 e = 0; e < ELEC_COUNT; e++)
        gauge_row(14 + e, elec_txt(e, ET_LONG), res.yes_elec[e], UI_STYLE_DIM);
    buf[0] = 0;
    strcat(buf, "Popularité : ");
    cat_num(buf, pop_before, 1); strcat(buf, " % → "); cat_num(buf, pop_after, 1); strcat(buf, " %");
    ui_icon_draw(1, 20, UI_ICON_POPULARITY);
    ui_text_draw(3, 20, buf, res.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    buf[0] = 0;
    strcat(buf, "Défiance : ");
    cat_num(buf, def_before, 0); strcat(buf, " → "); cat_num(buf, def_after, 0);
    ui_icon_draw(1, 21, UI_ICON_DEFIANCE);
    ui_text_draw(3, 21, buf, res.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
}

static void ref_draw(void)
{
    if (dirty) {
        dirty = FALSE;
        art_reset();
        draw_header();
        ui_window(0, 3, 40, 25);
        switch (phase) {
        case P_PICK:
            refresh_disabled();
            ui_list_draw(&list);
            ui_separator(1, 9, 38);
            ui_footer(1, 26, 38, "A : choisir   B : retour");
            detail_dirty = TRUE;
            break;
        case P_SUSPENSE:
            // La chaîne d'info annonce le dépouillement ; l'urne (PAL3) sous le texte.
            ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
            ui_gueule_draw(GUEULE_JOUR_CHAINE_INFO, EXPR_NEUTRE, 29, 4, PAL2);
            ui_text_draw(1, 5, "Édition spéciale", UI_STYLE_HIGHLIGHT);
            ui_text_flow(1, 7, 27, 5, txt[TXT_REF_ANNONCE], UI_STYLE_TEXT);
            {   // urne centrée sous le texte (colonne de 27 cases)
                u16 aw, ah;
                ui_art_size(ART_DECOR_URNE, &aw, &ah);
                ui_art_draw_pal(ART_DECOR_URNE, 1 + (27 - aw) / 2, 13, PAL3);
            }
            ui_gueule_say(ui_gueule_talk_ms(txt[TXT_REF_ANNONCE]));
            ui_footer(1, 26, 38, "A : passer");
            break;
        case P_RESULT:
            draw_result();
            ui_footer(1, 26, 38, "A : continuer");
            break;
        default:
            break;
        }
    }
    if (phase == P_PICK && detail_dirty) {
        detail_dirty = FALSE;
        draw_detail();
    }
}

static void ref_exit(void)
{
    art_reset();
}

const Screen scr_referendum = { ref_init, ref_update, ref_draw, ref_exit };
