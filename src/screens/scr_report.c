// Mega Président — journal du trimestre écoulé (tâche 6.2).

#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "screens/scr_txt.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_laws.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"
#include "util/strbuf.h"

#define LAST_ROW   25   // dernière ligne utilisable pour le contenu

// Instantané pris par scr_game.c au moment de la fin du trimestre.
extern s16 g_report_snap[3 + PIL_TOTAL + 1];

static bool dirty;
static u8 page;   // 0 : bilan, 1 : vie politique (si besoin)

// Mise en page de la une (charte V3) :
//  1-5    bandeau du journal (illustration 38 x 5)
//  6      date du bilan, 7 filet
//  8-19   « photo de une » : gueule du président x 29-38 (10 x 12 tuiles, PAL2) ;
//         le texte coule à sa gauche sur 27 colonnes, puis sur toute la largeur.
#define PHOTO_X   29
#define PHOTO_Y   8
#define PHOTO_W   10
#define PHOTO_H   12
#define FLOW_Y0   8

// Fin de ligne (colonne exclue) à la rangée y : à gauche de la photo, ou pleine largeur.
static bool photo_on;   // une (page 0) : le texte contourne la photo

static u16 row_end(u16 y)
{
    return (photo_on && y >= PHOTO_Y && y < PHOTO_Y + PHOTO_H) ? PHOTO_X - 1 : 39;
}

// Rubrique : titre doré suivi d'un filet jusqu'au bout de la ligne.
static u16 rubric(u16 y, const char *title)
{
    u16 n;
    if (y > LAST_ROW) return y;
    n = ui_text_draw(1, y, title, UI_STYLE_HIGHLIGHT);
    if (1 + n + 1 < row_end(y) - 1)
        ui_separator(2 + n, y, row_end(y) - 2 - n);
    return y + 1;
}

// Paragraphe qui coule autour de la photo, à partir de la colonne x ; renvoie la ligne suivante.
static u16 flow(u16 x, u16 y, u16 max_lines, const char *text, UiStyle st)
{
    while (text && y <= LAST_ROW && max_lines--) {
        text = ui_text_wrap_line(x, y, row_end(y) - x, text, st);
        y++;
    }
    return y;
}

// Ligne d'indicateur : flèche colorée (hausse/baisse, bonne/mauvaise nouvelle),
// libellé, « avant → après » aligné à droite.
static u16 indicator(u16 y, const char *label, s16 before, s16 after, u16 dec, const char *unit,
                     bool up_is_good)
{
    bool up = after > before;
    bool good = up == up_is_good;
    if (before == after || y > LAST_ROW) return y;
    ui_icon_draw(1, y, up ? (good ? UI_ICON_UP_GOOD : UI_ICON_UP_BAD)
                          : (good ? UI_ICON_DOWN_GOOD : UI_ICON_DOWN_BAD));
    ui_text_draw(3, y, label, UI_STYLE_DIM);
    // Colonnes fixes : unité (2 cases), « après » (6), flèche, « avant » (6).
    // À côté de la photo, seule la valeur « après » tient.
    {
        u16 xe = row_end(y);
        UiStyle st = good ? UI_STYLE_TEXT : UI_STYLE_ALERT;
        SB_LINE(sb);
        ui_text_draw(xe - 2, y, unit[0] ? "%" : "", UI_STYLE_DIM);
        sb_num(&sb, after, dec);
        ui_text_right(xe - 3, y, sb.buf, st);
        if (xe >= 39) {
            ui_text_draw(xe - 11, y, "→", UI_STYLE_DIM);
            sb_init(&sb, sb_mem, SB_LINE_SIZE);
            sb_num(&sb, before, dec);
            ui_text_right(xe - 12, y, sb.buf, UI_STYLE_DIM);
        }
    }
    return y + 1;
}

// Pages : 0 bilan ; 1 censure et destitution ; 2 affaire et budget reconduit ;
// 3 (V2) gouvernement de cohabitation et Sénat.
static u8 has_page(u8 p)
{
    const TurnReport *r = &g_game.report;
    if (p == 1) return (u8)(r->censure != CENSURE_NONE || r->impeach != IMP_EV_NONE);
    if (p == 3) return (u8)(r->gov_law || r->budget_gov || r->senate_renewed);
    return (u8)(r->affair || r->budget_reconduit || r->senate_amend);
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
    SB_LINE(sb);
    sb_str(&sb, label);
    sb_num(&sb, votes, 0);
    sb_str(&sb, " (majorité ");
    sb_num(&sb, majority, 0);
    sb_str(&sb, ")");
    ui_text_draw(1, y, sb.buf, st);
}

// Page 2 : motion de censure, destitution, affaire, budget reconduit.
static void draw_politics(void)
{
    const TurnReport *r = &g_game.report;
    u16 y = 5;

    ui_text_center(1, 3, 38, "Vie politique", UI_STYLE_DIM);

    if (page == 1 && r->censure != CENSURE_NONE) {
        u8 ok = (r->censure == CENSURE_ADOPTEE);
        y = rubric(y, "Motion de censure");
        y = para(y, txt[ok ? TXT_CENSURE_ADOPTEE : TXT_CENSURE_REJETEE], ok ? UI_STYLE_ALERT : UI_STYLE_TEXT);
        votes_line(y++, "Voix pour : ", r->censure_votes, SEATS_MAJORITY,
                   ok ? UI_STYLE_ALERT : UI_STYLE_DIM);
        if (ok && r->new_pm < PM_COUNT) {
            SB_LINE(sb);
            sb_str(&sb, "Nouveau PM : ");
            sb_str(&sb, pm_name(r->new_pm));
            ui_text_draw_n(1, y++, sb.buf, 38, 0, UI_STYLE_HIGHLIGHT);
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
        y = rubric(y, "Destitution");
        y = para(y, txt[t], r->impeach == IMP_EV_REJETEE ? UI_STYLE_TEXT : UI_STYLE_ALERT);
        if (r->impeach == IMP_EV_VOTEE)
            y = para(y, txt[TXT_DESTITUTION_SENAT_VOTEE], UI_STYLE_ALERT);
        if (r->impeach != IMP_EV_ENGAGEE) {
            SB_LINE(sb);
            sb_str(&sb, "Assemblée : ");
            sb_num(&sb, r->impeach_votes, 0);
            sb_str(&sb, " / ");
            sb_num(&sb, SEATS_TOTAL, 0);
            sb_str(&sb, " (il en faut ");
            sb_num(&sb, IMPEACH_MAJORITY, 0);
            sb_str(&sb, ")");
            ui_text_draw(1, y++, sb.buf, UI_STYLE_TEXT);
            sb_init(&sb, sb_mem, SB_LINE_SIZE);
            sb_str(&sb, "Sénat : ");
            sb_num(&sb, r->impeach_senate_votes, 0);
            sb_str(&sb, " / ");
            sb_num(&sb, SENATE_SEATS, 0);
            sb_str(&sb, " (il en faut ");
            sb_num(&sb, SENATE_IMPEACH_MAJORITY, 0);
            sb_str(&sb, ")");
            ui_text_draw(1, y++, sb.buf, UI_STYLE_TEXT);
        }
        y++;
    }
    if (page == 2 && r->affair && y <= LAST_ROW) {
        y = rubric(y, "Une affaire éclate");
        y = para(y, "Un scandale secoue l’Élysée. La presse\nfouille, l’opposition s’en réjouit.", UI_STYLE_ALERT);
        y++;
    }
    if (page == 3) {
        const Country *c = &g_game.country;
        if ((r->gov_law || r->budget_gov) && y <= LAST_ROW)
            y = rubric(y, "Cohabitation");
        if (r->gov_law && y <= LAST_ROW) {
            y = para(y, txt[TXT_COHAB_LOI_GOUV], UI_STYLE_HIGHLIGHT);
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y++, law_name[r->gov_law - 1], 35, 0, UI_STYLE_TEXT);
            y++;
        }
        if (r->budget_gov && y <= LAST_ROW) {
            y = para(y, txt[TXT_COHAB_BUDGET_GOUV], UI_STYLE_HIGHLIGHT);
            y = para(y, txt[TXT_COHAB_PLAN_GAUCHE + sim_cohab_camp(c)], UI_STYLE_TEXT);
            y++;
        }
        if (r->senate_renewed && y <= LAST_ROW) {
            u16 before = 0, after = sim_senate_bloc_seats(c);
            for (u8 g = 0; g < GRP_COUNT; g++)
                if (sim_in_bloc(c, g)) before += r->senate_before[g];
            y = rubric(y, "Sénat");
            if (after < before)   // perte : le titre en rouge, le corps reste en ivoire (revue DA P2-3)
                ui_text_draw(1, y - 1, "Sénat", UI_STYLE_ALERT);
            y = para(y, txt[after >= before ? TXT_SENAT_RENOUVELE_GAIN : TXT_SENAT_RENOUVELE_PERTE],
                     UI_STYLE_TEXT);
            SB_LINE(sb);
            sb_str(&sb, "Sénat, bloc présidentiel : ");
            sb_arrow(&sb, before, after, 0);
            if (y <= LAST_ROW) ui_text_draw(1, y++, sb.buf, UI_STYLE_HIGHLIGHT);
        }
    }
    if (page == 2 && r->budget_reconduit && y <= LAST_ROW) {
        y = rubric(y, "Budget reconduit");
        y = para(y, txt[TXT_BUDJ_RECONDUIT], UI_STYLE_TEXT);
        y++;
    }
    if (page == 2 && r->senate_amend && y <= LAST_ROW) {
        y = rubric(y, "Le Sénat amende le budget");
        y = para(y, txt[TXT_BUDJ_SENAT_AMENDE], UI_STYLE_ALERT);
        SB_LINE(sb);
        sb_str(&sb, "Ligne amendée : ");
        sb_str(&sb, bl_long((u8)(r->senate_amend - 1)));
        if (y <= LAST_ROW) ui_text_draw_n(1, y++, sb.buf, 38, 0, UI_STYLE_HIGHLIGHT);
    }

    // Une gueule en bas à droite si la page laisse la place : le président du Sénat
    // au renouvellement, sinon l'éditorialiste (ravi des mauvaises nouvelles).
    if (y <= 13) {
        GueuleId id = GUEULE_JOUR_EDITORIALISTE;
        Expr e = (r->censure == CENSURE_ADOPTEE || r->affair || r->impeach != IMP_EV_NONE)
               ? EXPR_CONTENT : EXPR_NEUTRE;
        if (page == 3 && r->senate_renewed) {
            u16 before = 0;
            for (u8 g = 0; g < GRP_COUNT; g++)
                if (sim_in_bloc(&g_game.country, g)) before += r->senate_before[g];
            id = GUEULE_SENAT_PRESIDENT;
            e = sim_senate_bloc_seats(&g_game.country) >= before ? EXPR_NEUTRE : EXPR_FURIEUX;
        }
        ui_medallion(29, 14, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(id, e, 29, 14, PAL2);
        ui_gueule_say(2400);   // l'éditorialiste (ou le président du Sénat) commente la page
        ui_text_right(28, 25, id == GUEULE_SENAT_PRESIDENT ? "Le président du Sénat" : "L’éditorialiste",
                      UI_STYLE_DIM);
    }
}

static void report_draw(void)
{
    const Country *c = &g_game.country;
    const TurnReport *r = &g_game.report;
    SB_LINE(sb);
    u16 y;
    u8 t;
    s16 dp;

    if (!dirty) return;
    dirty = FALSE;

    art_reset();
    photo_on = (page == 0);
    if (page != 0) {
        ui_window_title(0, 0, 40, 28, "LE QUOTIDIEN DU MATIN", FALSE);
        draw_politics();
        ui_footer(1, 26, 38, next_page(page) ? "A : suite" : "A : continuer");
        return;
    }

    ui_window(0, 0, 40, 28);
    ui_art_draw_pal(ART_DECOR_JOURNAL, 1, 1, PAL3);   // PAL2 : photo (portrait)

    t = c->turn > 0 ? c->turn - 1 : 0;   // tour (trimestre ou mois) qui vient de s'écouler
    if (game_is_monthly()) {
        sb_str(&sb, "Bilan du mois : ");
        sb_str(&sb, game_month_name(t));
        sb_str(&sb, " ");
        sb_num(&sb, game_calendar_year(t), 0);
    } else {
        sb_str(&sb, "Bilan du trimestre T");
        sb_num(&sb, (t & 3) + 1, 0);
        sb_str(&sb, " ");
        sb_num(&sb, game_calendar_year(t), 0);
    }
    ui_text_center(1, 6, 38, sb.buf, UI_STYLE_HIGHLIGHT);
    ui_separator(1, 7, 38);

    // Photo de une : le président, content ou gêné selon le trimestre.
    ui_medallion(PHOTO_X, PHOTO_Y, PHOTO_W, PHOTO_H);
    {
        s16 d = r->popularity_after - r->popularity_before;
        Expr e = (r->affair || d <= -15) ? EXPR_GENE : (d >= 10 ? EXPR_CONTENT : EXPR_NEUTRE);
        ui_gueule_draw(GUEULE_PRES(c->orientation), e, PHOTO_X, PHOTO_Y, PAL2);
    }

    // Popularité et capital politique
    y = FLOW_Y0;
    dp = r->popularity_after - r->popularity_before;
    ui_icon_draw(1, y, UI_ICON_POPULARITY);
    ui_text_draw(3, y, "Popularité", UI_STYLE_DIM);
    sb_init(&sb, sb_mem, SB_LINE_SIZE);
    sb_signed(&sb, dp, 1);
    ui_text_right(row_end(y), y, sb.buf, dp >= 0 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
    y++;
    sb_init(&sb, sb_mem, SB_LINE_SIZE);
    sb_num(&sb, r->popularity_before, 1);
    sb_str(&sb, " % → ");
    sb_num(&sb, r->popularity_after, 1);
    sb_str(&sb, " %");
    ui_text_draw(3, y++, sb.buf, dp >= 0 ? UI_STYLE_TEXT : UI_STYLE_ALERT);

    ui_icon_draw(1, y, UI_ICON_CP);
    ui_text_draw(3, y, "Capital politique", UI_STYLE_DIM);
    sb_init(&sb, sb_mem, SB_LINE_SIZE);
    sb_str(&sb, "+");
    sb_num(&sb, r->cp_gain, 0);
    ui_text_right(row_end(y), y, sb.buf, UI_STYLE_HIGHLIGHT);
    y++;
    {
        u8 nt = sim_cp_turns_to_next(c);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_str(&sb, "Total ");
        sb_num(&sb, c->cp, 0);
        sb_str(&sb, " · prochain ");
        if (nt <= 1) {
            sb_str(&sb, "au tour suivant");
        } else {
            sb_str(&sb, "dans ");
            sb_num(&sb, nt, 0);
            sb_str(&sb, " mois");
        }
        y = flow(3, y, 2, sb.buf, UI_STYLE_TEXT);
    }
    y++;

    // À la une : lois votées ce tour
    if (law_mask_any(&g_game.laws_tried) && y <= LAST_ROW) {
        y = rubric(y, "À la une");
        for (u16 i = 0; i < LOI_COUNT && y <= LAST_ROW; i++) {
            u8 ok;
            if (!law_mask_get(&g_game.laws_tried, (u8)i)) continue;
            ok = law_mask_get(&g_game.laws_adopted, (u8)i);
            ui_icon_draw(1, y, ok ? UI_ICON_CHECK : UI_ICON_CROSS);
            y = flow(3, y, 2, ok ? law_journal_ok[i] : law_journal_ko[i], ok ? UI_STYLE_TEXT : UI_STYLE_ALERT);
        }
        y++;
    }

    // Lois dont les effets arrivent
    if (r->n_matured && y <= LAST_ROW) {
        y = rubric(y, "Entrent en vigueur");
        for (u16 i = 0; i < r->n_matured && y <= LAST_ROW; i++) {
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y, law_name[r->matured[i]], row_end(y) - 3, 0, UI_STYLE_TEXT);
            y++;
        }
        y++;
    }

    // Indicateurs qui ont bougé
    if (y <= LAST_ROW) {
        u16 y0 = y;
        y = rubric(y, "Indicateurs");
        y = indicator(y, "Croissance", g_report_snap[0], c->growth, 2, " %", TRUE);
        y = indicator(y, "Chômage", g_report_snap[1], c->unemployment, 2, " %", FALSE);
        y = indicator(y, "Dette", g_report_snap[2], c->debt, 1, " %", FALSE);
        for (u16 i = 0; i < PIL_TOTAL; i++)
            y = indicator(y, pillar_name((u8)i, FALSE), g_report_snap[3 + i], c->pillars[i], 1, "", TRUE);
        y = indicator(y, "Tension", g_report_snap[3 + PIL_TOTAL], c->gauges[JAUGE_TENSION], 0, " %", FALSE);
        if (y == y0 + 1)
            ui_text_draw(3, y, "Rien de notable.", UI_STYLE_DIM);
    }

    ui_footer(1, 26, 38, has_politics() ? "A : suite" : "A : continuer");
}

static void report_exit(void)
{
    art_reset();
}

const Screen scr_report = { report_init, report_update, report_draw, report_exit };
