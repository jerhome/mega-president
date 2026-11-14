// Mega Président — widgets : liste avec curseur, boîte de message à choix.
#include "audio/sfx.h"
#include "ui/ui_internal.h"
#include "ui/input.h"

// --- Liste ------------------------------------------------------------------

static bool item_enabled(const UiList *l, u16 i)
{
    if (i < 32) return !(l->disabled & ((u32)1 << i));
    if (i < 64) return !(l->disabled_hi & ((u32)1 << (i - 32)));
    return TRUE;
}

bool ui_list_enabled(const UiList *l, u16 i)
{
    return item_enabled(l, i);
}

void ui_list_set_disabled(UiList *l, u16 i, bool on)
{
    if (i < 32) {
        if (on) l->disabled |= (u32)1 << i; else l->disabled &= ~((u32)1 << i);
    } else if (i < 64) {
        if (on) l->disabled_hi |= (u32)1 << (i - 32); else l->disabled_hi &= ~((u32)1 << (i - 32));
    }
}

static void list_fix_top(UiList *l)
{
    if (l->sel < l->top)
        l->top = l->sel;
    else if (l->sel >= l->top + l->rows)
        l->top = l->sel - l->rows + 1;
}

void ui_list_init(UiList *list, u16 x, u16 y, u16 w, u16 rows,
                  const char *const *items, u16 count)
{
    list->items = items;
    list->count = (u8)count;
    list->disabled = 0;
    list->disabled_hi = 0;
    list->x = (u8)x;
    list->y = (u8)y;
    list->w = (u8)w;
    list->rows = (u8)(rows ? rows : 1);
    list->sel = 0;
    list->top = 0;
}

UiListResult ui_list_update(UiList *l)
{
    u16 rep = input_repeat();
    u16 prs = input_pressed();
    u16 old = l->sel;

    if (l->count == 0)
        return (prs & INPUT_CANCEL) ? UI_LIST_CANCELLED : UI_LIST_IDLE;

    if (rep & INPUT_UP)
        l->sel = l->sel ? l->sel - 1 : l->count - 1;          // reboucle
    else if (rep & INPUT_DOWN)
        l->sel = (l->sel + 1 < l->count) ? l->sel + 1 : 0;
    else if (rep & INPUT_LEFT)                                  // page précédente
        l->sel = l->sel > l->rows ? l->sel - l->rows : 0;
    else if (rep & INPUT_RIGHT)                                 // page suivante
        l->sel = (l->sel + l->rows < l->count) ? l->sel + l->rows : l->count - 1;

    if (l->sel != old) {
        list_fix_top(l);
        sfx_play(SFX_MOVE);
        return UI_LIST_MOVED;
    }
    if (prs & INPUT_OK) {
        if (item_enabled(l, l->sel)) {
            sfx_play(SFX_OK);
            return UI_LIST_CHOSEN;
        }
        sfx_play(SFX_ERROR);
    }
    if (prs & INPUT_CANCEL) {
        sfx_play(SFX_CANCEL);
        return UI_LIST_CANCELLED;
    }
    return UI_LIST_IDLE;
}

void ui_list_draw(const UiList *l)
{
    u16 r, text_w = l->w > 2 ? l->w - 2 : 1;

    for (r = 0; r < l->rows; r++) {
        u16 i = l->top + r;
        u16 y = l->y + r;

        if (i < l->count) {
            UiStyle st = !item_enabled(l, i) ? UI_STYLE_DIM
                       : (i == l->sel ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
            if (i == l->sel)
                ui_icon_draw(l->x, y, UI_ICON_CURSOR);
            else
                ui_clear_rect(l->x, y, 1, 1);
            ui_text_draw_n(l->x + 1, y, l->items[i], text_w, text_w, st);
        } else {
            ui_clear_rect(l->x, y, l->w, 1);
        }
    }
    // Flèches de défilement dans la dernière colonne.
    ui_clear_rect(l->x + l->w - 1, l->y, 1, l->rows);
    if (l->top > 0)
        ui_icon_draw(l->x + l->w - 1, l->y, UI_ICON_ARROW_UP);
    if (l->top + l->rows < l->count)
        ui_icon_draw(l->x + l->w - 1, l->y + l->rows - 1, UI_ICON_ARROW_DOWN);
}

// --- Boîte de message -------------------------------------------------------

#define MSG_X 0
#define MSG_W UI_COLS
#define MSG_TEXT_W UI_TEXT_COLS
#define MSG_MAX_TEXT_LINES 12
#define MSG_MAX_CHOICES 6

static const char *const ok_choice[1] = { "OK" };

static struct {
    bool open;
    bool drawn;
    u8 y, h;
    const char *title;
    const char *text;
    UiList list;
} msg;

void ui_msgbox_open(const char *title, const char *text,
                    const char *const *choices, u16 nchoices)
{
    u16 lines, h, w = 0, i;

    if (msg.open)
        ui_msgbox_close();
    if (nchoices == 0) {
        choices = ok_choice;
        nchoices = 1;
    }
    if (nchoices > MSG_MAX_CHOICES)
        nchoices = MSG_MAX_CHOICES;

    lines = text ? ui_text_count_lines(MSG_TEXT_W, text) : 0;
    if (lines > MSG_MAX_TEXT_LINES)
        lines = MSG_MAX_TEXT_LINES;
    // cadre (2) + titre et ligne vide (2) + texte + ligne vide + choix
    h = 2 + (title ? 2 : 0) + lines + (lines ? 1 : 0) + nchoices;

    for (i = 0; i < nchoices; i++) {
        u16 n = ui_text_len(choices[i]);
        if (n > w)
            w = n;
    }
    w += 2;   // curseur + colonne des flèches
    if (w > MSG_TEXT_W)
        w = MSG_TEXT_W;

    msg.open = TRUE;
    msg.drawn = FALSE;
    msg.h = (u8)h;
    msg.y = (u8)(UI_ROWS - 1 - h);   // une ligne de marge en bas (débordement TV)
    msg.title = title;
    msg.text = text;
    ui_list_init(&msg.list, MSG_X + 1, msg.y + h - 1 - nchoices, w, nchoices, choices, nchoices);
}

static void msgbox_draw(void)
{
    u16 y = msg.y + 1;

    ui_window(MSG_X, msg.y, MSG_W, msg.h);
    if (msg.title) {
        ui_text_draw_n(MSG_X + 1, y, msg.title, MSG_TEXT_W, 0, UI_STYLE_HIGHLIGHT);
        y += 2;
    }
    if (msg.text)
        ui_text_wrap(MSG_X + 1, y, MSG_TEXT_W, MSG_MAX_TEXT_LINES, msg.text, UI_STYLE_TEXT);
    ui_list_draw(&msg.list);
}

s16 ui_msgbox_update(void)
{
    UiListResult r;

    if (!msg.open)
        return -1;
    if (!msg.drawn) {
        msgbox_draw();
        msg.drawn = TRUE;
        return -1;   // pas de saisie à l'image d'ouverture
    }
    r = ui_list_update(&msg.list);
    if (r == UI_LIST_MOVED)
        ui_list_draw(&msg.list);
    else if (r == UI_LIST_CHOSEN)
        return msg.list.sel;
    return -1;
}

bool ui_msgbox_is_open(void)
{
    return msg.open;
}

void ui_msgbox_close(void)
{
    if (msg.open && msg.drawn)
        ui_window_erase(MSG_X, msg.y, MSG_W, msg.h);
    msg.open = FALSE;
}
