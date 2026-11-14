// Mega Président — ROM de test de la phase 0 (tâche 0.4).
// Vérifie : affichage, détection PAL/NTSC, manette, sauvegarde SRAM,
// et le cœur de simulation (src/sim) compilé pour le 68000.

#include <genesis.h>
#include "sim/rng.h"

#define SRAM_MAGIC_0 'M'
#define SRAM_MAGIC_1 'P'
#define SRAM_MAGIC_2 '0'
#define SRAM_MAGIC_3 '1'
#define SRAM_OFS_COUNT 4    // compteur 16 bits (octets 4 et 5)

static u16 boot_count;

static u16 sram_load_and_count_boot(void)
{
    u16 count;

    SRAM_enable();
    if (SRAM_readByte(0) != SRAM_MAGIC_0 || SRAM_readByte(1) != SRAM_MAGIC_1 ||
        SRAM_readByte(2) != SRAM_MAGIC_2 || SRAM_readByte(3) != SRAM_MAGIC_3) {
        // SRAM vierge : on l'initialise.
        SRAM_writeByte(0, SRAM_MAGIC_0);
        SRAM_writeByte(1, SRAM_MAGIC_1);
        SRAM_writeByte(2, SRAM_MAGIC_2);
        SRAM_writeByte(3, SRAM_MAGIC_3);
        count = 0;
    } else {
        count = ((u16)SRAM_readByte(SRAM_OFS_COUNT) << 8) | SRAM_readByte(SRAM_OFS_COUNT + 1);
    }
    count++;
    SRAM_writeByte(SRAM_OFS_COUNT, count >> 8);
    SRAM_writeByte(SRAM_OFS_COUNT + 1, count & 0xFF);
    SRAM_disable();
    return count;
}

static void sram_reset(void)
{
    SRAM_enable();
    SRAM_writeByte(SRAM_OFS_COUNT, 0);
    SRAM_writeByte(SRAM_OFS_COUNT + 1, 0);
    SRAM_disable();
    boot_count = 0;
}

static void draw_u16(u16 v, u16 x, u16 y)
{
    char buf[8];
    uintToStr(v, buf, 5);
    VDP_drawText(buf, x, y);
}

static void draw_static(void)
{
    VDP_drawText("MEGA PRESIDENT", 13, 2);
    VDP_drawText("ROM DE TEST - PHASE 0", 9, 3);

    VDP_drawText("Video   :", 2, 6);
    VDP_drawText(IS_PAL_SYSTEM ? "PAL 50 Hz" : "NTSC 60 Hz", 12, 6);

    VDP_drawText("Demarrages sauvegardes :", 2, 8);

    VDP_drawText("Manette :", 2, 11);

    VDP_drawText("Tirage aleatoire (B) :", 2, 14);

    VDP_drawText("A : relire la SRAM", 2, 20);
    VDP_drawText("B : nouveau tirage", 2, 21);
    VDP_drawText("C : remettre le compteur a zero", 2, 22);
    VDP_drawText("Reset ou Start : relancer pour tester", 2, 24);
    VDP_drawText("la sauvegarde", 2, 25);
}

static void draw_buttons(u16 pad)
{
    static const char *names[] = { "HAUT", "BAS", "GAUCHE", "DROITE", "A", "B", "C", "START" };
    static const u16 masks[] = { BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT,
                                 BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_START };
    u16 x = 2, i;

    VDP_clearText(2, 12, 36);
    for (i = 0; i < 8; i++) {
        if (pad & masks[i]) {
            VDP_drawText(names[i], x, 12);
            x += strlen(names[i]) + 1;
        }
    }
}

int main(bool hard)
{
    Rng rng;
    u16 pad, prev = 0;

    boot_count = sram_load_and_count_boot();
    rng_seed(&rng, 0xC0FFEE);

    VDP_setScreenWidth320();
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));   // fond bleu nuit
    PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));  // texte blanc

    draw_static();
    draw_u16(boot_count, 27, 8);
    draw_u16(rng_below(&rng, 100), 25, 14);

    while (TRUE) {
        pad = JOY_readJoypad(JOY_1);
        if (pad != prev) {
            u16 pressed = pad & ~prev;
            draw_buttons(pad);

            if (pressed & BUTTON_A) {
                u16 c;
                SRAM_enableRO();
                c = ((u16)SRAM_readByte(SRAM_OFS_COUNT) << 8) | SRAM_readByte(SRAM_OFS_COUNT + 1);
                SRAM_disable();
                draw_u16(c, 27, 8);
            }
            if (pressed & BUTTON_B)
                draw_u16(rng_below(&rng, 100), 25, 14);
            if (pressed & BUTTON_C) {
                sram_reset();
                draw_u16(boot_count, 27, 8);
            }
            if (pressed & BUTTON_START)
                SYS_hardReset();
            prev = pad;
        }
        SYS_doVBlankProcess();
    }
    return 0;
}
