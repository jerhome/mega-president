#include "util/strbuf.h"

void sb_init(StrBuf *sb, char *buf, u16 cap)
{
    sb->buf = buf;
    sb->cap = cap;
    sb->len = 0;
    sb->truncated = 0;
    if (cap)
        buf[0] = 0;
}

void sb_str(StrBuf *sb, const char *s)
{
    u16 room, n = 0, i;

    if (!s || !sb->cap)
        return;
    room = (u16)(sb->cap - 1 - sb->len);
    while (s[n])
        n++;
    if (n > room) {
        n = room;
        // s[n] est le premier octet non copié : si c'est une suite UTF-8
        // (10xxxxxx), on est au milieu d'un caractère, on recule.
        while (n > 0 && ((u8)s[n] & 0xC0) == 0x80)
            n--;
        sb->truncated = 1;
    }
    for (i = 0; i < n; i++)
        sb->buf[sb->len + i] = s[i];
    sb->len = (u16)(sb->len + n);
    sb->buf[sb->len] = 0;
}

static void sb_digits(StrBuf *sb, s32 value, u16 decimals, u8 plus)
{
    char out[16];   // signe + 10 chiffres + virgule + terminateur
    char tmp[12];
    u32 v;
    u16 n = 0, len = 0;

    if (decimals > 9)
        decimals = 9;
    if (value < 0) {
        out[len++] = '-';
        v = 0u - (u32)value;
    } else {
        if (plus && value > 0)
            out[len++] = '+';
        v = (u32)value;
    }
    do {
        tmp[n++] = (char)('0' + v % 10);
        v /= 10;
    } while (v || n <= decimals);   // au moins un chiffre avant la virgule
    while (n) {
        if (n == decimals)
            out[len++] = ',';
        out[len++] = tmp[--n];
    }
    out[len] = 0;
    sb_str(sb, out);
}

void sb_num(StrBuf *sb, s32 value, u16 decimals)
{
    sb_digits(sb, value, decimals, 0);
}

void sb_signed(StrBuf *sb, s32 value, u16 decimals)
{
    sb_digits(sb, value, decimals, 1);
}

void sb_arrow(StrBuf *sb, s32 before, s32 after, u16 decimals)
{
    sb_num(sb, before, decimals);
    sb_str(sb, " \xe2\x86\x92 ");   // →
    sb_num(sb, after, decimals);
}
