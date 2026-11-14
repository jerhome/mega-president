// Mega Président — journal du trimestre écoulé (tâche 6.2).

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

#define START_YEAR 2027
#define LAST_ROW   25   // dernière ligne utilisable pour le contenu

// Instantané pris par scr_game.c au moment de la fin du trimestre.
extern s16 g_report_snap[9];

static bool dirty;
static u8 page;   // 0 : bilan, 1 : vie politique (si besoin)

static void cat_num(char *dst, s32 v, u16 dec)
{
    char b[14];
    ui_format_number(b, v, dec);
    strcat(dst, b);
}

static void cat_signed(char *dst, s32 v, u16 dec)
{
    if (v > 0) strcat(dst, "+");
    cat_num(dst, v, dec);
}

// Ligne d'indicateur : flèche, libellé, « avant -> après ».
static u16 indicator(u16 y, const char *label, s16 before, s16 after, u16 dec, const char *unit)
{
    char buf[40];
    if (before == after || y > LAST_ROW) return y;
    ui_icon_draw(1, y, after > before ? UI_ICON_ARROW_UP : UI_ICON_ARROW_DOWN);
    ui_text_draw(3, y, label, UI_STYLE_TEXT);
    buf[0] = 0;
    cat_num(buf, before, dec);
    strcat(buf, " -> ");
    cat_num(buf, after, dec);
    strcat(buf, unit);
    ui_text_draw(38 - ui_text_len(buf), y, buf, UI_STYLE_HIGHLIGHT);
    return y + 1;
}

// Pages : 0 bilan ; 1 censure et destitution ; 2 affaire et budget reconduit ;
// 3 (V2) gouvernement de cohabitation et Sénat.
static u8 has_page(u8 p)
{
    const TurnReport *r = &g_game.report;
    if (p == 1) return (u8)(r->censure != CENSURE_NONE || r->impeach != IMP_EV_NONE);
    if (p == 3) return (u8)(r->gov_law || r->budget_gov || r->senate_renewed);
    return (u8)(r->affair || r->budget_reconduit);
}

static u8 next_page(u8 p)
{
    for (u8 i = (u8)(p + 1); i <= 3; i++)
        if (has_page(i)) return i;
    return 0;   // plus de page
}

static u8 has_politics(void)
{
    return next_page(0) != 0;
}

static void report_init(void)
{
    page = 0;
    dirty = TRUE;
}

static void report_update(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_START)) {
        u8 n = next_page(page);
        if (n) {
            page = n;
            dirty = TRUE;
        } else {
            game_after_report();
        }
    }
}

// Texte wrappé sur 38 colonnes ; renvoie la ligne suivante.
static u16 para(u16 y, const char *text, UiStyle st)
{
    u16 n;
    if (y > LAST_ROW) return y;
    n = ui_text_wrap(1, y, 38, LAST_ROW + 1 - y, text, st);
    return y + n;
}

static void votes_line(u16 y, const char *label, u16 votes, u16 majority, UiStyle st)
{
    char buf[40];
    buf[0] = 0;
    strcat(buf, label);
    cat_num(buf, votes, 0);
    strcat(buf, " (majorité ");
    cat_num(buf, majority, 0);
    strcat(buf, ")");
    ui_text_draw(1, y, buf, st);
}

// Page 2 : motion de censure, destitution, affaire, budget reconduit.
static void draw_politics(void)
{
    const TurnReport *r = &g_game.report;
    char buf[40];
    u16 y = 4;

    ui_text_center(1, 2, 38, "Vie politique", UI_STYLE_DIM);

    if (page == 1 && r->censure != CENSURE_NONE) {
        u8 ok = (r->censure == CENSURE_ADOPTEE);
        ui_text_draw(1, y++, "Motion de censure", UI_STYLE_HIGHLIGHT);
        y = para(y, txt[ok ? TXT_CENSURE_ADOPTEE : TXT_CENSURE_REJETEE], ok ? UI_STYLE_ALERT : UI_STYLE_TEXT);
        votes_line(y++, "Voix pour : ", r->censure_votes, SEATS_MAJORITY,
                   ok ? UI_STYLE_ALERT : UI_STYLE_DIM);
        if (ok && r->new_pm < PM_COUNT) {
            buf[0] = 0;
            strcat(buf, "Nouveau PM : ");
            strcat(buf, txt[TXT_PM_TECHNOCRATE_NOM + 2 * r->new_pm]);
            ui_text_draw_n(1, y++, buf, 38, 0, UI_STYLE_HIGHLIGHT);
            y = para(y, txt[TXT_NOUVEAU_PM], UI_STYLE_DIM);
        }
        y++;
    }
    if (page == 1 && r->impeach != IMP_EV_NONE) {
        u8 sen_no = (u8)(r->impeach == IMP_EV_REJETEE && r->impeach_votes >= IMPEACH_MAJORITY
                         && r->impeach_senate_votes < SENATE_IMPEACH_MAJORITY);
        u8 t = r->impeach == IMP_EV_ENGAGEE ? TXT_DESTITUTION_PROCEDURE
             : r->impeach == IMP_EV_REJETEE ? (sen_no ? TXT_DESTITUTION_SENAT_REJETEE : TXT_DESTITUTION_REJETEE)
             : TXT_DESTITUTION_VOTEE;
        ui_text_draw(1, y++, "Destitution", UI_STYLE_HIGHLIGHT);
        y = para(y, txt[t], r->impeach == IMP_EV_REJETEE ? UI_STYLE_TEXT : UI_STYLE_ALERT);
        if (r->impeach == IMP_EV_VOTEE)
            y = para(y, txt[TXT_DESTITUTION_SENAT_VOTEE], UI_STYLE_ALERT);
        if (r->impeach != IMP_EV_ENGAGEE) {
            buf[0] = 0;
            strcat(buf, "Assemblée : ");
            cat_num(buf, r->impeach_votes, 0);
            strcat(buf, " / ");
            cat_num(buf, SEATS_TOTAL, 0);
            strcat(buf, " (il en faut ");
            cat_num(buf, IMPEACH_MAJORITY, 0);
            strcat(buf, ")");
            ui_text_draw(1, y++, buf, UI_STYLE_TEXT);
            buf[0] = 0;
            strcat(buf, "Sénat : ");
            cat_num(buf, r->impeach_senate_votes, 0);
            strcat(buf, " / ");
            cat_num(buf, SENATE_SEATS, 0);
            strcat(buf, " (il en faut ");
            cat_num(buf, SENATE_IMPEACH_MAJORITY, 0);
            strcat(buf, ")");
            ui_text_draw(1, y++, buf, UI_STYLE_TEXT);
        }
        y++;
    }
    if (page == 2 && r->affair && y <= LAST_ROW) {
        ui_text_draw(1, y++, "Une affaire éclate", UI_STYLE_HIGHLIGHT);
        y = para(y, "Un scandale secoue l’Élysée. La presse\nfouille, l’opposition s’en réjouit.", UI_STYLE_ALERT);
        y++;
    }
    if (page == 3) {
        const Country *c = &g_game.country;
        if (r->gov_law && y <= LAST_ROW) {
            y = para(y, txt[TXT_COHAB_LOI_GOUV], UI_STYLE_HIGHLIGHT);
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y++, law_name[r->gov_law - 1], 35, 0, UI_STYLE_TEXT);
            y++;
        }
        if (r->budget_gov && y <= LAST_ROW) {
            y = para(y, txt[TXT_COHAB_BUDGET_GOUV], UI_STYLE_HIGHLIGHT);
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y++, txt[TXT_BUD_RIGUEUR + sim_cohab_budget(c)], 35, 0, UI_STYLE_TEXT);
            y++;
        }
        if (r->senate_renewed && y <= LAST_ROW) {
            u16 before = 0, after = sim_senate_bloc_seats(c);
            for (u8 g = 0; g < GRP_COUNT; g++)
                if (sim_in_bloc(c, g)) before += r->senate_before[g];
            y = para(y, txt[after >= before ? TXT_SENAT_RENOUVELE_GAIN : TXT_SENAT_RENOUVELE_PERTE],
                     after >= before ? UI_STYLE_TEXT : UI_STYLE_ALERT);
            buf[0] = 0;
            strcat(buf, "Bloc présidentiel au Sénat : ");
            cat_num(buf, before, 0);
            strcat(buf, " -> ");
            cat_num(buf, after, 0);
            if (y <= LAST_ROW) ui_text_draw(1, y++, buf, UI_STYLE_HIGHLIGHT);
        }
    }
    if (page == 2 && r->budget_reconduit && y <= LAST_ROW) {
        ui_text_draw(1, y++, "Budget reconduit", UI_STYLE_HIGHLIGHT);
        y = para(y, "Faute de budget adopté dans l’année,\nl’État reconduit le budget d’équilibre.", UI_STYLE_TEXT);
    }
}

static void report_draw(void)
{
    static const char *const pillar_names[PIL_TOTAL] = {
        "Social", "Économie", "Technologie", "Écologie", "Sécurité", "International"
    };
    const Country *c = &g_game.country;
    const TurnReport *r = &g_game.report;
    char buf[40];
    u16 y, n;
    u8 t;

    if (!dirty) return;
    dirty = FALSE;

    art_reset();
    ui_window(0, 0, 40, 28);
    if (page != 0) {
        ui_text_center(1, 1, 38, "LE QUOTIDIEN DU MATIN", UI_STYLE_HIGHLIGHT);
        draw_politics();
        ui_text_draw(1, 26, next_page(page) ? "A : suite" : "A : continuer", UI_STYLE_TEXT);
        return;
    }

    // Page 1 : bandeau « une » (38 x 5 tuiles, PAL2 : pas de style DIM sur cette page).
    ui_art_draw(ART_DECOR_JOURNAL, 1, 1);

    t = c->turn > 0 ? c->turn - 1 : 0;   // tour (trimestre ou mois) qui vient de s'écouler
    buf[0] = 0;
    if (game_is_monthly()) {
        strcat(buf, "Bilan du mois : ");
        strcat(buf, game_month_name(t));
        strcat(buf, " ");
        cat_num(buf, game_calendar_year(t), 0);
    } else {
        strcat(buf, "Bilan du trimestre T");
        cat_num(buf, (t & 3) + 1, 0);
        strcat(buf, " ");
        cat_num(buf, START_YEAR + 5 * c->mandate + (t >> 2), 0);
    }
    ui_text_center(1, 6, 38, buf, UI_STYLE_TEXT);

    // Popularité et capital politique
    y = 8;
    buf[0] = 0;
    strcat(buf, "Popularité : ");
    cat_num(buf, r->popularity_before, 1);
    strcat(buf, " % -> ");
    cat_num(buf, r->popularity_after, 1);
    strcat(buf, " % (");
    cat_signed(buf, r->popularity_after - r->popularity_before, 1);
    strcat(buf, ")");
    ui_text_draw(1, y++, buf, r->popularity_after >= r->popularity_before ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    buf[0] = 0;
    strcat(buf, "Capital politique : +");
    cat_num(buf, r->cp_gain, 0);
    strcat(buf, " (total ");
    cat_num(buf, c->cp, 0);
    strcat(buf, ")");
    ui_text_draw(1, y++, buf, UI_STYLE_TEXT);

    // À la une : lois votées ce tour
    if (law_mask_any(&g_game.laws_tried)) {
        ui_text_draw(1, y++, "À la une", UI_STYLE_HIGHLIGHT);
        for (u16 i = 0; i < LOI_COUNT && y <= LAST_ROW; i++) {
            if (!law_mask_get(&g_game.laws_tried, (u8)i)) continue;
            ui_icon_draw(1, y, UI_ICON_BULLET);
            n = ui_text_wrap(3, y, 35, LAST_ROW + 1 - y > 2 ? 2 : LAST_ROW + 1 - y,
                             law_mask_get(&g_game.laws_adopted, (u8)i) ? law_journal_ok[i] : law_journal_ko[i],
                             law_mask_get(&g_game.laws_adopted, (u8)i) ? UI_STYLE_TEXT : UI_STYLE_ALERT);
            y += n ? n : 1;
        }
        y++;
    }

    // Lois dont les effets arrivent
    if (r->n_matured && y <= LAST_ROW) {
        ui_text_draw(1, y++, "Entrent en vigueur", UI_STYLE_HIGHLIGHT);
        for (u16 i = 0; i < r->n_matured && y <= LAST_ROW; i++) {
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y++, law_name[r->matured[i]], 35, 0, UI_STYLE_TEXT);
        }
        y++;
    }

    // Indicateurs qui ont bougé
    if (y <= LAST_ROW) {
        u16 y0 = y;
        ui_text_draw(1, y++, "Indicateurs", UI_STYLE_HIGHLIGHT);
        y = indicator(y, "Croissance", g_report_snap[0], c->growth, 2, " %");
        y = indicator(y, "Chômage", g_report_snap[1], c->unemployment, 2, " %");
        y = indicator(y, "Dette", g_report_snap[2], c->debt, 1, " %");
        for (u16 i = 0; i < PIL_TOTAL; i++)
            y = indicator(y, pillar_names[i], g_report_snap[3 + i], c->pillars[i], 1, "");
        y = indicator(y, "Tension", g_report_snap[8], c->gauges[JAUGE_TENSION], 0, " %");
        if (y == y0 + 1)
            ui_text_draw(1, y, "Rien de notable.", UI_STYLE_TEXT);
    }

    ui_text_draw(1, 26, has_politics() ? "A : suite" : "A : continuer", UI_STYLE_TEXT);
}

static void report_exit(void)
{
    art_reset();
}

const Screen scr_report = { report_init, report_update, report_draw, report_exit };
