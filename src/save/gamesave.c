// Mega Président — sauvegarde de partie, logique portable (voir gamesave.h).
#include "save/gamesave.h"

// Fletcher-16 à modulo différé : par blocs de 256 octets les sommes tiennent
// largement dans un u32 ; un seul modulo par bloc au lieu de deux par octet.
typedef struct { u32 a, b; } Fl;

static void fl_run(Fl *f, const u8 *p, u16 n)
{
    while (n) {
        u16 k = n > 256 ? 256 : n;
        n = (u16)(n - k);
        while (k--) { f->a += *p++; f->b += f->a; }
        f->a %= 255; f->b %= 255;
    }
}

u16 gamesave_fletcher16(const u8 *p, u16 n)
{
    Fl f = {0, 0};
    fl_run(&f, p, n);
    return (u16)((f.b << 8) | f.a);
}

static u16 block_sum(const u8 *blk, u16 size)
{
    Fl f = {0, 0};
    fl_run(&f, blk + 4, 4);
    fl_run(&f, blk + 10, 4);
    fl_run(&f, blk + GAMESAVE_HEADER, size);
    return (u16)((f.b << 8) | f.a);
}

static u8 has_sig(const u8 *in)
{
    return (u8)(in[0] == 'M' && in[1] == 'P' && in[2] == 'G' && in[3] == '1');
}

u16 gamesave_pack(const void *src, u16 size, u16 fp, u16 gen, u8 *out)
{
    const u8 *s = (const u8 *)src;
    u16 i, sum;

    out[0] = 'M'; out[1] = 'P'; out[2] = 'G'; out[3] = '1';
    out[4] = GAMESAVE_VERSION;
    out[5] = 0;
    out[6] = (u8)(size >> 8); out[7] = (u8)size;
    out[10] = (u8)(fp >> 8);  out[11] = (u8)fp;
    out[12] = (u8)(gen >> 8); out[13] = (u8)gen;
    for (i = 0; i < size; i++)
        out[GAMESAVE_HEADER + i] = s[i];
    sum = block_sum(out, size);
    out[8] = (u8)(sum >> 8);  out[9] = (u8)sum;
    return (u16)(GAMESAVE_HEADER + size);
}

u8 gamesave_unpack(void *dst, u16 size, u16 fp, const u8 *in, u16 len)
{
    u8 *d = (u8 *)dst;
    u16 i;

    if (len < (u16)(GAMESAVE_HEADER + size)) return 0;
    if (!has_sig(in)) return 0;
    if (in[4] != GAMESAVE_VERSION) return 0;
    if ((u16)(((u16)in[6] << 8) | in[7]) != size) return 0;
    if ((u16)(((u16)in[10] << 8) | in[11]) != fp) return 0;
    if (block_sum(in, size) != (u16)(((u16)in[8] << 8) | in[9])) return 0;
    if (d)
        for (i = 0; i < size; i++)
            d[i] = in[GAMESAVE_HEADER + i];
    return 1;
}

u8 gamesave_is_stale(const u8 *in, u16 len, u16 size, u16 fp)
{
    if (len < GAMESAVE_HEADER) return 0;
    if (!has_sig(in)) return 0;
    return (u8)!gamesave_unpack(0, size, fp, in, len);
}

u16 gamesave_gen(const u8 *in)
{
    return (u16)(((u16)in[12] << 8) | in[13]);
}

s8 gamesave_pick(const u8 *a, const u8 *b, u16 len, u16 size, u16 fp)
{
    u8 va = gamesave_unpack(0, size, fp, a, len);
    u8 vb = gamesave_unpack(0, size, fp, b, len);
    if (va && vb)
        return (s8)(((s16)(u16)(gamesave_gen(b) - gamesave_gen(a)) > 0) ? 1 : 0);
    if (va) return 0;
    if (vb) return 1;
    return -1;
}
