#include "audio/voice.h"
#include "snd_voice.h"

// Durée de l'échantillon en ticks (1/300 s), plus une marge : on n'interroge
// pas le Z80 juste après le lancement (il n'a pas encore pris la commande en
// compte et répondrait « arrêté », ce qui coupait la voix dès la 1re image).
#define VOICE_RATE_HZ   13400
#define VOICE_TICKS     ((u32)sizeof(snd_forsure) * 300 / VOICE_RATE_HZ + 30)

static bool s_done;     // la voix a déjà été lancée depuis l'allumage
static bool s_active;   // driver chargé, lecture en cours
static u32  s_end;      // tick (getTick) de fin prévue

bool voice_play_intro_once(void)
{
    if (s_done) return FALSE;
    s_done = TRUE;
    Z80_loadDriver(Z80_DRIVER_PCM, TRUE);
    SND_PCM_startPlay(snd_forsure, sizeof(snd_forsure), SOUND_PCM_RATE_13400, SOUND_PAN_CENTER, FALSE);
    s_end = getTick() + VOICE_TICKS;
    s_active = TRUE;
    return TRUE;
}

bool voice_busy(void)
{
    if (!s_active) return FALSE;
    if ((s32)(getTick() - s_end) < 0) return TRUE;
    if (SND_PCM_isPlaying()) return TRUE;
    Z80_unloadDriver();
    s_active = FALSE;
    return FALSE;
}
