"""Composition en code et export VGM (YM2612 + DAC en flux + PSG) pour XGM2.

Une chanson (Song) contient :
- 5 voies FM (0 à 4) ; la 6e voie FM est le DAC (percussions PCM, canal PCM 1 de XGM2) ;
- des coups de percussions PCM (échantillons de pcm.py) ;
- des charleys / shakers sur le bruit du PSG (canal 3).

Le temps est exprimé en temps (noires) ; le VGM est cadencé à 60 images/s (le
pilote XGM2 adapte la cadence en PAL : le tempo est identique en PAL et NTSC).
"""
import itertools
import math
import struct

from fmlib import BANK, SLOT_OFFSET

FPS = 60
SAMPLES_PER_FRAME = 44100 // FPS      # 735
YM_CLOCK = 7670453
PSG_CLOCK = 3579545
PCM_RATE = 13300

NOTE_BASE = {'c': 0, 'd': 2, 'e': 4, 'f': 5, 'g': 7, 'a': 9, 'b': 11}


def midi(name):
    """'C4' -> 60, 'F#3', 'Bb2', 'eb5'."""
    if isinstance(name, int):
        return name
    s = name.strip().lower()
    n = NOTE_BASE[s[0]]
    i = 1
    while i < len(s) and s[i] in '#b':
        n += 1 if s[i] == '#' else -1
        i += 1
    octave = int(s[i:])
    return 12 * (octave + 1) + n


# ------------------------------------------------------------------ accords
CHORD_TYPES = {
    '': (0, 4, 7), 'maj': (0, 4, 7), 'm': (0, 3, 7), '6': (0, 4, 7, 9), 'm6': (0, 3, 7, 9),
    'maj7': (0, 4, 7, 11), '7': (0, 4, 7, 10), 'm7': (0, 3, 7, 10), 'm7b5': (0, 3, 6, 10),
    'dim': (0, 3, 6), 'dim7': (0, 3, 6, 9), 'aug': (0, 4, 8),
    'maj9': (0, 4, 7, 11, 14), 'm9': (0, 3, 7, 10, 14), '9': (0, 4, 7, 10, 14),
    '13': (0, 4, 10, 14, 21), '7b9': (0, 4, 7, 10, 13), '7#9': (0, 4, 10, 15),
    'sus4': (0, 5, 7), 'sus2': (0, 2, 7), '7sus4': (0, 5, 7, 10), 'sus': (0, 5, 7, 10),
    '9sus4': (0, 5, 10, 14), 'add9': (0, 4, 7, 14), 'madd9': (0, 3, 7, 14),
    '69': (0, 4, 9, 14), 'm69': (0, 3, 9, 14), 'm11': (0, 3, 10, 14, 17),
    'maj7#11': (0, 4, 11, 14, 18), 'mmaj7': (0, 3, 7, 11), '5': (0, 7),
}


def parse_chord(sym):
    """'Fmaj7' -> (racine pc, intervalles, basse pc). 'C/E' : basse sur mi."""
    bass = None
    if '/' in sym:
        sym, b = sym.split('/')
        bass = midi(b + '0') % 12
    root = NOTE_BASE[sym[0].lower()]
    i = 1
    while i < len(sym) and sym[i] in '#b':
        root += 1 if sym[i] == '#' else -1
        i += 1
    q = sym[i:]
    if q not in CHORD_TYPES:
        raise ValueError('accord inconnu : ' + sym)
    root %= 12
    return root, CHORD_TYPES[q], (bass if bass is not None else root)


def chord_pcs(sym, voices):
    """Classes de hauteur à jouer à « voices » voix (voicings sans fondamentale si possible)."""
    root, iv, _ = parse_chord(sym)
    tones = list(dict.fromkeys(iv))
    if len(tones) > voices and 0 in tones:
        tones.remove(0)
    if len(tones) > voices and 7 in tones:
        tones.remove(7)
    while len(tones) > voices:
        tones.pop()          # retire l'extension la plus haute
    while len(tones) < voices:
        tones.append(tones[len(tones) % len(tones)] + 12)
    return [(root + t) for t in tones]


def voice_chord(sym, voices, prev=None, lo=52, hi=76, center=63):
    """Choisit les hauteurs MIDI des voix en minimisant les mouvements depuis « prev »."""
    pcs = chord_pcs(sym, voices)
    options = []
    for pc in pcs:
        opts = [m for m in range(lo, hi + 1) if m % 12 == pc % 12]
        options.append(opts)
    best, best_cost = None, 1e9
    for combo in itertools.product(*options):
        notes = sorted(combo)
        if len(set(notes)) < len(notes):
            continue
        spread = notes[-1] - notes[0]
        if spread > 16:
            continue
        cost = 0.0
        for a, b in zip(notes, notes[1:]):
            if b - a < 2:
                cost += 6                       # seconde mineure serrée : dur
            if a < 55 and b - a < 4:
                cost += 4                       # grave et serré : boueux
        if prev:
            cost += sum(abs(a - b) for a, b in zip(notes, sorted(prev))) * 1.0
        else:
            cost += abs(sum(notes) / len(notes) - center) * 0.6
        cost += abs(sum(notes) / len(notes) - center) * 0.15
        if cost < best_cost:
            best, best_cost = notes, cost
    return best


def bass_root(sym, lo=33):
    root, _, bass = parse_chord(sym)
    m = lo + ((bass - lo) % 12)
    return m


# ------------------------------------------------------------------ chanson
class Song:
    def __init__(self, name, desc, bpm, bars, beats_per_bar=4, swing=0.0, swing8=0.0,
                 lfo=None, gain=0, ch_vol=None):
        self.name = name
        self.desc = desc
        self.bpm = bpm
        self.bars = bars
        self.bpb = beats_per_bar
        self.swing = swing      # retard des doubles-croches impaires (fraction de double)
        self.swing8 = swing8    # retard des croches impaires (fraction de croche ; 1/3 = ternaire)
        self.lfo = lfo          # None ou 0..7 (fréquence du LFO global)
        self.gain = gain        # atténuation globale ajoutée (pas de 0,75 dB, + = plus doux)
        self.ch_vol = ch_vol or [0, 0, 0, 0, 0]
        self.inst = [None] * 5
        self.notes = [[] for _ in range(5)]
        self.drums = []
        self.hats = []
        self.chord_track = []   # (temps, accord) pour référence

    # ---- temps
    @property
    def length(self):
        return self.bars * self.bpb

    def bar(self, n):
        return n * self.bpb

    def swung(self, beat):
        b = beat
        q = b * 4
        if abs(q - round(q)) < 1e-6 and int(round(q)) % 2 == 1:
            b += self.swing * 0.25
        e = beat * 2
        if abs(e - round(e)) < 1e-6 and int(round(e)) % 2 == 1:
            b += self.swing8 * 0.5
        return b

    def frame(self, beat):
        return int(round(self.swung(beat) * 60.0 * FPS / self.bpm))

    def total_frames(self):
        return int(round(self.length * 60.0 * FPS / self.bpm))

    # ---- écriture
    def use(self, ch, inst):
        self.inst[ch] = BANK[inst] if isinstance(inst, str) else inst

    def n(self, ch, beat, dur, pitch, vel=96, gate=0.92, inst=None, cents=0):
        if beat >= self.length - 1e-9:
            return
        ins = BANK[inst] if isinstance(inst, str) else inst
        self.notes[ch].append((beat, dur, midi(pitch), vel, ins, gate, cents))

    def mel(self, ch, start, text, unit=0.5, vel=96, gate=0.92, inst=None, transpose=0):
        """Mélodie : « e5:2 g5 r:1 c#6:3! a4? » ; durée en unités (défaut croche).
        ! accent (+16), ? note douce (-24), ~ legato (gate 1.0). Retourne le temps de fin."""
        t = start
        for tok in text.split():
            v, g = vel, gate
            while tok[-1] in '!?~':
                if tok[-1] == '!':
                    v = min(127, v + 16)
                elif tok[-1] == '?':
                    v = max(20, v - 24)
                else:
                    g = 1.0
                tok = tok[:-1]
            if ':' in tok:
                name, d = tok.split(':')
                d = float(d)
            else:
                name, d = tok, 1.0
            if name.lower() != 'r':
                self.n(ch, t, d * unit, midi(name) + transpose, v, g, inst)
            t += d * unit
        return t

    def comp(self, chs, start, prog, rhythm, vel=80, gate=0.9, inst=None, lo=52, hi=76,
             center=63, transpose=0, strum=0.0):
        """Accords sur les voies « chs ». prog : [(accord, temps)]. rhythm(durée) -> [(décalage,
        durée, vélocité relative)] ; ou liste fixe répétée par mesure. strum : décalage entre voix."""
        t = start
        prev = None
        for sym, beats in prog:
            notes = voice_chord(sym, len(chs), prev, lo, hi, center)
            prev = notes
            self.chord_track.append((t, sym))
            hits = rhythm(beats) if callable(rhythm) else rhythm
            for off, d, rv in hits:
                if off >= beats:
                    continue
                d = min(d, beats - off)
                for k, (ch, m) in enumerate(zip(chs, notes)):
                    self.n(ch, t + off + k * strum, d - k * strum, m + transpose,
                           int(vel * rv), gate, inst)
            t += beats
        return t

    def bassline(self, ch, start, prog, pattern, vel=104, gate=0.85, inst=None, lo=33, transpose=0):
        """Basse. pattern(beats) -> [(décalage, durée, degré, vélocité relative)] ; degrés :
        'R' fondamentale, '5', '8' octave, '3' tierce de l'accord, 'b7', '7', 'r5' quinte grave,
        '6', '9', 'A' approche chromatique de l'accord suivant (par en dessous), 'Ah' par au-dessus,
        ou un entier (demi-tons depuis la fondamentale)."""
        t = start
        items = list(prog)
        for idx, (sym, beats) in enumerate(items):
            root = bass_root(sym, lo) + transpose
            _, iv, _ = parse_chord(sym)
            nxt = items[(idx + 1) % len(items)][0]
            nroot = bass_root(nxt, lo) + transpose
            third = 3 if 3 in iv else 4
            sev = 10 if 10 in iv else 11
            for off, d, deg, rv in pattern(beats):
                if off >= beats:
                    continue
                d = min(d, beats - off)
                if isinstance(deg, int):
                    m = root + deg
                else:
                    m = {
                        'R': root, '5': root + (6 if 6 in iv and 7 not in iv else 7), '8': root + 12,
                        '3': root + third, 'b7': root + 10, '7': root + sev, 'r5': root - 5,
                        '6': root + 9, '9': root + 14, '4': root + 5, '2': root + 2,
                        'A': nroot - 1 if nroot - 1 != root else nroot + 1,
                        'Ah': nroot + 1, 'N': nroot,
                    }[deg]
                self.n(ch, t + off, d, m, int(vel * rv), gate, inst)
            t += beats
        return t

    def drum(self, beat, name):
        if beat < self.length - 1e-9:
            self.drums.append((beat, name))

    def hat(self, beat, kind='c', vel=1.0):
        if beat < self.length - 1e-9:
            self.hats.append((beat, kind, vel))

    def groove(self, start_bar, nbars, k='', s='', h='', g='', r='', extra=None, hv=1.0):
        """Batterie sur une grille de 16 doubles-croches par mesure (chaînes répétées) :
        k grosse caisse, s caisse claire, g caisse fantôme, r rimshot, h charley PSG
        (x fermé, o ouvert, s shaker, . rien ; majuscule = accent). extra : {lettre: échantillon}."""
        lines = {'kick': k, 'snare': s, 'ghost': g, 'rim': r}
        if extra:
            lines.update(extra)
        steps = self.bpb * 4
        for b in range(start_bar, start_bar + nbars):
            base = self.bar(b)
            for name, pat in lines.items():
                if not pat:
                    continue
                p = pat.replace(' ', '').replace('|', '')
                for i in range(steps):
                    c = p[(i + (b - start_bar) * steps) % len(p)]
                    if c in 'xX':
                        self.drum(base + i * 0.25, name)
            if h:
                p = h.replace(' ', '').replace('|', '')
                for i in range(steps):
                    c = p[(i + (b - start_bar) * steps) % len(p)]
                    if c == '.':
                        continue
                    acc = 1.0 if c.isupper() else 0.7
                    kind = {'x': 'c'}.get(c.lower(), c.lower())
                    self.hat(base + i * 0.25, kind, acc * hv)


# ------------------------------------------------------------------ export VGM
def fm_freq(m, cents=0.0):
    f = 440.0 * 2 ** ((m - 69 + cents / 100.0) / 12.0)
    for block in range(8):
        fnum = int(round(f * (1 << (21 - block)) / (YM_CLOCK / 144.0)))
        if fnum < 1300 or block == 7:
            return block, max(1, min(2047, fnum))
    return 7, 2047


HAT_ENV = {   # atténuations PSG image par image (15 = muet)
    'c': (4, 7, 10, 13, 15),
    'o': (4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15),
    's': (8, 6, 7, 9, 11, 13, 15),
}


class VgmWriter:
    def __init__(self):
        self.cmds = bytearray()
        self.loop_off = None
        self.samples = 0

    def ym(self, port, reg, val):
        self.cmds += bytes((0x52 + port, reg & 0xFF, val & 0xFF))

    def psg(self, val):
        self.cmds += bytes((0x50, val & 0xFF))

    def wait_frames(self, n):
        while n > 0:
            k = min(n, 65535 // SAMPLES_PER_FRAME)
            if k == 1:
                self.cmds.append(0x62)
            else:
                self.cmds += b'\x61' + struct.pack('<H', k * SAMPLES_PER_FRAME)
            self.samples += k * SAMPLES_PER_FRAME
            n -= k

    def mark_loop(self):
        self.loop_off = len(self.cmds)
        self.loop_samples_start = self.samples

    def build(self):
        header = bytearray(0x100)
        data = bytes(self.cmds) + b'\x66'
        header[0:4] = b'Vgm '
        struct.pack_into('<I', header, 0x04, 0x100 + len(data) - 4)
        struct.pack_into('<I', header, 0x08, 0x160)
        struct.pack_into('<I', header, 0x0C, PSG_CLOCK)
        struct.pack_into('<I', header, 0x18, self.samples)
        if self.loop_off is not None:
            struct.pack_into('<I', header, 0x1C, 0x100 + self.loop_off - 0x1C)
            struct.pack_into('<I', header, 0x20, self.samples - self.loop_samples_start)
        struct.pack_into('<I', header, 0x24, FPS)
        struct.pack_into('<H', header, 0x28, 0x0009)
        header[0x2A] = 16
        struct.pack_into('<I', header, 0x2C, YM_CLOCK)
        struct.pack_into('<I', header, 0x34, 0x100 - 0x34)
        return bytes(header) + data


def export_vgm(song, pcm_bank):
    """Convertit une chanson en VGM. pcm_bank : {nom: octets PCM non signés 13,3 kHz}."""
    w = VgmWriter()
    total = song.total_frames()
    # --- en-tête musical (avant le point de boucle)
    used = sorted({name for _, name in song.drums})
    offsets, block = {}, bytearray()
    for name in used:
        offsets[name] = (len(block), len(pcm_bank[name]))
        block += pcm_bank[name]
    w.ym(0, 0x22, 0x08 | song.lfo if song.lfo is not None else 0x00)
    w.ym(0, 0x27, 0x00)
    for ch in range(6):                                  # tout couper proprement
        w.ym(0, 0x28, (ch if ch < 3 else ch + 1))
    w.ym(0, 0x2B, 0x80)                                  # DAC actif (voie 6 = percussions PCM)
    w.ym(1, 0xB6, 0xC0)                                  # DAC au centre
    for c in range(4):
        w.psg(0x9F | (c << 5))
    if block:
        w.cmds += b'\x67\x66\x00' + struct.pack('<I', len(block)) + bytes(block)
        w.cmds += bytes((0x90, 0x00, 0x02, 0x00, 0x2A))
        w.cmds += bytes((0x91, 0x00, 0x00, 0x01, 0x00))
        w.cmds += b'\x92\x00' + struct.pack('<I', PCM_RATE)
    w.mark_loop()

    # --- événements par image : [ (ordre, fonction d'écriture) ]
    events = {}

    def add(frame, order, fn):
        events.setdefault(frame, []).append((order, fn))

    for ch in range(5):
        port, c = (0, ch) if ch < 3 else (1, ch - 3)
        keych = ch if ch < 3 else ch + 1
        notes = sorted(song.notes[ch], key=lambda e: e[0])
        state = {'inst': None, 'tl': {}}
        sched = []
        for i, (beat, dur, m, vel, ins, gate, cents) in enumerate(notes):
            ins = ins or song.inst[ch]
            if ins is None:
                raise ValueError('%s : voie %d sans instrument' % (song.name, ch))
            f_on = song.frame(beat)
            f_off = song.frame(beat + dur * gate) if gate < 1.0 else song.frame(beat + dur)
            if f_off <= f_on:
                f_off = f_on + 1
            if i + 1 < len(notes):
                nxt = song.frame(notes[i + 1][0])
                f_off = min(f_off, nxt - 1)
            f_off = min(f_off, total - 1)
            if f_on >= total or f_off <= f_on:
                continue
            sched.append((f_on, f_off, m, vel, ins, cents))
        for f_on, f_off, m, vel, ins, cents in sched:
            regs = []
            if state['inst'] is not ins:
                state['inst'] = ins
                state['tl'] = {}
                regs.append((0xB0 + c, (ins.fb << 3) | ins.alg))
                regs.append((0xB4 + c, 0xC0 | (ins.ams << 4) | ins.fms))
                for i in range(4):
                    for base, val in ins.op_regs(i, 127).items():
                        if base != 0x40:
                            regs.append((base + SLOT_OFFSET[i] + c, val))
            att = int(round(20 * math.log10(127.0 / max(1, vel)) / 0.75))
            for i in range(4):
                tl = ins.ops[i]['tl']
                if i in ins.carriers():
                    tl += ins.level + att + song.gain + song.ch_vol[ch]
                else:
                    tl += int(round((127 - vel) / 127.0 * ins.vel_bright))
                tl = max(0, min(127, tl))
                if state['tl'].get(i) != tl:
                    state['tl'][i] = tl
                    regs.append((0x40 + SLOT_OFFSET[i] + c, tl))
            block_, fnum = fm_freq(m + 12 * ins.octave, cents + ins.detune_cents)
            regs.append((0xA4 + c, (block_ << 3) | (fnum >> 8)))
            regs.append((0xA0 + c, fnum & 0xFF))

            def on(regs=regs, port=port, keych=keych):
                for r, v in regs:
                    w.ym(port, r, v)
                w.ym(0, 0x28, 0xF0 | keych)

            def off(keych=keych):
                w.ym(0, 0x28, keych)
            add(f_on, 1, on)
            add(f_off, 0, off)

    for beat, name in song.drums:
        f = song.frame(beat)
        if f >= total:
            continue
        o, ln = offsets[name]

        def hit(o=o, ln=ln):
            w.cmds += b'\x93\x00' + struct.pack('<I', o) + b'\x01' + struct.pack('<I', ln)
        add(f, 2, hit)

    # charleys PSG : enveloppes image par image (un nouveau coup remplace le précédent)
    # ne garder pour chaque image que le coup le plus récent
    cleaned = {}
    for beat, kind, vel in sorted(song.hats):
        f0 = song.frame(beat)
        boost = int(round((1.0 - vel) * 6))
        for k, a in enumerate(HAT_ENV[kind]):
            fr = f0 + k
            if fr >= total:
                break
            cleaned[fr] = (min(15, a + boost + (0 if a == 15 else max(0, song.gain) // 3)), kind, k == 0)
    last_kind = [None]
    for fr, (att, kind, first) in sorted(cleaned.items()):
        def psgw(att=att, kind=kind, first=first):
            if first:
                rate = 0 if kind != 's' else 1
                if last_kind[0] != rate:
                    w.psg(0xE4 | rate)
                    last_kind[0] = rate
            w.psg(0xF0 | att)
        add(fr, 3, psgw)

    # --- émission
    cur = 0
    for fr in sorted(events):
        if fr > cur:
            w.wait_frames(fr - cur)
            cur = fr
        for _, fn in sorted(events[fr], key=lambda e: e[0]):
            fn()
    if total > cur:
        w.wait_frames(total - cur)
    return w.build()
