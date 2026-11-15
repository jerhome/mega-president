#include "audio/sfx.h"
#include "snd_audio.h"

typedef struct {
    const u8 *data;
    u16 len;           // octets (multiple de 256, alignés par rescomp)
} SfxSample;

#define S(x) { x, sizeof(x) }
static const SfxSample s_sounds[SFX_COUNT] = {
    S(sfx_move), S(sfx_ok), S(sfx_cancel), S(sfx_error), S(sfx_adopted),
    S(sfx_rejected), S(sfx_event), S(sfx_win), S(sfx_lose)
};

#define PCM_RATE_HZ 13300
#define SFX_PRIO    6      // sous la priorité de la voix

static u8 s_id;
static u16 s_left_ms;      // temps restant estimé (on n'interroge pas le Z80 : voir voice.c)
static u16 s_frame_ms;
static bool s_enabled;

void sfx_init(void)
{
    Z80_loadDriver(Z80_DRIVER_XGM2, TRUE);
    s_left_ms = 0;
    s_enabled = TRUE;
    s_frame_ms = IS_PAL_SYSTEM ? 20 : 17;
}

void sfx_play(SfxId id)
{
    if (!s_enabled || id >= SFX_COUNT) return;
    // Une élection en cours n'est pas coupée par le simple déplacement du curseur.
    if (s_left_ms && id == SFX_MOVE && (s_id == SFX_ELECTION_WIN || s_id == SFX_ELECTION_LOSE)) return;
    s_id = (u8)id;
    const SfxSample *s = &s_sounds[id];
    s_left_ms = (u16)(((u32)s->len * 1000) / PCM_RATE_HZ);
    XGM2_playPCMEx(s->data, s->len, SOUND_PCM_CH2, SFX_PRIO, FALSE, FALSE);
}

void sfx_update(void)
{
    if (s_left_ms > s_frame_ms) s_left_ms -= s_frame_ms;
    else s_left_ms = 0;
}

void sfx_set_enabled(bool on)
{
    s_enabled = on;
    if (!on) {
        XGM2_stopPCM(SOUND_PCM_CH2);
        s_left_ms = 0;
    }
}
