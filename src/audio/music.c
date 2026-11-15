#include "audio/music.h"
#include "audio/music_tracks.h"
#include "snd_audio.h"
#include "audio/voice.h"
#include "ui/screen.h"

// Piste de la ressource XGM2 multi-pistes pour chaque morceau.
static const u8 s_track[MUS_COUNT] = {
    [MUS_TITLE]      = TRACK_TITLE,
    [MUS_YEAR1]      = TRACK_YEAR1,
    [MUS_YEAR2]      = TRACK_YEAR2,
    [MUS_YEAR3]      = TRACK_YEAR3,
    [MUS_YEAR4]      = TRACK_YEAR4,
    [MUS_YEAR5]      = TRACK_YEAR5,
    [MUS_ELECTION]   = TRACK_ELECTION,
    [MUS_REPORT]     = TRACK_REPORT,
    [MUS_EVENT]      = TRACK_EVENT,
    [MUS_ASSEMBLY]   = TRACK_ASSEMBLY,
    [MUS_MAP]        = TRACK_MAP,
    [MUS_BUDGET]     = TRACK_BUDGET,
    [MUS_REFERENDUM] = TRACK_REFERENDUM,
    [MUS_RESHUFFLE]  = TRACK_RESHUFFLE,
    [MUS_IMPEACH]    = TRACK_IMPEACH,
    [MUS_CAMPAIGN]   = TRACK_CAMPAIGN,
    [MUS_MANDATE2]   = TRACK_MANDATE2,
    [MUS_END_WIN]    = TRACK_END_WIN,
    [MUS_END_LOSE]   = TRACK_END_LOSE,
    [MUS_SCORES]     = TRACK_SCORES,
    [MUS_HELP]       = TRACK_HELP,
};

// Fondus en millisecondes (convertis en images selon PAL/NTSC).
#define FADE_OUT_MS 350
#define FADE_IN_MS  250
#define STOP_MS     150

static u8 s_id;        // morceau demandé (MusicId), 0xFF = aucun
static u8 s_playing;   // piste XGM2 en cours, 0xFF = silence
static u8 s_pending;   // piste à lancer à la fin du fondu de sortie, 0xFF = aucune
static bool s_fading;  // fondu de sortie en cours
static bool s_enabled;
static u8 s_fm = 100, s_psg = 100;   // volumes XGM2 voulus (réglage « Musique »)
static bool s_vol_pending;           // à réappliquer à la fin du fondu en cours

// Cran perçu -> valeurs XGM2 (FM : pas de 0,75 dB, PSG : pas de 2 dB).
static const u8 s_vol_fm[5]  = { 0, 31, 46, 61, 100 };
static const u8 s_vol_psg[5] = { 0, 13, 35, 61, 100 };

static u16 ms_to_frames(u16 ms)
{
    return (u16)(((u32)ms * (IS_PAL_SYSTEM ? 50 : 60)) / 1000);
}

static void start_track(u8 track)
{
    XGM2_playTrack(track);
    XGM2_fadeIn(ms_to_frames(FADE_IN_MS));
    s_playing = track;
}

void music_init(void)
{
    s_id = 0xFF;
    s_playing = 0xFF;
    s_pending = 0xFF;
    s_fading = FALSE;
    s_enabled = TRUE;
    Z80_loadDriver(Z80_DRIVER_XGM2, TRUE);
    XGM2_load(xgm2_music);   // table des échantillons de la ressource multi-pistes (pile, pas de tas)
}

static void request_track(u8 track)
{
    if (track == s_playing && !s_fading) return;
    if (s_playing == 0xFF && !s_fading && voice_busy()) {
        s_pending = track;        // la voix « For sure ! » passe d'abord, seule
        return;
    }
    if (s_playing == 0xFF && !s_fading) {
        start_track(track);
        return;
    }
    s_pending = track;
    if (!s_fading) {
        XGM2_fadeOutAndStop(ms_to_frames(FADE_OUT_MS));
        s_fading = TRUE;
    }
}

void music_play(MusicId id)
{
    if (id >= MUS_COUNT) return;
    u8 track = s_track[id];
    s_id = (u8)id;
    if (!s_enabled) return;   // mémorisé : joué si la musique est réactivée
    request_track(track);
}

void music_play_year(u8 year)
{
    // Mandat de 5 ans : le 2nd mandat reprend les mêmes 5 thèmes.
    music_play((MusicId)(MUS_YEAR1 + year % 5));
}

void music_for_screen(u8 screen_id, u8 year)
{
    MusicId id;
    switch (screen_id) {
    case SCREEN_TITLE:      id = MUS_TITLE; break;
    case SCREEN_GAME:       id = (MusicId)(MUS_YEAR1 + year % 5); break;
    case SCREEN_EVENT:      id = MUS_EVENT; break;
    case SCREEN_ELECTION:   id = MUS_ELECTION; break;
    case SCREEN_REPORT:     id = MUS_REPORT; break;
    case SCREEN_END:        id = year == MUSIC_END_WIN ? MUS_END_WIN : MUS_END_LOSE; break;
    case SCREEN_SCORES:     id = MUS_SCORES; break;
    case SCREEN_HELP:       id = MUS_HELP; break;
    case SCREEN_IMPEACH:    id = MUS_IMPEACH; break;
    case SCREEN_CAMPAIGN:   id = MUS_CAMPAIGN; break;
    case SCREEN_MANDATE2:   id = MUS_MANDATE2; break;
    case SCREEN_BUDGET:     id = MUS_BUDGET; break;
    case SCREEN_REFERENDUM: id = MUS_REFERENDUM; break;
    case SCREEN_RESHUFFLE:  id = MUS_RESHUFFLE; break;
    case SCREEN_MAP:        id = MUS_MAP; break;
    default: return;
    }
    music_play(id);
}

void music_stop(void)
{
    s_id = 0xFF;
    s_pending = 0xFF;
    if (s_playing != 0xFF && !s_fading) {
        XGM2_fadeOutAndStop(ms_to_frames(STOP_MS));
        s_fading = TRUE;
    }
}

void music_set_volume(u8 cran)
{
    if (cran > 4)
        cran = 4;
    if (cran == 1 && !MUSIC_HAS_25)
        cran = 2;
    if (cran == 0) {
        music_set_enabled(FALSE);
        return;
    }
    s_fm = s_vol_fm[cran];
    s_psg = s_vol_psg[cran];
    // Mémorisés par XGM2 (fondus d'entrée et nouveaux morceaux les reprennent).
    XGM2_setFMVolume(s_fm);
    XGM2_setPSGVolume(s_psg);
    s_vol_pending = XGM2_isProcessingFade();
    music_set_enabled(TRUE);
}

void music_update(void)
{
    if (s_vol_pending && !XGM2_isProcessingFade()) {
        // La dernière image d'un fondu a réécrit son volume cible : on remet le réglage.
        s_vol_pending = FALSE;
        XGM2_setFMVolume(s_fm);
        XGM2_setPSGVolume(s_psg);
    }
    if (!s_fading) {
        // Morceau mis en attente pendant la voix de démarrage
        if (s_pending != 0xFF && s_playing == 0xFF && s_enabled && !voice_busy()) {
            u8 t = s_pending;
            s_pending = 0xFF;
            start_track(t);
        }
        return;
    }
    if (XGM2_isProcessingFade()) return;   // fondu traité par SGDK à chaque VBlank
    s_fading = FALSE;
    s_playing = 0xFF;                      // XGM2_stop() a été appelé à la fin du fondu
    if (s_pending != 0xFF && s_enabled) {
        u8 t = s_pending;
        s_pending = 0xFF;
        start_track(t);
    }
}

void music_set_enabled(bool on)
{
    if (on == s_enabled) return;
    s_enabled = on;
    if (!on) {
        s_pending = 0xFF;
        if (!s_fading && s_playing != 0xFF) {   // un fondu en cours finit seul par un arrêt
            XGM2_stop();
            s_playing = 0xFF;
        }
    } else if (s_id < MUS_COUNT) {
        request_track(s_track[s_id]);
    }
}
