// Mega Président — manette (tâche 1.4). Voir input.h.
#include "ui/input.h"
#include "audio/sfx.h"

// Temps compté en 1/300 s : une image dure exactement 6 (PAL) ou 5 (NTSC).
#define TICKS_PER_S 300
#define MS_TO_TICKS(ms) ((u16)((ms) * TICKS_PER_S / 1000))

static u16 held, pressed, repeat;
static u16 repeat_dir;     // direction en cours de répétition
static u16 repeat_timer;   // ticks avant la prochaine répétition
static u16 frame_ticks;
static u8 blocked;         // fondu en cours (input_block)
static u16 deferred;       // fronts mis de côté pendant un blocage INPUT_BLOCK_DEFER

void input_init(void)
{
    held = pressed = repeat = repeat_dir = repeat_timer = 0;
    frame_ticks = IS_PAL_SYSTEM ? TICKS_PER_S / 50 : TICKS_PER_S / 60;
}

#ifdef MP_DEBUG_SCRIPT
// ROM de démonstration seulement (captures d'écran sans clavier) : manette
// simulée par une chaîne « w15 S w6 A D A ». wN = attendre N x 100 ms ;
// S A B C U D L R = appui de 100 ms suivi de 200 ms relâché.
static const char *script_p = MP_DEBUG_SCRIPT;
static s16 script_timer;
static u16 script_btn;
static u8 script_gap;

static u16 script_read(void)
{
    static const char keys[] = "SABCUDLR";
    static const u16 btn[] = { INPUT_START, INPUT_A, INPUT_B, INPUT_C, INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT };

    if (script_timer > 0) {
        script_timer -= frame_ticks;
        return script_btn;
    }
    if (script_gap) {
        script_gap = 0;
        script_btn = 0;
        script_timer = 60;
        return 0;
    }
    script_btn = 0;
    while (*script_p == ' ')
        script_p++;
    if (*script_p == 'w') {
        u16 n = 0;
        script_p++;
        while (*script_p >= '0' && *script_p <= '9')
            n = n * 10 + (u16)(*script_p++ - '0');
        script_timer = (s16)(n * 30);
    } else if (*script_p) {
        for (u16 i = 0; keys[i]; i++)
            if (keys[i] == *script_p)
                script_btn = btn[i];
        script_p++;
        script_timer = 30;
        script_gap = 1;
    }
    return script_btn;
}
#endif

void input_update(void)
{
    u16 now = JOY_readJoypad(JOY_1);
    u16 dirs;

#ifdef MP_DEBUG_SCRIPT
    now |= script_read();
#endif

    pressed = now & ~held;
    held = now;
    repeat = pressed;

    dirs = now & INPUT_DIRS;
    if (pressed & INPUT_DIRS) {
        // Nouvelle direction : on repart du délai initial.
        repeat_dir = pressed & INPUT_DIRS;
        repeat_timer = MS_TO_TICKS(INPUT_REPEAT_DELAY_MS);
    } else if (repeat_dir && (dirs & repeat_dir)) {
        repeat_dir &= dirs;
        if (repeat_timer > frame_ticks) {
            repeat_timer -= frame_ticks;
        } else {
            repeat |= repeat_dir;
            repeat_timer += MS_TO_TICKS(INPUT_REPEAT_RATE_MS) - frame_ticks;
        }
    } else {
        repeat_dir = 0;
    }
    if (blocked) {
        if (blocked == INPUT_BLOCK_DEFER)
            deferred |= pressed;
        pressed = 0;
        repeat = 0;
    } else if (deferred) {
        // Fronts survenus pendant une apparition douce : rendus à la fin.
        pressed |= deferred;
        repeat |= deferred;
        deferred = 0;
    }
}


void input_block(u8 mode)
{
    if (mode == INPUT_BLOCK_DISCARD)
        deferred = 0;
    blocked = mode;
}

u16 input_held(void) { return held; }
u16 input_pressed(void) { return pressed; }
u16 input_repeat(void) { return repeat; }
u16 input_frame_ticks(void) { return frame_ticks; }

bool input_go(void)
{
    if (!(pressed & INPUT_OK))
        return FALSE;
    sfx_play(SFX_OK);
    return TRUE;
}

bool input_back(void)
{
    if (!(pressed & INPUT_CANCEL))
        return FALSE;
    sfx_play(SFX_CANCEL);
    return TRUE;
}
