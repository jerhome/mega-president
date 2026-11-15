#!/usr/bin/env python3
"""Génère la musique FM et les bruitages PCM de Mega Président (V3).

Sorties (versionnées) :
- res/music/<morceau>.vgm   VGM YM2612 + DAC + PSG, convertis en XGM2 par rescomp au build ;
- res/sfx/sfx_*.wav         bruitages PCM 8 bits 13 300 Hz (canal PCM 2 de XGM2) ;
- res/snd_audio.res         ressources (musique multi-pistes XGM2, bruitages, voix) ;
- src/audio/music_tracks.h  ordre des pistes dans la ressource XGM2.

Usage : python3 tools/music/make_music.py [--only titre,...]
Écoute : tools/music/preview.py (ROM jouée par BlastEm, voies YM + PSG recalculé).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)

from compose import export_vgm   # noqa: E402
import pcm                       # noqa: E402
import songs_v3 as songs          # noqa: E402
from legacy_psg import legacy_vgm, LEGACY_NAMES  # noqa: E402

# Thèmes PSG de la V2 gardés en attendant leur version FM (index dans legacy_psg_data.SONGS)
LEGACY_KEPT = []

SFX = ['move', 'ok', 'cancel', 'error', 'adopted', 'rejected', 'event', 'win', 'lose']


def main():
    music_dir = os.path.join(ROOT, 'res', 'music')
    sfx_dir = os.path.join(ROOT, 'res', 'sfx')
    os.makedirs(music_dir, exist_ok=True)
    os.makedirs(sfx_dir, exist_ok=True)

    drums = pcm.drum_bank()
    names = []
    for fn in songs.SONGS:
        s = fn()
        data = export_vgm(s, drums)
        path = os.path.join(music_dir, s.name + '.vgm')
        with open(path, 'wb') as f:
            f.write(data)
        names.append((s.name, s.desc, s.total_frames() / 60.0))
        print('%-10s %5.1f s  %6d o  %s' % (s.name, s.total_frames() / 60.0, len(data), s.desc))

    for idx in LEGACY_KEPT:
        data, dur = legacy_vgm(idx)
        name = 'v2_' + LEGACY_NAMES[idx]
        with open(os.path.join(music_dir, name + '.vgm'), 'wb') as f:
            f.write(data)
        desc = 'thème PSG de la V2 (%s), en attendant sa version FM' % LEGACY_NAMES[idx]
        names.append((name, desc, dur))
        print('%-10s %5.1f s  %6d o  %s' % (name, dur, len(data), desc))

    bank = pcm.sfx_bank()
    for name in SFX:
        pcm.write_wav(os.path.join(sfx_dir, 'sfx_%s.wav' % name), bank[name])

    with open(os.path.join(ROOT, 'res', 'snd_audio.res'), 'w') as f:
        f.write('// Généré par tools/music/make_music.py : ne pas modifier à la main.\n')
        f.write('// Musique : une ressource XGM2 multi-pistes (échantillons de percussions partagés).\n')
        f.write('XGM2 xgm2_music %s\n' % ' '.join('"music/%s.vgm"' % n for n, _, _ in names))
        f.write('// Bruitages : canal PCM 2 de XGM2 (13 300 Hz).\n')
        for name in SFX:
            f.write('WAV sfx_%s "sfx/sfx_%s.wav" XGM2\n' % (name, name))
        f.write('// Voix « For sure ! » (tools/voice/make_voice.py) : canal PCM 3 de XGM2.\n')
        f.write('WAV snd_forsure "sfx/forsure.wav" XGM2\n')

    with open(os.path.join(ROOT, 'src', 'audio', 'music_tracks.h'), 'w') as f:
        f.write('// Généré par tools/music/make_music.py : ne pas modifier à la main.\n')
        f.write('// Index des pistes dans la ressource XGM2 multi-pistes xgm2_music.\n')
        f.write('#ifndef AUDIO_MUSIC_TRACKS_H\n#define AUDIO_MUSIC_TRACKS_H\n\n')
        for i, (n, desc, dur) in enumerate(names):
            f.write('#define TRACK_%-10s %2d   // %4.1f s : %s\n' % (n.upper(), i, dur, desc))
        f.write('#define TRACK_COUNT      %2d\n\n#endif\n' % len(names))


if __name__ == '__main__':
    main()
