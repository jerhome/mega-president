"""Échantillons PCM synthétisés (percussions de la musique, bruitages des menus).

Tout est calculé ici, de façon reproductible (bruit pseudo-aléatoire à graine fixe) :
aucun fichier audio externe. Format : 8 bits non signés, mono, 13 300 Hz (XGM2).
"""
import math
import struct

RATE = 13300


class Noise:
    def __init__(self, seed):
        self.s = seed & 0xFFFFFFFF

    def __call__(self):
        self.s = (self.s * 1103515245 + 12345) & 0x7FFFFFFF
        return (self.s >> 8) / float(1 << 22) - 1.0


def lowpass(x, alpha):
    y, out = 0.0, []
    for v in x:
        y += alpha * (v - y)
        out.append(y)
    return out


def highpass(x, alpha):
    lp = lowpass(x, alpha)
    return [a - b for a, b in zip(x, lp)]


def normalize(x, peak):
    m = max(1e-9, max(abs(v) for v in x))
    return [v * peak / m for v in x]


def to_u8(x, pad=256):
    out = bytearray()
    for v in x:
        v = max(-1.0, min(1.0, v))
        out.append(max(0, min(255, int(round(128 + v * 127)))))
    while len(out) % pad:
        out.append(128)
    return bytes(out)


def soft_clip(x, drive=1.4):
    t = math.tanh(drive)
    return [math.tanh(drive * v) / t for v in x]


def env_exp(n, tau, attack=0.002):
    a = max(1, int(attack * RATE))
    return [min(1.0, i / a) * math.exp(-i / (tau * RATE)) for i in range(n)]


# ------------------------------------------------------------------ percussions
def kick():
    n = int(0.30 * RATE)
    ph, out, nz = 0.0, [], Noise(1)
    for i in range(n):
        t = i / RATE
        f = 47 + 78 * math.exp(-t / 0.028)
        ph += 2 * math.pi * f / RATE
        amp = min(1.0, t / 0.002) * math.exp(-t / 0.15)
        out.append(math.sin(ph) * amp + nz() * 0.10 * math.exp(-t / 0.003))
    return normalize(soft_clip(lowpass(out, 0.6)), 0.62)


def snare(level=0.52, length=0.24, seed=2):
    n = int(length * RATE)
    nz = Noise(seed)
    noise = lowpass([nz() for _ in range(n)], 0.45)
    out = []
    for i in range(n):
        t = i / RATE
        body = math.sin(2 * math.pi * 186 * t) * math.exp(-t / 0.045) * 0.55
        body += math.sin(2 * math.pi * 330 * t) * math.exp(-t / 0.03) * 0.2
        out.append(body + noise[i] * 0.85 * math.exp(-t / 0.085) * min(1.0, t / 0.001))
    return normalize(out, level)


def ghost():
    return snare(0.20, 0.12, 3)


def rim():
    n = int(0.09 * RATE)
    nz = Noise(4)
    out = []
    for i in range(n):
        t = i / RATE
        v = math.sin(2 * math.pi * 1650 * t) * math.exp(-t / 0.009)
        v += math.sin(2 * math.pi * 820 * t) * math.exp(-t / 0.016) * 0.6
        v += nz() * math.exp(-t / 0.004) * 0.35
        out.append(v)
    return normalize(out, 0.36)


def typewriter():
    """Frappe de machine à écrire : clic sec puis retour de la barre."""
    n = int(0.07 * RATE)
    nz = Noise(5)
    out = []
    for i in range(n):
        t = i / RATE
        v = nz() * math.exp(-t / 0.0025) + math.sin(2 * math.pi * 310 * t) * math.exp(-t / 0.008) * 0.5
        t2 = t - 0.024
        if t2 > 0:
            v += nz() * math.exp(-t2 / 0.002) * 0.45
        out.append(v)
    return normalize(highpass(out, 0.08), 0.40)


def ding():
    """Clochette du retour chariot."""
    n = int(0.85 * RATE)
    out = []
    for i in range(n):
        t = i / RATE
        e = min(1.0, t / 0.001)
        v = math.sin(2 * math.pi * 2093 * t) * math.exp(-t / 0.30)
        v += math.sin(2 * math.pi * 5270 * t) * math.exp(-t / 0.09) * 0.35
        v += math.sin(2 * math.pi * 3140 * t) * math.exp(-t / 0.15) * 0.15
        out.append(v * e)
    return normalize(out, 0.30)


def tom():
    n = int(0.40 * RATE)
    ph, out = 0.0, []
    for i in range(n):
        t = i / RATE
        f = 92 + 38 * math.exp(-t / 0.04)
        ph += 2 * math.pi * f / RATE
        out.append(math.sin(ph) * min(1.0, t / 0.002) * math.exp(-t / 0.22))
    return normalize(out, 0.50)


def brush():
    n = int(0.26 * RATE)
    nz = Noise(6)
    noise = lowpass([nz() for _ in range(n)], 0.32)
    out = [noise[i] * min(1.0, (i / RATE) / 0.015) * math.exp(-(i / RATE) / 0.10) for i in range(n)]
    return normalize(out, 0.26)


def drum_bank():
    return {name: to_u8(fn()) for name, fn in (
        ('kick', kick), ('snare', snare), ('ghost', ghost), ('rim', rim),
        ('type', typewriter), ('ding', ding), ('tom', tom), ('brush', brush))}


# ------------------------------------------------------------------ bruitages
def tone(freq, dur, tau, partials=((1, 1.0, 1.0), (4, 0.22, 0.25), (10, 0.06, 0.08)), attack=0.002):
    """Note « vibraphone » : partiels (rapport, amplitude, facteur de durée)."""
    n = int(dur * RATE)
    out = [0.0] * n
    for ratio, amp, tf in partials:
        f = freq * ratio
        if f > RATE * 0.45:
            continue
        for i in range(n):
            t = i / RATE
            out[i] += amp * math.sin(2 * math.pi * f * t) * math.exp(-t / (tau * tf))
    a = max(1, int(attack * RATE))
    rel = int(0.006 * RATE)
    for i in range(n):
        g = min(1.0, i / a)
        if i > n - rel:
            g *= (n - i) / rel
        out[i] *= g
    return out


def seq(notes, total=None):
    """notes : [(début s, fréquence, durée s, tau, amplitude)] mélangées."""
    end = max(s + d for s, _, d, _, _ in notes)
    n = int((total or end) * RATE) + 1
    out = [0.0] * n
    for s, f, d, tau, amp in notes:
        x = tone(f, d, tau)
        o = int(s * RATE)
        for i, v in enumerate(x):
            if o + i < n:
                out[o + i] += v * amp
    return out


def hz(name):
    from compose import midi
    return 440.0 * 2 ** ((midi(name) - 69) / 12.0)


def sfx_bank():
    S = {}
    S['move'] = normalize(tone(hz('E6'), 0.035, 0.012, ((1, 1, 1), (3, 0.15, 0.5))), 0.40)
    S['ok'] = normalize(seq([(0, hz('E6'), 0.09, 0.05, 1), (0.06, hz('A6'), 0.16, 0.07, 1)]), 0.70)
    S['cancel'] = normalize(seq([(0, hz('A5'), 0.09, 0.05, 1), (0.06, hz('E5'), 0.16, 0.07, 1)]), 0.65)
    S['error'] = normalize(lowpass(seq([(0, hz('G3'), 0.11, 0.05, 1), (0.13, hz('F#3'), 0.14, 0.06, 1)]), 0.5), 0.75)
    S['adopted'] = normalize(seq([(0.00, hz('C6'), 0.20, 0.10, 1), (0.08, hz('E6'), 0.20, 0.10, 1),
                                  (0.16, hz('G6'), 0.20, 0.10, 1), (0.24, hz('C7'), 0.40, 0.18, 1)]), 0.72)
    S['rejected'] = normalize(seq([(0.00, hz('G5'), 0.20, 0.10, 1), (0.09, hz('Eb5'), 0.20, 0.10, 1),
                                   (0.18, hz('C5'), 0.20, 0.10, 1), (0.27, hz('G4'), 0.45, 0.20, 1)]), 0.72)
    S['event'] = normalize(seq([(0.00, hz('B5'), 0.15, 0.08, 1), (0.12, hz('B5'), 0.15, 0.08, 1),
                                (0.24, hz('E6'), 0.45, 0.22, 1), (0.24, hz('B6'), 0.45, 0.15, 0.3)]), 0.72)
    win = []
    for k, (s, n_, d) in enumerate([(0, 'C5', .15), (.14, 'C5', .15), (.28, 'C5', .15), (.42, 'E5', .3),
                                    (.66, 'C5', .15), (.80, 'E5', .15), (.94, 'G5', .6)]):
        win.append((s, hz(n_), d, 0.25, 1.0))
        win.append((s, hz(n_) * 2, d, 0.20, 0.35))
    S['win'] = normalize(seq(win), 0.78)
    lose = [(0, 'A4', .32), (.30, 'G4', .32), (.60, 'F4', .32), (.90, 'E4', .7)]
    S['lose'] = normalize(seq([(s, hz(n_), d, 0.3, 1.0) for s, n_, d in lose] +
                              [(s, hz(n_) * 1.5, d, 0.25, 0.25) for s, n_, d in lose]), 0.72)
    return S


def write_wav(path, samples_u8_or_float):
    if isinstance(samples_u8_or_float, (bytes, bytearray)):
        data = bytes(samples_u8_or_float)
    else:
        data = to_u8(samples_u8_or_float, pad=1)
    with open(path, 'wb') as f:
        f.write(b'RIFF' + struct.pack('<I', 36 + len(data)) + b'WAVE')
        f.write(b'fmt ' + struct.pack('<IHHIIHH', 16, 1, 1, RATE, RATE, 1, 8))
        f.write(b'data' + struct.pack('<I', len(data)) + data)
