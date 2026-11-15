// Mega Président — début du second mandat et législatives.
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "content/content_world.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"
#include "ui/ui_hemi.h"
#include <string.h>

static void cat_n(char *dst, s32 v)
{
    char b[14];
    ui_format_number(b, v, 0);
    strcat(dst, b);
}

static bool dirty;

static void mandate2_init(void)
{
    dirty = TRUE;
}

static void mandate2_update(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_START))
        game_mandate2_done();
}

static void mandate2_draw(void)
{
    const LegislativeResult *l = &g_game.legislatives;
    const Country *c = &g_game.country;
    u8 g, cohab = sim_cohabitation(c);
    char buf[UI_LINE_BUF];
    const char *name;

    if (!dirty)
        return;
    dirty = FALSE;
    art_reset();
    ui_window_title(0, 0, 40, 28, "Second mandat · législatives", FALSE);

    // Gueule du Premier ministre (ou du PM de cohabitation, ravi) à droite.
    ui_medallion(29, 3, UI_PORTRAIT_W, UI_PORTRAIT_H);
    if (cohab) {
        ui_gueule_draw(GUEULE_COHAB(sim_cohab_camp(c)), EXPR_CONTENT, 29, 3, PAL2);
        name = txt[TXT_COHAB_PM_GAUCHE_NOM + sim_cohab_camp(c)];
    } else {
        ui_gueule_draw(GUEULE_PM(c->pm), l->won ? EXPR_CONTENT : EXPR_GENE, 29, 3, PAL2);
        name = txt[TXT_PM_TECHNOCRATE_NOM + 2 * c->pm];
    }
    ui_text_flow(1, 4, 27, 4, txt[TXT_SECOND_MANDAT], UI_STYLE_TEXT);

    // Sièges par groupe, avant et après.
    ui_text_draw(1, 9, "Groupe", UI_STYLE_DIM);
    ui_text_right(21, 9, "Avant", UI_STYLE_DIM);
    ui_text_right(28, 9, "Après", UI_STYLE_DIM);
    for (g = 0; g < GRP_COUNT; g++) {
        UiStyle st = (g == c->player_group) ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;
        u16 y = 10 + g;
        if (sim_in_bloc(c, g))
            ui_icon_draw(1, y, g == c->player_group ? UI_ICON_CP : UI_ICON_BULLET);
        ui_text_draw(3, y, grp_sigle[g], st);
        buf[0] = 0;
        cat_n(buf, l->seats_before[g]);
        ui_text_right(21, y, buf, UI_STYLE_DIM);
        buf[0] = 0;
        cat_n(buf, l->seats_after[g]);
        ui_text_right(28, y, buf, l->seats_after[g] >= l->seats_before[g] ? st : UI_STYLE_ALERT);
    }

    // Premier ministre (le nom ne tient pas sous la gueule : pleine largeur).
    ui_icon_draw(1, 16, UI_ICON_PERSON);
    ui_text_draw(3, 16, "Premier ministre", UI_STYLE_DIM);
    ui_text_right(39, 16, name, cohab ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);

    ui_icon_draw(1, 18, UI_ICON_ASSEMBLY);
    ui_text_draw(3, 18, "Bloc présidentiel", UI_STYLE_DIM);
    buf[0] = 0;
    cat_n(buf, l->bloc_before);
    strcat(buf, " → ");
    cat_n(buf, l->bloc_after);
    strcat(buf, " sièges");
    ui_text_right(39, 18, buf, UI_STYLE_TEXT);
    ui_hemi_majority(1, 19, 38, l->bloc_after, SEATS_TOTAL, SEATS_MAJORITY);
    ui_text_right(39, 21, l->majority ? "Majorité absolue" : "Majorité relative",
                  l->majority ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);

    // Début ou fin de cohabitation : ce texte remplace celui des législatives.
    if (l->cohab_started || l->cohab_ended)
        ui_text_wrap(1, 22, 38, 3, txt[l->cohab_started ? TXT_COHAB_DEBUT : TXT_COHAB_FIN],
                     l->cohab_started ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    else
        ui_text_wrap(1, 22, 38, 3, txt[l->won ? TXT_LEGISLATIVES_GAGNEES : TXT_LEGISLATIVES_PERDUES],
                     UI_STYLE_TEXT);
    ui_footer(1, 26, 38, "A : commencer le second mandat");
}

static void mandate2_exit(void)
{
    art_reset();
}

const Screen scr_mandate2 = { mandate2_init, mandate2_update, mandate2_draw, mandate2_exit };
