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
#include "screens/evt_art.h"
#include "ui/ui_art.h"

// P_INTRO : annonce de la procédure, avec l'hémicycle (pas de place pour lui à l'écran de défense).
typedef enum { P_INTRO, P_MAIN, P_GROUP } Phase;

enum { I_TV, I_SACRIFICE, I_NEGOCIER, I_CONTINUE, I_COUNT };

static Phase phase;
static bool dirty;        // redessin complet
static bool top_dirty;    // aperçu du vote, jauges
static bool list_dirty;   // liste, message
static bool side_dirty;   // gueule de droite (selon la défense choisie)
static UiList def_list;
static UiList grp_list;
static char def_tv[UI_LINE_BUF];
static char def_neg[UI_LINE_BUF];
static const char *def_items[I_COUNT];
static char grp_label[GRP_COUNT][UI_LINE_BUF];
static const char *grp_items[GRP_COUNT];
static u8 grp_id[GRP_COUNT];
static u8 grp_n;
static char msg[4][UI_LINE_BUF];
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
    strcat(def_neg, "Négocier un appui (");
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
    ui_list_init(&def_list, 1, 15, 27, I_COUNT, def_items, I_COUNT);
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
    phase = P_INTRO;
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
    // Quatre lignes « avant → après » (colonne de gauche, 27 cases).
    {
        static const char *const lab[4] = { "Scandale  ", "Défiance  ", "Assemblée ", "Sénat     " };
        u16 before[4], after[4];
        before[0] = sb; after[0] = c->gauges[JAUGE_SCANDALE];
        before[1] = db; after[1] = c->gauges[JAUGE_DEFIANCE];
        before[2] = vb; after[2] = votes_now();
        before[3] = sb2; after[3] = sim_impeach_senate_votes(c);
        for (u8 i = 0; i < 4; i++) {
            msg[i][0] = 0;
            strcat(msg[i], lab[i]);
            cat_num(msg[i], before[i]);
            strcat(msg[i], " → ");
            cat_num(msg[i], after[i]);
        }
    }
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
        grp_items[grp_n] = grp_label[grp_n];
        grp_id[grp_n] = g;
        grp_n++;
    }
    ui_list_init(&grp_list, 1, 15, 27, grp_n, grp_items, grp_n);
}

static void impeach_update(void)
{
    if (dirty) return;
    if (phase == P_INTRO) {
        if (input_pressed() & (INPUT_OK | INPUT_START)) {
            phase = P_MAIN;
            dirty = TRUE;
        }
        return;
    }
    if (phase == P_MAIN) {
        switch (ui_list_update(&def_list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&def_list);
            side_dirty = TRUE;
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
            side_dirty = TRUE;
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
    char buf[UI_LINE_BUF];
    u16 v = votes_now();
    u16 sv = sim_impeach_senate_votes(c);
    u8 pass = sim_impeach_would_pass(c);

    ui_clear_rect(1, 8, 38, 6);
    ui_icon_draw(1, 8, UI_ICON_ASSEMBLY);
    ui_text_draw(3, 8, "Assemblée", UI_STYLE_DIM);
    buf[0] = 0;
    cat_num(buf, v);
    strcat(buf, " / ");
    cat_num(buf, SEATS_TOTAL);
    strcat(buf, " (il en faut ");
    cat_num(buf, IMPEACH_MAJORITY);
    strcat(buf, ")");
    ui_text_right(39, 8, buf, v >= IMPEACH_MAJORITY ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    ui_gauge_draw(1, 9, 38, v, SEATS_TOTAL, v >= IMPEACH_MAJORITY ? UI_GAUGE_NEGATIVE : UI_GAUGE_POSITIVE);
    ui_icon_draw(1, 10, UI_ICON_ASSEMBLY);
    ui_text_draw(3, 10, "Sénat", UI_STYLE_DIM);
    buf[0] = 0;
    cat_num(buf, sv);
    strcat(buf, " / ");
    cat_num(buf, SENATE_SEATS);
    strcat(buf, " (il en faut ");
    cat_num(buf, SENATE_IMPEACH_MAJORITY);
    strcat(buf, ")");
    ui_text_right(39, 10, buf, sv >= SENATE_IMPEACH_MAJORITY ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    ui_gauge_draw(1, 11, 38, sv, SENATE_SEATS,
                  sv >= SENATE_IMPEACH_MAJORITY ? UI_GAUGE_NEGATIVE : UI_GAUGE_POSITIVE);
    ui_icon_draw(1, 12, pass ? UI_ICON_CROSS : UI_ICON_CHECK);
    ui_text_draw(3, 12, pass ? "Les deux chambres : vous seriez destitué" : "Au moins une chambre vous sauve",
                 pass ? UI_STYLE_ALERT : UI_STYLE_TEXT);
}

// Gueule de droite selon l'option : journaliste (télé), ministre de l'Intérieur
// (sacrifié), chef du groupe (négociation), président du Sénat (attendre).
static void draw_side(void)
{
    const Country *c = &g_game.country;
    GueuleId id;
    Expr e = EXPR_NEUTRE;
    const char *cap;

    if (phase == P_GROUP) {
        u8 g = grp_id[grp_list.sel];
        id = GUEULE_CHEF(g);
        e = c->relation[g] >= 50 ? EXPR_CONTENT : EXPR_FURIEUX;
        cap = grp_sigle[g];
        ui_clear_rect(1, 21, 27, 3);
        ui_text_wrap(1, 21, 27, 2, grp_name[g], UI_STYLE_DIM);
    } else {
        switch (def_list.sel) {
        case I_TV: id = GUEULE_JOUR_CHAINE_INFO; cap = "Le 20 h"; break;
        case I_SACRIFICE: id = GUEULE_MIN_INTERIEUR; e = EXPR_GENE; cap = "Le fusible"; break;
        case I_NEGOCIER: id = GUEULE_CHEF(c->player_group); cap = "Votre camp"; break;
        default: id = GUEULE_SENAT_PRESIDENT; e = EXPR_FURIEUX; cap = "Le Sénat"; break;
        }
    }
    ui_art_close();   // une seule gueule à l'écran : la VRAM ne s'accumule pas
    ui_medallion(29, 14, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(id, e, 29, 14, PAL2);
    ui_clear_rect(1, 26, 38, 1);
    ui_text_center(29, 26, 10, cap, UI_STYLE_DIM);   // sous la gueule, sans toucher le cadre
}

static void draw_list_zone(void)
{
    ui_clear_rect(1, 14, 27, 12);
    for (u8 y = 15; y < 15 + I_COUNT; y++)   // barre de l'autre liste (défense / groupe)
        ui_select_bar_clear(1, y, 26);
    if (phase == P_MAIN) {
        ui_text_draw(1, 14, "Votre défense", UI_STYLE_HIGHLIGHT);
        def_list.bar_sel = 0xFF;
        ui_list_draw(&def_list);
        if (has_msg)
            for (u8 i = 0; i < 4; i++)
                ui_text_draw(3, 20 + i, msg[i], (i >= 2 && msg_alert) ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    } else {
        ui_text_draw(1, 14, "Avec quel groupe ?", UI_STYLE_HIGHLIGHT);
        grp_list.bar_sel = 0xFF;
        ui_list_draw(&grp_list);
    }
    side_dirty = TRUE;
}

static void impeach_draw(void)
{
    if (dirty) {
        dirty = FALSE;
        art_reset();
        ui_banner(0, 0, 40, "Procédure de destitution", TRUE);
        ui_icon_draw(37, 1, UI_ICON_SCANDAL);
        ui_window(0, 3, 40, 25);
        ui_text_wrap(1, 4, 38, 3, txt[TXT_DESTITUTION_PROCEDURE], UI_STYLE_TEXT);
        if (phase == P_INTRO) {
            char buf[UI_LINE_BUF];
            // Hémicycle illustré à gauche, président du Sénat (furieux) à droite.
            ui_art_draw_pal(ART_DECOR_HEMICYCLE, 4, 10, PAL3);
            ui_medallion(29, 9, UI_PORTRAIT_W, UI_PORTRAIT_H);
            ui_gueule_draw(GUEULE_SENAT_PRESIDENT, EXPR_FURIEUX, 29, 9, PAL2);
            buf[0] = 0;
            strcat(buf, "Scandale ");
            cat_num(buf, g_game.country.gauges[JAUGE_SCANDALE]);
            strcat(buf, " %   Défiance ");
            cat_num(buf, g_game.country.gauges[JAUGE_DEFIANCE]);
            strcat(buf, " %");
            ui_text_center(1, 22, 38, buf, UI_STYLE_ALERT);
            ui_footer(1, 26, 38, "A : préparer la défense");
            return;
        }
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
    if (side_dirty) {
        side_dirty = FALSE;
        draw_side();
        ui_footer(1, 26, 27, phase == P_MAIN ? "A : valider" : "A : valider   B : retour");
    }
}

static void impeach_exit(void)
{
    art_reset();
}

const Screen scr_impeach = { impeach_init, impeach_update, impeach_draw, impeach_exit };
