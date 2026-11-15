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

// Portrait du PM en PAL3 (le style ALERT est donc remplacé par HIGHLIGHT sur cet écran ;
// PAL2/DIM reste libre pour les éléments grisés de la liste).
#define PORTRAIT_X 31
#define PORTRAIT_Y 5

static void draw_portrait(u8 pm, u16 x, u16 y)
{
    if (pm < PM_COUNT)
        ui_art_draw_pal(ART_PM(pm), x, y, PAL3);
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
    ui_window(0, 0, 40, 3);
    ui_text_draw(1, 1, txt[TXT_RSH_TITRE], UI_STYLE_HIGHLIGHT);
    buf[0] = 0;
    strcat(buf, "CP ");
    cat_num(buf, g_game.country.cp, 0);
    strcat(buf, "/");
    cat_num(buf, CP_MAX, 0);
    ui_text_draw(38 - ui_text_len(buf), 1, buf, UI_STYLE_TEXT);
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
    ui_list_init(&list, 1, 5, 38, n_items, items, n_items);
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

    ui_clear_rect(1, 5 + n_items + 1, 38, 20 - n_items);
    draw_portrait(pm, PORTRAIT_X, PORTRAIT_Y);
    ui_text_wrap(1, 5 + n_items + 1, 28, 4, txt[TXT_PM_TECHNOCRATE_DESC + 2 * pm], UI_STYLE_TEXT);
    if (err == RSH_ERR_YEAR)
        ui_text_wrap(1, 16, 38, 2, txt[TXT_RSH_ANNEE], UI_STYLE_HIGHLIGHT);
    else if (err == RSH_ERR_COHAB)
        ui_text_wrap(1, 16, 38, 2, txt[TXT_RSH_COHAB], UI_STYLE_HIGHLIGHT);
    else if (err == RSH_ERR_CP)
        ui_text_draw(1, 16, "Capital politique insuffisant", UI_STYLE_HIGHLIGHT);
    else if (err != RSH_OK)
        ui_text_draw(1, 16, "Impossible pour l’instant", UI_STYLE_HIGHLIGHT);
    else
        ui_text_wrap(1, 16, 38, 4, txt[TXT_RSH_AIDE], UI_STYLE_DIM);
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
            ui_text_draw(1, 4, "Nouveau Premier ministre :", UI_STYLE_DIM);
            ui_list_draw(&list);
            ui_text_draw(1, 26, "A : choisir   B : retour", UI_STYLE_DIM);
            detail_dirty = TRUE;
        } else if (phase == P_RESULT) {
            const Country *c = &g_game.country;
            ui_text_draw(1, 5, txt[TXT_PM_TECHNOCRATE_NOM + 2 * c->pm], UI_STYLE_HIGHLIGHT);
            draw_portrait(c->pm, 16, 18);   // sous les textes, au centre
            ui_text_wrap(1, 7, 38, 3, txt[TXT_RSH_FAIT], UI_STYLE_TEXT);
            ui_text_wrap(1, 11, 38, 3, txt[TXT_PM_TECHNOCRATE_DESC + 2 * c->pm], UI_STYLE_DIM);
            buf[0] = 0;
            strcat(buf, "Popularité : ");
            cat_num(buf, pop_before, 1); strcat(buf, " % -> "); cat_num(buf, pop_after, 1); strcat(buf, " %");
            ui_text_draw(1, 16, buf, UI_STYLE_HIGHLIGHT);
            ui_text_draw(1, 26, "A : continuer", UI_STYLE_DIM);
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
