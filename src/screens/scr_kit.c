// Mega Président — kit d'écran (voir scr_kit.h).
#include "scr_kit.h"
#include "scr_txt.h"
#include "game/game.h"
#include "util/strbuf.h"
#include "content/content_world.h"
#include "evt_art.h"

void kit_header(const char *title)
{
    const Country *c = &g_game.country;
    SB_LINE(sb);
    u16 n;

    if (title) {
        ui_banner(0, 0, 40, title, TRUE);
    } else {
        char lbl[UI_LINE_BUF];
        game_turn_label(c->turn, lbl);
        ui_banner(0, 0, 40, lbl, TRUE);
    }
    sb_num(&sb, c->cp, 0);
    sb_str(&sb, "/");
    sb_num(&sb, CP_MAX, 0);
    n = ui_text_len(sb.buf);
    ui_text_right(39, 1, " CP", UI_STYLE_DIM);
    ui_text_right(36, 1, sb.buf, UI_STYLE_TEXT);
    ui_icon_draw(34 - n, 1, UI_ICON_CP);
}

void kit_page(const char *title)
{
    art_reset_quiet();
    kit_header(title);
}

KitErr kit_err_from_law(u8 e)
{
    switch (e) {
    case LAW_OK:          return KIT_ERR_NONE;
    case LAW_ERR_CP:      return KIT_ERR_CP;
    case LAW_ERR_ADOPTED: return KIT_ERR_ADOPTED;
    case LAW_ERR_FULL:    return KIT_ERR_QUEUE;
    case LAW_ERR_493:     return KIT_ERR_493;
    case LAW_ERR_COHAB:   return KIT_ERR_COHAB;
    default:              return KIT_ERR_OTHER;
    }
}

void kit_err_line(u16 y, KitErr err)
{
    switch (err) {
    case KIT_ERR_NONE:
        break;
    case KIT_ERR_CP:
        ui_text_draw(1, y, "Capital politique insuffisant", UI_STYLE_ALERT);
        break;
    case KIT_ERR_ADOPTED:
        ui_text_draw(1, y, "Loi déjà adoptée", UI_STYLE_HIGHLIGHT);
        break;
    case KIT_ERR_QUEUE:
        ui_text_draw(1, y, "File d’attente pleine : patientez", UI_STYLE_ALERT);
        break;
    case KIT_ERR_493:
        ui_text_draw(1, y, "49.3 déjà utilisé cette année", UI_STYLE_ALERT);
        break;
    case KIT_ERR_COHAB:
        ui_text_wrap(1, y, 38, 2, txt[TXT_COHAB_LOI_REFUSEE], UI_STYLE_ALERT);
        break;
    default:
        ui_text_draw(1, y, "Impossible pour l’instant", UI_STYLE_ALERT);
        break;
    }
}

void kit_group_seats(u16 y, const u16 *seats, const u16 *before, const u8 *pour)
{
    const Country *c = &g_game.country;
    for (u16 g = 0; g < GRP_COUNT; g++) {
        u16 x = 2 + g * 7;   // cases de 7 colonnes, x 2 à 36
        SB_LINE(sb);
        if (pour) ui_icon_draw(x + 1, y, pour[g] ? UI_ICON_CHECK : UI_ICON_CROSS);
        ui_text_draw(x + 2, y, grp_sigle[g], g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
        if (seats) {
            if (before) sb_arrow(&sb, before[g], seats[g], 0);
            else sb_num(&sb, seats[g], 0);
            ui_text_center(x, y + 1, 7, sb.buf, UI_STYLE_DIM);
        }
    }
}
