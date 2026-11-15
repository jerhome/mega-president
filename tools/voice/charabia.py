#!/usr/bin/env python3
"""Charabia des personnages (tâche 4.18) : synthèse formantique maison, 100 % fictive.

Chaque personnage (31 gueules, fiches dans charabia_persos.py) reçoit une banque de
8 syllabes CV : 4 « d'attaque » (1re syllabe) et 4 « finales » (2de syllabe, avec la
mélodie de fin). Le jeu enchaîne une attaque et une finale tirées au hasard
(16 répliques possibles, jamais deux fois la même de suite : voice.c).

Synthèse : source glottique (impulsions de Rosenberg dérivées, irrégularité, souffle),
4 résonateurs de formants en cascade (Klatt), pôle nasal, consonnes simples
(occlusives avec explosion, nasales, liquides, semi-voyelles, fricative v),
courbe de hauteur par syllabe. Calcul à 26 600 Hz, filtrage passe-bas puis
décimation à 6 650 Hz (cadence « halfRate » de XGM2), 8 bits.

Filtre de mots : chaque syllabe et chaque paire attaque + finale est comparée à une
liste phonétique de mots français et anglais (et prénoms courants, mots enfantins,
grossièretés) ; un tirage qui forme un mot est rejeté.

Graine fixe par personnage : même entrée, même fichier, octet pour octet.

Usage :
  python3 tools/voice/charabia.py                 # res/voice/*.wav, res/snd_voice.res, src/audio/voice_chb_data.h
  python3 tools/voice/charabia.py --preview DIR [perso ...]
        # planches d'écoute : 6 répliques par personnage, telles que la console les joue
"""
import math
import os
import struct
import sys
import wave

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
from charabia_persos import PERSOS, fiche  # noqa: E402

OUT_RATE = 6650           # XGM2 halfRate
OS = 4                    # suréchantillonnage interne
FS = OUT_RATE * OS        # 26 600 Hz
BLOCK = 256               # XGM2 : échantillons alignés et de longueur multiple de 256
N_INIT, N_FIN = 4, 4

# --- Phonétique --------------------------------------------------------------------
# Formants (F1, F2, F3, F4) d'une voix d'homme ; voyelles du français.
VOWELS = {
    "a": (730, 1300, 2450, 3400),
    "e": (380, 2050, 2650, 3400),   # é
    "è": (540, 1750, 2500, 3400),
    "i": (290, 2200, 2900, 3500),
    "o": (420, 880, 2400, 3300),
    "ou": (310, 760, 2300, 3200),
    "eu": (400, 1450, 2300, 3300),
    "u": (290, 1750, 2150, 3300),
    "an": (640, 1080, 2450, 3300),
    "on": (440, 820, 2400, 3200),
    "in": (580, 1500, 2450, 3300),
}
NASAL_V = {"an", "on", "in"}
CLOSED_V = {"i", "u", "ou", "eu", "e"}   # bouche mi-ouverte

# Consonnes : type, lieu (F2 de départ des transitions), labiale (bouche fermée).
CONS = {
    "m": ("nasal", 1000, True), "n": ("nasal", 1500, False),
    "b": ("voiced", 850, True), "d": ("voiced", 1700, False),
    "p": ("unvoiced", 850, True), "t": ("unvoiced", 1750, False), "k": ("unvoiced", 1900, False),
    "l": ("liquid", 1150, False), "w": ("glide", 750, True), "y": ("glide", 2200, False),
    "v": ("fric", 1100, True),
}

# Mots à ne jamais former (transcription phonétique dans l'alphabet ci-dessus,
# syllabes séparées par des points ; « è » est confondu avec « e »).
WORDS_FR = """
ma.man pa.pa ma.mi pa.pi be.be pe.pe me.me do.do lo.lo bo.bo mi.mi nou.nou dou.dou lou.lou
tou.tou tu.tu ta.ta ton.ton bon.bon pon.pon din.don ki.ki pi.pi ka.ka mo.mo no.no to.to yo.yo
ko.ko lu.lu ba.ba da.da na.na ma.ma ni.ni bi.bi
bi.don ba.ton mou.ton la.pin ma.tin ma.lin mou.lin pan.tin ba.din lin.di mi.di mi.ni mi.ne
meu.nu pan.da ban.di bi.de bi.ye mo.le ba.le pa.le va.le vo.le pou.le bou.le mu.le du.ve na.ve
bo.ne ka.do be.ton ta.bou ta.pi ta.lon ba.lon me.lo ve.lo lo.to mo.to de.po de.bu mi.nou bou.don
pou.pon la.ma ti.pi ki.wi wi.ki ta.ko ka.no me.mo de.mo ve.to vi.de vi.lin veu.lou ma.ni de.ni
bou.yi tou.pi lo.bi de.mon li.mon mi.lan li.on va.lon meu.lon ma.non ni.non mo.na lo.la li.la
mi.la ni.na ti.na da.na la.na li.li lo.li li.do do.ne bou.lo de.li de.bi wa.la mou.le ma.lo
bi.dou nou.yo ko.pin ka.pon ka.bo pi.yon lou.pi pa.ta to.ma ba.na bi.zou ma.ri pe.ti peu.ti
po.li po.lo pu.ni mu.ni nu.li ma.nie mon.ta ba.ta tou.pe nou.vo be.ni de.ne do.mi
ko.lo bou.ton mou.ve lou.pe pi.ton ti.mon mi.ton bon.de lan.de man.ton
bo.kou mo.mon la.vi di.ne neu.ve deu.ve meu.ne le.ve nou.vou la.mou de.va de.la de.le di.va
de.bo la.min bo.bon do.mo ma.di tou.ti ya.la ka.tou ma.mou ma.do la.do don.na dou.ma ma.na
ma.lou lou.don dou.do bou.do de.da de.nou me.di ko.kou bo.do mon.do la.tou ma.le me.ne bou.le
bo.te ve.ni lou.ve ne.ve ve.lou you.pi pou.ma la.va la.mi bon.di mon.de mon.deu ban.de ban.deu
vi.la vi.va vi.ve la.ve ni.ve mo.de bou.de lou.de ma.lin bo.dou ma.ve
na.bo va.ne ni.veu li.di li.mi vo.dou vi.ne ba.la da.me kou.deu bo.ko mi.le be.le mou.la
dou.la von.la da.te ba.bou ka.fe bou.ba van.ne man.ne ba.be ba.lan na.da tou.teu tou.te mi.lo
dou.te bou.da ban.le man.ba do.wa
"""
WORDS_EN = """
be.bi bo.di le.di ma.ni da.di na.ni pe.ni ta.mi da.mi be.li lo.li mo.li li.li bi.li wi.li ki.ti
po.ti bou.ti bou.bi bou.li li.ma la.ma ko.ko ta.ko de.li de.mo me.mo ki.wi ve.to vi.to tou.na
lou.na mou.vi lou.ni pi.ti me.ni dou.dou pou.pou wi.wi ti.ti pe.ti no.ti kou.kou kou.ki po.ni
to.ni ni.ko li.no di.no be.ti ke.ti mo.ni ou.ni yo.ga ma.ko de.ma me.ta be.ta lo.bi ba.di
na.me de.te me.ni ti.ti
"""
# Syllabes seules à éviter : grossièretés, et mots qui disent « non » ou « pas » (charabia accueillant).
SYL_BAN = {"kon", "ku", "pu", "no", "non", "nan", "pa", "pe", "wi", "ta"}

# Mots-outils du français : en 1re position, ils se lisent comme un article ou un
# pronom (« la vie », « de là », « nous vous ») ; on les garde pour la 2de syllabe.
POS1_BAN = {"la", "le", "de", "me", "ne", "te", "ma", "mon", "ta", "ton", "nou", "vou", "bon", "deu"}

FORBID = set()
for w in (WORDS_FR + WORDS_EN).split():
    FORBID.add(w.replace("è", "e"))


def key(s):
    return (s[0] + s[1]).replace("è", "e")


def is_word(a, b):
    k = key(a) + "." + key(b)
    if k in FORBID or key(a) in POS1_BAN:
        return True
    # redoublement (« bobo », « dada ») ou presque (même consonne et même voyelle)
    return key(a) == key(b)


# --- Aléa reproductible ------------------------------------------------------------
class Rng:
    def __init__(self, seed):
        self.s = (seed * 2654435761 + 12345) & 0xFFFFFFFF or 1

    def u32(self):
        x = self.s
        x ^= (x << 13) & 0xFFFFFFFF
        x ^= x >> 17
        x ^= (x << 5) & 0xFFFFFFFF
        self.s = x & 0xFFFFFFFF
        return self.s

    def uni(self):                 # [0, 1)
        return self.u32() / 4294967296.0

    def sym(self):                 # [-1, 1)
        return self.uni() * 2.0 - 1.0

    def pick(self, seq):
        return seq[self.u32() % len(seq)]


def seed_of(name):
    h = 2166136261
    for c in name.encode():
        h = ((h ^ c) * 16777619) & 0xFFFFFFFF
    return h


# --- Choix des syllabes ------------------------------------------------------------
def choose_syllables(p, rng):
    """4 attaques + 4 finales, distinctes, sans mot réel (seules et en paire)."""
    vow, cons = p["vow"], p["cons"]

    def draw(taken, ok):
        for _ in range(2000):
            s = (rng.pick(cons), rng.pick(vow))
            if key(s) in SYL_BAN or key(s) in {key(t) for t in taken} or not ok(s):
                continue
            if s[0] == "w" and s[1] in ("ou", "o", "on"):   # « wou » : glissement inaudible
                continue
            if s[0] == "y" and s[1] in ("i",):
                continue
            # variété : pas trois fois la même voyelle ni la même consonne dans la banque
            if sum(1 for t in taken if t[1] == s[1]) >= 3 or sum(1 for t in taken if t[0] == s[0]) >= 3:
                continue
            return s
        raise RuntimeError("filtre de mots : banque impossible pour " + p["name"])

    init, fin = [], []
    for _ in range(N_INIT):
        init.append(draw(init, lambda s: key(s) not in POS1_BAN))
    for _ in range(N_FIN):
        fin.append(draw(init + fin, lambda s: not any(is_word(a, s) for a in init)))
    assert not any(is_word(a, b) for a in init for b in fin)
    return init, fin


# --- Synthèse ----------------------------------------------------------------------
class Reson:
    """Résonateur de Klatt (2 pôles), coefficients modifiables à la volée."""
    __slots__ = ("a", "b", "c", "y1", "y2")

    def __init__(self):
        self.a = 1.0
        self.b = self.c = self.y1 = self.y2 = 0.0

    def set(self, f, bw):
        f = min(f, FS * 0.45)
        r = math.exp(-math.pi * bw / FS)
        self.c = -r * r
        self.b = 2.0 * r * math.cos(2.0 * math.pi * f / FS)
        self.a = 1.0 - self.b - self.c

    def __call__(self, x):
        y = self.a * x + self.b * self.y1 + self.c * self.y2
        self.y2 = self.y1
        self.y1 = y
        return y


def interp(curve, t):
    if t <= curve[0][0]:
        return curve[0][1]
    for (t0, v0), (t1, v1) in zip(curve, curve[1:]):
        if t <= t1:
            return v0 + (v1 - v0) * (t - t0) / max(1e-9, t1 - t0)
    return curve[-1][1]


def smoothstep(x):
    x = 0.0 if x < 0 else (1.0 if x > 1 else x)
    return x * x * (3 - 2 * x)


def synth_syllable(p, syl, n_out, final, rng, f0mul):
    """Une syllabe : renvoie (échantillons à 6 650 Hz en flottants, c_ms, v_ms)."""
    c, v = syl
    ctype, locus, _lab = CONS[c]
    n = n_out * OS
    fs_scale, smile = p["fs"], p["smile"]
    tgt = [f * fs_scale * (1.0 + smile * (0.5 if i == 0 else 1.0)) for i, f in enumerate(VOWELS[v])]
    nas_v = (0.8 if v in NASAL_V else 0.0) + p["nasal"] * 0.6

    # Durées (en échantillons internes)
    ms = FS / 1000.0
    tail = int(14 * ms)                          # silence final (bloc de 256 complété)
    if ctype in ("voiced", "unvoiced"):
        c_len = int((32 if ctype == "voiced" else 38) * ms)
    elif ctype == "fric":
        c_len = int(45 * ms)
    else:
        c_len = int(40 * ms)
    trans = int(38 * ms)
    rel = int(45 * ms)                           # décroissance de fin de voyelle
    v_end = n - tail
    if v_end - c_len < int(50 * ms):
        c_len = max(int(20 * ms), v_end - int(50 * ms))

    # Formants de départ selon la consonne (transitions CV)
    if ctype == "nasal":
        start = [260 * fs_scale, locus * fs_scale, 2300 * fs_scale, 3300 * fs_scale]
    elif ctype == "liquid":
        start = [360 * fs_scale, locus * fs_scale, 2700 * fs_scale, 3400 * fs_scale]
    elif ctype == "glide":
        start = [280 * fs_scale, locus * fs_scale, 2400 * fs_scale if c == "w" else 2900 * fs_scale,
                 3300 * fs_scale]
    else:
        start = [250 * fs_scale, locus * fs_scale * 0.6 + tgt[1] * 0.4,
                 tgt[2] * 0.8 + 2400 * fs_scale * 0.2, tgt[3]]
    bws = [70.0, 95.0, 150.0, 220.0]

    res = [Reson() for _ in range(4)]
    rnas = Reson()
    rnas.set(270 * fs_scale, 90)
    rnoise = Reson()
    tilt_y = 0.0
    tilt = p["tilt"]

    curve = p["c2"] if final else p["c1"]
    f0b = p["f0"] * f0mul
    vib_d, vib_f = p["vib"]
    jit, breath, oq = p["jit"], p["breath"], p["oq"]
    phase, period_jit, amp_jit = 0.0, 1.0, 1.0
    sav = san = saf = snm = senv = 0.0     # amplitudes lissées (pas de clic entre blocs)
    ks, kf = 1.0 / (2.5 * FS / 1000.0), 1.0 / (0.8 * FS / 1000.0)
    prev_g = 0.0
    out = [0.0] * n

    burst_c = {"b": 900, "p": 900, "d": 2600, "t": 2900, "k": 1900}.get(c, 2000) * fs_scale
    rnoise.set(burst_c, 900)
    step = 32
    for i0 in range(0, n, step):
        # paramètres par bloc de 32 échantillons
        t = i0
        if t < c_len:
            fm = start
            phase_c = True
        else:
            k = smoothstep((t - c_len) / trans)
            fm = [s + (g - s) * k for s, g in zip(start, tgt)]
            phase_c = False
        for j in range(4):
            bw = bws[j] * (1.0 + (0.6 * nas_v if j == 0 else 0.0))
            if ctype == "nasal" and phase_c:
                bw *= 1.6
            res[j].set(fm[j], bw)
        # amplitudes
        if t < c_len:
            if ctype == "nasal":
                av, an, af = 0.45, 0.0, 0.0
            elif ctype == "liquid":
                av, an, af = 0.55, 0.0, 0.0
            elif ctype == "glide":
                av, an, af = 0.65, 0.0, 0.0
            elif ctype == "voiced":
                bar = t < c_len - int(7 * ms)
                av, an, af = (0.12 if bar else 0.25), 0.0, (0.0 if bar else 0.35)
            elif ctype == "unvoiced":
                if t < c_len - int(16 * ms):
                    av, an, af = 0.0, 0.0, 0.0                    # occlusion
                elif t < c_len - int(10 * ms):
                    av, an, af = 0.0, 0.0, 0.45                   # explosion
                else:
                    av, an, af = 0.08, 0.30, 0.05                 # aspiration douce
            else:   # fricative v
                av, an, af = 0.40, 0.0, 0.12
            nasmix = 0.9 if ctype == "nasal" else 0.0
        else:
            av, an, af = 1.0, 0.0, 0.0
            nasmix = nas_v * 0.5
        # enveloppe globale : attaque douce, décroissance finale
        env = smoothstep(t / (6 * ms))
        if t > v_end - rel:
            env *= smoothstep((v_end - t) / rel)
        if t >= v_end:
            env = 0.0
        tn = t / max(1, v_end)
        f0 = f0b * interp(curve, tn)
        for i in range(i0, min(n, i0 + step)):
            sav += ks * (av - sav)
            san += kf * (an - san)
            saf += kf * (af - saf)
            snm += ks * (nasmix - snm)
            senv += ks * (env - senv)
            # source glottique
            vib = 1.0 + vib_d * math.sin(2 * math.pi * vib_f * i / FS)
            phase += f0 * vib * period_jit / FS
            if phase >= 1.0:
                phase -= 1.0
                period_jit = 1.0 + jit * rng.sym()
                amp_jit = 1.0 + jit * 3.0 * rng.sym()
            ph = phase
            o1 = oq * 0.66
            if ph < o1:
                g = 0.5 * (1.0 - math.cos(math.pi * ph / o1))
            elif ph < oq:
                g = math.cos(0.5 * math.pi * (ph - o1) / (oq - o1))
            else:
                g = 0.0
            dg = (g - prev_g) * (FS / 1000.0)
            prev_g = g
            noise = rng.sym()
            src = sav * dg * amp_jit + (breath * (0.4 + g) + san) * noise * 0.8
            tilt_y += (1.0 - 0.45 * tilt) * (src - tilt_y)
            x = tilt_y
            y = x
            for r in res:
                y = r(y)
            y = y * (1.0 - snm * 0.5) + rnas(x) * snm * 1.2
            y += rnoise(noise) * saf * 0.7
            out[i] = y * senv
    # passe-bas + décimation
    o = decimate(out)
    return o, c_len / ms, (v_end) / ms


_FIR = None


def fir():
    global _FIR
    if _FIR is None:
        taps, fc = 63, 3000.0 / FS
        h = []
        for k in range(taps):
            m = k - (taps - 1) / 2
            s = 2 * fc if m == 0 else math.sin(2 * math.pi * fc * m) / (math.pi * m)
            w = 0.42 - 0.5 * math.cos(2 * math.pi * k / (taps - 1)) + 0.08 * math.cos(4 * math.pi * k / (taps - 1))
            h.append(s * w)
        sm = sum(h)
        _FIR = [x / sm for x in h]
    return _FIR


def decimate(x):
    h = fir()
    half = len(h) // 2
    n = len(x)
    out = []
    for i in range(0, n, OS):
        acc = 0.0
        for k, hk in enumerate(h):
            j = i + k - half
            if 0 <= j < n:
                acc += hk * x[j]
        out.append(acc)
    return out


# --- Banque d'un personnage --------------------------------------------------------
def build_voice(name):
    p = fiche(name)
    rng = Rng(seed_of(name))
    init, fin = choose_syllables(p, rng)
    sylls = []
    for idx, s in enumerate(init + fin):
        final = idx >= N_INIT
        nblk = p["blk"][1 if final else 0]
        f0mul = 1.0 + 0.04 * rng.sym()
        o, c_ms, v_ms = synth_syllable(p, s, nblk * BLOCK, final, rng, f0mul)
        sylls.append(dict(syl=s, final=final, nblk=nblk, data=o, c_ms=c_ms, v_ms=v_ms))
    # pré-emphase légère (8 bits, petite bande) puis normalisation commune à la banque
    for sy in sylls:
        d = sy["data"]
        sy["data"] = [d[0]] + [d[i] + 0.6 * (d[i] - d[i - 1]) for i in range(1, len(d))]
    peak = max(max(abs(x) for x in sy["data"]) for sy in sylls)
    rms = math.sqrt(sum(x * x for sy in sylls for x in sy["data"]) / sum(len(sy["data"]) for sy in sylls))
    gain = min(0.88 / peak, 0.26 / max(rms, 1e-9))
    for sy in sylls:
        d = [x * gain for x in sy["data"]]
        k = min(20, len(d))
        for i in range(k):                 # pas de clic aux bords
            d[i] *= i / k
        sy["data"] = d
    return p, sylls


def to_u8(x):
    return bytes(max(0, min(255, int(round(v * 127.0)) + 128)) for v in x)


def write_wav(path, pcm, rate=OUT_RATE):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(rate)
        w.writeframes(pcm)


def flags_of(syl):
    f = 0
    if CONS[syl[0]][2]:
        f |= 1                      # labiale : bouche fermée pendant la consonne
    if syl[1] in CLOSED_V:
        f |= 2                      # voyelle fermée : bouche mi-ouverte
    return f


def generate():
    res_lines = ["// Généré par tools/voice/charabia.py : ne pas modifier à la main.",
                 "// Charabia des personnages (4.18) : canal PCM 3 de XGM2, 6 650 Hz (halfRate).",
                 "// Une ressource par personnage : 4 syllabes d'attaque puis 4 finales,",
                 "// chacune alignée sur 256 octets (voir src/audio/voice_chb_data.h)."]
    h = ["// Généré par tools/voice/charabia.py : ne pas modifier à la main.",
         "// Charabia (4.18) : banques de syllabes, ordre = GueuleId. Inclus par voice.c seulement.",
         "#ifndef AUDIO_VOICE_CHB_DATA_H", "#define AUDIO_VOICE_CHB_DATA_H", "",
         "#include \"snd_voice.h\"", "",
         "#define CHB_COUNT %d" % len(PERSOS), "#define CHB_INIT %d" % N_INIT, "#define CHB_FIN %d" % N_FIN,
         "#define CHB_RATE_HZ %d" % OUT_RATE, "",
         "// off, nblk : position et longueur en blocs de 256 octets ; c_ms : fin de la consonne ;",
         "// v_ms : fin de la voyelle ; flags : 1 consonne labiale, 2 voyelle fermée.",
         "typedef struct { u8 off, nblk, c_ms, v_ms, flags; } ChbSyl;",
         "typedef struct { const u8 *data; u8 gap_ms; ChbSyl syl[CHB_INIT + CHB_FIN]; } ChbVoice;", "",
         "static const ChbVoice chb_voices[CHB_COUNT] = {"]
    total = 0
    report = []
    for name, fam, _ in PERSOS:
        p, sylls = build_voice(name)
        pcm = bytearray()
        entries = []
        for sy in sylls:
            off = len(pcm) // BLOCK
            b = to_u8(sy["data"])
            assert len(b) == sy["nblk"] * BLOCK
            pcm += b
            entries.append("{%d,%d,%d,%d,%d}" % (off, sy["nblk"], int(sy["c_ms"]), int(sy["v_ms"]), flags_of(sy["syl"])))
        total += len(pcm)
        write_wav(os.path.join(ROOT, "res", "voice", "chb_%s.wav" % name), bytes(pcm))
        res_lines.append('WAV chb_%s "voice/chb_%s.wav" XGM2 %d' % (name, name, OUT_RATE))
        h.append("    { chb_%s, %d, { %s } },   // %s" % (name, p["gap"], ", ".join(entries), fam))
        report.append("%-22s %-10s %s | %s" % (name, fam, " ".join("".join(s["syl"]) for s in sylls[:N_INIT]),
                                              " ".join("".join(s["syl"]) for s in sylls[N_INIT:])))
    h += ["};", "", "#endif", ""]
    with open(os.path.join(ROOT, "res", "snd_voice.res"), "w") as f:
        f.write("\n".join(res_lines) + "\n")
    with open(os.path.join(ROOT, "src", "audio", "voice_chb_data.h"), "w") as f:
        f.write("\n".join(h))
    print("\n".join(report))
    print("Charabia : %d personnages, %d octets (%.1f Ko) à %d Hz" % (len(PERSOS), total, total / 1024, OUT_RATE))


def preview(outdir, names):
    """Planche d'écoute : 6 répliques par personnage, comme la console les enchaîne
    (attaque, silence de gap, finale ; jamais deux fois la même paire)."""
    os.makedirs(outdir, exist_ok=True)
    allp = []
    for name in names or [n for n, _, _ in PERSOS]:
        p, sylls = build_voice(name)
        rng = Rng(seed_of(name) ^ 0xABCD)
        pcm = bytearray()
        last = None
        for _ in range(6):
            while True:
                a, b = rng.u32() % N_INIT, N_INIT + rng.u32() % N_FIN
                if (a, b) != last:
                    break
            last = (a, b)
            pcm += to_u8(sylls[a]["data"])
            pcm += bytes([128]) * int(p["gap"] * OUT_RATE / 1000)
            pcm += to_u8(sylls[b]["data"])
            pcm += bytes([128]) * int(0.7 * OUT_RATE)
        path = os.path.join(outdir, "charabia_%s.wav" % name)
        write_wav(path, bytes(pcm))
        allp.append(pcm)
        print(path)


if __name__ == "__main__":
    if len(sys.argv) > 2 and sys.argv[1] == "--preview":
        preview(sys.argv[2], sys.argv[3:])
    else:
        generate()
