"""Outils communs aux gueules V3 (buste, cou, oreilles, textures) et registre.

Tous les personnages sont fictifs. Repères (80 x 96) : axe x = 40, tête vers
y 10-66, cou 54-76, épaules à partir de y 72.
Les personnages sont définis dans g_*.py ; ALL donne l'ordre (= enum PortraitId).
"""
import math

from gueules import Portrait, W, H
from raster import ellipse, poly, rect, line, polyline, border
import traits as T

INK = ("ink", 0)
CX = 40


def P_(ramps):
    r = {"ink": [(1, 0, 1)]}
    r.update(ramps)
    return Portrait(r)


# --- buste -------------------------------------------------------------------------

def bust(P, top=72, wide=34, suit="suit", shirt="white", tie=None, collar=True,
         lapels=True, vdepth=14, vw=7, sweater=False, bow=False, open_collar=False):
    """À appeler APRÈS neck() : la veste et le col recouvrent le bas du cou."""
    """Épaules, veste, chemise, cravate. Retourne la couche de la veste."""
    s = P.layer(suit)
    pts = [(CX - wide - 6, H + 2), (CX - wide, top + 10), (CX - wide + 6, top + 4),
           (CX - 12, top - 1), (CX + 12, top - 1), (CX + wide - 6, top + 4),
           (CX + wide, top + 10), (CX + wide + 6, H + 2)]
    s.shape(poly(pts), h=10, r=9)
    if shirt:
        v = poly([(CX - vw - 1, top - 3), (CX + vw + 1, top - 3), (CX, top + vdepth)])
        if open_collar:   # col ouvert : la peau du cou continue dans l'échancrure
            sh = P.layer("skin", ink=True)
            sh.shape(v, h=3, r=3)
            sh.cut(ellipse(CX, top - 4, vw + 2, 3))
        else:
            sh = P.layer(shirt, ink=True)
            sh.shape(v, h=3, r=3)
        if collar and not sweater:
            c = P.layer(shirt, ink="always", cast=(1, 2))
            c.shape(poly([(CX - vw - 2, top - 5), (CX - 1, top + 2), (CX - 4, top + 7),
                           (CX - vw - 3, top + 1)]), h=2, r=2)
            c.shape(poly([(CX + vw + 2, top - 5), (CX + 1, top + 2), (CX + 4, top + 7),
                           (CX + vw + 3, top + 1)]), h=2, r=2)
    if tie and not bow:
        t = P.layer(tie, ink=True, cast=(1, 1))
        t.shape(poly([(CX - 2.5, top + 1), (CX + 2.5, top + 1), (CX + 4, top + 16),
                      (CX, top + 22), (CX - 4, top + 16)]), h=2, r=2)
        t.shape(poly([(CX - 3, top - 3), (CX + 3, top - 3), (CX + 2, top + 2), (CX - 2, top + 2)]), h=3, r=2)
    if lapels and not sweater:
        lp = P.layer(suit, ink="always", cast=(1, 1))
        lp.shape(poly([(CX - vw - 3, top - 3), (CX - 1, top + vdepth + 1), (CX - 6, top + vdepth + 6),
                       (CX - 13, top + 4)]), h=3, r=3)
        lp.shape(poly([(CX + vw + 3, top - 3), (CX + 1, top + vdepth + 1), (CX + 6, top + vdepth + 6),
                       (CX + 13, top + 4)]), h=3, r=3)
    if tie and bow:
        t = P.layer(tie, ink=True, cast=(1, 1))
        t.shape(poly([(CX - 7, top - 2), (CX, top + 1), (CX - 7, top + 4)]), h=2, r=2)
        t.shape(poly([(CX + 7, top - 2), (CX, top + 1), (CX + 7, top + 4)]), h=2, r=2)
        t.ell(CX, top + 1, 2, 2, h=2)
    return s


def neck(P, w=16, top=54, bottom=76, skin="skin"):
    n = P.layer(skin)
    n.shape(rect(CX - w // 2, top, CX + w // 2 - 1, bottom), h=6, r=7)
    return n


def ears(head, y=40, dx=21, rx=3.5, ry=6):
    head.ell(CX - dx, y, rx, ry, h=3)
    head.ell(CX + dx, y, rx, ry, h=3)


def ear_detail(cv, y=40, dx=21):
    for s in (-1, 1):
        x = CX + s * dx
        T.wrinkle(cv, [(x, y - 3), (x + s, y - 1), (x + s, y + 2), (x, y + 3)])


# --- textures ---------------------------------------------------------------------

def curl_marks(cv, layer, centers, key=("hair", 0), hi=None):
    """Boucles : arc sombre en bas à droite de chaque boucle, reflet en haut à gauche."""
    for (x, y, r) in centers:
        for a in range(-20, 140, 12):
            px = int(math.floor(x + r * 0.8 * math.cos(math.radians(a))))
            py = int(math.floor(y + r * 0.75 * math.sin(math.radians(a))))
            if cv.owner_is(px, py, layer) and cv.get(px, py)[0] != "ink":
                cv.put(px, py, key)
        if hi:
            hx, hy = int(x - r * 0.35), int(y - r * 0.4)
            for dx in (0, 1):
                if cv.owner_is(hx + dx, hy, layer):
                    cv.put(hx + dx, hy, hi)


def stripes(cv, layer, period=3, steps=1, x0=0):
    """Velours côtelé / rayures : colonnes assombries."""
    for y in range(H):
        for x in range(W):
            if (x - x0) % period == 0 and cv.owner_is(x, y, layer):
                k = cv.get(x, y)
                if k[0] != "ink":
                    cv.put(x, y, (k[0], max(0, k[1] - steps)))


def comb(cv, layer, lines, key):
    """Mèches : traits (polylignes) dessinés seulement sur la couche."""
    for pts in lines:
        m = polyline([(int(round(a)), int(round(b))) for a, b in pts])
        for (x, y) in m:
            if cv.owner_is(x, y, layer) and cv.get(x, y)[0] != "ink":
                cv.put(x, y, key)


def round_glasses(cv, y, dx=8, rx=6, ry=5.2, cx=CX, key=INK):
    for s in (-1, 1):
        m = ellipse(cx + s * dx, y, rx, ry)
        cv.fill(border(m), key)
        hx, hy = int(cx + s * dx - rx * 0.5), int(y - ry * 0.45)
        cv.put(hx, hy, ("white", 1))
        cv.put(hx + 1, hy - 1, ("white", 1))
    cv.fill(line(int(cx - dx + rx) - 1, int(y) - 1, int(cx + dx - rx), int(y) - 1), key)
    cv.fill(line(int(cx - dx - rx), int(y) - 1, int(cx - dx - rx) - 4, int(y)), key)
    cv.fill(line(int(cx + dx + rx) - 1, int(y) - 1, int(cx + dx + rx) + 3, int(y)), key)


def rect_glasses(cv, y, dx=8, w=11, h=6, cx=CX, key=INK, thick=1):
    for s in (-1, 1):
        x0 = int(round(cx + s * dx - w / 2))
        m = rect(x0, y, x0 + w - 1, y + h - 1)
        cv.fill(border(m), key)
        if thick > 1:
            cv.fill(line(x0, y + 1, x0 + w - 1, y + 1), key)
        cv.put(x0 + 1, y + 1 + (thick > 1), ("white", 1))
        cv.put(x0 + 2, y + 1 + (thick > 1), ("white", 1))
    xl = int(round(cx - dx - w / 2))
    xr = int(round(cx + dx + w / 2)) - 1
    cv.fill(line(xl + w, y + 1, int(round(cx + dx - w / 2)) - 1, y + 1), key)
    cv.fill(line(xl - 1, y + 1, xl - 3, y + 2), key)
    cv.fill(line(xr + 1, y + 1, xr + 3, y + 2), key)


EXP_EYE = {   # (ouverture, inclinaison, paupière inf. remontée)
    "neutre": (0.9, 0.0, 0.0),
    "content": (0.55, 0.0, 1.6),
    "furieux": (0.6, 2.2, 0.0),
    "gene": (0.95, -1.2, 0.0),
}
EXP_BROW = {  # (intérieur, extérieur)
    "neutre": (0, 0),
    "content": (-1, -2),
    "furieux": (4, -1),
    "gene": (-3, 1),
}


def skull(P, cy=38, rx=17, ry=21, jaw=None, chin=None, ear_y=None, ear_dx=None, ear_r=(3.5, 6),
          sockets=(8, 3), cheeks=None, cast=(1, 3), gain=1.0, cx=CX, mat="skin"):
    """Tête standard : crâne (ellipsoïde), mâchoire (rx, ry, cy), menton (rx, ry, cy),
    oreilles, orbites creusées (dx, profondeur) au niveau des yeux, pommettes (dx, y, r)."""
    head = P.layer(mat, cast=cast, gain=gain)
    head.ell(cx, cy, rx, ry, h=min(rx, ry) - 2)
    if jaw:
        head.ell(cx, jaw[2], jaw[0], jaw[1], h=min(jaw[0], jaw[1]))
    if chin:
        head.ell(cx, chin[2], chin[0], chin[1], h=min(chin[0], chin[1]))
    if ear_dx:
        head.ell(cx - ear_dx, ear_y, ear_r[0], ear_r[1], h=3)
        head.ell(cx + ear_dx, ear_y, ear_r[0], ear_r[1], h=3)
    if sockets:
        dx, depth = sockets
        y = (ear_y - 3) if ear_y else cy - 1
        head.ell(cx - dx, y, 5.5, 3, h=depth, mode="sub")
        head.ell(cx + dx, y, 5.5, 3, h=depth, mode="sub")
    if cheeks:
        dx, y, r = cheeks
        head.ell(cx - dx, y, r, r * 0.85, h=2.5, mode="add")
        head.ell(cx + dx, y, r, r * 0.85, h=2.5, mode="add")
    return head


def hair(P, mask, h=4, r=3, cast=(1, 2), gain=1.3, mat="hair", smooth=1, ink=True, dome=None):
    """Chevelure : masque gonflé ; dome = (cx, cy, rx, ry, h) ajoute un volume arrondi
    (calotte) pour que le dessus de la tête prenne la lumière comme une sphère."""
    l = P.layer(mat, cast=cast, gain=gain, smooth=smooth, ink=ink)
    l.shape(mask, h=h, r=r)
    if dome:
        dx, dy, rx, ry, dh = dome
        l.ell(dx, dy, rx, ry, h=dh, mode="add")
    return l


def nose_layer(P, cast=(1, 1), dirs="rdl"):
    return P.layer("skin", ink="shadow", cast=cast, ink_dirs=dirs)


def nostrils(cv, y, dx=3, cx=CX):
    cv.put(cx - dx, y, INK)
    cv.put(cx + dx - 1, y, INK)


def checks(cv, layer, period=6, key_steps=1):
    """Chemise à carreaux : lignes et colonnes assombries."""
    for y in range(H):
        for x in range(W):
            if cv.owner_is(x, y, layer) and (x % period in (0, 1) or y % period in (0, 1)):
                k = cv.get(x, y)
                if k[0] != "ink":
                    d = key_steps + (1 if (x % period in (0, 1) and y % period in (0, 1)) else 0)
                    cv.put(x, y, (k[0], max(0, k[1] - d)))


def dots(cv, layer, period=4, key=("white", 1)):
    for y in range(H):
        for x in range(W):
            if cv.owner_is(x, y, layer) and x % period == 0 and y % period == (x // period) % 2 * 2:
                if cv.get(x, y)[0] != "ink":
                    cv.put(x, y, key)


def face(P, eye_y=37, eye_dx=8, eye_w=8, eye_h=4, iris_r=1.8, lid=1, lashes=False, bags=0,
         droop=0.0, lidfold=True, eyes=None, brow_y=None, brow_dx=None, brow_w=11, brow_thick=3,
         brow_key=("hair", 0), brow_arch=2.0, taper=True, brows=None, mouth_y=56, mouths=None,
         extras=None, glasses=None, cx=CX, look=None, iris=("iris", 0)):
    """Traits standard du visage selon l'expression.
    eyes : expr -> dict de paramètres de T.eye (open_, tilt, raise_, look, closed_smile...)
    brows : expr -> (intérieur, extérieur) ou ((g_int, g_ext), (d_int, d_ext))
    mouths : expr -> dict de paramètres de T.mouth (kind, w, curve, open_...), dx/dy relatifs
    extras : expr -> fonction(cv) (sueur, veine, rougeur...)
    glasses : fonction(cv, e) dessinée après les yeux."""
    brow_y = eye_y - 8 if brow_y is None else brow_y
    brow_dx = eye_dx + 1 if brow_dx is None else brow_dx

    def draw(cv, e):
        ep = {"open_": EXP_EYE[e][0], "tilt": EXP_EYE[e][1], "raise_": EXP_EYE[e][2],
              "look": 2 if e == "gene" else 0}
        if eyes and e in eyes:
            ep.update(eyes[e])
        for s in (-1, 1):
            q = dict(ep)
            right = q.pop("right", {})   # surcharge pour l'œil droit (asymétrie)
            if s == 1:
                q.update(right)
            T.eye(cv, cx + s * eye_dx, eye_y, w=eye_w, h=eye_h, side=s, iris_r=iris_r, lid=lid,
                  lashes=lashes, bags=bags, droop=q.pop("droop", droop), lidfold=lidfold, iris=iris, **q)
        b = (brows or {}).get(e, EXP_BROW[e])
        if isinstance(b[0], tuple):
            bl, br = b
        else:
            bl = br = b
        for s, (idy, ody) in ((-1, bl), (1, br)) if brow_w else ():
            T.brow(cv, cx + s * brow_dx, brow_y, w=brow_w, side=s, inner_dy=idy, outer_dy=ody,
                   arch=brow_arch, thick=brow_thick, key=brow_key, taper=taper)
        if glasses:
            glasses(cv, e)
        m = dict((mouths or {}).get(e, {}))
        if not m:
            m = {"neutre": {"kind": "line", "curve": 0.5}, "content": {"kind": "smile", "open_": 3},
                 "furieux": {"kind": "shout", "open_": 3.5}, "gene": {"kind": "wavy"}}[e]
        mx = cx + m.pop("dx", 0)
        my = mouth_y + m.pop("dy", 0)
        m.setdefault("w", 11)
        lips = m.pop("lips", None)
        T.mouth(cv, mx, my, **m)
        if lips:
            lips(cv, mx, my, m)
        if extras and e in extras:
            extras[e](cv)
    P.draw(draw)
    return draw


def lipstick(cv, mx, my, m, key=("acc", 0), key2=("acc", 1)):
    """Lèvres colorées autour d'une bouche fermée ou ouverte."""
    w = m.get("w", 10)
    hw = w / 2.0
    for x in range(int(mx - hw + 2), int(mx + hw - 1)):
        for (y, k) in ((my - 1, key), (my + 1, key2), (my + 2, key)):
            c = cv.get(x, y)
            if c and c[0] == "skin":
                cv.put(x, y, k)


def sweat_at(x, y):
    return lambda cv: T.sweat(cv, x, y)


def vein_at(x, y, key=("acc", 0)):
    return lambda cv: T.vein(cv, x, y, key)


def blush_at(y, dx=11, w=4):
    def f(cv):
        for s in (-1, 1):
            T.blush(cv, CX + s * dx - 2, y, w=w)
    return f


def both(*fs):
    def f(cv):
        for g in fs:
            g(cv)
    return f


def registry():
    import g_demo
    import g_pouvoir
    import g_candidats
    import g_gouv
    import g_divers
    order = ["pres_gauche", "pres_centre", "pres_droite", "pm_technocrate", "pm_rassembleur",
             "pm_reformateur", "cohab_gauche", "cohab_centre", "cohab_droite",
             "cand_gauche_radicale", "cand_ecolo", "cand_centre", "cand_droite", "cand_nationaliste",
             "min_economie", "min_interieur", "min_sante", "min_education", "min_ecologie",
             "min_affaires_etr", "chef_ugr", "chef_rsv", "chef_mjm", "chef_utl", "chef_lso",
             "jour_chaine_info", "jour_editorialiste", "senat_president", "synd_ust", "patron_upe", "huissier"]
    found = {}
    for mod in (g_demo, g_pouvoir, g_candidats, g_gouv, g_divers):
        for name, fn in mod.ALL:
            found[name] = fn
    return [(n, found[n]) for n in order if n in found]


def __getattr__(name):
    if name == "ALL":
        return registry()
    raise AttributeError(name)
