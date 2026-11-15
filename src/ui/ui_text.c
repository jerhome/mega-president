// Mega Président — texte UTF-8, police accentuée, retour à la ligne (tâche 1.3).
#include "ui/ui_internal.h"
#include "ui/ui_font_map.h"

#define GLYPH_SPACE    0
#define GLYPH_QUESTION ('?' - 32)
#define GLYPH_NBSP     0xFE   // espace insécable : glyphe espace, mais pas de coupure
#define GLYPH_NEWLINE  0xFF

// Décode un point de code UTF-8 et avance *p. Renvoie 0 en fin de chaîne.
// Une séquence invalide donne U+FFFD et n'avance que d'un octet.
static u16 utf8_next(const char **p)
{
    const u8 *s = (const u8 *)*p;
    u8 c = s[0];

    if (c == 0)
        return 0;
    if (c < 0x80) {
        *p += 1;
        return c;
    }
    if ((c & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80) {
        *p += 2;
        return ((u16)(c & 0x1F) << 6) | (s[1] & 0x3F);
    }
    if ((c & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80) {
        *p += 3;
        return ((u16)(c & 0x0F) << 12) | ((u16)(s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    }
    *p += 1;
    return 0xFFFD;
}

// Point de code -> glyphe (ou GLYPH_NBSP / GLYPH_NEWLINE).
static u8 glyph_of(u16 cp)
{
    u16 lo, hi;

    if (cp >= 32 && cp <= 126)
        return (u8)(cp - 32);
    switch (cp) {
    case '\n':   return GLYPH_NEWLINE;
    case 0x00A0:                          // espace insécable
    case 0x202F: return GLYPH_NBSP;       // espace fine insécable
    case 0x2010: case 0x2011:
    case 0x2013: case 0x2014: return '-' - 32;   // tirets
    case 0x2018: return '\'' - 32;
    case 0x201C: case 0x201D: return '"' - 32;
    default: break;
    }
    lo = 0;
    hi = UI_FONT_EXTRA;
    while (lo < hi) {
        u16 mid = (lo + hi) >> 1;
        if (ui_font_extra_cp[mid] == cp)
            return ui_font_extra_glyph[mid];
        if (ui_font_extra_cp[mid] < cp)
            lo = mid + 1;
        else
            hi = mid;
    }
    return GLYPH_QUESTION;
}

static u8 next_glyph(const char **p, bool *end)
{
    u16 cp = utf8_next(p);
    *end = (cp == 0);
    return cp ? glyph_of(cp) : 0;
}

// Écrit n glyphes d'un tampon de ligne, complétés par des espaces jusqu'à pad.
static void put_row(u16 x, u16 y, const u8 *glyphs, u16 n, u16 pad, UiStyle style)
{
    u16 row[UI_COLS];
    u16 i, total = n > pad ? n : pad;

    if (x >= UI_COLS || y >= UI_ROWS)
        return;
    if (x + total > UI_COLS)
        total = UI_COLS - x;
    if (total == 0)
        return;
    for (i = 0; i < total; i++) {
        u8 g = i < n ? glyphs[i] : GLYPH_SPACE;
        if (g == GLYPH_NBSP)
            g = GLYPH_SPACE;
        row[i] = UI_ATTR(UI_STYLE_PAL(style), UI_FONT_BASE(style) + g);
    }
    VDP_setTileMapDataRow(BG_A, row, y, x, total, CPU);
}

u16 ui_text_len(const char *str)
{
    u16 n = 0;
    bool end;

    for (;;) {
        u8 g = next_glyph(&str, &end);
        if (end)
            return n;
        if (g != GLYPH_NEWLINE)
            n++;
    }
}

u16 ui_text_draw_n(u16 x, u16 y, const char *str, u16 max_cols, u16 pad_cols, UiStyle style)
{
    u8 buf[UI_COLS];
    u16 n = 0;
    bool end;

    if (max_cols > UI_COLS)
        max_cols = UI_COLS;
    while (n < max_cols) {
        u8 g = next_glyph(&str, &end);
        if (end || g == GLYPH_NEWLINE)
            break;
        buf[n++] = g;
    }
    put_row(x, y, buf, n, pad_cols, style);
    return n;
}

u16 ui_text_draw(u16 x, u16 y, const char *str, UiStyle style)
{
    return ui_text_draw_n(x, y, str, UI_COLS, 0, style);
}

void ui_text_center(u16 x, u16 y, u16 w, const char *str, UiStyle style)
{
    u16 n = ui_text_len(str);

    if (n > w)
        n = w;
    ui_text_draw_n(x + (w - n) / 2, y, str, w, 0, style);
}

// Découpe la prochaine ligne de *p sur w colonnes (w <= UI_COLS).
// Remplit buf, renvoie le nombre de glyphes, met *end en fin de texte.
static u16 wrap_line(const char **p, u16 w, u8 *buf, bool *end)
{
    const char *s = *p;
    const char *look;
    u16 n = 0;
    bool e;

    *end = FALSE;
    // Espaces ordinaires en début de ligne ignorés.
    for (;;) {
        look = s;
        if (next_glyph(&look, &e) != GLYPH_SPACE || e)
            break;
        s = look;
    }

    for (;;) {
        // Mesure du mot suivant (jusqu'à une espace, un saut ou la fin).
        const char *word = s;
        u16 len = 0;
        u8 g;

        look = s;
        for (;;) {
            const char *before = look;
            g = next_glyph(&look, &e);
            if (e || g == GLYPH_SPACE || g == GLYPH_NEWLINE) {
                look = before;
                break;
            }
            len++;
        }

        if (len > 0) {
            u16 need = n ? n + 1 + len : len;
            if (need > w) {
                if (n > 0)
                    break;               // le mot passe à la ligne suivante
                len = w;                 // mot plus long que la ligne : coupé
            }
            if (n)
                buf[n++] = GLYPH_SPACE;
            look = word;
            while (len--)
                buf[n++] = next_glyph(&look, &e);
            s = look;
        }

        // Séparateur après le mot.
        look = s;
        g = next_glyph(&look, &e);
        if (e) {
            *end = TRUE;
            s = look;
            break;
        }
        if (g == GLYPH_NEWLINE) {
            s = look;
            break;
        }
        if (g == GLYPH_SPACE) {
            // Espaces multiples réduites à une seule.
            do {
                s = look;
                g = next_glyph(&look, &e);
            } while (!e && g == GLYPH_SPACE);
            if (n >= w)
                break;
            continue;
        }
        // Mot coupé (trop long) : on continue sur la ligne suivante.
        break;
    }

    // Fin de texte juste après cette ligne ?
    if (!*end) {
        look = s;
        for (;;) {
            u8 g = next_glyph(&look, &e);
            if (e) {
                *end = TRUE;
                break;
            }
            if (g != GLYPH_SPACE)
                break;
        }
    }
    *p = s;
    return n;
}

u16 ui_text_wrap(u16 x, u16 y, u16 w, u16 max_lines, const char *str, UiStyle style)
{
    u8 buf[UI_COLS];
    u16 lines = 0;
    bool end = FALSE;

    if (w == 0 || w > UI_COLS)
        w = UI_COLS;
    while (!end && lines < max_lines) {
        u16 n = wrap_line(&str, w, buf, &end);
        put_row(x, y + lines, buf, n, 0, style);
        lines++;
    }
    return lines;
}

const char *ui_text_wrap_line(u16 x, u16 y, u16 w, const char *str, UiStyle style)
{
    u8 buf[UI_COLS];
    bool end = FALSE;
    u16 n;

    if (!str)
        return NULL;
    if (w == 0 || w > UI_COLS)
        w = UI_COLS;
    n = wrap_line(&str, w, buf, &end);
    put_row(x, y, buf, n, 0, style);
    return end ? NULL : str;
}

void ui_text_right(u16 x_end, u16 y, const char *str, UiStyle style)
{
    u16 n = ui_text_len(str);

    if (n > x_end)
        n = x_end;
    ui_text_draw_n(x_end - n, y, str, n, 0, style);
}

u16 ui_text_count_lines(u16 w, const char *str)
{
    u8 buf[UI_COLS];
    u16 lines = 0;
    bool end = FALSE;

    if (w == 0 || w > UI_COLS)
        w = UI_COLS;
    while (!end) {
        wrap_line(&str, w, buf, &end);
        lines++;
    }
    return lines;
}
