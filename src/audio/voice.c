#include "audio/voice.h"
#include "snd_voice.h"

static bool s_done;     // la voix a déjà été lancée depuis l'allumage
static bool s_active;   // driver chargé, lecture en cours

bool voice_play_intro_once(void)
{
    if (s_done) return FALSE;
    s_done = TRUE;
    Z80_loadDriver(Z80_DRIVER_PCM, TRUE);
    SND_PCM_startPlay(snd_forsure, sizeof(snd_forsure), SOUND_PCM_RATE_13400, SOUND_PAN_CENTER, FALSE);
    s_active = TRUE;
    return TRUE;
}

bool voice_busy(void)
{
    if (!s_active) return FALSE;
    if (SND_PCM_isPlaying()) return TRUE;
    Z80_unloadDriver();
    s_active = FALSE;
    return FALSE;
}
