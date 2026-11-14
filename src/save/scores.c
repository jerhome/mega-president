// Mega Président — classements des high scores (portable, voir scores.h).
#include "save/scores.h"

static const u8 SIGNATURE[4] = { 'M', 'P', 'V', '1' };

void score_table_reset(ScoreTable *t)
{
    u8 i;
    for (i = 0; i < SCORES_COUNT; i++) {
        t->e[i].ini[0] = t->e[i].ini[1] = t->e[i].ini[2] = 0;
        t->e[i].score = 0;
        t->e[i].orient = 0;
        t->e[i].year = 0;
    }
}

void scores_reset(ScoreBoard *b)
{
    u8 i;
    for (i = 0; i < SCORES_TABLES; i++)
        score_table_reset(&b->t[i]);
}

s8 score_table_rank(const ScoreTable *t, u16 score)
{
    u8 i;
    if (score == 0)
        return -1;
    for (i = 0; i < SCORES_COUNT; i++)
        if (score > t->e[i].score)
            return (s8)i;
    return -1;
}

static char norm_letter(char c)
{
    if (c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 'A');
    return (c >= 'A' && c <= 'Z') ? c : 'A';
}

s8 score_table_insert(ScoreTable *t, const char ini[3], u16 score, u8 orient, u16 year)
{
    s8 r = score_table_rank(t, score);
    s16 i;
    u16 k;
    if (r < 0)
        return -1;
    k = (u16)r;
    for (i = SCORES_COUNT - 1; i > (s16)k; i--)
        t->e[i] = t->e[i - 1];
    t->e[k].ini[0] = norm_letter(ini[0]);
    t->e[k].ini[1] = norm_letter(ini[1]);
    t->e[k].ini[2] = norm_letter(ini[2]);
    t->e[k].score = score;
    t->e[k].orient = orient;
    t->e[k].year = year;
    return r;
}

// Somme de contrôle : rotation + xor, graine propre à chaque tableau.
static u16 checksum(const u8 *p, u16 n, u8 table)
{
    u16 s = (u16)(0x5A5A ^ (table * 0x0101));
    u16 i;
    for (i = 0; i < n; i++) {
        s = (u16)((s << 3) | (s >> 13));
        s = (u16)(s ^ p[i]);
        s = (u16)(s + 0x9D);
    }
    return s;
}

static u8 *put16(u8 *p, u16 v)
{
    *p++ = (u8)(v >> 8);
    *p++ = (u8)v;
    return p;
}

u16 scores_serialize(const ScoreBoard *b, u8 *buf)
{
    u8 *p = buf;
    u8 t, i;
    *p++ = SIGNATURE[0]; *p++ = SIGNATURE[1];
    *p++ = SIGNATURE[2]; *p++ = SIGNATURE[3];
    *p++ = SCORES_VERSION;
    for (t = 0; t < SCORES_TABLES; t++) {
        u8 *start = p;
        for (i = 0; i < SCORES_COUNT; i++) {
            const ScoreEntry *e = &b->t[t].e[i];
            *p++ = (u8)e->ini[0]; *p++ = (u8)e->ini[1]; *p++ = (u8)e->ini[2];
            p = put16(p, e->score);
            *p++ = e->orient;
            p = put16(p, e->year);
        }
        p = put16(p, checksum(start, SCORES_COUNT * SCORES_ENTRY_SIZE, t));
    }
    return SCORES_BLOB_SIZE;
}

// Tableau valide : initiales A-Z (ou 0,0,0 avec score 0), classement décroissant.
static u8 table_valid(const ScoreTable *t)
{
    u8 i, k;
    for (i = 0; i < SCORES_COUNT; i++) {
        const ScoreEntry *e = &t->e[i];
        if (e->score == 0) {
            if (e->ini[0] || e->ini[1] || e->ini[2])
                return 0;
        } else {
            for (k = 0; k < 3; k++)
                if (e->ini[k] < 'A' || e->ini[k] > 'Z')
                    return 0;
        }
        if (i > 0 && e->score > t->e[i - 1].score)
            return 0;
    }
    return 1;
}

u8 scores_deserialize(ScoreBoard *b, const u8 *buf, u16 len)
{
    u8 t, i, bad = 0;
    const u8 *p;
    scores_reset(b);
    if (!buf || len < SCORES_BLOB_SIZE)
        return SCORES_TABLES;
    for (i = 0; i < 4; i++)
        if (buf[i] != SIGNATURE[i])
            return SCORES_TABLES;
    if (buf[4] != SCORES_VERSION)
        return SCORES_TABLES;
    p = buf + 5;
    for (t = 0; t < SCORES_TABLES; t++, p += SCORES_TABLE_SIZE) {
        const u8 *q = p;
        u16 sum = (u16)((p[SCORES_COUNT * SCORES_ENTRY_SIZE] << 8)
                        | p[SCORES_COUNT * SCORES_ENTRY_SIZE + 1]);
        if (sum != checksum(p, SCORES_COUNT * SCORES_ENTRY_SIZE, t)) {
            bad++;
            continue;
        }
        for (i = 0; i < SCORES_COUNT; i++, q += SCORES_ENTRY_SIZE) {
            ScoreEntry *e = &b->t[t].e[i];
            e->ini[0] = (char)q[0]; e->ini[1] = (char)q[1]; e->ini[2] = (char)q[2];
            e->score = (u16)((q[3] << 8) | q[4]);
            e->orient = q[5];
            e->year = (u16)((q[6] << 8) | q[7]);
        }
        if (!table_valid(&b->t[t])) {
            score_table_reset(&b->t[t]);
            bad++;
        }
    }
    return bad;
}
