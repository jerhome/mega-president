// Mega Président — accès à la SRAM (console seulement, voir sram.h).
#include "save/sram.h"

u8 sram_scores_load(ScoreBoard *b)
{
    u8 buf[SCORES_BLOB_SIZE];
    u16 i;
    u8 r;

    SRAM_enableRO();
    for (i = 0; i < SCORES_BLOB_SIZE; i++)
        buf[i] = SRAM_readByte(i);
    SRAM_disable();
    r = scores_deserialize(b, buf, SCORES_BLOB_SIZE);
    return r;
}

void sram_scores_save(const ScoreBoard *b)
{
    u8 buf[SCORES_BLOB_SIZE];
    u16 i;

    scores_serialize(b, buf);
    SRAM_enable();
    for (i = 0; i < SCORES_BLOB_SIZE; i++)
        SRAM_writeByte(i, buf[i]);
    SRAM_disable();
}

u8 sram_tagline_next(u8 n)
{
    u8 i;
    SRAM_enableRO();
    i = SRAM_readByte(SRAM_TAGLINE_OFFSET);
    SRAM_disable();
    i = (u8)((i + 1) % n);          // SRAM vierge (0xFF) : n'importe quel départ convient
    SRAM_enable();
    SRAM_writeByte(SRAM_TAGLINE_OFFSET, i);
    SRAM_disable();
    return i;
}
