// Mega Président — garde-fou de pile (-fstack-protector-strong, voir Makefile).
// Un tableau local qui déborde écrase la sentinelle placée par gcc avant l'adresse
// de retour : au lieu de sauter n'importe où (plantage V2 de la 4e année), le jeu
// s'arrête sur un message explicite.
#include <genesis.h>

__attribute__((externally_visible)) u32 __stack_chk_guard = 0x4D505632;   // « MPV2 »

__attribute__((externally_visible, noreturn)) void __stack_chk_fail(void)
{
    SYS_die("Debordement de pile (texte trop long)");
    while (1);
}
