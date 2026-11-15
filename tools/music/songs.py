"""Morceaux de Mega Président (V3) : compositions originales, écrites en code.

Conventions : voie 0 = basse, voies 1 à 3 = accords / nappes / arpèges, voie 4 = mélodie.
Mélodies en croches (unit=0.5) : une mesure à 4 temps = 8 unités.
"""
from compose import Song, voice_chord


# ------------------------------------------------------------------ motifs communs
def rh_citypop(beats):
    if beats >= 4:
        return [(0, 1.3, 1.0), (1.5, 0.45, 0.75), (2.5, 1.4, 0.9)]
    return [(0, 1.0, 1.0), (1.5, 0.45, 0.8)]


def rh_hold(beats):
    return [(0, beats, 1.0)]


def rh_charleston(beats):
    if beats >= 4:
        return [(0, 0.7, 1.0), (1.5, 0.4, 0.8), (3.0, 0.4, 0.55)]
    return [(0, 0.7, 1.0), (1.5, 0.4, 0.8)]


def bass_citypop(beats):
    if beats >= 4:
        return [(0, 1.25, 'R', 1.0), (1.5, 0.4, '8', 0.62), (1.75, 0.5, 'R', 0.85),
                (2.5, 0.8, '5', 0.85), (3.5, 0.45, 'A', 0.72)]
    return [(0, 1.0, 'R', 1.0), (1.0, 0.4, '8', 0.62), (1.5, 0.45, 'A', 0.75)]


def bass_lofi(beats):
    if beats >= 4:
        return [(0, 1.3, 'R', 1.0), (1.5, 0.45, 'R', 0.6), (2.5, 0.9, '5', 0.85), (3.5, 0.45, 'A', 0.7)]
    return [(0, 1.3, 'R', 1.0), (1.5, 0.45, 'A', 0.7)]


def bass_walk(beats):
    if beats >= 4:
        return [(0, 0.95, 'R', 1.0), (1, 0.95, '3', 0.8), (2, 0.95, '5', 0.88), (3, 0.95, 'A', 0.78)]
    return [(0, 0.95, 'R', 1.0), (1, 0.95, 'A', 0.8)]


def arp(song, ch, start, prog, order, step=0.5, vel=60, gate=0.9, inst=None, lo=55, hi=79,
        voices=4):
    """Arpège : « order » = indices dans l'accord (voix triées), répétés tous les « step » temps."""
    t, prev = start, None
    for sym, beats in prog:
        notes = voice_chord(sym, voices, prev, lo, hi, (lo + hi) // 2)
        prev = notes
        k = 0
        while k * step < beats - 1e-9:
            idx = order[k % len(order)]
            song.n(ch, t + k * step, step, notes[idx], int(vel * (1.0 if k % 2 == 0 else 0.85)), gate, inst)
            k += 1
        t += beats


# ================================================================== TITRE
def title():
    s = Song('title', 'Titre : city pop douce, progression « royale » en fa (IVmaj7-III7-vim7)',
             bpm=94, bars=16, swing=0.12, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'vibes')
    prog = [('Bbmaj7', 4), ('A7', 4), ('Dm9', 4), ('Cm7', 2), ('F7', 2),
            ('Bbmaj7', 4), ('A7', 4), ('Dm9', 4), ('Gm7', 2), ('C7sus4', 2),
            ('Gm9', 4), ('Am7', 4), ('Bbmaj7', 4), ('C7sus4', 2), ('C7', 2),
            ('Gm9', 4), ('Am7', 2), ('D7b9', 2), ('Gm9', 2), ('C9sus4', 2), ('Fmaj9', 4)]
    s.bassline(0, 0, prog, bass_citypop, vel=110)
    s.comp((1, 2, 3), 0, prog, rh_citypop, vel=78, lo=53, hi=74, center=62)
    mel = ('r:1 d5 f5 a5:3 g5 f5 | e5:2 c#5 e5 g5:3 r:1 | f5:3 e5 d5 c5 a4:2 | bb4:2 c5 eb5:2 d5 c5 a4 |'
           'r:1 d5 f5 a5:2 c6 a5 g5 | a5:3 g5 e5:2 c#5 e5 | d5:4 r:1 a4 c5 e5 | f5:3 d5 c5:4 |'
           'bb5:2 a5 g5 f5:2 d5 f5 | e5:3 c5 e5:2 g5:2 | a5:3 f5 d5:2 f5 a5 | c6:3 bb5 g5:2 e5:2 |'
           'f5:2 g5 a5 bb5:2 a5 g5 | a5:2 g5 e5 f#5:2 eb5 d5 | d5:3 f5 g5:2 bb5 a5 | a5:5 r:3')
    s.mel(4, 0, mel.replace('|', ' '), vel=104)
    # batterie : discrète au début, complète ensuite
    s.groove(0, 4, k='x.........x.....', s='....x.......x...', h='x.x.x.x.x.x.x.x.', hv=0.8)
    s.groove(4, 12, k='x.........x..x..', s='....x.......x...', g='.......x.......x',
             h='X.x.X.x.X.x.X.xo')
    return s


# ================================================================== ANNÉE 1
def year1():
    s = Song('year1', 'Année 1, état de grâce : sol majeur lo-fi, arpège montant sol-si-ré en signature',
             bpm=92, bars=16, swing=0.22, lfo=1, gain=-2, ch_vol=[1, 2, 2, 4, 0])
    s.use(0, 'ebass')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'guitar')
    s.use(4, 'vibes')
    prog = [('Gmaj9', 4), ('Cmaj9', 4), ('Am9', 4), ('D9sus4', 4),
            ('Gmaj9', 4), ('Bm7', 2), ('E7', 2), ('Am9', 4), ('D7sus4', 2), ('D7', 2),
            ('Cmaj9', 4), ('Bm7', 4), ('Em9', 4), ('A9', 4),
            ('Am9', 4), ('Bm7', 2), ('E7b9', 2), ('Am9', 2), ('D9sus4', 2), ('G6', 2), ('D7sus4', 2)]
    s.bassline(0, 0, prog, bass_lofi, vel=110)
    s.comp((1, 2), 0, prog, rh_hold, vel=72, gate=0.98, lo=55, hi=74, center=64)
    arp(s, 3, 0, prog, (0, 2, 1, 3, 2, 1, 3, 2), step=0.5, vel=62, lo=55, hi=79)
    mel = ('g4 b4 d5 f#5:3 a5:2 | g5:3 e5 d5:2 b4:2 | c5 e5 g5 b5:3 a5 g5 | a5:6 r:2 |'
           'g4 b4 d5 a5:3 g5 f#5 | f#5:3 d5 g#5:2 e5 d5 | c5:2 e5 g5 b5:2 a5 g5 | a5:4 f#5:2 r:2 |'
           'e5:2 g5 b5 d6:3 b5 | a5:3 f#5 d5:2 a5:2 | g5:2 f#5 e5 b4:2 d5 e5 | c#5:3 e5 g5:2 b5:2 |'
           'a5:2 g5 e5 c5:2 e5 g5 | f#5:2 a5 d6 b5:2 g#5 f5 | e5:3 g5 a5:2 g5 e5 | b4 d5 e5 g5 a5:4')
    # 1re fois sans mélodie sur 2 mesures ? Non : la signature arrive tout de suite.
    s.mel(4, 0, mel.replace('|', ' '), vel=100)
    s.groove(0, 16, k='x.......x.x.....', s='....x.......x...', g='..........x....x',
             h='x.xxx.xxx.xxx.xo', hv=0.85)
    return s


# ================================================================== JOURNAL DE FIN DE TOUR
def report():
    s = Song('report', 'Journal : rédaction (machine à écrire, retour chariot), indicatif de JT feutré, ré dorien',
             bpm=100, bars=16, swing=0.1, lfo=2, gain=0, ch_vol=[0, 4, 4, 4, 2])
    s.use(0, 'upright')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'marimba')
    prog_a = [('Dm9', 4), ('G13', 4), ('Dm9', 4), ('G13', 4), ('Dm9', 4), ('G13', 4),
              ('Bbmaj7', 4), ('A7sus4', 2), ('A7b9', 2)]
    prog_b = [('Dm9', 4), ('G13', 4), ('Gm9', 4), ('C13', 4), ('Fmaj9', 4), ('Bbmaj7', 4),
              ('Em7b5', 2), ('A7b9', 2), ('Dm9', 2), ('A7sus4', 2)]
    # indicatif (mesures 0-1 et 8-9) : cuivres feutrés et cloche
    jingle_rh = [(0, 0.4, 1.0), (0.5, 0.4, 0.85), (1.0, 0.4, 0.9), (1.5, 2.3, 1.0)]
    for b0, lead, v in ((0, 'bell', 106), (8, 'vibes', 96)):
        s.comp((1, 2, 3), s.bar(b0), [('Dm9', 4)], jingle_rh, vel=88, inst='horn', lo=55, hi=76)
        s.comp((1, 2, 3), s.bar(b0 + 1), [('G13', 4)], [(0, 1.0, 0.9), (1.0, 2.8, 0.8)],
               vel=80, inst='horn', lo=55, hi=76)
        s.mel(4, s.bar(b0), 'd5:1! a5 d6 f6:5 e6:2 d6:2 a5:4', vel=v, inst=lead)
    # accords du rédacteur : Rhodes en Charleston le reste du temps
    s.comp((1, 2, 3), s.bar(2), prog_a[2:], rh_charleston, vel=74, lo=53, hi=74)
    s.comp((1, 2, 3), s.bar(10), prog_b[2:], rh_charleston, vel=74, lo=53, hi=74)
    # basse : notes tenues sous l'indicatif, puis walking
    for b0 in (0, 8):
        s.bassline(0, s.bar(b0), [('Dm9', 4), ('G13', 4)],
                   lambda beats: [(0, 0.4, 'R', 1.0), (0.5, 0.4, 'R', 0.8), (1.0, 0.4, 'R', 0.85),
                                  (1.5, 2.3, 'R', 1.0)], vel=104)
    s.bassline(0, s.bar(2), prog_a[2:], bass_walk, vel=100)
    s.bassline(0, s.bar(10), prog_b[2:], bass_walk, vel=100)
    # « téléscripteur » au marimba
    s.mel(4, s.bar(2), 'r:1 a4 c5 d5 e5 f5 e5 c5 d5:2 b4 a4 e5:3 r:1 r:1 a4 c5 d5 f5:2 e5 c5 d5:4 r:4 '
                       'f5 a5 c6 a5 f5:2 d5:2 e5:2 d5 e5 c#5:2 bb4 a4', vel=92)
    s.mel(4, s.bar(10), 'bb4 d5 f5 a5:2 g5 f5 d5 e5:3 d5 c5:2 a4:2 a4 c5 e5 g5:3 f5 e5 d5:4 r:4 '
                        'g5:2 bb5 g5 e5:2 c#5 bb4 d5:3 f5 e5:4', vel=92)
    # rédaction : frappes irrégulières, grosse caisse feutrée, rimshot, clochette en fin de phrase
    type_a = '..x..xx...x..x.x'
    type_b = '...x..x..x.x.x.x'
    for b in range(16):
        if b in (5, 13):       # retour chariot : frappes, puis « ding »
            s.groove(b, 1, k='x...............', extra={'type': '..x..xx.........'})
            s.drum(s.bar(b) + 2.5, 'ding')
            continue
        s.groove(b, 1, k='x.......x.......', r='....x.......x...',
                 extra={'type': type_a if b % 2 == 0 else type_b},
                 h='s.s.s.s.s.s.s.s.', hv=0.8)
    return s


SONGS = [title, year1, report]
