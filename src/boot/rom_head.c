#include "genesis.h"

// En-tête de cartouche Mega Président.
// SRAM : sauvegarde sur batterie, octets impairs, 0x200001-0x20FFFF
// (format attendu par l'EverDrive MD V3 et les émulateurs).
__attribute__((externally_visible))
const ROMHeader rom_header = {
    "SEGA MEGA DRIVE ",
    "(C)JERH 2026.NOV",
    "MEGA PRESIDENT                                  ",
    "MEGA PRESIDENT                                  ",
    "GM 00000001-00",
    0x000,
    "J               ",
    0x00000000,
    0x000FFFFF,
    0xE0FF0000,
    0xE0FFFFFF,
    "RA",
    0xF820,
    0x00200001,
    0x0020FFFF,
    "            ",
    "JEU DE GESTION PRESIDENTIELLE           ",
    "JUE             "
};
