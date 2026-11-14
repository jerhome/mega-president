#include "audio/sfx.h"

// Un son = suite de notes {fréquence Hz (0 = silence), volume 0-15 (15 = muet), durée ms}
// terminée par une durée nulle. Volume PSG : 0 = max ; on reste autour de 4-8.
typedef struct {
    u16 freq;
    u8 vol;
    u16 ms;
} SfxNote;

#define CH 0
#define END {0, 15, 0}

static const SfxNote s_move[]   = { {1200, 9, 25}, END };
static const SfxNote s_ok[]     = { {880, 5, 50}, {1320, 5, 60}, {1320, 9, 10}, END };
static const SfxNote s_cancel[] = { {880, 6, 50}, {587, 6, 60}, {587, 10, 10}, END };
static const SfxNote s_error[]  = { {196, 4, 80}, {0, 15, 30}, {196, 4, 80}, END };
static const SfxNote s_adopted[] = {
    {523, 5, 90}, {659, 5, 90}, {784, 5, 90}, {1047, 5, 90}, {1047, 9, 40}, END };
static const SfxNote s_rejected[] = {
    {784, 6, 90}, {622, 6, 90}, {523, 6, 90}, {392, 6, 90}, {392, 10, 40}, END };
static const SfxNote s_event[] = {
    {988, 5, 70}, {0, 15, 40}, {988, 5, 70}, {1319, 5, 120}, END };
static const SfxNote s_win[] = {
    {523, 4, 120}, {523, 4, 120}, {523, 4, 120}, {659, 4, 200},
    {523, 5, 120}, {659, 4, 120}, {784, 4, 300}, {784, 8, 40}, END };
static const SfxNote s_lose[] = {
    {440, 5, 220}, {392, 5, 220}, {349, 5, 220}, {330, 6, 330}, {330, 10, 60}, END };

static const SfxNote *const s_sounds[SFX_COUNT] = {
    s_move, s_ok, s_cancel, s_error, s_adopted, s_rejected, s_event, s_win, s_lose
};

static const SfxNote *s_cur;   // note courante, NULL si silence
static u8 s_id;
static u16 s_left_x10;         // temps restant de la note, en 1/10 ms
static u16 s_frame_x10;        // durée d'une image, en 1/10 ms
static bool s_enabled;

static void start_note(void)
{
    if (s_cur->ms == 0) {
        PSG_setEnvelope(CH, PSG_ENVELOPE_MIN);
        s_cur = NULL;
        return;
    }
    if (s_cur->freq) PSG_setFrequency(CH, s_cur->freq);
    PSG_setEnvelope(CH, s_cur->vol);
    s_left_x10 = (u16)(s_cur->ms * 10);
}

void sfx_init(void)
{
    PSG_reset();
    PSG_setEnvelope(CH, PSG_ENVELOPE_MIN);
    s_cur = NULL;
    s_enabled = TRUE;
    s_frame_x10 = IS_PAL_SYSTEM ? 200 : 167;
}

void sfx_play(SfxId id)
{
    if (!s_enabled || id >= SFX_COUNT) return;
    // Une élection en cours n'est pas coupée par le simple déplacement du curseur.
    if (s_cur && id == SFX_MOVE && (s_id == SFX_ELECTION_WIN || s_id == SFX_ELECTION_LOSE)) return;
    s_id = (u8)id;
    s_cur = s_sounds[id];
    start_note();
}

void sfx_update(void)
{
    if (!s_cur) return;
    if (s_left_x10 > s_frame_x10) {
        s_left_x10 -= s_frame_x10;
        return;
    }
    s_cur++;
    start_note();
}

void sfx_set_enabled(bool on)
{
    s_enabled = on;
    if (!on) {
        PSG_setEnvelope(CH, PSG_ENVELOPE_MIN);
        s_cur = NULL;
    }
}
