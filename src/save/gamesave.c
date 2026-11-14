// Mega Président — sauvegarde de partie, logique portable (voir gamesave.h).
#include "save/gamesave.h"

static u16 fletcher16(const u8 *p, u16 n)
{
    u16 a = 0, b = 0, i;
    for (i = 0; i < n; i++) {
        a = (u16)((a + p[i]) % 255);
        b = (u16)((b + a) % 255);
    }
    return (u16)((b << 8) | a);
}

u16 gamesave_pack(const void *src, u16 size, u8 *out)
{
    const u8 *s = (const u8 *)src;
    u16 i, sum = fletcher16(s, size);

    out[0] = 'M'; out[1] = 'P'; out[2] = 'G'; out[3] = '1';
    out[4] = GAMESAVE_VERSION;
    out[5] = 0;
    out[6] = (u8)(size >> 8); out[7] = (u8)size;
    out[8] = (u8)(sum >> 8);  out[9] = (u8)sum;
    for (i = 0; i < size; i++)
        out[GAMESAVE_HEADER + i] = s[i];
    return (u16)(GAMESAVE_HEADER + size);
}

u8 gamesave_unpack(void *dst, u16 size, const u8 *in, u16 len)
{
    u8 *d = (u8 *)dst;
    u16 i, stored;

    if (len < (u16)(GAMESAVE_HEADER + size)) return 0;
    if (in[0] != 'M' || in[1] != 'P' || in[2] != 'G' || in[3] != '1') return 0;
    if (in[4] != GAMESAVE_VERSION) return 0;
    if ((u16)(((u16)in[6] << 8) | in[7]) != size) return 0;
    stored = (u16)(((u16)in[8] << 8) | in[9]);
    if (fletcher16(in + GAMESAVE_HEADER, size) != stored) return 0;
    if (d)
        for (i = 0; i < size; i++)
            d[i] = in[GAMESAVE_HEADER + i];
    return 1;
}
