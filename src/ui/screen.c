// Mega Président — gestion des écrans (tâche 1.1). Voir screen.h.
#include "audio/music.h"
#include "audio/sfx.h"
#include "audio/voice.h"
#include "ui/screen.h"
#include "ui/input.h"
#include "ui/ui_internal.h"
#include "ui/ui_art.h"

#define NO_SCREEN 0xFF

static bool fading_out;

static const Screen *screens[SCREEN_COUNT];
static u8 current = NO_SCREEN;
static u8 pending = NO_SCREEN;
static u32 enter_tick;   // getTick() (1/300 s) à l'entrée dans l'écran courant

#ifdef MP_DEBUG_PERF
// Mesure de performance (ROM `make rom-perf` seulement, docs/AUDIT_V4_1.md § 5).
// Durée d'update + draw de chaque image, en 1/76800 s (getSubTick : getTick() n'a
// que la résolution d'une image) ; images perdues d'après l'écart de getTick()
// entre deux tours de boucle. Remis à zéro à chaque changement d'écran.
// La PREMIÈRE image d'un écran (dessin complet, affichage encore coupé par
// ui_transition_hide) est comptée à part (E, en images) : pendant ce temps l'écran est
// noir, getSubTick() n'y est pas fiable et la perte n'est pas visible. M et L ne
// couvrent que les images suivantes (affichage allumé). P = maximum d'update + draw
// compté en images entières d'après getTick() (le compteur de VBlank) : il voit les
// dessins qui s'étalent sur plusieurs images, que M (getSubTick) sous-estime.
static u32 perf_max;    // maximum d'update + draw (1/76800 s)
static u16 perf_lost;   // images perdues sur l'écran courant
static u32 perf_last;
static u16 perf_entry;  // images de la première image de l'écran (affichage coupé)
static u8 perf_first;   // TRUE : la prochaine image est la première de l'écran
static u16 perf_p;      // maximum d'update + draw en images entières (getTick)

static void perf_reset(void)
{
    perf_max = 0;
    perf_lost = 0;
    perf_last = 0;
    perf_entry = 0;
    perf_first = TRUE;
    perf_p = 0;
}

static u16 perf_put(char *p, u32 v)   // entier décimal, retourne la longueur
{
    char t[10];
    u16 n = 0, i = 0;
    do { t[n++] = (char)('0' + v % 10); v /= 10; } while (v && n < 9);
    while (n) p[i++] = t[--n];
    return i;
}

static void perf_show(void)
{
    char b[32];
    u16 n = 0;
    u32 d = (perf_max * 10) / 768;   // dixièmes de milliseconde
    b[n++] = 'M';
    n += perf_put(b + n, d / 10);
    b[n++] = '.';
    b[n++] = (char)('0' + d % 10);
    b[n++] = ' ';
    b[n++] = 'L';
    n += perf_put(b + n, perf_lost);
    b[n++] = ' ';
    b[n++] = 'E';
    n += perf_put(b + n, perf_entry);
    b[n++] = ' ';
    b[n++] = 'P';
    n += perf_put(b + n, perf_p);
    b[n] = 0;
    ui_text_draw_n(14, 27, b, 24, 24, UI_STYLE_ALERT);
}
#endif

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
    return ((getTick() - enter_tick) * 10) / 3;
}

static void (*enter_hook)(ScreenId id);

void screen_set_enter_hook(void (*hook)(ScreenId id))
{
    enter_hook = hook;
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
    enter_tick = getTick();
#ifdef MP_DEBUG_PERF
    perf_reset();
#endif
    s = screens[current];
    if (s && s->init)
        s->init();
    // Après init() : l'écran titre y lance la voix, que la musique laisse passer
    // (le rappel est celui de main.c : musique de l'écran).
    if (enter_hook)
        enter_hook((ScreenId)current);
}

void screen_run(ScreenId first)
{
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
                continue;
            }
            fading_out = FALSE;
            ui_transition_hide();
            switch_screen();
        }
#ifdef MP_DEBUG_PERF
        u32 perf_t0;
        {
            u32 now = getTick() / (IS_PAL_SYSTEM ? 6 : 5);   // images écoulées
            if (perf_last && now > perf_last + 1)
                perf_lost += (u16)(now - perf_last - 1);
            perf_last = now;
        }
        perf_t0 = getSubTick();
        u32 perf_tick0 = getTick();
#endif
        input_update();
        s = screens[current];
        if (s && s->update)
            s->update();
        // Si update() a demandé un changement d'écran, on ne dessine pas l'ancien.
        if (pending == NO_SCREEN && s && s->draw)
            s->draw();
#ifdef MP_DEBUG_PERF
        {
            u32 d = getSubTick() - perf_t0;
            if (!perf_first && (getTick() - perf_tick0) / (IS_PAL_SYSTEM ? 6 : 5) > perf_p)
                perf_p = (u16)((getTick() - perf_tick0) / (IS_PAL_SYSTEM ? 6 : 5));
            if (!perf_first && d > perf_max && d < 76800)
                perf_max = d;
            if (pending == NO_SCREEN)
                perf_show();
        }
#endif
        voice_update();            // charabia (4.18)
        ui_gueule_anim_update();   // visages animés (3.18)
        sfx_update();
        music_update();
        ui_frame_end();
        SYS_doVBlankProcess();
#ifdef MP_DEBUG_PERF
        if (perf_first) {
            perf_first = FALSE;
            perf_last = 0;   // le décompte des images perdues commence à l'image suivante
            perf_entry = (u16)((getTick() - perf_tick0) / (IS_PAL_SYSTEM ? 6 : 5));
        }
#endif
    }
}
