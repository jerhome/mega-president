#include "audio/music.h"

// Séquenceur : chaque morceau a jusqu'à 3 pistes (ton, ton, bruit).
// Une note = {hauteur, durée en unités de tempo}. Hauteur : 12*octave + demi-ton
// (0 = silence), octave 0 = do0 (do4 = 48 = do du milieu du clavier). La piste de
// bruit traite toute hauteur non nulle comme un coup. Fin de piste = {0, 0} : retour au début.
// Les pistes d'un morceau ont la même durée totale, donc la boucle reste synchrone.
typedef struct {
    u8 pitch;
    u8 dur;
} MusNote;

typedef struct {
    const MusNote *seq;   // NULL = piste inutilisée
    u8 vol;               // atténuation PSG (0 = fort, 15 = muet)
    u8 hold;              // durée max sonore d'une note, en 10 ms (0 = toute la durée)
} MusTrack;

typedef struct {
    u16 unit_ms;
    MusTrack trk[3];
} MusSong;

// Noms de notes (demi-tons) ; bémol : si b = la# .
enum { kC, kCs, kD, kDs, kE, kF, kFs, kG, kGs, kA, kBb, kB };
#define NT(n, o, d) { (u8)((n) + 12 * (o)), (d) }
#define R(d)        { 0, (d) }
#define END         { 0, 0 }
#define HIT(d)      { 1, (d) }

// ---------------------------------------------------------------- MUS_TITLE
// Do majeur, marche à 4 temps (unité = croche, 250 ms), 16 mesures = 32 s.
static const MusNote t_mel[] = {
    NT(kE,4,3), NT(kG,4,1), NT(kC,5,4),
    NT(kB,4,2), NT(kA,4,2), NT(kG,4,4),
    NT(kD,5,3), NT(kB,4,1), NT(kG,4,2), NT(kD,4,2),
    NT(kG,4,6), R(2),
    NT(kC,5,3), NT(kA,4,1), NT(kE,5,4),
    NT(kD,5,2), NT(kC,5,2), NT(kA,4,4),
    NT(kA,4,3), NT(kC,5,1), NT(kF,5,2), NT(kE,5,2),
    NT(kD,5,6), R(2),
    NT(kG,4,2), NT(kC,5,2), NT(kE,5,2), NT(kG,5,2),
    NT(kE,5,3), NT(kD,5,1), NT(kB,4,4),
    NT(kA,4,2), NT(kC,5,2), NT(kF,5,4),
    NT(kE,5,2), NT(kD,5,2), NT(kC,5,4),
    NT(kA,4,3), NT(kC,5,1), NT(kA,4,2), NT(kF,4,2),
    NT(kB,4,2), NT(kD,5,2), NT(kG,5,4),
    NT(kE,5,4), NT(kC,5,2), NT(kG,4,2),
    NT(kC,5,6), R(2),
    END
};
#define BASS_B(r, ro, f, fo) NT(r,ro,2), NT(f,fo,2), NT(r,ro,2), NT(f,fo,2)
static const MusNote t_bass[] = {
    BASS_B(kC,3,kG,3), BASS_B(kC,3,kG,3), BASS_B(kG,3,kD,4), BASS_B(kG,3,kD,4),
    BASS_B(kA,3,kE,4), BASS_B(kA,3,kE,4), BASS_B(kF,3,kC,4), BASS_B(kG,3,kD,4),
    BASS_B(kC,3,kG,3), BASS_B(kE,3,kB,3), BASS_B(kF,3,kC,4), BASS_B(kC,3,kG,3),
    BASS_B(kF,3,kC,4), BASS_B(kG,3,kD,4), BASS_B(kC,3,kG,3), BASS_B(kC,3,kG,3),
    END
};
// Caisse claire très discrète sur les temps 2 et 4.
#define SNARE R(2), HIT(1), R(3), HIT(1), R(1)
static const MusNote t_perc[] = {
    SNARE, SNARE, SNARE, SNARE, SNARE, SNARE, SNARE, SNARE,
    SNARE, SNARE, SNARE, SNARE, SNARE, SNARE, SNARE, SNARE,
    END
};

// ------------------------------------------------------- MUS_YEAR1 à MUS_YEAR5
// Un thème par année du mandat (tableaux générés puis figés, 2 octets par note).
// 1 : sol majeur, allegretto, fanfare en arpège montant (état de grâce) ; contretemps de bruit.
// 2 : fa majeur, basse en notes répétées et caisse « horloge » (mise au travail).
// 3 : mi mineur, seconde mineure et syncopes (mi-mandat tendu).
// 4 : la mineur, lent, ligne descendante, simple battement (doute, usure).
// 5 : ré majeur, marche qui monte d'un ton toutes les 8 mesures, roulements (vers l'élection).
static const MusNote y1_mel[] = {
    NT(kG,4,2), NT(kB,4,2), NT(kD,5,4), NT(kB,4,2), NT(kG,4,2), NT(kD,5,4), NT(kA,4,2),
    NT(kC,5,1), NT(kD,5,1), NT(kFs,5,4), NT(kE,5,2), NT(kD,5,2), NT(kA,4,4), NT(kG,4,2),
    NT(kB,4,2), NT(kE,5,4), NT(kE,5,3), NT(kD,5,1), NT(kC,5,4), NT(kB,4,2), NT(kD,5,2),
    NT(kA,4,2), NT(kFs,4,2), NT(kD,5,6), R(2), NT(kG,5,2), NT(kD,5,2), NT(kB,4,2), NT(kG,4,2),
    NT(kE,5,2), NT(kG,5,2), NT(kB,4,2), NT(kG,4,2), NT(kC,5,2), NT(kE,5,2), NT(kG,5,4),
    NT(kFs,5,2), NT(kA,5,2), NT(kD,5,4), NT(kG,5,4), NT(kB,4,2), NT(kD,5,2), NT(kE,5,2),
    NT(kC,5,2), NT(kG,4,2), NT(kC,5,2), NT(kD,5,2), NT(kFs,5,2), NT(kA,5,2), NT(kFs,5,2),
    NT(kG,5,6), R(2), END
};
static const MusNote y1_bass[] = {
    NT(kG,3,2), NT(kD,4,2), NT(kG,3,2), NT(kD,4,2), NT(kG,3,2), NT(kD,4,2), NT(kG,3,2),
    NT(kD,4,2), NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2),
    NT(kD,3,2), NT(kA,3,2), NT(kE,3,2), NT(kB,3,2), NT(kE,3,2), NT(kB,3,2), NT(kC,3,2),
    NT(kG,3,2), NT(kC,3,2), NT(kG,3,2), NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2),
    NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2), NT(kG,3,2), NT(kD,4,2), NT(kG,3,2),
    NT(kD,4,2), NT(kE,3,2), NT(kB,3,2), NT(kE,3,2), NT(kB,3,2), NT(kC,3,2), NT(kG,3,2),
    NT(kC,3,2), NT(kG,3,2), NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2), NT(kG,3,2),
    NT(kD,4,2), NT(kG,3,2), NT(kD,4,2), NT(kC,3,2), NT(kG,3,2), NT(kC,3,2), NT(kG,3,2),
    NT(kD,3,2), NT(kA,3,2), NT(kD,3,2), NT(kA,3,2), NT(kG,3,2), NT(kD,4,2), NT(kG,3,2),
    NT(kD,4,2), END
};
static const MusNote y1_perc[] = {
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), END
};
static const MusNote y2_mel[] = {
    NT(kA,4,1), NT(kA,4,1), NT(kA,4,2), NT(kC,5,2), NT(kA,4,2), NT(kA,4,1), NT(kA,4,1),
    NT(kA,4,2), NT(kG,4,2), NT(kF,4,2), NT(kBb,4,1), NT(kBb,4,1), NT(kBb,4,2), NT(kD,5,2),
    NT(kBb,4,2), NT(kBb,4,1), NT(kBb,4,1), NT(kBb,4,2), NT(kA,4,2), NT(kF,4,2), NT(kG,4,1),
    NT(kG,4,1), NT(kG,4,2), NT(kBb,4,2), NT(kD,5,2), NT(kC,5,1), NT(kC,5,1), NT(kC,5,2),
    NT(kE,5,2), NT(kG,4,2), NT(kA,4,2), NT(kC,5,2), NT(kF,5,4), NT(kE,5,2), NT(kD,5,2),
    NT(kC,5,2), NT(kA,4,2), NT(kD,5,1), NT(kD,5,1), NT(kD,5,2), NT(kF,5,2), NT(kD,5,2),
    NT(kD,5,1), NT(kD,5,1), NT(kD,5,2), NT(kBb,4,2), NT(kG,4,2), NT(kE,5,1), NT(kE,5,1),
    NT(kE,5,2), NT(kG,5,2), NT(kE,5,2), NT(kC,5,2), NT(kE,5,2), NT(kG,5,2), NT(kE,5,2),
    NT(kF,5,2), NT(kA,5,2), NT(kC,6,4), NT(kD,5,2), NT(kF,5,2), NT(kBb,5,4), NT(kG,5,2),
    NT(kE,5,2), NT(kD,5,2), NT(kC,5,2), NT(kF,5,6), R(2), END
};
static const MusNote y2_bass[] = {
    NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kC,4,1),
    NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1),
    NT(kC,4,1), NT(kF,3,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1),
    NT(kBb,2,1), NT(kF,3,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1),
    NT(kBb,2,1), NT(kBb,2,1), NT(kF,3,1), NT(kBb,2,1), NT(kG,3,1), NT(kG,3,1), NT(kG,3,1),
    NT(kG,3,1), NT(kG,3,1), NT(kG,3,1), NT(kD,4,1), NT(kG,3,1), NT(kC,3,1), NT(kC,3,1),
    NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kG,3,1), NT(kC,3,1), NT(kF,3,1),
    NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kC,4,1), NT(kF,3,1),
    NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kC,4,1),
    NT(kF,3,1), NT(kD,3,1), NT(kD,3,1), NT(kD,3,1), NT(kD,3,1), NT(kD,3,1), NT(kD,3,1),
    NT(kA,3,1), NT(kD,3,1), NT(kG,3,1), NT(kG,3,1), NT(kG,3,1), NT(kG,3,1), NT(kG,3,1),
    NT(kG,3,1), NT(kD,4,1), NT(kG,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1),
    NT(kC,3,1), NT(kC,3,1), NT(kG,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1),
    NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kG,3,1), NT(kC,3,1), NT(kF,3,1), NT(kF,3,1),
    NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kC,4,1), NT(kF,3,1), NT(kBb,2,1),
    NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kBb,2,1), NT(kF,3,1), NT(kBb,2,1),
    NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kC,3,1), NT(kG,3,1),
    NT(kC,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1), NT(kF,3,1),
    NT(kC,4,1), NT(kF,3,1), END
};
static const MusNote y2_perc[] = {
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), END
};
static const MusNote y3_mel[] = {
    NT(kE,5,3), NT(kF,5,1), NT(kE,5,2), NT(kB,4,2), NT(kE,5,3), NT(kF,5,1), NT(kE,5,2),
    NT(kBb,4,2), NT(kF,5,3), NT(kG,5,1), NT(kF,5,2), NT(kC,5,2), NT(kE,5,2), NT(kD,5,2),
    NT(kB,4,4), NT(kA,4,3), NT(kB,4,1), NT(kC,5,2), NT(kE,5,2), NT(kC,5,3), NT(kB,4,1),
    NT(kA,4,2), NT(kE,5,2), NT(kDs,5,3), NT(kE,5,1), NT(kFs,5,2), NT(kDs,5,2), NT(kB,4,4), R(4),
    NT(kE,5,2), NT(kG,5,2), NT(kB,5,4), NT(kA,5,2), NT(kG,5,2), NT(kE,5,4), NT(kG,5,3),
    NT(kE,5,1), NT(kC,5,2), NT(kE,5,2), NT(kDs,5,2), NT(kFs,5,2), NT(kB,5,4), NT(kA,5,3),
    NT(kG,5,1), NT(kF,5,2), NT(kE,5,2), NT(kF,5,3), NT(kE,5,1), NT(kC,5,2), NT(kA,4,2),
    NT(kDs,5,2), NT(kFs,5,2), NT(kA,5,2), NT(kFs,5,2), NT(kB,5,2), R(2), NT(kB,4,1), NT(kB,4,1),
    NT(kB,4,2), END
};
static const MusNote y3_bass[] = {
    NT(kE,3,3), R(1), NT(kE,3,1), R(1), NT(kE,3,2), NT(kE,3,3), R(1), NT(kE,3,1), R(1),
    NT(kE,3,2), NT(kF,3,3), R(1), NT(kF,3,1), R(1), NT(kF,3,2), NT(kE,3,3), R(1), NT(kE,3,1),
    R(1), NT(kE,3,2), NT(kA,3,3), R(1), NT(kA,3,1), R(1), NT(kA,3,2), NT(kA,3,3), R(1),
    NT(kA,3,1), R(1), NT(kA,3,2), NT(kB,2,3), R(1), NT(kB,2,1), R(1), NT(kB,2,2), NT(kB,2,3),
    R(1), NT(kB,2,1), R(1), NT(kB,2,2), NT(kE,3,3), R(1), NT(kE,3,1), R(1), NT(kE,3,2),
    NT(kE,3,3), R(1), NT(kE,3,1), R(1), NT(kE,3,2), NT(kC,3,3), R(1), NT(kC,3,1), R(1),
    NT(kC,3,2), NT(kB,2,3), R(1), NT(kB,2,1), R(1), NT(kB,2,2), NT(kA,3,3), R(1), NT(kA,3,1),
    R(1), NT(kA,3,2), NT(kF,3,3), R(1), NT(kF,3,1), R(1), NT(kF,3,2), NT(kB,2,3), R(1),
    NT(kB,2,1), R(1), NT(kB,2,2), NT(kB,2,3), R(1), NT(kB,2,1), R(1), NT(kB,2,2), END
};
static const MusNote y3_perc[] = {
    R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1),
    R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1),
    HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1),
    R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2),
    HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1),
    R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1),
    HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1),
    R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1),
    R(1), R(1), HIT(1), R(2), HIT(1), R(1), HIT(1), R(1), END
};
static const MusNote y4_mel[] = {
    NT(kE,5,4), NT(kD,5,2), NT(kC,5,2), NT(kC,5,4), NT(kA,4,2), NT(kF,4,2), NT(kD,5,4),
    NT(kC,5,2), NT(kA,4,2), NT(kB,4,3), NT(kGs,4,1), NT(kE,4,4), NT(kA,4,2), NT(kC,5,2),
    NT(kE,5,4), NT(kF,5,4), NT(kE,5,2), NT(kC,5,2), NT(kD,5,4), NT(kB,4,2), NT(kGs,4,2),
    NT(kA,4,6), R(2), END
};
static const MusNote y4_bass[] = {
    NT(kA,2,4), NT(kE,3,4), NT(kF,3,4), NT(kC,4,4), NT(kD,3,4), NT(kA,3,4), NT(kE,3,4),
    NT(kB,3,4), NT(kA,2,4), NT(kE,3,4), NT(kF,3,4), NT(kC,4,4), NT(kD,3,4), NT(kA,3,4),
    NT(kA,2,4), NT(kE,3,4), END
};
static const MusNote y4_perc[] = {
    HIT(1), R(1), HIT(1), R(5), HIT(1), R(1), HIT(1), R(5), HIT(1), R(1), HIT(1), R(5), HIT(1),
    R(1), HIT(1), R(5), HIT(1), R(1), HIT(1), R(5), HIT(1), R(1), HIT(1), R(5), HIT(1), R(1),
    HIT(1), R(5), HIT(1), R(1), HIT(1), R(5), END
};
static const MusNote y5_mel[] = {
    NT(kD,5,2), NT(kD,5,1), NT(kD,5,1), NT(kFs,5,2), NT(kA,5,2), NT(kA,5,2), NT(kG,5,1),
    NT(kFs,5,1), NT(kE,5,4), NT(kFs,5,2), NT(kFs,5,1), NT(kFs,5,1), NT(kD,5,2), NT(kB,4,2),
    NT(kB,4,2), NT(kD,5,2), NT(kG,5,4), NT(kFs,5,2), NT(kA,5,2), NT(kD,6,4), NT(kB,5,2),
    NT(kG,5,2), NT(kD,5,2), NT(kB,4,2), NT(kE,5,2), NT(kA,5,2), NT(kCs,6,2), NT(kA,5,2),
    NT(kA,5,6), R(2), NT(kE,5,2), NT(kE,5,1), NT(kE,5,1), NT(kGs,5,2), NT(kB,5,2), NT(kB,5,2),
    NT(kA,5,1), NT(kGs,5,1), NT(kFs,5,4), NT(kGs,5,2), NT(kGs,5,1), NT(kGs,5,1), NT(kE,5,2),
    NT(kCs,5,2), NT(kCs,5,2), NT(kE,5,2), NT(kA,5,4), NT(kGs,5,2), NT(kB,5,2), NT(kE,6,4),
    NT(kCs,6,2), NT(kA,5,2), NT(kE,5,2), NT(kCs,5,2), NT(kFs,5,2), NT(kB,5,2), NT(kDs,6,2),
    NT(kB,5,2), NT(kB,5,6), R(2), NT(kFs,5,2), NT(kFs,5,1), NT(kFs,5,1), NT(kBb,5,2), NT(kCs,6,2),
    NT(kCs,6,2), NT(kB,5,1), NT(kBb,5,1), NT(kGs,5,4), NT(kBb,5,2), NT(kBb,5,1), NT(kBb,5,1),
    NT(kFs,5,2), NT(kDs,5,2), NT(kDs,5,2), NT(kFs,5,2), NT(kB,5,4), NT(kBb,5,2), NT(kCs,6,2),
    NT(kFs,6,4), NT(kDs,6,2), NT(kB,5,2), NT(kFs,5,2), NT(kDs,5,2), NT(kGs,5,2), NT(kCs,6,2),
    NT(kF,6,2), NT(kCs,6,2), NT(kCs,6,6), R(2), END
};
static const MusNote y5_bass[] = {
    NT(kD,3,2), NT(kD,3,1), NT(kA,3,1), NT(kD,3,2), NT(kD,3,1), NT(kA,3,1), NT(kA,2,2),
    NT(kA,2,1), NT(kE,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kB,2,2), NT(kB,2,1),
    NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kG,2,2), NT(kG,2,1), NT(kD,3,1),
    NT(kG,2,2), NT(kG,2,1), NT(kD,3,1), NT(kD,3,2), NT(kD,3,1), NT(kA,3,1), NT(kD,3,2),
    NT(kD,3,1), NT(kA,3,1), NT(kG,2,2), NT(kG,2,1), NT(kD,3,1), NT(kG,2,2), NT(kG,2,1),
    NT(kD,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1),
    NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kE,3,2),
    NT(kE,3,1), NT(kB,3,1), NT(kE,3,2), NT(kE,3,1), NT(kB,3,1), NT(kB,2,2), NT(kB,2,1),
    NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1),
    NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kA,2,2),
    NT(kA,2,1), NT(kE,3,1), NT(kE,3,2), NT(kE,3,1), NT(kB,3,1), NT(kE,3,2), NT(kE,3,1),
    NT(kB,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1), NT(kA,2,2), NT(kA,2,1), NT(kE,3,1),
    NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kB,2,2),
    NT(kB,2,1), NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kFs,3,2), NT(kFs,3,1),
    NT(kCs,4,1), NT(kFs,3,2), NT(kFs,3,1), NT(kCs,4,1), NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1),
    NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1), NT(kDs,3,2), NT(kDs,3,1), NT(kBb,3,1), NT(kDs,3,2),
    NT(kDs,3,1), NT(kBb,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1),
    NT(kFs,3,1), NT(kFs,3,2), NT(kFs,3,1), NT(kCs,4,1), NT(kFs,3,2), NT(kFs,3,1), NT(kCs,4,1),
    NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kB,2,2), NT(kB,2,1), NT(kFs,3,1), NT(kCs,3,2),
    NT(kCs,3,1), NT(kGs,3,1), NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1), NT(kCs,3,2), NT(kCs,3,1),
    NT(kGs,3,1), NT(kCs,3,2), NT(kCs,3,1), NT(kGs,3,1), END
};
static const MusNote y5_perc[] = {
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1),
    HIT(1), HIT(1), R(1), HIT(1), R(1), HIT(1), R(1), HIT(1), HIT(1), HIT(1), R(1), HIT(1), R(1),
    HIT(1), R(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), END
};



// ------------------------------------------------------------- MUS_ELECTION
// Mesures 1-4 : pédale de mi, seconde mineure ; 5-8 : montée chromatique et
// roulement ; 9-11 : dominante ; 12 : silence ; 13-20 : résolution en do majeur.
// Unité = croche, 200 ms, 20 mesures = 32 s.
#define PULSE(n, o) NT(n,o,1), R(1), NT(n,o,1), R(1), NT(n,o,1), R(1), NT(n,o,1), R(1)
static const MusNote e_mel[] = {
    NT(kB,4,8), NT(kC,5,8), NT(kB,4,4), NT(kF,5,4), NT(kBb,4,8),
    NT(kE,5,4), NT(kF,5,4), NT(kF,5,4), NT(kFs,5,4),
    NT(kG,5,4), NT(kGs,5,4), NT(kA,5,8),
    NT(kD,5,4), NT(kB,4,4), NT(kG,5,4), NT(kF,5,4),
    NT(kD,5,2), NT(kF,5,2), NT(kG,5,2), NT(kB,5,2),
    R(8),
    NT(kG,5,3), NT(kE,5,1), NT(kC,5,4),
    NT(kA,4,4), NT(kC,5,2), NT(kF,5,2),
    NT(kD,5,4), NT(kB,4,2), NT(kG,4,2),
    NT(kC,5,8),
    NT(kA,4,2), NT(kC,5,2), NT(kF,5,4),
    NT(kG,5,4), NT(kD,5,4),
    NT(kE,5,4), NT(kG,5,2), NT(kC,6,2),
    NT(kC,5,6), R(2),
    END
};
static const MusNote e_bass[] = {
    PULSE(kE,3), PULSE(kE,3), PULSE(kE,3), PULSE(kE,3),
    PULSE(kF,3), PULSE(kF,3), PULSE(kFs,3), PULSE(kG,3),
    PULSE(kG,3), PULSE(kG,3), PULSE(kG,3), R(8),
    NT(kC,3,4), NT(kG,3,4), NT(kF,3,4), NT(kC,4,4),
    NT(kG,3,4), NT(kD,4,4), NT(kC,3,8),
    NT(kF,3,4), NT(kC,4,4), NT(kG,3,4), NT(kD,4,4),
    NT(kC,3,4), NT(kG,3,4), NT(kC,3,6), R(2),
    END
};
#define ROLL HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1), HIT(1)
#define SOFT HIT(1), R(3), HIT(1), R(3)
static const MusNote e_perc[] = {
    R(8), R(8), R(8), R(8),
    ROLL, ROLL, ROLL, ROLL,
    ROLL, ROLL, ROLL, R(8),
    SOFT, SOFT, SOFT, SOFT, SOFT, SOFT, SOFT, R(8),
    END
};

static const MusSong s_songs[MUS_COUNT] = {
    { 250, { { t_mel, 6, 0 }, { t_bass, 8, 0 }, { t_perc, 12, 4 } } },
    { 200, { { y1_mel, 8, 0 }, { y1_bass, 10, 0 }, { y1_perc, 13, 3 } } },
    { 230, { { y2_mel, 8, 0 }, { y2_bass, 10, 12 }, { y2_perc, 13, 2 } } },
    { 210, { { y3_mel, 8, 0 }, { y3_bass, 10, 14 }, { y3_perc, 12, 3 } } },
    { 450, { { y4_mel, 9, 0 }, { y4_bass, 11, 0 }, { y4_perc, 13, 5 } } },
    { 160, { { y5_mel, 7, 0 }, { y5_bass, 9, 10 }, { y5_perc, 11, 3 } } },
    { 200, { { e_mel, 6, 0 }, { e_bass, 8, 12 }, { e_perc, 12, 4 } } },
};

// Fréquences de l'octave 0 multipliées par 16 (= octave 4 en Hz).
static const u16 s_freq16[12] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };

static const MusSong *s_song;       // NULL = arrêt
static const MusNote *s_cur[3];
static s32 s_left[3];               // temps restant de la note, en 1/10 ms
static s32 s_snd[3];                // temps restant avant la coupure de la note
static bool s_sounding[3];
static s32 s_frame_x10;
static bool s_enabled;
static u8 s_id;

static void silence(u8 t)
{
    PSG_setEnvelope((u8)(t + 1), PSG_ENVELOPE_MIN);
    s_sounding[t] = FALSE;
}

static void start_note(u8 t)
{
    const MusTrack *tr = &s_song->trk[t];
    const MusNote *n = s_cur[t];
    if (n->dur == 0) { n = s_cur[t] = tr->seq; }
    s32 carry = s_left[t];   // <= 0 : temps déjà écoulé sur la note précédente
    s32 dur_ms = (s32)n->dur * s_song->unit_ms;
    s32 gap_ms = dur_ms >= 140 ? 35 : dur_ms / 4;   // petit silence entre deux notes
    s_left[t] = carry + dur_ms * 10;
    s_snd[t] = carry + (dur_ms - gap_ms) * 10;
    if (tr->hold && s_snd[t] > carry + (s32)tr->hold * 100) s_snd[t] = carry + (s32)tr->hold * 100;
    if (n->pitch == 0) { silence(t); return; }
    if (t == 2) {
        PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK4);
    } else {
        u8 p = n->pitch;
        u32 f = ((u32)s_freq16[p % 12] << (p / 12)) >> 4;
        if (f < 110) f = 110;     // limite basse du PSG (10 bits)
        PSG_setFrequency((u8)(t + 1), (u16)f);
    }
    PSG_setEnvelope((u8)(t + 1), tr->vol);
    s_sounding[t] = TRUE;
}

void music_init(void)
{
    s_song = NULL;
    s_enabled = TRUE;
    s_id = 0xFF;
    s_frame_x10 = IS_PAL_SYSTEM ? 200 : 167;
    for (u8 t = 0; t < 3; t++) {
        s_sounding[t] = FALSE;
        PSG_setEnvelope((u8)(t + 1), PSG_ENVELOPE_MIN);
    }
}

void music_stop(void)
{
    s_song = NULL;
    s_id = 0xFF;
    for (u8 t = 0; t < 3; t++) {
        PSG_setEnvelope((u8)(t + 1), PSG_ENVELOPE_MIN);
        s_sounding[t] = FALSE;
    }
}

void music_play(MusicId id)
{
    if (id >= MUS_COUNT) return;
    if (s_enabled && s_song && s_id == (u8)id) return;
    music_stop();
    s_id = (u8)id;
    if (!s_enabled) return;   // mémorisé : joué si la musique est réactivée
    s_song = &s_songs[id];
    for (u8 t = 0; t < 3; t++) {
        if (!s_song->trk[t].seq) continue;
        s_cur[t] = s_song->trk[t].seq;
        s_left[t] = 0;
        start_note(t);
    }
}

void music_update(void)
{
    if (!s_song) return;
    for (u8 t = 0; t < 3; t++) {
        if (!s_song->trk[t].seq) continue;
        s_left[t] -= s_frame_x10;
        s_snd[t] -= s_frame_x10;
        if (s_sounding[t] && s_snd[t] <= 0) silence(t);
        while (s_left[t] <= 0) {
            s_cur[t]++;
            start_note(t);
        }
    }
}

void music_play_year(u8 year)
{
    // Mandat de 5 ans : le 2nd mandat reprend les mêmes 5 thèmes. Sans effet si le thème joue déjà.
    music_play((MusicId)(MUS_YEAR1 + year % 5));
}

void music_set_enabled(bool on)
{
    if (on == s_enabled) return;
    s_enabled = on;
    if (!on) {
        u8 id = s_id;
        music_stop();
        s_id = id;            // mémorisé pour la réactivation
    } else if (s_id < MUS_COUNT) {
        u8 id = s_id;
        s_id = 0xFF;
        music_play((MusicId)id);
    }
}
