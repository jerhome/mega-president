"""Banque d'instruments FM YM2612 de Mega Président (V3).

Un instrument = algorithme, feedback, sensibilités LFO et 4 opérateurs décrits
dans l'ordre logique S1..S4 de la documentation Yamaha (l'ordre des registres
du YM2612 est S1, S3, S2, S4 : voir SLOT_OFFSET).

Repères pour régler les opérateurs (YM2612, horloge 7,67 MHz) :
- TL : 0,75 dB par pas. Pour un modulateur, indice de modulation ≈ 25 × 10^(-TL/26,7) rad
  (TL 28 ≈ 2,2 rad, brillant ; TL 36 ≈ 1 rad, chaud ; TL 45 ≈ 0,5 rad, discret).
- D1R / D2R : 6 ≈ 6 dB/s, 8 ≈ 13 dB/s, 10 ≈ 26 dB/s, 12 ≈ 52 dB/s, 15 ≈ 150 dB/s.
- RR : 7 ≈ 10 dB/s (longue queue), 9 ≈ 20 dB/s, 11 ≈ 40 dB/s, 13 ≈ 90 dB/s.
- AR : 31 instantané, 20 ≈ 40 ms, 16 ≈ 150 ms, 13 ≈ 400 ms (nappe).
- SL (D1L) : 3 dB par pas.
"""

SLOT_OFFSET = (0, 8, 4, 12)   # S1, S2, S3, S4 -> décalage de registre

# Opérateurs porteurs (audibles) pour chaque algorithme (indices 0..3 = S1..S4)
CARRIERS = {
    0: (3,), 1: (3,), 2: (3,), 3: (3,),
    4: (1, 3), 5: (1, 2, 3), 6: (1, 2, 3), 7: (0, 1, 2, 3),
}


def op(mul=1, dt=0, tl=127, ar=31, d1r=0, d2r=0, sl=0, rr=8, ks=0, am=0):
    """Paramètres d'un opérateur. dt : -3..+3 (désaccord fin)."""
    return dict(mul=mul, dt=dt, tl=tl, ar=ar, d1r=d1r, d2r=d2r, sl=sl, rr=rr, ks=ks, am=am)


class Instrument:
    def __init__(self, name, alg, fb, ops, ams=0, fms=0, level=0, vel_bright=0, octave=0,
                 detune_cents=0.0):
        self.name = name
        self.alg = alg
        self.fb = fb
        self.ops = ops          # liste de 4 dict (S1..S4)
        self.ams = ams          # sensibilité amplitude au LFO (0..3)
        self.fms = fms          # sensibilité fréquence au LFO (0..7)
        self.level = level      # atténuation globale ajoutée aux porteurs (pas de 0,75 dB)
        self.vel_bright = vel_bright  # vélocité faible -> modulateurs plus doux (pas de TL max)
        self.octave = octave    # transposition (octaves) appliquée aux notes jouées
        self.detune_cents = detune_cents

    def carriers(self):
        return CARRIERS[self.alg]

    def op_regs(self, i, tl_value):
        """Valeurs des registres 0x30..0x90 de l'opérateur i (S1..S4) pour un TL donné."""
        o = self.ops[i]
        dt = o['dt']
        dtv = dt if dt >= 0 else 4 - dt   # -1 -> 5, -2 -> 6, -3 -> 7
        return {
            0x30: (dtv << 4) | (o['mul'] & 15),
            0x40: max(0, min(127, tl_value)),
            0x50: (o['ks'] << 6) | (o['ar'] & 31),
            0x60: (o['am'] << 7) | (o['d1r'] & 31),
            0x70: o['d2r'] & 31,
            0x80: ((o['sl'] & 15) << 4) | (o['rr'] & 15),
            0x90: 0,
        }


I = Instrument

# --------------------------------------------------------------------- basses
EBASS = I('basse électrique ronde', alg=4, fb=2, ops=[
    op(mul=1, tl=34, d1r=13, sl=4, d2r=4, rr=12, ks=1),        # S1 -> S2 : attaque du doigt
    op(mul=1, tl=0, d1r=6, sl=15, d2r=0, rr=11),               # S2 porteur : fondamentale
    op(mul=1, tl=46, d1r=10, sl=6, d2r=3, rr=12),              # S3 -> S4
    op(mul=2, tl=17, d1r=8, sl=15, rr=11),                     # S4 porteur : octave (présence)
], vel_bright=10, octave=0)

SBASS = I('basse synthé douce', alg=4, fb=5, ops=[
    op(mul=1, tl=30, d1r=11, sl=6, d2r=2, rr=11, ks=1),        # balayage de « filtre »
    op(mul=1, dt=1, tl=0, d1r=3, sl=2, d2r=2, rr=10),
    op(mul=1, tl=52, d1r=6, sl=4, rr=11),
    op(mul=1, dt=-1, tl=6, d1r=3, sl=2, d2r=2, rr=10),         # désaccordé : épaisseur
], vel_bright=12)

UPRIGHT = I('contrebasse feutrée', alg=4, fb=0, ops=[
    op(mul=1, tl=40, d1r=16, sl=8, d2r=6, rr=12, ks=1),
    op(mul=1, tl=0, d1r=9, sl=15, rr=11),
    op(mul=3, tl=56, d1r=18, sl=15, rr=12),
    op(mul=1, dt=1, tl=10, d1r=10, sl=15, rr=11),
], vel_bright=8)

# --------------------------------------------------------------------- claviers
RHODES = I('piano électrique', alg=4, fb=0, ams=1, ops=[
    op(mul=14, tl=38, d1r=19, sl=15, rr=12, ks=2),             # S1 -> S2 : lame (« ding »)
    op(mul=1, dt=1, tl=16, d1r=9, sl=15, rr=9, am=1),          # S2 porteur de la lame
    op(mul=1, tl=32, d1r=9, sl=5, d2r=3, rr=9, ks=1),          # S3 -> S4 : corps
    op(mul=1, dt=-1, tl=0, d1r=6, sl=3, d2r=3, rr=8, am=1),    # S4 porteur du corps
], vel_bright=14)

VIBES = I('vibraphone', alg=7, fb=0, ams=2, ops=[
    op(mul=1, tl=8, d1r=7, sl=15, rr=7, am=1),                 # fondamentale
    op(mul=4, tl=27, d1r=12, sl=15, rr=9),                     # partiel 4 (lame)
    op(mul=10, tl=44, d1r=19, sl=15, rr=11, ks=1),             # attaque de mailloche
    op(mul=1, dt=2, tl=12, d1r=7, sl=15, rr=7, am=1),          # battement doux
], level=0)

BELL = I('cloche douce', alg=4, fb=0, ops=[
    op(mul=7, tl=42, d1r=10, sl=15, rr=8),                     # rapport 7/2 : inharmonique
    op(mul=2, tl=14, d1r=7, sl=15, rr=7),
    op(mul=1, tl=48, d1r=8, sl=15, rr=8),
    op(mul=1, tl=0, d1r=5, sl=15, rr=7),
], level=2)

MARIMBA = I('marimba', alg=4, fb=0, ops=[
    op(mul=4, tl=40, d1r=18, sl=15, rr=12, ks=1),
    op(mul=1, tl=0, d1r=11, sl=15, rr=11),
    op(mul=10, tl=56, d1r=21, sl=15, rr=12, ks=1),
    op(mul=4, tl=30, d1r=15, sl=15, rr=12),
], level=2)

ORGAN = I('orgue feutré', alg=7, fb=0, ams=1, ops=[
    op(mul=1, tl=12, ar=26, sl=0, rr=9, am=1),
    op(mul=2, tl=18, ar=26, sl=0, rr=9),
    op(mul=4, tl=30, ar=26, sl=0, rr=9),
    op(mul=0, tl=18, ar=26, sl=0, rr=9),                       # MUL 0 = demi-fréquence (sous-octave)
], level=6)

# --------------------------------------------------------------------- nappes et solistes
PAD = I('nappe chaude', alg=5, fb=3, fms=2, ops=[
    op(mul=1, tl=42, ar=12, d1r=2, sl=2, d2r=0, rr=7),         # S1 module S2, S3, S4
    op(mul=1, dt=1, tl=6, ar=13, d1r=1, sl=1, d2r=0, rr=7),
    op(mul=2, dt=-2, tl=18, ar=12, d1r=2, sl=2, d2r=1, rr=7),
    op(mul=1, dt=-1, tl=6, ar=13, d1r=1, sl=1, d2r=0, rr=7),
], level=8)

STRINGS = I('cordes synthé', alg=4, fb=4, fms=2, ops=[
    op(mul=1, tl=34, ar=14, d1r=2, sl=3, rr=7),
    op(mul=1, dt=2, tl=4, ar=14, d1r=1, sl=1, rr=7),
    op(mul=1, tl=38, ar=13, d1r=2, sl=3, rr=7),
    op(mul=1, dt=-2, tl=4, ar=14, d1r=1, sl=1, rr=7),
], level=10)

FLUTE = I('flûte douce', alg=4, fb=0, fms=3, ops=[
    op(mul=1, tl=52, ar=20, d1r=4, sl=3, rr=9),
    op(mul=1, tl=0, ar=21, d1r=0, sl=0, rr=9),
    op(mul=2, tl=60, ar=20, d1r=4, sl=3, rr=9),
    op(mul=2, tl=30, ar=20, d1r=2, sl=2, rr=9),
], level=4)

GUITAR = I('guitare clean', alg=4, fb=4, ops=[
    op(mul=3, tl=42, d1r=14, sl=15, rr=12, ks=1),              # médiator
    op(mul=1, tl=4, d1r=7, sl=15, rr=11),
    op(mul=1, tl=34, d1r=9, sl=6, d2r=3, rr=12),
    op(mul=2, tl=15, d1r=8, sl=15, rr=11),
], vel_bright=10, level=2)

HORN = I('cuivres feutrés', alg=4, fb=4, ops=[
    op(mul=1, tl=34, ar=18, d1r=4, sl=3, d2r=2, rr=10),
    op(mul=1, dt=1, tl=2, ar=22, d1r=2, sl=1, rr=10),
    op(mul=1, tl=40, ar=19, d1r=4, sl=3, rr=10),
    op(mul=1, dt=-1, tl=6, ar=22, d1r=2, sl=1, rr=10),
], vel_bright=12, level=4)

BANK = {
    'ebass': EBASS, 'sbass': SBASS, 'upright': UPRIGHT,
    'rhodes': RHODES, 'vibes': VIBES, 'bell': BELL, 'marimba': MARIMBA, 'organ': ORGAN,
    'pad': PAD, 'strings': STRINGS, 'flute': FLUTE, 'guitar': GUITAR, 'horn': HORN,
}
