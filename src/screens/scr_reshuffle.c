// Mega Président — remaniement (V2, tâche 2.07) : choix du nouveau profil de
// Premier ministre (hors PM actuel), confirmation, résultat.
#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

// Gueule du PM pressenti à droite (10 x 12 tuiles, PAL2), texte sur 27 colonnes à gauche.
// Le nom va dans la colonne de texte : sous la gueule, 12 cases ne suffisent pas.
#define PORTRAIT_X 29
#define PORTRAIT_Y 4
#define TEXT_W     27

static void draw_portrait(u8 pm, Expr e)
{
    if (pm >= PM_COUNT)
        return;
    ui_medallion(PORTRAIT_X, PORTRAIT_Y, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(GUEULE_PM(pm), e, PORTRAIT_X, PORTRAIT_Y, PAL2);
}

typedef enum { P_PICK, P_CONFIRM, P_RESULT } Phase;

static Phase phase;
static bool dirty;
static bool detail_dirty;
static UiList list;
static const char *items[PM_COUNT];
static u8 item_pm[PM_COUNT];
static u8 n_items;
static u8 chosen;
static s16 pop_before, pop_after;
static char confirm_text[100];

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
    ui_banner(0, 0, 40, txt[TXT_RSH_TITRE], TRUE);
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
    for (u8 i = 0; i < n_items; i++)
        if (sim_reshuffle_check(&g_game.country, item_pm[i]) != RSH_OK)
            ui_list_set_disabled(&list, i, TRUE);
}

static void rsh_init(void)
{
    const Country *c = &g_game.country;
    n_items = 0;
    for (u8 p = 0; p < PM_COUNT; p++) {
        if (p == c->pm) continue;
        item_pm[n_items] = p;
        items[n_items++] = txt[TXT_PM_TECHNOCRATE_NOM + 2 * p];
    }
    ui_list_init(&list, 1, 6, TEXT_W, n_items, items, n_items);
    refresh_disabled();
    for (u8 i = 0; i < n_items; i++)
        if (ui_list_enabled(&list, i)) { list.sel = i; break; }
    phase = P_PICK;
    dirty = TRUE;
}

static void draw_detail(void)
{
    u8 pm = item_pm[list.sel];
    u8 err = sim_reshuffle_check(&g_game.country, pm);

    ui_clear_rect(1, 9, TEXT_W, 16);
    ui_clear_rect(1, 18, 38, 7);
    draw_portrait(pm, err == RSH_OK ? EXPR_CONTENT : EXPR_GENE);
    ui_text_draw(1, 10, txt[TXT_PM_TECHNOCRATE_NOM + 2 * pm], UI_STYLE_HIGHLIGHT);
    ui_text_flow(1, 11, TEXT_W, 5, txt[TXT_PM_TECHNOCRATE_DESC + 2 * pm], UI_STYLE_TEXT);
    if (err == RSH_ERR_YEAR)
        ui_text_wrap(1, 19, 38, 3, txt[TXT_RSH_ANNEE], UI_STYLE_ALERT);
    else if (err == RSH_ERR_COHAB)
        ui_text_wrap(1, 19, 38, 3, txt[TXT_RSH_COHAB], UI_STYLE_ALERT);
    else if (err == RSH_ERR_CP)
        ui_text_draw(1, 19, "Capital politique insuffisant", UI_STYLE_ALERT);
    else if (err != RSH_OK)
        ui_text_draw(1, 19, "Impossible pour l’instant", UI_STYLE_ALERT);
    else
        ui_text_wrap(1, 19, 38, 5, txt[TXT_RSH_AIDE], UI_STYLE_DIM);
}

static void rsh_update(void)
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
            static const char *const ch[2] = { "Remanier", "Annuler" };
            chosen = item_pm[list.sel];
            confirm_text[0] = 0;
            strcat(confirm_text, txt[TXT_RSH_CONFIRME]);
            strcat(confirm_text, "\n");
            strcat(confirm_text, txt[TXT_PM_TECHNOCRATE_NOM + 2 * chosen]);
            ui_msgbox_open(txt[TXT_RSH_TITRE], confirm_text, ch, 2);
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
        if (r == 0) {
            Country *c = &g_game.country;
            pop_before = sim_popularity(c);
            if (sim_reshuffle(c, chosen) == RSH_OK) {
                pop_after = sim_popularity(c);
                sfx_play(SFX_ADOPTED);
                phase = P_RESULT;
            } else {
                sfx_play(SFX_ERROR);
                phase = P_PICK;
            }
        } else {
            phase = P_PICK;
        }
        dirty = TRUE;
        break;
    }
    default:
        if (input_pressed() & (INPUT_OK | INPUT_CANCEL | INPUT_START))
            screen_change(SCREEN_GAME);
        break;
    }
}

static void rsh_draw(void)
{
    if (dirty) {
        char buf[UI_LINE_BUF];
        dirty = FALSE;
        art_reset();
        draw_header();
        ui_window(0, 3, 40, 25);
        if (phase == P_PICK) {
            refresh_disabled();
            ui_text_draw(1, 4, "Profils disponibles", UI_STYLE_DIM);
            ui_separator(1, 5, TEXT_W);
            ui_list_draw(&list);
            ui_footer(1, 26, 38, "A : choisir   B : retour");
            detail_dirty = TRUE;
        } else if (phase == P_RESULT) {
            const Country *c = &g_game.country;
            draw_portrait(c->pm, EXPR_CONTENT);
            ui_text_flow(1, 5, TEXT_W, 5, txt[TXT_RSH_FAIT], UI_STYLE_TEXT);
            ui_text_draw(1, 11, "À Matignon", UI_STYLE_DIM);
            ui_separator(1, 12, TEXT_W);
            ui_text_draw(1, 13, txt[TXT_PM_TECHNOCRATE_NOM + 2 * c->pm], UI_STYLE_HIGHLIGHT);
            ui_text_flow(1, 14, TEXT_W, 2, txt[TXT_PM_TECHNOCRATE_DESC + 2 * c->pm], UI_STYLE_DIM);
            ui_icon_draw(1, 19, UI_ICON_POPULARITY);
            ui_text_draw(3, 19, "Popularité", UI_STYLE_DIM);
            buf[0] = 0;
            cat_num(buf, pop_before, 1); strcat(buf, " % → "); cat_num(buf, pop_after, 1); strcat(buf, " %");
            ui_text_right(39, 19, buf, pop_after >= pop_before ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
            ui_footer(1, 26, 38, "A : continuer");
        }
    }
    if (phase == P_PICK && detail_dirty) {
        detail_dirty = FALSE;
        draw_detail();
    }
}

static void rsh_exit(void)
{
    art_reset();
}

const Screen scr_reshuffle = { rsh_init, rsh_update, rsh_draw, rsh_exit };
