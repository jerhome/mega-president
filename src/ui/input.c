// Mega Président — manette (tâche 1.4). Voir input.h.
#include "ui/input.h"

// Temps compté en 1/300 s : une image dure exactement 6 (PAL) ou 5 (NTSC).
#define TICKS_PER_S 300
#define MS_TO_TICKS(ms) ((u16)((ms) * TICKS_PER_S / 1000))

static u16 held, pressed, repeat;
static u16 repeat_dir;     // direction en cours de répétition
static u16 repeat_timer;   // ticks avant la prochaine répétition
static u16 frame_ticks;

void input_init(void)
{
    held = pressed = repeat = repeat_dir = repeat_timer = 0;
    frame_ticks = IS_PAL_SYSTEM ? TICKS_PER_S / 50 : TICKS_PER_S / 60;
}

void input_update(void)
{
    u16 now = JOY_readJoypad(JOY_1);
    u16 dirs;

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
}

u16 input_held(void) { return held; }
u16 input_pressed(void) { return pressed; }
u16 input_repeat(void) { return repeat; }
u16 input_frame_ticks(void) { return frame_ticks; }
