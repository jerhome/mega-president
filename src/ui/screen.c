// Mega Président — gestion des écrans (tâche 1.1). Voir screen.h.
#include "ui/screen.h"
#include "ui/input.h"
#include "ui/ui.h"

#define NO_SCREEN 0xFF

static const Screen *screens[SCREEN_COUNT];
static u8 current = NO_SCREEN;
static u8 pending = NO_SCREEN;
static u32 ticks;   // 1/300 s depuis l'entrée dans l'écran courant

void screen_register(ScreenId id, const Screen *screen)
{
    if (id < SCREEN_COUNT)
        screens[id] = screen;
}

void screen_change(ScreenId id)
{
    if (id < SCREEN_COUNT)
        pending = id;
}

ScreenId screen_current(void)
{
    return (ScreenId)current;
}

u32 screen_time_ms(void)
{
    return (ticks * 10) / 3;
}

static void switch_screen(void)
{
    const Screen *s;

    if (current != NO_SCREEN && screens[current] && screens[current]->exit)
        screens[current]->exit();
    if (ui_msgbox_is_open())
        ui_msgbox_close();
    ui_clear();
    current = pending;
    pending = NO_SCREEN;
    ticks = 0;
    s = screens[current];
    if (s && s->init)
        s->init();
}

void screen_run(ScreenId first)
{
    ui_init();
    input_init();
    pending = first;

    while (TRUE) {
        const Screen *s;

        if (pending != NO_SCREEN)
            switch_screen();
        input_update();
        s = screens[current];
        if (s && s->update)
            s->update();
        // Si update() a demandé un changement d'écran, on ne dessine pas l'ancien.
        if (pending == NO_SCREEN && s && s->draw)
            s->draw();
        SYS_doVBlankProcess();
        ticks += input_frame_ticks();
    }
}
