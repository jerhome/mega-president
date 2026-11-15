#include "audio/voice.h"
#include "snd_audio.h"

// Durée de l'échantillon en ticks (1/300 s), plus une marge : on n'interroge
// pas le Z80 juste après le lancement (il n'a pas encore pris la commande en
// compte et répondrait « arrêté », ce qui coupait la voix dès la 1re image).
#define VOICE_RATE_HZ   13300
#define VOICE_TICKS     ((u32)sizeof(snd_forsure) * 300 / VOICE_RATE_HZ + 30)
#define VOICE_PRIO      15

static bool s_done;     // la voix a déjà été lancée depuis l'allumage
static bool s_active;   // lecture en cours
static u32  s_end;      // tick (getTick) de fin prévue
static bool s_off;      // réglage « Voix » à « Non »

void voice_set_enabled(bool on)
{
    s_off = !on;
    if (s_off && s_active) {
        XGM2_stopPCM(SOUND_PCM_CH3);
        s_active = FALSE;
    }
}

static bool voice_start(void)
{
    Z80_loadDriver(Z80_DRIVER_XGM2, TRUE);
    if (!XGM2_playPCMEx(snd_forsure, sizeof(snd_forsure), SOUND_PCM_CH3, VOICE_PRIO, FALSE, FALSE))
        return FALSE;
    s_end = getTick() + VOICE_TICKS;
    s_active = TRUE;
    return TRUE;
}

void voice_preview(void)
{
    if (!s_off)
        voice_start();
}

bool voice_play_intro_once(void)
{
    if (s_done) return FALSE;
    s_done = TRUE;
    if (s_off)
        return FALSE;   // voix coupée : la musique du titre démarre sans attendre
    return voice_start();
}

bool voice_busy(void)
{
    if (!s_active) return FALSE;
    if ((s32)(getTick() - s_end) < 0) return TRUE;
    if (XGM2_isPlayingPCM(SOUND_PCM_CH3_MSK)) return TRUE;
    s_active = FALSE;
    return FALSE;
}
