// Mega Président — gestion des écrans (tâche 1.1). Voir screen.h.
#include "audio/music.h"
#include "audio/sfx.h"
#include "ui/screen.h"
#include "ui/input.h"
#include "ui/ui_internal.h"
#include "ui/ui_art.h"
#include "game/game.h"

#define NO_SCREEN 0xFF

static bool fading_out;

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

// Musique de l'écran (3.14) : thème de l'année pour le tableau de bord, issue de la
// partie pour l'écran de fin (victoire = deux mandats menés à terme sans destitution).
static void screen_music(u8 id)
{
    u8 arg = 0;
    if (id == SCREEN_GAME)
        arg = sim_year(&g_game.country);
    else if (id == SCREEN_END)
        arg = (!game_destitue() && game_two_mandates()) ? MUSIC_END_WIN : MUSIC_END_LOSE;
    music_for_screen(id, arg);
}

static void switch_screen(void)
{
    const Screen *s;

    if (current != NO_SCREEN && screens[current] && screens[current]->exit)
        screens[current]->exit();
    if (ui_msgbox_is_open())
        ui_msgbox_close();
    // Après le fondu au noir : illustrations libérées, palettes de l'interface rechargées.
    ui_art_close();
    ui_palettes_reset();
    ui_clear();
    current = pending;
    pending = NO_SCREEN;
    ticks = 0;
    s = screens[current];
    if (s && s->init)
        s->init();
    // Après init() : l'écran titre y lance la voix, que la musique laisse passer.
    screen_music(current);
}

void screen_run(ScreenId first)
{
    ui_init();
    input_init();
    sfx_init();
    music_init();
    pending = first;

    while (TRUE) {
        const Screen *s;

        if (pending != NO_SCREEN) {
            // Fondu au noir non bloquant (musique et bruitages continuent), puis
            // nouvel écran préparé affichage coupé, puis fondu d'apparition
            // (lancé par ui_frame_end à la fin de cette image).
            if (!fading_out && current != NO_SCREEN) {
                ui_fade_out_start();
                fading_out = TRUE;
            }
            if (fading_out && ui_fade_busy()) {
                input_update();
                sfx_update();
                music_update();
                SYS_doVBlankProcess();
                ticks += input_frame_ticks();
                continue;
            }
            fading_out = FALSE;
            ui_transition_hide();
            switch_screen();
        }
        input_update();
        s = screens[current];
        if (s && s->update)
            s->update();
        // Si update() a demandé un changement d'écran, on ne dessine pas l'ancien.
        if (pending == NO_SCREEN && s && s->draw)
            s->draw();
        ui_gueule_anim_update();   // visages animés (3.18)
        sfx_update();
        music_update();
        ui_frame_end();
        SYS_doVBlankProcess();
        ticks += input_frame_ticks();
    }
}
