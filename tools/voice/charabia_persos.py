"""Fiches de voix du charabia (tâche 4.18, avis DA : docs/REVUE_DA_V3.md § 7.2).

Six familles de timbre, déclinées sur les 31 gueules (ordre = GueuleId,
tools/art/persos.py) selon l'âge, la corpulence et le tempérament. Toutes les
voix restent ACCUEILLANTES (décision du joueur, 15/11) : même la famille
« grave » est courtoise, jamais agressive. Personnages 100 % fictifs ; aucune
voix réelle n'est imitée, ni par le timbre ni par un tic.

Paramètres (unités entre parenthèses) :
  f0      hauteur de base (Hz)
  fs      échelle des formants (longueur du conduit vocal : 1,0 homme adulte,
          ~1,16 femme, < 1 corpulent)
  blk     (blocs de la 1re syllabe, blocs de la 2de) ; 1 bloc = 256 octets
          = 38,5 ms à 6 650 Hz : c'est le débit
  gap     silence entre les deux syllabes (ms)
  breath  souffle (0 à 0,3)        tilt    douceur du timbre (0 clair à 0,8 rond)
  oq      quotient d'ouverture glottique (0,45 pressé à 0,75 soufflé)
  jit     irrégularité de hauteur (voix rauque, âgée)
  vib     (profondeur, Hz) du vibrato ou du chevrotement
  nasal   nasalité générale (0 à 1)
  smile   sourire (formants relevés, 0 à 0,08)
  c1, c2  courbes mélodiques des syllabes 1 et 2 : [(t 0..1, facteur)]
  vow     voyelles préférées ; cons  consonnes préférées
"""

# Familles (§ 7.2) : valeurs par défaut, surchargées par personnage.
FAMILLES = {
    # mélodie qui monte puis tombe, comme une promesse ; voyelles « o, a »
    "chaleureux": dict(fs=0.98, blk=(4, 4), gap=45, breath=0.06, tilt=0.55, oq=0.62, jit=0.006,
                       vib=(0.012, 5.0), nasal=0.0, smile=0.05,
                       c1=[(0, 1.00), (1, 1.12)], c2=[(0, 1.16), (0.3, 1.22), (1, 0.88)],
                       vow=["o", "a", "ou", "on", "e"], cons=["m", "b", "l", "n", "w", "d"]),
    # mélodie plate, débit régulier ; « é, i »
    "lisse": dict(fs=1.0, blk=(4, 4), gap=35, breath=0.03, tilt=0.35, oq=0.55, jit=0.002,
                  vib=(0.0, 5.0), nasal=0.0, smile=0.06,
                  c1=[(0, 1.00), (1, 1.04)], c2=[(0, 1.06), (1, 0.97)],
                  vow=["e", "i", "è", "a", "eu"], cons=["l", "n", "m", "d", "y", "v"]),
    # grave et posé, finale qui tombe ; attaques nettes mais courtoises
    "grave": dict(fs=0.95, blk=(3, 4), gap=50, breath=0.04, tilt=0.5, oq=0.5, jit=0.005,
                  vib=(0.0, 5.0), nasal=0.05, smile=0.03,
                  c1=[(0, 1.06), (1, 1.02)], c2=[(0, 1.10), (0.25, 1.12), (1, 0.84)],
                  vow=["o", "a", "ou", "on", "eu"], cons=["d", "b", "m", "t", "k", "l"]),
    # nasal et amusé, finale qui remonte ; « an, on, in »
    "nasal": dict(fs=1.0, blk=(4, 4), gap=40, breath=0.04, tilt=0.35, oq=0.55, jit=0.004,
                  vib=(0.0, 5.0), nasal=0.45, smile=0.05,
                  c1=[(0, 1.02), (1, 0.96)], c2=[(0, 0.96), (0.6, 1.05), (1, 1.24)],
                  vow=["an", "on", "in", "e", "a"], cons=["m", "n", "l", "v", "b"]),
    # aigu, rapide, staccato, finale montante ; « i, a »
    "vif": dict(fs=1.16, blk=(3, 4), gap=25, breath=0.05, tilt=0.25, oq=0.58, jit=0.003,
                vib=(0.0, 5.0), nasal=0.0, smile=0.07,
                c1=[(0, 1.00), (1, 1.14)], c2=[(0, 1.10), (1, 1.36)],
                vow=["i", "a", "e", "è", "ou"], cons=["t", "d", "l", "y", "p", "m"]),
    # alto poitriné, un peu rauque, attaque marquée, finale franche
    "alto": dict(fs=1.12, blk=(3, 4), gap=40, breath=0.09, tilt=0.45, oq=0.52, jit=0.012,
                 vib=(0.0, 5.0), nasal=0.0, smile=0.05,
                 c1=[(0, 1.05), (1, 1.10)], c2=[(0, 1.16), (0.35, 1.18), (1, 0.92)],
                 vow=["a", "è", "o", "ou", "on"], cons=["b", "d", "m", "l", "v", "n"]),
}

# Ordre = GueuleId (src/ui/ui_art_gueules.h).
PERSOS = [
    ("pres_gauche", "chaleureux", dict(f0=118)),                         # Verdier : ours chaleureux
    ("pres_centre", "lisse", dict(f0=132, smile=0.08)),                  # Delmas : sourire de publicité
    ("pres_droite", "grave", dict(f0=98, tilt=0.55)),                    # Saint-Aubin : courtois, autoritaire
    ("pm_technocrate", "lisse", dict(f0=124, c1=[(0, 1.0), (1, 1.0)],    # Delaunay : monotone comptable
                                     c2=[(0, 1.01), (1, 0.98)], gap=30, tilt=0.2)),
    ("pm_rassembleur", "alto", dict(f0=205, fs=1.17, jit=0.004, breath=0.06, tilt=0.6,   # Deschamps : ronde, maternelle
                                    c2=[(0, 1.18), (0.3, 1.26), (1, 0.9)])),
    ("pm_reformateur", "vif", dict(f0=160, fs=1.05, blk=(3, 3), gap=15)),  # Lestrade : jeune, pressé
    ("cohab_gauche", "alto", dict(f0=150, fs=1.1, jit=0.02, breath=0.12)),  # Carrel : stentor rauque
    ("cohab_centre", "lisse", dict(f0=116, fs=0.96, tilt=0.55, blk=(4, 5),  # Lacombe : prudent, rond
                                   c2=[(0, 1.04), (1, 0.93)])),
    ("cohab_droite", "nasal", dict(f0=114, fs=0.97, nasal=0.55)),        # Montclar : notable ironique
    ("cand_gauche_radicale", "alto", dict(f0=162, fs=1.12, jit=0.018, breath=0.11, gap=30)),  # Rouvière : rauque, en tribune
    ("cand_ecolo", "vif", dict(f0=215, fs=1.16, tilt=0.45, blk=(4, 4), gap=35,   # Vasseur : plus douce
                               c2=[(0, 1.08), (1, 1.22)], breath=0.08)),
    ("cand_centre", "lisse", dict(f0=205, fs=1.16, smile=0.07)),         # Marchetti : compromis souriant
    ("cand_droite", "grave", dict(f0=108, fs=0.92, tilt=0.65, smile=0.06,   # Valmont : rond, aimable
                                  vow=["o", "a", "on", "ou"], cons=["b", "m", "d", "l"])),
    ("cand_nationaliste", "grave", dict(f0=104, fs=0.97, oq=0.47, gap=40)),  # Lenoir : tribun, bouche immense
    ("min_economie", "lisse", dict(f0=142, fs=1.04, c1=[(0, 1.0), (1, 1.0)],    # Fontaine : énumération comptable
                                   c2=[(0, 1.0), (1, 0.99)], blk=(3, 4), gap=25, tilt=0.2)),
    ("min_interieur", "grave", dict(f0=90, fs=0.93, oq=0.46)),           # Fermont : bouledogue, posé
    ("min_sante", "alto", dict(f0=172, fs=1.15, breath=0.14, tilt=0.6, blk=(4, 4), gap=55,  # Varenne : lasse, drôle
                               c2=[(0, 1.1), (0.3, 1.12), (1, 0.86)])),
    ("min_education", "lisse", dict(f0=128, fs=0.99, gap=80, blk=(4, 4),  # Marchal : syllabes détachées
                                    c1=[(0, 1.02), (1, 1.08)], c2=[(0, 1.1), (1, 0.95)],
                                    vow=["e", "i", "a", "o", "ou", "eu"], cons=["d", "t", "l", "m", "b"])),
    ("min_ecologie", "vif", dict(f0=258, fs=1.2, blk=(3, 3), gap=20)),   # Kervella : bondissante
    ("min_affaires_etr", "nasal", dict(f0=122, fs=0.97, blk=(4, 5), tilt=0.45, nasal=0.4)),  # Valcourt : salon
    ("chef_ugr", "chaleureux", dict(f0=104, fs=0.93, tilt=0.6, breath=0.08)),  # Brossard : ogre jovial
    ("chef_rsv", "alto", dict(f0=188, fs=1.15, jit=0.006, breath=0.07)),  # Aubert : patiente, solide
    ("chef_mjm", "lisse", dict(f0=124, fs=0.96, tilt=0.5, blk=(4, 4),     # Poulain : ni pour ni contre
                               c1=[(0, 1.03), (1, 0.97)], c2=[(0, 1.02), (0.5, 1.06), (1, 0.96)],
                               vow=["eu", "a", "o", "e"], cons=["b", "m", "l", "p"])),
    ("chef_utl", "nasal", dict(f0=112, fs=0.98, nasal=0.5, blk=(4, 4))),  # Dubreuil-Castagnac : ironie glacée
    ("chef_lso", "nasal", dict(f0=134, fs=1.03, blk=(3, 4), gap=30, nasal=0.35)),  # Corbeau : jeune loup
    ("jour_chaine_info", "vif", dict(f0=245, fs=1.17, blk=(3, 3), gap=15, smile=0.08)),  # Lopez : tout est urgent
    ("jour_editorialiste", "nasal", dict(f0=106, fs=0.95, blk=(4, 5), gap=55, nasal=0.55,  # Lenglet : pompeux
                                         c1=[(0, 1.06), (1, 1.0)], c2=[(0, 0.95), (0.5, 1.08), (1, 1.2)])),
    ("senat_president", "chaleureux", dict(f0=112, fs=0.97, blk=(4, 5), gap=70, breath=0.14,  # Mirebeau : doyen lent
                                           jit=0.02, vib=(0.035, 5.5), tilt=0.6)),
    ("synd_ust", "alto", dict(f0=185, fs=1.14, jit=0.01, breath=0.08, smile=0.07)),  # Benali : franche, chaleureuse
    ("patron_upe", "chaleureux", dict(f0=118, fs=0.99, blk=(4, 5), gap=40, smile=0.08,  # Dumoulin : glissando mielleux
                                      c1=[(0, 1.22), (1, 0.98)], c2=[(0, 1.26), (1, 0.84)],
                                      vow=["o", "ou", "an", "a"], cons=["m", "l", "n", "w"])),
    ("huissier", "grave", dict(f0=94, fs=0.94, blk=(4, 5), gap=60, tilt=0.6, oq=0.55,  # Tisserand : huissier impassible et affable
                               breath=0.05, smile=0.05, cons=["b", "m", "d", "l", "n"])),
]


def fiche(name):
    for n, fam, over in PERSOS:
        if n == name:
            p = dict(FAMILLES[fam])
            p.update(over)
            p["name"], p["famille"] = n, fam
            return p
    raise KeyError(name)
