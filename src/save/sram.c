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
