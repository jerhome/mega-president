// Mega Président — sauvegarde de partie en SRAM (voir sram_game.h).
#include "save/sram_game.h"
#include "save/gamesave.h"

#define GS_LEN ((u16)(GAMESAVE_HEADER + sizeof(Game)))

// Tampon statique : évite de monter la pile de ~sizeof(Game) octets.
static u8 s_buf[GAMESAVE_HEADER + sizeof(Game)];

static void read_block(void)
{
    u16 i;
    SRAM_enableRO();
    for (i = 0; i < GS_LEN; i++)
        s_buf[i] = SRAM_readByte(SRAM_GAME_OFFSET + i);
    SRAM_disable();
}

void sram_game_save(const Game *g)
{
    u16 i, n = gamesave_pack(g, (u16)sizeof(Game), s_buf);
    SRAM_enable();
    for (i = 0; i < n; i++)
        SRAM_writeByte(SRAM_GAME_OFFSET + i, s_buf[i]);
    SRAM_disable();
}

u8 sram_game_load(Game *g)
{
    read_block();
    return gamesave_unpack(g, (u16)sizeof(Game), s_buf, GS_LEN);
}

u8 sram_game_exists(void)
{
    read_block();
    return gamesave_unpack(0, (u16)sizeof(Game), s_buf, GS_LEN);
}

u8 sram_game_stale(void)
{
    read_block();
    return gamesave_is_stale(s_buf, GS_LEN, (u16)sizeof(Game));
}

void sram_game_erase(void)
{
    u16 i;
    SRAM_enable();
    for (i = 0; i < 4; i++)  // détruire la signature suffit
        SRAM_writeByte(SRAM_GAME_OFFSET + i, 0);
    SRAM_disable();
}
