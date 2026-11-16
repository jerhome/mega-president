// Mega Président — remaniement (V2, tâche 2.07) : choix du nouveau profil de
// Premier ministre (hors PM actuel), confirmation, résultat.
#include "audio/sfx.h"
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"
#include "util/strbuf.h"
#include "screens/scr_kit.h"
#include "screens/scr_txt.h"

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

static bool pm_enabled(u16 i, void *ctx)
{
    (void)ctx;
    return sim_reshuffle_check(&g_game.country, item_pm[i]) == RSH_OK;
}

static void rsh_init(void)
{
    const Country *c = &g_game.country;
    n_items = 0;
    for (u8 p = 0; p < PM_COUNT; p++) {
        if (p == c->pm) continue;
        item_pm[n_items] = p;
        items[n_items++] = pm_name(p);
    }
    ui_list_init(&list, 1, 6, TEXT_W, n_items, items, n_items);
    ui_list_set_enabled_by(&list, pm_enabled, NULL);
    ui_list_select_first_enabled(&list);
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
    ui_text_draw(1, 10, pm_name(pm), UI_STYLE_HIGHLIGHT);
    ui_text_flow(1, 11, TEXT_W, 5, pm_desc(pm), UI_STYLE_TEXT);
    if (err == RSH_ERR_YEAR)
        ui_text_wrap(1, 19, 38, 3, txt[TXT_RSH_ANNEE], UI_STYLE_ALERT);
    else if (err == RSH_ERR_COHAB)
        ui_text_wrap(1, 19, 38, 3, txt[TXT_RSH_COHAB], UI_STYLE_ALERT);
    else if (err != RSH_OK)
        kit_err_line(19, err == RSH_ERR_CP ? KIT_ERR_CP : KIT_ERR_OTHER);
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
            StrBuf sb;
            chosen = item_pm[list.sel];
            sb_init(&sb, confirm_text, sizeof confirm_text);
            sb_str(&sb, txt[TXT_RSH_CONFIRME]);
            sb_str(&sb, "\n");
            sb_str(&sb, pm_name(chosen));
            ui_confirm_open(txt[TXT_RSH_TITRE], confirm_text, "Remanier", "Annuler", FALSE);
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
        s16 r = ui_confirm_update();
        if (r < 0) break;
        ui_confirm_close();
        if (r == 1) {
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
        dirty = FALSE;
        kit_page(txt[TXT_RSH_TITRE]);
        ui_window(0, 3, 40, 25);
        if (phase == P_PICK) {
            ui_list_set_enabled_by(&list, pm_enabled, NULL);
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
            ui_text_draw(1, 13, pm_name(c->pm), UI_STYLE_HIGHLIGHT);
            ui_text_flow(1, 14, TEXT_W, 2, pm_desc(c->pm), UI_STYLE_DIM);
            ui_icon_draw(1, 19, UI_ICON_POPULARITY);
            ui_text_draw(3, 19, "Popularité", UI_STYLE_DIM);
            {
                SB_LINE(sb);
                sb_num(&sb, pop_before, 1);
                sb_str(&sb, " % → ");
                sb_num(&sb, pop_after, 1);
                sb_str(&sb, " %");
                ui_text_right(39, 19, sb.buf, pop_after >= pop_before ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
            }
            ui_footer(1, 26, 38, "A : continuer");
            ui_soft_replay();   // résultat d'une décision payée : fondu doux (revue DA V41-1)
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
