#!/usr/bin/env python3
"""Génère res/sfx/forsure.wav (8 bits non signé, mono, 13400 Hz) pour le driver Z80 PCM de SGDK.

Voix : synthèse Windows SAPI (voix française lisant « For sure! », accent assumé),
appelée via powershell.exe. Aucune voix réelle n'est imitée.
Usage : python3 tools/voice/make_voice.py [texte]
Sans argument, on réutilise tools/voice/raw.wav (versionné) s'il existe ;
avec un texte, la synthèse est relancée.
"""
import os, struct, subprocess, sys, wave

RATE = 13400                      # = SOUND_PCM_RATE_13400
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
RAW = os.path.join(HERE, "raw.wav")
OUT = os.path.join(ROOT, "res", "sfx", "forsure.wav")
TEXT = sys.argv[1] if len(sys.argv) > 1 else "For sure!"

def synth():
    ps1 = subprocess.check_output(["wslpath", "-w", os.path.join(HERE, "say.ps1")]).decode().strip()
    out = subprocess.check_output(["wslpath", "-w", RAW]).decode().strip()
    subprocess.check_call(["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass",
                           "-File", ps1, "-Text", TEXT, "-Out", out])

def read(path):
    with wave.open(path, "rb") as w:
        n, ch, sw, sr = w.getnframes(), w.getnchannels(), w.getsampwidth(), w.getframerate()
        data = w.readframes(n)
    assert sw == 2, "PCM 16 bits attendu"
    v = struct.unpack("<%dh" % (len(data) // 2), data)
    if ch == 2:
        v = [(v[i] + v[i + 1]) // 2 for i in range(0, len(v), 2)]
    return list(v), sr

def resample(v, sr, dr):
    # Moyenne glissante (passe-bas simple) puis interpolation linéaire.
    if sr > dr:
        k = max(1, sr // dr)
        s = [sum(v[max(0, i - k):i + k + 1]) / (2 * k + 1) for i in range(len(v))]
    else:
        s = v
    n = int(len(s) * dr / sr)
    o = []
    for i in range(n):
        x = i * sr / dr
        j = int(x); f = x - j
        a = s[j]; b = s[min(j + 1, len(s) - 1)]
        o.append(a + (b - a) * f)
    return o

def trim(v, thr=0.03):
    t = max(abs(x) for x in v) * thr
    a = next(i for i, x in enumerate(v) if abs(x) > t)
    b = len(v) - next(i for i, x in enumerate(reversed(v)) if abs(x) > t)
    return v[max(0, a - 100):min(len(v), b + 100)]

def main():
    if not os.path.exists(RAW) or len(sys.argv) > 1:
        synth()
    v, sr = read(RAW)
    v = resample(v, sr, RATE)
    v = trim(v)
    # pré-emphase légère : parole plus claire à 13 kHz / 8 bits
    v = [v[0]] + [v[i] + 0.4 * (v[i] - v[i - 1]) for i in range(1, len(v))]
    p = max(abs(x) for x in v)
    v = [x * 0.98 * 127 / p for x in v]
    n = len(v)
    for i in range(min(200, n)):      # fondu de sortie court
        v[n - 1 - i] *= i / 200
    n256 = (len(v) + 255) // 256 * 256  # longueur multiple de 256 exigée par le driver
    v += [0.0] * (n256 - len(v))
    pcm = bytes(max(0, min(255, int(round(x)) + 128)) for x in v)
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with wave.open(OUT, "wb") as w:
        w.setnchannels(1); w.setsampwidth(1); w.setframerate(RATE)
        w.writeframes(pcm)
    print("%s : %d octets, %.2f s" % (OUT, len(pcm), len(pcm) / RATE))

main()
