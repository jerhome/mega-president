// Mega Président — sauvegarde de partie en SRAM (voir sram_game.h).
#include "save/sram_game.h"

#define GS_LEN SRAM_GAME_SLOT

// Tampon statique : évite de monter la pile de ~sizeof(Game) octets.
// Aligné sur 2 : game_sanitize lit la struct Game directement dans le tampon.
static u8 s_buf[GAMESAVE_HEADER + sizeof(Game)] __attribute__((aligned(2)));

// État mis en cache (SRAM lue une seule fois, jusqu'à la prochaine écriture).
static u8  s_known;
static u8  s_valid[2];
static u16 s_gen[2];
static u8  s_stale;

static u16 slot_base(u8 k) { return (u16)(SRAM_GAME_OFFSET + (u16)k * GS_LEN); }

static void read_slot(u8 k)
{
    u16 i, base = slot_base(k);
    SRAM_enableRO();
    for (i = 0; i < GS_LEN; i++)
        s_buf[i] = SRAM_readByte(base + i);
    SRAM_disable();
}

static void refresh(void)
{
    u8 k, sig = 0;
    u16 fp;

    if (s_known) return;
    fp = game_fingerprint();
    for (k = 0; k < 2; k++) {
        read_slot(k);
        s_valid[k] = (u8)(gamesave_unpack(0, (u16)sizeof(Game), fp, s_buf, GS_LEN)
                          && game_sanitize((const Game *)(s_buf + GAMESAVE_HEADER)));
        s_gen[k] = gamesave_gen(s_buf);
        if (s_buf[0] == 'M' && s_buf[1] == 'P' && s_buf[2] == 'G' && s_buf[3] == '1')
            sig = 1;
    }
    s_stale = (u8)(sig && !s_valid[0] && !s_valid[1]);
    s_known = 1;
}

// Emplacement valide le plus récent, -1 si aucun.
static s8 best(void)
{
    refresh();
    if (s_valid[0] && s_valid[1])
        return (s8)(((s16)(u16)(s_gen[1] - s_gen[0]) > 0) ? 1 : 0);
    if (s_valid[0]) return 0;
    if (s_valid[1]) return 1;
    return -1;
}

void sram_game_save(const Game *g)
{
    s8 b = best();
    u8 target = (u8)(b == 0 ? 1 : 0);
    u16 gen = (u16)(b < 0 ? 1 : s_gen[b] + 1);
    u16 i, n, base = slot_base(target);

    n = gamesave_pack(g, (u16)sizeof(Game), game_fingerprint(), gen, s_buf);
    SRAM_enable();
    SRAM_writeByte(base, 0);                 // invalide l'emplacement cible d'abord
    for (i = 1; i < n; i++)
        SRAM_writeByte(base + i, s_buf[i]);
    SRAM_writeByte(base, s_buf[0]);          // signature en dernier : bloc complet
    SRAM_disable();
    s_known = 0;
}

u8 sram_game_load(Game *g)
{
    s8 b = best();
    if (b < 0) return 0;
    read_slot((u8)b);
    return gamesave_unpack(g, (u16)sizeof(Game), game_fingerprint(), s_buf, GS_LEN);
}

u8 sram_game_exists(void)
{
    return (u8)(best() >= 0);
}

u8 sram_game_stale(void)
{
    refresh();
    return s_stale;
}

void sram_game_erase(void)
{
    u8 k;
    SRAM_enable();
    for (k = 0; k < 2; k++) {
        u16 i, base = slot_base(k);
        for (i = 0; i < 4; i++)  // détruire la signature suffit
            SRAM_writeByte(base + i, 0);
    }
    SRAM_disable();
    s_known = 0;
}
