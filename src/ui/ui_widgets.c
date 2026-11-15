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

void ui_list_set_enabled_by(UiList *l, bool (*enabled)(u16 i, void *ctx), void *ctx)
{
    u16 i;

    for (i = 0; i < l->count && i < 64; i++)
        ui_list_set_disabled(l, i, !enabled(i, ctx));
}

s16 ui_nav_cycle(s16 sel, s16 first, s16 count, bool dec, bool inc, bool wrap)
{
    s16 n, last = first + count - 1;

    if (count <= 0 || dec == inc)
        return sel;
    if (dec)
        n = sel > first ? sel - 1 : (wrap ? last : first);
    else
        n = sel < last ? sel + 1 : (wrap ? first : last);
    if (n != sel)
        sfx_play(SFX_MOVE);
    return n;
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
    list->flags = 0;
    list->bar_sel = 0xFF;
}

bool ui_list_select_first_enabled(UiList *l)
{
    u16 i;

    if (l->sel < l->count && item_enabled(l, l->sel))
        return TRUE;
    for (i = 0; i < l->count; i++) {
        if (item_enabled(l, i)) {
            l->sel = (u8)i;
            list_fix_top(l);
            return TRUE;
        }
    }
    return FALSE;
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

void ui_list_draw(UiList *l)
{
    u16 r, text_w = l->w > 2 ? l->w - 2 : 1;
    u16 bar_w = l->w > 1 ? l->w - 1 : 1;     // la barre laisse la colonne des flèches
    u8 bar = (l->sel >= l->top && l->sel < l->top + l->rows) ? (u8)(l->sel - l->top) : 0xFF;

    // Barre de sélection (plan B) : retirée de l'ancienne ligne, posée sur la nouvelle.
    if (!(l->flags & UI_LIST_NOBAR)) {
        if (l->bar_sel != 0xFF && l->bar_sel != bar && l->bar_sel < l->rows)
            ui_select_bar_clear(l->x, l->y + l->bar_sel, bar_w);
        if (bar != 0xFF && l->count)
            ui_select_bar(l->x, l->y + bar, bar_w);
        l->bar_sel = bar;
    }

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

void ui_list_redraw(UiList *l)
{
    u16 r, bar_w = l->w > 1 ? l->w - 1 : 1;

    if (!(l->flags & UI_LIST_NOBAR))
        for (r = 0; r < l->rows; r++)
            ui_select_bar_clear(l->x, l->y + r, bar_w);
    l->bar_sel = 0xFF;
    ui_list_draw(l);
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
    u8 step;          // prochaine étape à dessiner (0 .. MSG_UNFOLD_STEPS), puis contenu
    u8 t;             // 1/300 s depuis la première image (dépliement)
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
    msg.step = 0;
    msg.t = 0;
    msg.h = (u8)h;
    msg.y = (u8)(UI_ROWS - 1 - h);   // une ligne de marge en bas (débordement TV)
    msg.title = title;
    msg.text = text;
    ui_list_init(&msg.list, MSG_X + 1, msg.y + h - 1 - nchoices, w, nchoices, choices, nchoices);
}

static void msgbox_draw(void)
{
    u16 y = msg.y + 1;

    if (msg.title) {
        ui_window_title(MSG_X, msg.y, MSG_W, msg.h, msg.title, TRUE);
        y += 2;
    } else {
        ui_window(MSG_X, msg.y, MSG_W, msg.h);
    }
    if (msg.text)
        ui_text_wrap(MSG_X + 1, y, MSG_TEXT_W, MSG_MAX_TEXT_LINES, msg.text, UI_STYLE_TEXT);
    msg.list.bar_sel = 0xFF;
    ui_list_draw(&msg.list);
}

#define MSG_UNFOLD_STEPS 3   // étapes de dépliement avant le contenu
#define MSG_UNFOLD_TICKS 6   // durée d'une étape : 20 ms (1/300 s), comme une image PAL

s16 ui_msgbox_update(void)
{
    UiListResult r;

    if (!msg.open)
        return -1;
    if (msg.step <= MSG_UNFOLD_STEPS) {
        u8 stage;

        // Étapes fondées sur le temps réel : une image perdue saute une étape.
        if (msg.step > 0 && msg.t < 255 - 30)
            msg.t += (u8)input_frame_ticks();
        stage = msg.t / MSG_UNFOLD_TICKS;
        if (stage > MSG_UNFOLD_STEPS)
            stage = MSG_UNFOLD_STEPS;
        if (stage < msg.step)
            return -1;
        if (stage < MSG_UNFOLD_STEPS) {
            // Dépliement : la fenêtre grandit vers le haut depuis le bas de la boîte.
            u16 h = 2 + (u16)((msg.h - 2) * (stage + 1)) / (MSG_UNFOLD_STEPS + 1);
            ui_window(MSG_X, msg.y + msg.h - h, MSG_W, h);
            msg.drawn = TRUE;
        } else {
            msgbox_draw();
        }
        msg.step = stage + 1;
        return -1;   // pas de saisie à l'image d'ouverture
    }
    r = ui_list_update(&msg.list);
    if (r == UI_LIST_MOVED)
        ui_list_draw(&msg.list);
    else if (r == UI_LIST_CHOSEN)
        return msg.list.sel;
    return -1;
}

void ui_msgbox_set_sel(u16 i)
{
    if (msg.open && i < msg.list.count)
        msg.list.sel = (u8)i;
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

// --- Confirmation oui / non ---------------------------------------------------

static const char *confirm_choices[2];

void ui_confirm_open(const char *title, const char *text,
                     const char *yes, const char *no, bool focus_no)
{
    confirm_choices[0] = yes;
    confirm_choices[1] = no;
    ui_msgbox_open(title, text, confirm_choices, 2);
    if (focus_no)
        ui_msgbox_set_sel(1);
}

s16 ui_confirm_update(void)
{
    s16 r = ui_msgbox_update();

    if (r < 0 && msg.open && msg.step > MSG_UNFOLD_STEPS && (input_pressed() & INPUT_CANCEL))
        return 0;   // B = non (le son d'annulation vient de la liste)
    return r == 0 ? 1 : (r > 0 ? 0 : -1);
}

// --- Indicateur ---------------------------------------------------------------

void ui_value_right(u16 x_end, u16 y, u16 w, const char *str, UiStyle style)
{
    ui_clear_rect(x_end - w, y, w, 1);
    ui_text_right(x_end, y, str, style);
}

#define STAT_VALUE_W 7

void ui_stat_row(u16 y, const UiStatRow *r)
{
    char buf[UI_LINE_BUF];
    u16 n = ui_format_number(buf, r->value, r->decimals);

    if (r->unit) {
        const char *u = r->unit;
        while (*u && n < sizeof(buf) - 1)
            buf[n++] = *u++;
        buf[n] = 0;
    }
    ui_icon_draw(1, y, r->icon);
    ui_text_draw_n(3, y, r->label, 13, 13, UI_STYLE_DIM);
    ui_gauge_draw(16, y, r->x_end - STAT_VALUE_W - 16, r->gauge, r->gauge_max, r->gauge_style);
    ui_value_right(r->x_end, y, STAT_VALUE_W, buf, r->value_style);
}
