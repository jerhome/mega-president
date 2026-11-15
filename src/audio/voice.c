#include "audio/voice.h"
#include "snd_audio.h"
#include "audio/voice_chb_data.h"

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

// --- Charabia des personnages (4.18) ---------------------------------------------
// Une réplique = une syllabe d'attaque, un silence (gap_ms du personnage), une syllabe
// finale, tirées au hasard dans la banque du personnage. Toutes les durées sont des
// ticks du compteur de temps (1/300 s) : identiques en PAL et en NTSC.

#define CHB_SYL_MS(sy)   ((u16)((u32)(sy)->nblk * 256 * 1000 / CHB_RATE_HZ))
#define MS_TO_TICKS(ms)  ((u32)(ms) * 3 / 10)

enum { CH_IDLE = 0, CH_SYL1, CH_GAP, CH_SYL2 };

static struct {
    u8 phase;
    u8 voice;        // index dans chb_voices (ordre de GueuleId)
    u8 syl[2];       // indices des syllabes (0..3 attaque, 4..7 finale)
    u32 t0;          // tick de début de la phase
} ch;
static u8 s_last_pair = 0xFF;   // dernier couple tiré (jamais deux fois de suite)
static u16 s_seed = 0xC4A7;

static u16 ch_rand(u16 n)
{
    s_seed ^= s_seed << 7;
    s_seed ^= s_seed >> 9;
    s_seed ^= s_seed << 8;
    return (u16)(s_seed % n);
}

static void ch_play(u8 k)
{
    const ChbVoice *v = &chb_voices[ch.voice];
    const ChbSyl *sy = &v->syl[ch.syl[k]];

    XGM2_playPCMEx(v->data + ((u16)sy->off << 8), (u32)sy->nblk << 8, SOUND_PCM_CH3,
                   VOICE_PRIO, TRUE, FALSE);
    ch.phase = k ? CH_SYL2 : CH_SYL1;
    ch.t0 = getTick();
}

bool voice_say(u16 gueule)
{
    u8 pair;

    if (s_off || gueule >= CHB_COUNT)
        return FALSE;
    s_seed ^= (u16)getTick() | 1;
    do {
        ch.syl[0] = (u8)ch_rand(CHB_INIT);
        ch.syl[1] = (u8)(CHB_INIT + ch_rand(CHB_FIN));
        pair = (u8)(ch.syl[0] * 4 + ch.syl[1] - CHB_INIT);
    } while (pair == s_last_pair);
    s_last_pair = pair;
    ch.voice = (u8)gueule;
    ch_play(0);
    return TRUE;
}

void voice_say_stop(void)
{
    if (ch.phase != CH_IDLE) {
        XGM2_stopPCM(SOUND_PCM_CH3);
        ch.phase = CH_IDLE;
    }
}

bool voice_talking(void)
{
    return ch.phase != CH_IDLE;
}

// Forme de la bouche : 0 repos, 1 peu ouverte, 2 ouverte.
u8 voice_mouth(void)
{
    const ChbVoice *v;
    const ChbSyl *sy;
    u32 e;

    if (ch.phase == CH_IDLE || ch.phase == CH_GAP)
        return 0;
    v = &chb_voices[ch.voice];
    sy = &v->syl[ch.syl[ch.phase == CH_SYL2]];
    e = (getTick() - ch.t0) * 10 / 3;   // millisecondes écoulées
    if (e >= sy->v_ms)
        return 0;                        // la syllabe s'éteint : seuls les yeux bougent
    if (e < sy->c_ms)                    // consonne : lèvres closes si labiale
        return (sy->flags & 1) ? 0 : 1;
    return (sy->flags & 2) ? 1 : 2;      // voyelle fermée : peu ouverte
}

void voice_update(void)
{
    const ChbVoice *v;
    u32 dt;

    if (ch.phase == CH_IDLE)
        return;
    if (s_off) {
        voice_say_stop();
        return;
    }
    v = &chb_voices[ch.voice];
    dt = getTick() - ch.t0;
    if (ch.phase == CH_SYL1) {
        if (dt >= MS_TO_TICKS(CHB_SYL_MS(&v->syl[ch.syl[0]]))) {
            ch.phase = CH_GAP;
            ch.t0 = getTick();
        }
    } else if (ch.phase == CH_GAP) {
        if (dt >= MS_TO_TICKS(v->gap_ms))
            ch_play(1);
    } else if (dt >= MS_TO_TICKS(CHB_SYL_MS(&v->syl[ch.syl[1]]))) {
        ch.phase = CH_IDLE;
    }
}
