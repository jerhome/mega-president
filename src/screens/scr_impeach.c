// Mega Président — procédure de destitution : aperçu du vote, choix de la
// défense (V1, tâche 1.10).

#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "content/content_world.h"

typedef enum { P_MAIN, P_GROUP } Phase;

enum { I_TV, I_SACRIFICE, I_NEGOCIER, I_CONTINUE, I_COUNT };

static Phase phase;
static bool dirty;        // redessin complet
static bool top_dirty;    // aperçu du vote, jauges
static bool list_dirty;   // liste, message
static UiList def_list;
static UiList grp_list;
static char def_tv[40];
static char def_neg[40];
static const char *def_items[I_COUNT];
static char grp_label[GRP_COUNT][40];
static const char *grp_items[GRP_COUNT];
static u8 grp_id[GRP_COUNT];
static u8 grp_n;
static char msg[2][40];
static u8 msg_alert;
static u8 has_msg;

static void cat_num(char *dst, s32 v)
{
    char b[14];
    ui_format_number(b, v, 0);
    strcat(dst, b);
}

static void items_build(void)
{
    const Country *c = &g_game.country;
    u32 dis = 0;

    def_tv[0] = 0;
    strcat(def_tv, "Discours télévisé (");
    cat_num(def_tv, DEFENSE_TV_CP);
    strcat(def_tv, " CP)");
    def_neg[0] = 0;
    strcat(def_neg, "Négocier avec un groupe (");
    cat_num(def_neg, DEFENSE_NEGOCIER_CP);
    strcat(def_neg, " CP)");
    def_items[I_TV] = def_tv;
    def_items[I_SACRIFICE] = "Sacrifier un ministre";
    def_items[I_NEGOCIER] = def_neg;
    def_items[I_CONTINUE] = "Ne rien faire de plus";
    if (!sim_defense_available(c, DEFENSE_TV)) dis |= 1u << I_TV;
    if (!sim_defense_available(c, DEFENSE_SACRIFICE)) dis |= 1u << I_SACRIFICE;
    if (!sim_defense_available(c, DEFENSE_NEGOCIER)) dis |= 1u << I_NEGOCIER;
    def_list.disabled = dis;
}

static void list_setup(void)
{
    u8 sel = def_list.sel;
    ui_list_init(&def_list, 1, 15, 38, I_COUNT, def_items, I_COUNT);
    items_build();
    def_list.sel = sel;
    if (def_list.disabled & (1u << sel)) {
        for (u8 i = 0; i < I_COUNT; i++)
            if (!(def_list.disabled & (1u << i))) { def_list.sel = i; break; }
    }
}

static void impeach_init(void)
{
    def_list.sel = 0;
    list_setup();
    phase = P_MAIN;
    has_msg = 0;
    dirty = TRUE;
}

static u16 votes_now(void)
{
    return sim_impeach_votes(&g_game.country);
}

static void apply_defense(u8 d, u8 group)
{
    Country *c = &g_game.country;
    u16 vb = votes_now();
    u16 sb2 = sim_impeach_senate_votes(c);
    u8 sb = c->gauges[JAUGE_SCANDALE];
    u8 db = c->gauges[JAUGE_DEFIANCE];

    if (!sim_defend(c, d, group)) {
        sfx_play(SFX_ERROR);
        return;
    }
    sfx_play(SFX_ADOPTED);
    msg[0][0] = 0;
    strcat(msg[0], "Scandale ");
    cat_num(msg[0], sb);
    strcat(msg[0], " -> ");
    cat_num(msg[0], c->gauges[JAUGE_SCANDALE]);
    strcat(msg[0], "  Défiance ");
    cat_num(msg[0], db);
    strcat(msg[0], " -> ");
    cat_num(msg[0], c->gauges[JAUGE_DEFIANCE]);
    msg[1][0] = 0;
    strcat(msg[1], "Assemblée ");
    cat_num(msg[1], vb);
    strcat(msg[1], " -> ");
    cat_num(msg[1], votes_now());
    strcat(msg[1], "  Sénat ");
    cat_num(msg[1], sb2);
    strcat(msg[1], " -> ");
    cat_num(msg[1], sim_impeach_senate_votes(c));
    msg_alert = sim_impeach_would_pass(c);
    has_msg = 1;
    phase = P_MAIN;
    list_setup();
    top_dirty = TRUE;
    dirty = TRUE;
}

static void group_setup(void)
{
    const Country *c = &g_game.country;
    grp_n = 0;
    for (u8 g = 0; g < GRP_COUNT; g++) {
        if (g == c->player_group) continue;
        grp_label[grp_n][0] = 0;
        strcat(grp_label[grp_n], grp_sigle[g]);
        strcat(grp_label[grp_n], "  ");
        strcat(grp_label[grp_n], grp_name[g]);
        grp_items[grp_n] = grp_label[grp_n];
        grp_id[grp_n] = g;
        grp_n++;
    }
    ui_list_init(&grp_list, 1, 15, 38, grp_n, grp_items, grp_n);
}

static void impeach_update(void)
{
    if (dirty) return;
    if (phase == P_MAIN) {
        switch (ui_list_update(&def_list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&def_list);
            break;
        case UI_LIST_CHOSEN:
            switch (def_list.sel) {
            case I_TV: apply_defense(DEFENSE_TV, 0); break;
            case I_SACRIFICE: apply_defense(DEFENSE_SACRIFICE, 0); break;
            case I_NEGOCIER:
                group_setup();
                phase = P_GROUP;
                list_dirty = TRUE;
                break;
            default: game_after_impeach(); break;
            }
            break;
        default:
            break;
        }
    } else {
        switch (ui_list_update(&grp_list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&grp_list);
            break;
        case UI_LIST_CHOSEN:
            apply_defense(DEFENSE_NEGOCIER, grp_id[grp_list.sel]);
            break;
        case UI_LIST_CANCELLED:
            phase = P_MAIN;
            list_dirty = TRUE;
            break;
        default:
            break;
        }
    }
}

static void draw_top(void)
{
    const Country *c = &g_game.country;
    char buf[40];
    u16 v = votes_now();
    u16 sv = sim_impeach_senate_votes(c);
    u8 pass = sim_impeach_would_pass(c);

    ui_clear_rect(1, 8, 38, 6);
    buf[0] = 0;
    strcat(buf, "Assemblée : ");
    cat_num(buf, v);
    strcat(buf, " / ");
    cat_num(buf, SEATS_TOTAL);
    strcat(buf, " (il en faut ");
    cat_num(buf, IMPEACH_MAJORITY);
    strcat(buf, ")");
    ui_text_draw(1, 8, buf, UI_STYLE_TEXT);
    ui_gauge_draw(1, 9, 38, v, SEATS_TOTAL, v >= IMPEACH_MAJORITY ? UI_GAUGE_NEGATIVE : UI_GAUGE_POSITIVE);
    buf[0] = 0;
    strcat(buf, "Sénat : ");
    cat_num(buf, sv);
    strcat(buf, " / ");
    cat_num(buf, SENATE_SEATS);
    strcat(buf, " (il en faut ");
    cat_num(buf, SENATE_IMPEACH_MAJORITY);
    strcat(buf, ")");
    ui_text_draw(1, 10, buf, UI_STYLE_TEXT);
    ui_gauge_draw(1, 11, 38, sv, SENATE_SEATS,
                  sv >= SENATE_IMPEACH_MAJORITY ? UI_GAUGE_NEGATIVE : UI_GAUGE_POSITIVE);
    ui_text_draw(1, 12, pass ? "Les deux chambres : vous seriez destitué" : "Au moins une chambre vous sauve",
                 pass ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    buf[0] = 0;
    strcat(buf, "Scandale ");
    cat_num(buf, c->gauges[JAUGE_SCANDALE]);
    strcat(buf, "   Défiance ");
    cat_num(buf, c->gauges[JAUGE_DEFIANCE]);
    strcat(buf, "   CP ");
    cat_num(buf, c->cp);
    ui_text_draw(1, 13, buf, UI_STYLE_DIM);
}

static void draw_list_zone(void)
{
    ui_clear_rect(1, 14, 38, 13);
    if (phase == P_MAIN) {
        ui_text_draw(1, 14, "Votre défense :", UI_STYLE_DIM);
        ui_list_draw(&def_list);
        if (has_msg) {
            ui_text_draw(1, 21, msg[0], UI_STYLE_TEXT);
            ui_text_draw(1, 22, msg[1], msg_alert ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
        }
        ui_text_draw(1, 26, "A : valider", UI_STYLE_DIM);
    } else {
        ui_text_draw(1, 14, "Avec quel groupe négocier ?", UI_STYLE_DIM);
        ui_list_draw(&grp_list);
        ui_text_draw(1, 26, "A : valider   B : retour", UI_STYLE_DIM);
    }
}

static void impeach_draw(void)
{
    if (dirty) {
        dirty = FALSE;
        ui_clear();
        ui_window(0, 0, 40, 3);
        ui_text_center(1, 1, 38, "PROCÉDURE DE DESTITUTION", UI_STYLE_ALERT);
        ui_window(0, 3, 40, 25);
        ui_text_wrap(1, 4, 38, 3, txt[TXT_DESTITUTION_PROCEDURE], UI_STYLE_TEXT);
        top_dirty = TRUE;
        list_dirty = TRUE;
    }
    if (top_dirty) {
        top_dirty = FALSE;
        draw_top();
    }
    if (list_dirty) {
        list_dirty = FALSE;
        draw_list_zone();
    }
}

const Screen scr_impeach = { impeach_init, impeach_update, impeach_draw, NULL };
