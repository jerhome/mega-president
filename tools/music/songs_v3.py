"""Morceaux V3 (suite) : années 2 à 5, élection et un thème par écran.

Même style que songs.py (titre, année 1, journal), validé par le joueur.
Les mélodies sont écrites mesure par mesure (« | ») et leur durée est vérifiée.
"""
from compose import Song
from songs import (title, year1, report, arp, rh_citypop, rh_hold, rh_charleston,
                   bass_citypop, bass_lofi, bass_walk)


# ------------------------------------------------------------------ outils
def P(text):
    """Progression « Fmaj7:4 C7:2 G7:2 » -> [(accord, temps)] (4 temps par défaut)."""
    out = []
    for tok in text.split():
        if ':' in tok:
            sym, b = tok.rsplit(':', 1)
            out.append((sym, float(b)))
        else:
            out.append((tok, 4.0))
    return out


def melody(s, ch, bar0, text, unit=0.5, **kw):
    """Mélodie mesure par mesure (« | » entre les mesures) ; vérifie la durée de chaque mesure."""
    per_bar = s.bpb / unit
    for k, seg in enumerate(text.split('|')):
        if not seg.strip():
            continue
        tot = 0.0
        for tok in seg.split():
            tok = tok.rstrip('!?~')
            tot += float(tok.split(':')[1]) if ':' in tok else 1.0
        if abs(tot - per_bar) > 1e-6:
            raise ValueError('%s : mesure %d de la mélodie = %g unités (au lieu de %g) : %s'
                             % (s.name, bar0 + k, tot, per_bar, seg))
        s.mel(ch, s.bar(bar0 + k), seg, unit=unit, **kw)


def rh_pulse(beats):
    return [(float(i), 0.45 if i % 2 else 0.8, 1.0 if i % 2 == 0 else 0.7) for i in range(int(beats))]


def rh_halves(beats):
    if beats >= 4:
        return [(0, 1.9, 1.0), (2, 1.9, 0.85)]
    return [(0, beats - 0.1, 1.0)]


def rh_bossa(beats):
    hits = [0, 0.75, 1.5, 2.5, 3.25]
    return [(h, 0.4, 1.0 if h in (0, 1.5) else 0.8) for h in hits if h < beats]


def rh_sync(beats):
    if beats >= 4:
        return [(0, 0.5, 1.0), (0.75, 1.0, 0.8), (2.75, 1.0, 0.85)]
    return [(0, 0.5, 1.0), (0.75, 1.0, 0.8)]


def rh_lofi_hold(beats):
    if beats >= 4:
        return [(0, 2.4, 1.0), (2.5, 1.3, 0.75)]
    return [(0, beats * 0.9, 1.0)]


def bass_eighths(beats):
    out = []
    for i in range(int(beats * 2)):
        deg = 'R'
        if beats >= 4 and i == 5:
            deg = '8'
        if beats >= 4 and i == 7:
            deg = 'A'
        out.append((i * 0.5, 0.4, deg, 1.0 if i % 2 == 0 else 0.72))
    return out


def bass_pedal(beats):
    return [(i * 0.5, 0.4, 'R', 0.75 + 0.25 * ((i % 4) == 0)) for i in range(int(beats * 2))]


def bass_sync(beats):
    if beats >= 4:
        return [(0, 0.7, 'R', 1.0), (0.75, 0.5, 'R', 0.6), (1.5, 0.45, '8', 0.7),
                (2.5, 0.7, '5', 0.9), (3.5, 0.45, 'R', 0.75)]
    return [(0, 0.7, 'R', 1.0), (0.75, 0.5, '8', 0.65), (1.5, 0.45, 'A', 0.75)]


def bass_halves(beats):
    if beats >= 4:
        return [(0, 1.9, 'R', 1.0), (2, 1.9, '5', 0.85)]
    return [(0, beats - 0.1, 'R', 1.0)]


def bass_march(beats):
    if beats >= 4:
        return [(0, 0.9, 'R', 1.0), (1, 0.9, '5', 0.8), (2, 0.9, '8', 0.85),
                (3, 0.45, '5', 0.7), (3.5, 0.45, 'A', 0.75)]
    return [(0, 0.9, 'R', 1.0), (1, 0.45, '5', 0.75), (1.5, 0.45, 'A', 0.75)]


def bass_bossa(beats):
    if beats >= 4:
        return [(0, 1.4, 'R', 1.0), (1.5, 0.45, '5', 0.8), (2, 1.4, '5', 0.9), (3.5, 0.45, 'R', 0.8)]
    return [(0, 1.4, 'R', 1.0), (1.5, 0.45, '5', 0.8)]


def bass_ostinato(beats):
    seq = (0, 0, 12, 0, 7, 0, 12, 7)
    return [(i * 0.5, 0.4, seq[i % 8], 1.0 if i % 4 == 0 else 0.75) for i in range(int(beats * 2))]


# ================================================================== ANNÉE 2
def year2():
    s = Song('year2', 'Année 2, mise au travail : fa majeur, basse en croches répétées, marimba « horloge »',
             bpm=98, bars=16, swing=0.12, lfo=2, gain=0, ch_vol=[1, 5, 5, 6, 0])
    s.use(0, 'ebass')
    s.use(1, 'rhodes')
    s.use(2, 'rhodes')
    s.use(3, 'marimba')
    s.use(4, 'flute')
    prog = P('Fmaj7 Fmaj7 Bbmaj7 Bbmaj7 Gm7 C9sus4 Am7 Dm9 '
             'Bbmaj7 C7 Am7 Dm7 Gm9 C9sus4 Fmaj9 C7sus4')
    s.bassline(0, 0, prog, bass_eighths, vel=104)
    s.comp((1, 2), 0, prog, rh_charleston, vel=78, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 2, 0, 2, 1, 2, 0, 2), step=0.5, vel=56, lo=60, hi=81, voices=3)
    melody(s, 4, 0, 'a4 a4 a4:2 c5:2 a4:2 | a4 a4 a4:2 g4:2 e4:2 | bb4 bb4 bb4:2 d5:2 bb4:2 |'
                    'a4 bb4 c5:2 d5:2 f5:2 | g4 g4 g4:2 bb4:2 d5:2 | c5 c5 c5:2 d5:2 bb4:2 |'
                    'e5 e5 e5:2 g5:2 e5:2 | f5:4 e5:2 c5:2 | f5 f5 f5:2 a5:2 f5:2 |'
                    'e5 e5 e5:2 g5:2 bb5:2 | a5:3 g5 e5:2 c5:2 | d5 e5 f5:2 a5:2 c6:2 |'
                    'bb5:3 a5 g5:2 f5:2 | d5 d5 d5:2 f5:2 g5:2 | a5:4 g5:2 e5:2 | f5:4 r:4', vel=100)
    s.groove(0, 16, k='x.......x.......', s='....x.......x...', r='..x...x...x...x.',
             h='s.s.s.s.s.s.s.s.', hv=0.8)
    return s


# ================================================================== ANNÉE 3
def year3():
    s = Song('year3', 'Année 3, mi-mandat tendu : mi mineur, syncopes, seconde mineure (fa sur mi)',
             bpm=88, bars=16, swing=0.15, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'sbass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'guitar')
    prog = P('Em9 Em9 Cmaj7#11 Cmaj7#11 Am9 B7b9 Em9 Fmaj7#11 '
             'Am9 D9sus4 Gmaj7 Cmaj7#11 Am9 B7b9 Em9 Fmaj7#11:2 B7#9:2')
    s.bassline(0, 0, prog, bass_sync, vel=108)
    s.comp((1, 2, 3), 0, prog, rh_sync, vel=76, lo=52, hi=74)
    melody(s, 4, 0, 'r:1 b4 e5:2 f#5 g5:3 | f#5:2 e5 d5 b4:4 | r:1 e5 f#5:2 g5 b5:3 | a5:2 g5 f#5 e5:4 |'
                    'r:1 c5 e5:2 g5 b5:3 | a5:2 f#5 d#5 c5:4 | b4:3 e5 f#5:2 g5:2 | f5:3 e5 c5:2 a4:2 |'
                    'e5:3 c5 b4:2 a4:2 | r:1 d5 g5:2 a5 c6:3 | b5:3 a5 f#5:2 d5:2 | e5:2 f#5 g5 f#5:4 |'
                    'r:1 a4 c5:2 e5 b5:3 | a5:2 c6 b5 f#5:4 | g5:3 f#5 e5:2 b4:2 | f5:2 e5 c5 d5:2 d#5:2',
           vel=104)
    s.groove(0, 16, k='x.....x...x.....', s='....x.......x...', g='.......x.....x..',
             h='x.xxx.xxx.xxx.xo', hv=0.85)
    return s


# ================================================================== ANNÉE 4
def year4():
    s = Song('year4', 'Année 4, doute et usure : la mineur lent, lignes descendantes, balais',
             bpm=70, bars=12, swing=0.1, lfo=1, gain=-1, ch_vol=[0, 2, 2, 6, 0])
    s.use(0, 'upright')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'rhodes')
    s.use(4, 'bell')
    prog = P('Am9 Fmaj7 Dm9 E7sus4:2 E7b9:2 Am9 Fmaj7 Bm7b5 E7b9 Fmaj7 Em7 Dm9 E7sus4:2 E7b9:2')
    s.bassline(0, 0, prog, bass_halves, vel=104)
    s.comp((1, 2), 0, prog, rh_hold, vel=74, gate=0.98, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 1, 2, 3, 2, 1, 2, 1), step=0.5, vel=58, lo=52, hi=76)
    melody(s, 4, 0, 'e5:3 d5 c5:2 b4:2 | a4:6 r:2 | r:2 f5:2 e5 d5 c5:2 | b4:4 g#4:2 r:2 |'
                    'c5:3 b4 a4:2 g4:2 | f5:3 e5 c5:2 a4:2 | d5:3 c5 b4:2 a4:2 | g#4:6 r:2 |'
                    'c6:3 b5 a5:2 g5:2 | g5:3 f#5 e5:2 d5:2 | f5:3 e5 d5:2 c5:2 | b4:4 g#4:2 e4:2',
           vel=100)
    s.groove(0, 12, k='x...............', extra={'brush': '....x.......x...'}, h='s.s.s.s.s.s.s.s.', hv=0.6)
    return s


# ================================================================== ANNÉE 5
def year5():
    s = Song('year5', 'Année 5, marche vers l\'élection : ré majeur puis mi majeur (+1 ton), roulements',
             bpm=100, bars=16, swing=0.08, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'horn')
    prog = P('Dmaj9 Gmaj9 Bm9 A7sus4 Dmaj9 Gmaj9 Em9 A7sus4:2 A7:2 '
             'Emaj9 Amaj9 C#m9 B7sus4 Emaj9 Amaj9 F#m9 Gmaj7:2 A7sus4:2')
    s.bassline(0, 0, prog, bass_march, vel=106)
    s.comp((1, 2, 3), 0, prog, rh_pulse, vel=70, lo=53, hi=74)
    mel_a = ('d5 e5 f#5:2 a5:3 f#5 | g5:2 f#5 e5 d5:2 b4:2 | d5 e5 f#5:2 b5:3 a5 | a5:4 g5:2 e5:2 |'
             'f#5 g5 a5:2 d6:3 c#6 | b5:2 a5 g5 f#5:2 d5:2 | e5 f#5 g5:2 b5:2 d6:2 |')
    melody(s, 4, 0, mel_a + ' d6:4 c#6:2 r:2', vel=100)
    melody(s, 4, 8, mel_a, vel=104, transpose=2)
    melody(s, 4, 15, 'b5:2 a5 g5 e5:2 d5:2', vel=100)
    s.groove(0, 7, k='x.......x.......', s='....x.......x...', g='..x...x...x...xx', h='x.x.x.x.x.x.x.x.')
    s.groove(7, 1, k='x...............', s='....x...x.xxxxxx', h='x.x.x.x.x.x.x.x.')
    s.groove(8, 7, k='x.......x.......', s='....x.......x...', g='..x...x...x...xx', h='X.x.X.x.X.x.X.x.')
    s.groove(15, 1, k='x...............', s='....x...xxxxxxxx', h='x.x.x.x.x.x.x.x.')
    return s


# ================================================================== ÉLECTION
def election():
    s = Song('election', 'Soirée électorale : suspense sur pédale de ré (horloge), puis résolution en fa',
             bpm=84, bars=16, swing=0.1, lfo=1, gain=0, ch_vol=[0, 3, 3, 6, 0])
    s.use(0, 'sbass')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'rhodes')
    s.use(4, 'vibes')
    prog_a = P('Dm9 Dm9 Bbmaj7/D Bbmaj7/D Gm9/D Gm9/D A7sus4/D A7b9/D')
    prog_b = P('Bbmaj7 C9sus4 Am7 Dm9 Gm9 C9sus4 Fmaj9 A7sus4')
    s.bassline(0, 0, prog_a, bass_pedal, vel=100)
    s.bassline(0, s.bar(8), prog_b, bass_lofi, vel=108)
    s.comp((1, 2), 0, prog_a + prog_b, rh_hold, vel=74, gate=0.98, lo=55, hi=74)
    arp(s, 3, 0, prog_a, (0, 2, 1, 3), step=1.0, vel=54, lo=57, hi=79)
    s.comp((3,), s.bar(8), prog_b, rh_citypop, vel=74, lo=60, hi=76, center=67)
    melody(s, 4, 0, 'r:4 a5:2 g5:2 | f5:4 e5:4 | r:4 d6:2 c6:2 | a5:4 f5:4 | r:4 bb5:2 a5:2 | g5:4 d5:4 |'
                    'r:2 e5 g5 a5:4 | g5:2 f5:2 e5:2 c#5:2 |'
                    'd5 f5 a5:2 c6:3 a5 | bb5:2 g5 f5 d5:4 | c5 e5 g5:2 c6:3 b5 | a5:4 f5:2 e5:2 |'
                    'd5 f5 a5:2 d6:3 c6 | bb5:2 a5 g5 f5:2 d5:2 | e5 f5 g5:2 a5:4 | g5:2 e5:2 d5:4', vel=102)
    s.groove(0, 8, k='x...............', r='..x...x...x...x.', h='..s...s...s...s.', hv=0.7)
    s.groove(8, 8, k='x.......x.x.....', s='....x.......x...', g='...............x', h='x.xxx.xxx.xxx.xo')
    return s


# ================================================================== ÉVÉNEMENT
def event():
    s = Song('event', 'Événement : dépêche feutrée, motif d\'alerte à la cloche (comme le bruitage)',
             bpm=92, bars=12, swing=0.15, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'bell')
    prog = P('Dm9 Bbmaj7 Gm9 A7sus4 Dm9 Bbmaj7 Gm9 A7b9 Bbmaj7 Am7 Gm9 A7sus4:2 A7b9:2')
    s.bassline(0, 0, prog, bass_lofi, vel=106)
    s.comp((1, 2, 3), 0, prog, rh_charleston, vel=76, lo=53, hi=74)
    melody(s, 4, 0, 'a5 a5 d6:2 r:4 | f5:2 d5 c5 a4:4 | bb4 d5 f5:2 a5:3 g5 | e5:4 d5:2 r:2 |'
                    'a5 a5 d6:2 r:2 c6 a5 | f5:3 d5 a5:2 f5:2 | g5 a5 bb5:2 d6:3 c6 | bb5:2 g5 e5 c#5:4 |'
                    'a5 a5 d6:2 c6:2 a5:2 | g5:3 e5 c5:2 e5:2 | f5:2 g5 a5 bb5:2 a5:2 | d5:2 e5:2 c#5:4',
           vel=104)
    s.groove(0, 12, k='x.........x.....', s='....x.......x...', g='.......x........',
             h='x.x.x.x.x.x.x.xo', hv=0.85)
    return s


# ================================================================== ASSEMBLÉE
def assembly():
    s = Song('assembly', 'Assemblée : solennel et feutré, orgue, contrebasse en walking, maillet de séance',
             bpm=90, bars=12, swing=0.1, lfo=1, gain=0, ch_vol=[0, 4, 4, 4, 0])
    s.use(0, 'upright')
    for c in (1, 2, 3):
        s.use(c, 'organ')
    s.use(4, 'horn')
    prog = P('Bbmaj7 Gm7 Cm9 F7sus4 Bbmaj7 Ebmaj7 Cm7:2 F7:2 Dm7:2 G7b9:2 Cm9 F9sus4 '
             'Ebmaj7:2 Edim7:2 Bbmaj7/F:2 F7sus4:2')
    s.bassline(0, 0, prog, bass_walk, vel=100)
    s.comp((1, 2, 3), 0, prog, rh_halves, vel=72, lo=53, hi=74)
    melody(s, 4, 0, 'd5:4 f5:2 bb5:2 | a5:4 g5:2 f5:2 | eb5:3 d5 c5:2 g5:2 | f5:6 r:2 |'
                    'd5:2 f5:2 a5:2 c6:2 | bb5:4 g5:4 | eb5:2 g5:2 a5:2 c6:2 | d6:2 c6:2 b5:2 ab5:2 |'
                    'g5:4 eb5:2 d5:2 | c5:4 eb5:2 f5:2 | g5:2 bb5:2 bb5:2 g5:2 | f5:4 r:4', vel=98)
    s.groove(0, 1, extra={'rim': 'x.x.x...........'})          # maillet du président de séance
    s.groove(1, 11, k='x.......x.......', extra={'brush': '....x.......x...'}, h='..s...s...s...s.', hv=0.7)
    return s


# ================================================================== CARTE DES RÉGIONS
def map_theme():
    s = Song('map', 'Carte des régions : bossa-nova de voyage en la majeur, guitare et flûte',
             bpm=120, bars=16, lfo=3, gain=0, ch_vol=[0, 6, 6, 6, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'guitar')
    s.use(4, 'flute')
    prog = P('Amaj7 Amaj7 Bm7 E7 Amaj7 F#m7 Bm7 E7sus4 Dmaj7 Dm6 C#m7 F#7 Bm7 E9 '
             'Amaj7:2 F#m7:2 Bm7:2 E7:2')
    s.bassline(0, 0, prog, bass_bossa, vel=104)
    s.comp((1, 2, 3), 0, prog, rh_bossa, vel=72, lo=55, hi=74, strum=0.02)
    melody(s, 4, 0, 'e5:3 c#5 b4:2 a4:2 | c#5 e5 g#5:2 f#5:4 | d5:3 f#5 a5:2 c#6:2 | b5:4 g#5:2 e5:2 |'
                    'e5:3 c#5 b4:2 a4:2 | a4 c#5 e5:2 g#5:4 | f#5:3 e5 d5:2 b4:2 | a4:6 r:2 |'
                    'f#5:3 a5 c#6:2 a5:2 | f5:3 a5 b5:2 a5:2 | g#5:3 e5 c#5:2 b4:2 | a#4:4 c#5:2 e5:2 |'
                    'd5:3 c#5 b4:2 f#5:2 | f#5:2 g#5:2 b5:4 | c#6:2 b5 a5 a5:2 g#5:2 | f#5:2 d5:2 e5:2 g#5:2',
           vel=100)
    s.groove(0, 16, k='x.......x.......', r='...x..x...x..x..', h='ssssssssssssssss', hv=0.6)
    return s


# ================================================================== BUDGET
def budget():
    s = Song('budget', 'Budget : comptabilité feutrée en mi bémol, marimba en escaliers, touches de calculette',
             bpm=96, bars=12, swing=0.12, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 1])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'marimba')
    prog = P('Fm9 Bb13 Ebmaj9 Cm9 Fm9 Bb13 Gm7:2 C7b9:2 Fm9:2 Bb7sus4:2 Abmaj7 Gm7 Fm9 '
             'Bb7sus4:2 Bb13:2')
    s.bassline(0, 0, prog, bass_citypop, vel=106)
    s.comp((1, 2, 3), 0, prog, rh_charleston, vel=74, lo=53, hi=74)
    melody(s, 4, 0, 'c5 eb5 f5 g5 ab5:2 g5 f5 | d5:3 f5 g5:4 | bb4 d5 f5 g5 bb5:2 g5 f5 | eb5:4 d5:2 r:2 |'
                    'c5 eb5 f5 g5 ab5:2 bb5 c6 | d6:3 c6 ab5:2 f5:2 | g5:2 f5 d5 e5:2 db5:2 | c5:2 eb5 g5 f5:4 |'
                    'c6:3 bb5 g5:2 eb5:2 | d5 f5 bb5:2 a5:4 | ab5:3 g5 f5:2 c5:2 | eb5:4 r:4', vel=100)
    for b in range(12):
        if b == 11:      # tiroir-caisse
            s.groove(b, 1, k='x...............', extra={'type': '..x.............'})
            s.drum(s.bar(b) + 2.0, 'ding')
            continue
        s.groove(b, 1, k='x.......x.......',
                 extra={'brush': '....x.......x...',
                        'type': '..x...x...x...x.' if b % 2 else '..x..x....x..x.x'})
    return s


# ================================================================== RÉFÉRENDUM
def referendum():
    s = Song('referendum', 'Référendum : question en suspens, accords suspendus qui ne se résolvent pas',
             bpm=80, bars=12, swing=0.1, lfo=1, gain=0, ch_vol=[0, 3, 3, 6, 0])
    s.use(0, 'ebass')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'rhodes')
    s.use(4, 'vibes')
    prog = P('Fmaj7 G9sus4 Em7 Am9 Dm9 G9sus4 Fmaj7 G9sus4 Am9 Fmaj7 Dm9:2 G9sus4:2 Csus2:2 G9sus4:2')
    s.bassline(0, 0, prog, bass_halves, vel=104)
    s.comp((1, 2), 0, prog, rh_hold, vel=74, gate=0.98, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 2, 1, 3, 2, 1, 3, 1), step=0.5, vel=56, lo=55, hi=79)
    melody(s, 4, 0, 'c5:2 e5:2 a5:3 g5 | a5:6 r:2 | b4:2 d5:2 g5:3 f#5 | e5:6 r:2 |'
                    'f5:2 a5:2 c6:3 b5 | a5:4 d5:4 | c5:2 e5:2 a5:2 c6:2 | d6:6 r:2 |'
                    'c6:3 b5 a5:2 e5:2 | g5:3 a5 e5:4 | f5:2 e5:2 d5:4 | d5:2 g5:2 a5:2 d5:2', vel=100)
    s.groove(0, 12, k='x...............', r='........x.......', h='s.s.s.s.s.s.s.s.', hv=0.6)
    return s


# ================================================================== REMANIEMENT
def reshuffle():
    s = Song('reshuffle', 'Remaniement : chaises musicales en shuffle, question-réponse vibraphone / guitare',
             bpm=108, bars=16, swing8=0.33, lfo=2, gain=0, ch_vol=[0, 5, 5, 2, 0])
    s.use(0, 'upright')
    s.use(1, 'rhodes')
    s.use(2, 'rhodes')
    s.use(3, 'guitar')
    s.use(4, 'vibes')
    prog = P('Bbmaj7 G7b9 Cm7 F7 Dm7 Db7 Cm7 F7 Ebmaj7 Edim7 Bbmaj7/F G7b9 Cm7 F7 '
             'Bbmaj7:2 G7b9:2 Cm7:2 F7:2')
    s.bassline(0, 0, prog, bass_walk, vel=100)
    s.comp((1, 2), 0, prog, rh_charleston, vel=74, lo=55, hi=74)
    calls = ['d5 f5 a5:2', 'b4 d5 f5:2', 'eb5 g5 bb5:2', 'a5:2 c6 a5', 'f5 a5 c6:2', 'f5 ab5 b5:2',
             'eb5 d5 c5:2', 'a4:2 c5:2', 'g5 bb5 d6:2', 'e5 g5 bb5:2', 'f5 bb5 d6:2', 'ab5 g5 f5:2',
             'g5 bb5 d6:2', 'a5 c6 eb6:2', 'd6:2 c6:2', 'g5:2 eb5:2']
    answers = ['c6 a5 f5:2', 'ab5 f5 d5:2', 'd6 bb5 g5:2', 'eb5:2 f5:2', 'e6 c6 a5:2', 'ab5 f5 db5:2',
               'g5 f5 eb5:2', 'eb5:4', 'f6 d6 bb5:2', 'db6 bb5 g5:2', 'c6 a5 f5:2', 'd5 b4 ab4:2',
               'eb6 d6 bb5:2', 'd6 c6 a5:2', 'b5:2 ab5:2', 'a5:2 f5:2']
    for b in range(16):
        s.mel(4, s.bar(b), calls[b], vel=102)
        s.mel(3, s.bar(b) + 2, answers[b], vel=100, transpose=-12)
    s.groove(0, 16, k='x.......x.......', s='....x.......x...', h='x.x.x.x.x.x.x.x.', hv=0.85)
    return s


# ================================================================== DESTITUTION
def impeach():
    s = Song('impeach', 'Destitution : tension en do mineur, ostinato de basse, battements de cœur, glas',
             bpm=76, bars=12, lfo=1, gain=0, ch_vol=[0, 4, 4, 2, 0])
    s.use(0, 'sbass')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'bell')
    s.use(4, 'strings')
    prog = P('Cm9 Cm9 Abmaj7#11 Abmaj7#11 Fm9 Fm9 G7b9 G7#9 Cmmaj7 Dbmaj7 Fm9 G7b9')
    s.bassline(0, 0, prog, bass_ostinato, vel=104)
    s.comp((1, 2), 0, prog, rh_hold, vel=74, gate=0.98, lo=55, hi=72)
    for b in range(0, 12, 2):
        s.n(3, s.bar(b), 4, 'G4', 90)       # glas
    melody(s, 4, 0, 'g5:8 | eb5:4 d5:4 | c5:8 | d5:4 eb5:4 | f5:6 g5:2 | ab5:4 g5:4 | f5:4 ab5:4 |'
                    'b4:4 d5:4 | c5:4 b4:4 | ab4:4 c5:4 | g5:4 ab5:4 | b5:4 ab5:4', vel=96)
    s.groove(0, 11, k='x..x....x..x....', h='..s...s...s...s.', hv=0.6)
    s.groove(11, 1, k='x..x............', extra={'tom': '........x...x.x.'})
    return s


# ================================================================== CAMPAGNE
def campaign():
    s = Song('campaign', 'Campagne : énergie douce de meeting en la majeur, cuivres et claquements',
             bpm=106, bars=16, swing=0.1, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    s.use(1, 'rhodes')
    s.use(2, 'rhodes')
    s.use(3, 'guitar')
    s.use(4, 'horn')
    prog = P('Amaj9 E/G# F#m9 Dmaj9 Amaj9 E/G# Dmaj9 E9sus4 Bm9 C#m7 Dmaj9 E9sus4 Bm9 C#m7 '
             'Dmaj9:2 E9sus4:2 Amaj9:2 E9sus4:2')
    s.bassline(0, 0, prog, bass_citypop, vel=108)
    s.comp((1, 2), 0, prog, rh_citypop, vel=76, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 1, 2, 3, 2, 1, 2, 3), step=0.5, vel=58, lo=57, hi=81)
    melody(s, 4, 0, 'e5 e5 a5:2 c#6:3 b5 | b5:2 g#5 e5 b4:4 | c#5 e5 a5:2 g#5:3 f#5 | e5:4 r:2 c#5 e5 |'
                    'e5 e5 a5:2 e6:3 c#6 | b5:2 g#5 b5 e6:4 | e6:3 c#6 a5:2 f#5:2 | e5:6 r:2 |'
                    'd5 f#5 a5:2 c#6:3 b5 | g#5:3 e5 c#5:2 b4:2 | a4 c#5 e5:2 f#5:3 e5 | d5:4 e5:2 f#5:2 |'
                    'f#5 a5 c#6:2 d6:3 c#6 | b5:2 g#5 e5 c#6:4 | a5:2 f#5:2 e5:2 d5:2 | c#5:2 e5:2 a5:2 b5:2',
           vel=102)
    s.groove(0, 16, k='x...x...x...x...', s='....x.......x...', h='..o...o...o...o.', hv=0.7)
    return s


# ================================================================== 2nd MANDAT
def mandate2():
    s = Song('mandate2', '2nd mandat : lever de soleil, le thème de l\'année 1 en la bémol, cordes et flûte',
             bpm=88, bars=12, swing=0.2, lfo=2, gain=-1, ch_vol=[1, 2, 2, 4, 0])
    s.use(0, 'ebass')
    s.use(1, 'strings')
    s.use(2, 'strings')
    s.use(3, 'guitar')
    s.use(4, 'flute')
    prog = P('Abmaj9 Dbmaj9 Bbm9 Eb9sus4 Abmaj9 Cm7:2 F7:2 Bbm9 Eb7sus4:2 Eb7:2 Dbmaj9 Cm7 Bbm9 Eb9sus4')
    s.bassline(0, 0, prog, bass_lofi, vel=110)
    s.comp((1, 2), 0, prog, rh_hold, vel=74, gate=0.98, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 2, 1, 3, 2, 1, 3, 2), step=0.5, vel=60, lo=56, hi=80)
    melody(s, 4, 0, 'g4 b4 d5 f#5:3 a5:2 | g5:3 e5 d5:2 b4:2 | c5 e5 g5 b5:3 a5 g5 | a5:6 r:2 |'
                    'g4 b4 d5 a5:3 g5 f#5 | f#5:3 d5 g#5:2 e5 d5 | c5:2 e5 g5 b5:2 a5 g5 | a5:4 f#5:2 r:2',
           vel=100, transpose=1)
    melody(s, 4, 8, 'f5:2 ab5 c6 eb6:3 c6 | bb5:3 g5 eb5:2 bb5:2 | db6:3 c6 bb5:2 f5:2 | eb5 f5 ab5 bb5 c6:4',
           vel=100)
    s.groove(0, 12, k='x.......x.x.....', s='....x.......x...', g='..........x....x',
             h='x.xxx.xxx.xxx.xo', hv=0.8)
    return s


# ================================================================== FIN : VICTOIRE
def end_win():
    s = Song('end_win', 'Fin de partie, victoire : le thème du titre en triomphe feutré, cuivres',
             bpm=92, bars=12, swing=0.12, lfo=2, gain=0, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'horn')
    prog = P('Bbmaj7 A7 Dm9 Cm7:2 F7:2 Bbmaj7 A7 Dm9 Gm7:2 C7sus4:2 Bbmaj7 C9sus4 Fmaj9 '
             'Fmaj9:2 C7sus4:2')
    s.bassline(0, 0, prog, bass_citypop, vel=110)
    s.comp((1, 2, 3), 0, prog, rh_citypop, vel=78, lo=53, hi=74)
    melody(s, 4, 0, 'r:1 d5 f5 a5:3 g5 f5 | e5:2 c#5 e5 g5:3 r:1 | f5:3 e5 d5 c5 a4:2 | bb4:2 c5 eb5:2 d5 c5 a4 |'
                    'r:1 d5 f5 a5:2 c6 a5 g5 | a5:3 g5 e5:2 c#5 e5 | d5:4 r:1 a4 c5 e5 | f5:3 d5 c5:4 |'
                    'd6:3 c6 a5:2 f5:2 | g5:3 f5 d5:2 c5:2 | a5:3 c6 e6:4 | f6:4 r:2 c6 bb5', vel=104)
    s.groove(0, 12, k='x.........x..x..', s='....x.......x...', g='.......x.......x', h='X.x.X.x.X.x.X.xo')
    return s


# ================================================================== FIN : DÉFAITE
def end_lose():
    s = Song('end_lose', 'Fin de partie, défaite : le thème du titre lent et mineur, balais',
             bpm=70, bars=10, swing=0.1, lfo=1, gain=-1, ch_vol=[0, 2, 2, 6, 2])
    s.use(0, 'upright')
    s.use(1, 'pad')
    s.use(2, 'pad')
    s.use(3, 'rhodes')
    s.use(4, 'vibes')
    prog = P('Bbmaj7 A7b9 Dm9 Dm9/C Gm9 A7sus4:2 A7b9:2 Dm9 Bbmaj7 Gm9 A7sus4')
    s.bassline(0, 0, prog, bass_halves, vel=102)
    s.comp((1, 2), 0, prog, rh_hold, vel=72, gate=0.98, lo=55, hi=72)
    arp(s, 3, 0, prog, (0, 1, 2, 3, 2, 1, 2, 1), step=0.5, vel=54, lo=52, hi=76)
    melody(s, 4, 0, 'r:1 d5 f5 a5:3 g5 f5 | e5:2 c#5 e5 bb5:3 r:1 | f5:3 e5 d5 c5 a4:2 | e5:6 r:2 |'
                    'd5:3 bb4 a4:2 g4:2 | a4:4 c#5:2 e5:2 | f5:4 e5:2 d5:2 | d5:3 c5 a4:4 |'
                    'bb4:3 a4 g4:2 f4:2 | e5:6 r:2', vel=96)
    s.groove(0, 10, k='x...............', extra={'brush': '....x.......x...'})
    return s


# ================================================================== MEILLEURS SCORES
def scores():
    s = Song('scores', 'Meilleurs scores : arcade chill, arpèges de cloche en doubles-croches, fa majeur',
             bpm=96, bars=12, swing=0.1, lfo=2, gain=0, ch_vol=[0, 5, 5, 8, 0])
    s.use(0, 'ebass')
    s.use(1, 'rhodes')
    s.use(2, 'rhodes')
    s.use(3, 'bell')
    s.use(4, 'vibes')
    prog = P('Fmaj7 A7 Dm7 Bb6 Gm7 C7 Am7:2 D7b9:2 Gm7:2 C7:2 Fmaj7 A7 Bbmaj7 C9sus4')
    s.bassline(0, 0, prog, bass_citypop, vel=106)
    s.comp((1, 2), 0, prog, rh_citypop, vel=76, lo=55, hi=74)
    arp(s, 3, 0, prog, (0, 1, 2, 3), step=0.25, vel=50, lo=67, hi=88, gate=0.7)
    melody(s, 4, 0, 'c5 f5 a5 c6 e6:2 c6:2 | c#6:3 a5 e5:2 g5:2 | f5 a5 d6 f6 e6:2 d6:2 | d6:3 c6 bb5:2 g5:2 |'
                    'bb4 d5 g5 bb5 a5:2 f5:2 | e5:3 g5 bb5:2 c6:2 | c6:2 a5 e5 f#5:2 eb6:2 | d6:2 bb5:2 e5:2 c5:2 |'
                    'c5 f5 a5 c6 e6:2 c6:2 | e6:3 c#6 a5:2 g5:2 | f5 bb5 d6 f6 a6:2 f6:2 | d6:4 c6:4', vel=102)
    s.groove(0, 12, k='x.........x.....', s='....x.......x...', h='x.x.x.x.x.x.x.xo', hv=0.85)
    return s


# ================================================================== AIDE
def help_theme():
    s = Song('help', 'Aide : étude calme lo-fi en do, II-V-I, beaucoup d\'espace',
             bpm=76, bars=12, swing=0.25, lfo=2, gain=-1, ch_vol=[0, 5, 5, 5, 0])
    s.use(0, 'ebass')
    for c in (1, 2, 3):
        s.use(c, 'rhodes')
    s.use(4, 'vibes')
    prog = P('Dm9 G13 Cmaj9 Am9 Dm9 G13 Em7 A7b9 Dm9 G13 Cmaj9:2 A7b9:2 Dm9:2 G13:2')
    s.bassline(0, 0, prog, bass_lofi, vel=104)
    s.comp((1, 2, 3), 0, prog, rh_lofi_hold, vel=72, lo=53, hi=74)
    melody(s, 4, 0, 'r:2 a4 c5 e5:4 | d5:2 b4:2 e5:4 | r:2 g4 b4 d5:4 | c5:3 b4 g4:4 |'
                    'r:2 f5 e5 c5:4 | b4:2 d5:2 e5:4 | r:2 b4 d5 g5:4 | e5:3 c#5 bb4:4 |'
                    'r:2 a4 c5 f5:4 | e5:3 d5 b4:4 | d5:2 e5:2 c#5:4 | c5:4 b4:4', vel=98)
    s.groove(0, 12, k='x.......x.x.....', s='....x.......x...', g='.......x.......x',
             h='x.x.x.x.x.x.x.x.', hv=0.75)
    return s


SONGS = [title, year1, report, year2, year3, year4, year5, election, event, assembly, map_theme,
         budget, referendum, reshuffle, impeach, campaign, mandate2, end_win, end_lose, scores, help_theme]
