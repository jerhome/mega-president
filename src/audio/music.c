#include "audio/music.h"
#include "audio/music_tracks.h"
#include "snd_audio.h"
#include "ui/screen.h"

// Pistes de la ressource XGM2 pour chaque morceau. KEEP : pas encore composé,
// on garde le morceau en cours (aucun changement de musique).
#define KEEP 0xFF
static const u8 s_track[MUS_COUNT] = {
    [MUS_TITLE]      = TRACK_TITLE,
    [MUS_YEAR1]      = TRACK_YEAR1,
    [MUS_YEAR2]      = TRACK_V2_YEAR2,
    [MUS_YEAR3]      = TRACK_V2_YEAR3,
    [MUS_YEAR4]      = TRACK_V2_YEAR4,
    [MUS_YEAR5]      = TRACK_V2_YEAR5,
    [MUS_ELECTION]   = TRACK_V2_ELECTION,
    [MUS_REPORT]     = TRACK_REPORT,
    [MUS_EVENT]      = KEEP,
    [MUS_ASSEMBLY]   = KEEP,
    [MUS_MAP]        = KEEP,
    [MUS_BUDGET]     = KEEP,
    [MUS_REFERENDUM] = KEEP,
    [MUS_RESHUFFLE]  = KEEP,
    [MUS_IMPEACH]    = KEEP,
    [MUS_CAMPAIGN]   = KEEP,
    [MUS_MANDATE2]   = TRACK_YEAR1,
    [MUS_END_WIN]    = TRACK_TITLE,
    [MUS_END_LOSE]   = TRACK_TITLE,
    [MUS_SCORES]     = TRACK_TITLE,
    [MUS_HELP]       = KEEP,
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
    if (track == KEEP) return;
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

void music_update(void)
{
    if (!s_fading) return;
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
