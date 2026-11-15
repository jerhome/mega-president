#!/usr/bin/env python3
"""Rendu d'écoute d'un morceau tel que joué par la ROM (BlastEm + pilote XGM2).

BlastEm (-b N : N images sans affichage, -y : une piste WAV par voie YM2612,
DAC compris) enregistre la partie FM et PCM ; le PSG (charleys) n'est pas
enregistré par BlastEm : il est recalculé ici depuis le VGM et superposé, calé
sur la première note de basse (voie 0).

Usage :
  preview.py --rom out/rom.bin --vgm res/music/title.vgm --out titre.wav [--loops 2] [--region E]
  preview.py --psg-only res/music/v2_year1.vgm --out ancien.wav [--loops 2]
Affiche crête, RMS et écrêtages (analyse des niveaux).
"""
import argparse
import array
import math
import os
import struct
import subprocess
import tempfile

BLASTEM = os.environ.get('BLASTEM', '/home/jerome/claude/mega-president/tools/blastem64-0.6.2/blastem')
YM_RATE = 53267
OUT_RATE = 44100
PSG_CLOCK = 3579545.0
PSG_FULL = 0.20      # crête d'un canal PSG relative à la pleine échelle du mixage


def read_wav16(path):
    with open(path, 'rb') as f:
        d = f.read()
    pos = 12
    while pos < len(d):
        cid, size = d[pos:pos + 4], struct.unpack('<I', d[pos + 4:pos + 8])[0]
        if cid == b'data':
            a = array.array('h')
            a.frombytes(d[pos + 8:pos + 8 + size])
            return a
        pos += 8 + size
    return array.array('h')


def parse_vgm(path):
    d = open(path, 'rb').read()
    start = 0x34 + struct.unpack('<I', d[0x34:0x38])[0]
    lo = struct.unpack('<I', d[0x1C:0x20])[0]
    loop = 0x1C + lo if lo else None
    return d, start, loop


def psg_events(path, loops):
    """[(échantillon 44,1 kHz, octet PSG)] en suivant la boucle du VGM."""
    d, o, loop = parse_vgm(path)
    t, ev, passes = 0, [], 0
    while o < len(d):
        c = d[o]
        o += 1
        if c in (0x52, 0x53):
            o += 2
        elif c == 0x50:
            ev.append((t, d[o]))
            o += 1
        elif c == 0x61:
            t += d[o] | (d[o + 1] << 8)
            o += 2
        elif c == 0x62:
            t += 735
        elif c == 0x63:
            t += 882
        elif 0x70 <= c <= 0x7F:
            t += (c & 15) + 1
        elif c == 0x66:
            if loop and passes < loops - 1:
                passes += 1
                o = loop
                continue
            break
        elif c == 0x67:
            o += 6 + struct.unpack('<I', d[o + 2:o + 6])[0]
        elif c in (0x90, 0x91, 0x95):
            o += 4
        elif c == 0x92:
            o += 5
        elif c == 0x93:
            o += 10
        elif c == 0x94:
            o += 1
        else:
            raise ValueError('commande VGM %02X' % c)
    return ev, t


def render_psg(events, n, rate):
    """PSG SN76489 simplifié rendu à « rate » Hz sur n échantillons (valeurs -1..1)."""
    reg = [0, 15, 0, 15, 0, 15, 0, 15]
    latch = 0
    phase = [0.0] * 4
    lfsr = 0x8000
    noise_out = 1
    out = array.array('f', [0.0]) * n
    ei = 0
    vol = [0.0] * 4
    for i in range(n):
        t44 = i * 44100.0 / rate
        while ei < len(events) and events[ei][0] <= t44:
            v = events[ei][1]
            ei += 1
            if v & 0x80:
                latch = (v >> 4) & 7
                reg[latch] = (reg[latch] & ~0xF) | (v & 0xF)
                if latch == 6:
                    lfsr = 0x8000
            elif latch & 1 or latch == 6:
                reg[latch] = v & 0xF
            else:
                reg[latch] = (reg[latch] & 0xF) | ((v & 0x3F) << 4)
            for c in range(4):
                a = reg[c * 2 + 1]
                vol[c] = 0.0 if a == 15 else PSG_FULL * 10 ** (-a / 10.0)
        s = 0.0
        for c in range(3):
            if vol[c]:
                nper = reg[c * 2] or 1024
                phase[c] += PSG_CLOCK / (32.0 * nper) / rate
                phase[c] -= int(phase[c])
                s += vol[c] * (1 if phase[c] < 0.5 else -1)
        if vol[3]:
            r = reg[6] & 3
            nper = (reg[4] or 1024) if r == 3 else (16 << r)
            phase[3] += PSG_CLOCK / (32.0 * nper) / rate
            while phase[3] >= 1.0:
                phase[3] -= 1.0
                fb = ((lfsr ^ (lfsr >> 3)) & 1) if reg[6] & 4 else (lfsr & 1)
                lfsr = (lfsr >> 1) | (fb << 15)
                noise_out = 1 if lfsr & 1 else -1
            s += vol[3] * noise_out
        out[i] = s
    return out


def resample(x, rin, rout):
    n = int(len(x) * rout / rin)
    out = array.array('f', [0.0]) * n
    step = rin / rout
    for i in range(n):
        p = i * step
        k = int(p)
        f = p - k
        a = x[k] if k < len(x) else 0.0
        b = x[k + 1] if k + 1 < len(x) else a
        out[i] = a + (b - a) * f
    return out


def write_wav(path, x, rate):
    pcm = array.array('h', (max(-32767, min(32767, int(v * 32767))) for v in x))
    data = pcm.tobytes()
    with open(path, 'wb') as f:
        f.write(b'RIFF' + struct.pack('<I', 36 + len(data)) + b'WAVE')
        f.write(b'fmt ' + struct.pack('<IHHIIHH', 16, 1, 1, rate, rate * 2, 2, 16))
        f.write(b'data' + struct.pack('<I', len(data)) + data)


def stats(x, label):
    peak = max(abs(v) for v in x) if len(x) else 0
    rms = math.sqrt(sum(v * v for v in x) / max(1, len(x)))
    clips = sum(1 for v in x if abs(v) >= 1.0)
    print('%-28s %5.1f s  crête %6.1f dBFS  RMS %6.1f dBFS  écrêtages %d' % (
        label, len(x) / OUT_RATE, 20 * math.log10(peak + 1e-9), 20 * math.log10(rms + 1e-9), clips))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--rom')
    ap.add_argument('--vgm')
    ap.add_argument('--psg-only')
    ap.add_argument('--out', required=True)
    ap.add_argument('--loops', type=int, default=2)
    ap.add_argument('--region', default='E')
    ap.add_argument('--gain', type=float, default=0.5, help='gain appliqué à la somme des voies YM')
    ap.add_argument('--keep-intro', action='store_true', help='garder ce qui précède la musique (voix)')
    a = ap.parse_args()

    if a.psg_only:
        ev, total = psg_events(a.psg_only, a.loops)
        n = int((total / 44100.0 + 1.0) * OUT_RATE)
        mix = render_psg(ev, n, OUT_RATE)
        stats(mix, os.path.basename(a.out))
        write_wav(a.out, mix, OUT_RATE)
        return

    ev, total = psg_events(a.vgm, a.loops)
    fps = 50 if a.region == 'E' else 60
    frames = int((total / 44100.0 + 4.0) * fps)
    with tempfile.TemporaryDirectory(dir='.') as tmp:
        subprocess.run([BLASTEM, '-b', str(frames), '-y', '-r', a.region, os.path.abspath(a.rom)],
                       cwd=tmp, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                       env=dict(os.environ, SDL_VIDEODRIVER='x11'), timeout=600)
        chans = [read_wav16(os.path.join(tmp, 'ym_channel_%d.wav' % c)) for c in range(6)]
    n = min(len(c) for c in chans)
    for c in range(6):
        pk = max(abs(v) for v in chans[c][:n]) / 32768.0
        print('  voie YM %d : crête %6.1f dBFS' % (c + 1, 20 * math.log10(pk + 1e-9)))
    # début de la musique : première note de la basse (voie 1)
    start = next((i for i in range(n) if abs(chans[0][i]) > 200), 0)
    begin = 0 if a.keep_intro else max(0, start - int(0.05 * YM_RATE))
    length = min(n - begin, int((total / 44100.0 + 2.5) * YM_RATE) + (start - begin))
    g = a.gain / 32768.0
    ym = array.array('f', [0.0]) * length
    for c in range(6):
        ch = chans[c]
        for i in range(length):
            ym[i] += ch[begin + i] * g
    psg = render_psg(ev, length - (start - begin), YM_RATE)
    off = start - begin
    for i in range(len(psg)):
        ym[off + i] += psg[i]
    mix = resample(ym, YM_RATE, OUT_RATE)
    stats(mix, os.path.basename(a.out))
    write_wav(a.out, mix, OUT_RATE)


if __name__ == '__main__':
    main()
