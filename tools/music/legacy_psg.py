"""Conversion des thèmes PSG de la V2 en VGM (PSG seul), joués par XGM2.

Sert de solution d'attente pour les morceaux pas encore recomposés en FM (le
68000 ne peut plus écrire dans le PSG pendant que le pilote XGM2 l'utilise) et
de point de comparaison. Même logique que l'ancien séquenceur de music.c :
piste 0 et 1 sur les tons 1 et 2, piste 2 sur le bruit blanc ; petit silence
entre deux notes ; « hold » = durée sonore maximale.
"""
from compose import VgmWriter, FPS, PSG_CLOCK
from legacy_psg_data import ARRAYS, SONGS

FREQ16 = (262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494)
LEGACY_NAMES = ['title', 'year1', 'year2', 'year3', 'year4', 'year5', 'election']


def legacy_vgm(index):
    unit, tracks = SONGS[index]
    events = {}       # frame -> [(ordre, octets)]

    def add(ms, order, data):
        fr = int(round(ms * FPS / 1000.0))
        events.setdefault(fr, []).append((order, data))

    total_ms = 0
    for t, (arr, vol, hold) in enumerate(tracks):
        ch = t + 1
        ms = 0
        for pitch, dur in ARRAYS[arr]:
            if dur == 0:
                break
            dur_ms = dur * unit
            gap = 35 if dur_ms >= 140 else dur_ms // 4
            snd = dur_ms - gap
            if hold and snd > hold * 10:
                snd = hold * 10
            if pitch:
                if ch == 3:
                    data = [0xE5, 0x90 | (ch << 5) | vol]
                else:
                    f = (FREQ16[pitch % 12] << (pitch // 12)) >> 4
                    f = max(110, f)
                    n = min(1023, int(PSG_CLOCK / (32 * f)))
                    data = [0x80 | (ch << 5) | (n & 15), (n >> 4) & 0x3F, 0x90 | (ch << 5) | vol]
                add(ms, 1, data)
                add(ms + snd, 0, [0x9F | (ch << 5)])
            ms += dur_ms
        total_ms = max(total_ms, ms)
    w = VgmWriter()
    for c in range(4):
        w.psg(0x9F | (c << 5))
    w.mark_loop()
    total = int(round(total_ms * FPS / 1000.0))
    cur = 0
    for fr in sorted(events):
        if fr >= total:
            continue
        if fr > cur:
            w.wait_frames(fr - cur)
            cur = fr
        for _, data in sorted(events[fr], key=lambda e: e[0]):
            for b in data:
                w.psg(b)
    w.wait_frames(total - cur)
    return w.build(), total_ms / 1000.0
