// Mega Président — illustrations : seul fichier à toucher VDP/PAL pour les illustrations.
#include "ui/ui_internal.h"
#include "ui/ui_art.h"
#include "gfx_ui.h"
#include "gfx_art.h"
#include "ui/input.h"
#include "audio/voice.h"

#define ART_SLOTS 24  // jeux de tuiles chargés en même temps (illustrations, gueules, expressions), au plus

// Même ordre que l'enum ArtId (et que res/gfx_art.res).
static const Image *const art_images[ART_COUNT] = {
    &art_title,
    &art_evt_greve, &art_evt_canicule, &art_evt_inondation, &art_evt_scandale, &art_evt_sommet,
    &art_evt_bourse, &art_evt_sante, &art_evt_pollution, &art_evt_securite, &art_evt_numerique,
    &art_evt_education, &art_evt_energie, &art_evt_agriculture, &art_evt_industrie, &art_evt_fete,
    &art_decor_hemicycle, &art_decor_urne, &art_decor_journal,
};

// Tuiles déjà chargées (réutilisées si la même illustration est redessinée).
static u16 slot_id[ART_SLOTS];   // ArtId, ou ART_COUNT + clé de gueule
static u16 slot_tile[ART_SLOTS];
static u8 slot_count;
static u16 next_tile;            // première tuile libre (0 : zone non initialisée)
static const u16 *pal_loaded[4]; // palette d'illustration chargée par ligne (NULL : ligne normale)

static void ga_off(void);        // visages animés (fin du fichier)

static u16 art_base(void)
{
    return ui_tile_icons + ts_ui_icons.numTile;   // juste après les tuiles de ui.c
}

void ui_art_size(ArtId id, u16 *w, u16 *h)
{
    if (id >= ART_COUNT) {
        *w = 0;
        *h = 0;
        return;
    }
    *w = art_images[id]->tilemap->w;
    *h = art_images[id]->tilemap->h;
}

static bool same_palette(const u16 *a, const u16 *b)
{
    u16 i;
    if (a == b)
        return TRUE;
    if (a == NULL || b == NULL)
        return FALSE;
    for (i = 0; i < 16; i++)
        if (a[i] != b[i])
            return FALSE;
    return TRUE;
}

static void use_palette(u16 pal, const u16 *data)
{
    if (!same_palette(pal_loaded[pal], data)) {
        PAL_setPalette(pal, data, DMA);
        pal_loaded[pal] = data;
    }
}

// Renvoie la première tuile en VRAM du jeu de tuiles (chargé au besoin), ou 0.
// key : identifiant unique du jeu (ArtId, ou ART_COUNT + clé de gueule).
static u16 load_tileset(u16 key, const TileSet *ts)
{
    u16 i, t;

    for (i = 0; i < slot_count; i++)
        if (slot_id[i] == key)
            return slot_tile[i];
    if (next_tile == 0)
        next_tile = art_base();
    if (slot_count >= ART_SLOTS || (u32)next_tile + ts->numTile > (u32)TILE_USER_MAX_INDEX + 1)
        return 0;
    t = next_tile;
    VDP_loadTileSet(ts, t, DMA);
    next_tile += ts->numTile;
    slot_id[slot_count] = key;
    slot_tile[slot_count] = t;
    slot_count++;
    return t;
}

u16 ui_art_reserve(u16 key, u16 ntiles)
{
    u16 i, t;

    key |= 0x8000;   // clés réservées : distinctes des ArtId et des gueules
    for (i = 0; i < slot_count; i++)
        if (slot_id[i] == key)
            return slot_tile[i];
    if (next_tile == 0)
        next_tile = art_base();
    if (slot_count >= ART_SLOTS || (u32)next_tile + ntiles > (u32)TILE_USER_MAX_INDEX + 1)
        return 0;
    t = next_tile;
    next_tile += ntiles;
    slot_id[slot_count] = key;
    slot_tile[slot_count] = t;
    slot_count++;
    return t;
}

void ui_art_claim_palette(u16 pal)
{
    static const u16 claimed[16] = { 0xFFFF };   // jamais égale à une vraie palette
    if (pal >= PAL2 && pal <= PAL3)
        pal_loaded[pal] = claimed;
}

bool ui_art_draw_pal(ArtId id, u16 x, u16 y, u16 pal)
{
    const Image *img;
    u16 t;

    if (id >= ART_COUNT)
        return FALSE;
    if (pal != PAL3)
        pal = PAL2;   // contrat V3 : PAL0/PAL1 réservées à l'interface
    img = art_images[id];
    ui_gueule_anim_cover(x, y, img->tilemap->w, img->tilemap->h);
    t = load_tileset(id, img->tileset);
    if (t == 0)
        return FALSE;
    use_palette(pal, img->palette->data);
    VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(pal, t), x, y, 0, 0,
                     img->tilemap->w, img->tilemap->h, CPU);
    return TRUE;
}

bool ui_art_draw(ArtId id, u16 x, u16 y)
{
    return ui_art_draw_pal(id, x, y, PAL2);
}

void ui_art_erase(ArtId id, u16 x, u16 y)
{
    u16 w, h;

    ui_art_size(id, &w, &h);
    if (w)
        VDP_clearTileMapRect(BG_B, x, y, w, h);
}

void ui_art_close(void)
{
    static const Palette *const normal[4] = { &pal_ui_text, &pal_ui_highlight, &pal_ui_dim, &pal_ui_alert };
    u16 i;

    for (i = PAL2; i <= PAL3; i++) {
        if (pal_loaded[i] != NULL) {
            PAL_setPalette(i, normal[i]->data, DMA);
            pal_loaded[i] = NULL;
        }
    }
    slot_count = 0;
    next_tile = 0;
    ga_off();   // tuiles libérées : plus d'animation (sans réécrire le plan)
}

// --- Gueules V3 ------------------------------------------------------------------

typedef struct {
    const Image *base;                 // portrait neutre (GUEULE_W x GUEULE_H tuiles)
    const Image *expr[EXPR_COUNT - 1]; // patchs content, furieux, gêné
    u8 px, py, pw, ph;                 // rectangle des patchs dans le portrait (tuiles)
    // Visages animés (3.18) : une planche par expression, [clignement | regard] sur la
    // rangée des yeux, [parole 1 | parole 2] en dessous (tools/art/make_art.py).
    const Image *anim[EXPR_COUNT];
    u8 aflags[EXPR_COUNT];             // GA_BLINK | GA_LOOK | GA_TALK selon l'expression
    u8 ex, ey, ew, eh;                 // rectangle des yeux (tuiles, inclus dans le patch)
    u8 mx, my, mw, mh;                 // rectangle de la bouche (idem)
} GueuleDef;

#define GA_BLINK 1
#define GA_LOOK  2
#define GA_TALK  4

#define UI_ART_GUEULES_TABLE
#include "ui/ui_art_gueules.h"

static u16 gueule_key(GueuleId id, Expr e)
{
    return ART_COUNT + (u16)id * EXPR_COUNT + e;
}

static u16 gueule_anim_key(GueuleId id, Expr e)
{
    return ART_COUNT + GUEULE_COUNT * EXPR_COUNT + (u16)id * EXPR_COUNT + e;
}

static u16 gueule_pal(u16 pal)
{
    return (pal == PAL3) ? PAL3 : PAL2;   // contrat V3 : PAL0/PAL1 réservées à l'interface
}

// Pose le patch de l'expression e (sans animation).
static bool expr_draw(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    const GueuleDef *g = &gueule_defs[id];
    const Image *img;
    u16 t;

    if (e == EXPR_NEUTRE) {   // retour au neutre : sous-rectangle du portrait de base
        img = g->base;
        t = load_tileset(gueule_key(id, EXPR_NEUTRE), img->tileset);
        if (t == 0)
            return FALSE;
        VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(pal, t), x + g->px, y + g->py,
                         g->px, g->py, g->pw, g->ph, CPU);
        return TRUE;
    }
    img = g->expr[e - 1];
    t = load_tileset(gueule_key(id, e), img->tileset);
    if (t == 0)
        return FALSE;
    use_palette(pal, img->palette->data);
    VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(pal, t), x + g->px, y + g->py, 0, 0,
                     g->pw, g->ph, CPU);
    return TRUE;
}

// --- Visages animés (3.18) -----------------------------------------------------------
// Une seule gueule animée : la dernière dessinée par ui_gueule_draw. Les durées sont en
// 1/300 s (input_frame_ticks : 6 en PAL, 5 en NTSC), donc identiques en PAL et en NTSC.
// Aléa : petit générateur à part, jamais celui de la simulation. Aucune allocation.

#define MS(ms) ((u16)((u32)(ms) * 3 / 10))   // millisecondes -> 1/300 s

enum { EYE_OPEN, EYE_CLOSED, EYE_GAP, EYE_LOOK };   // état des yeux
enum { F_REST = 0, F_ONE, F_TWO };                  // image : repos, 1re, 2e

static struct {
    bool on;
    u8 id, expr, x, y, pal;
    u8 eye_state, eye_shown, mouth_shown, mouth_want;
    bool double_blink;   // second clignement à venir
    u16 anim_tile;       // planche de l'expression en VRAM (0 : pas encore chargée)
    u16 eye_t;           // durée restante de l'état des yeux (fermés, entre-deux, regard)
    u16 blink_t, look_t; // avant le prochain clignement, le prochain regard
    u16 mouth_t;         // durée restante de l'image de bouche
    u32 talk_t;          // parole restante
    bool sung;           // bouche pilotée par la voix (charabia), pas par le hasard
} ga;

static u16 ga_seed = 0x5EED;
static void ga_rest(void);
static bool ga_enabled = TRUE;   // réglage « Visages » (Options, 3.19)

void ui_gueule_anim_enable(bool on)
{
    ga_enabled = on;
    if (!on) {
        ga_rest();       // image de base de l'expression, la gueule reste « suivie »
        ga.talk_t = 0;
    }
}

static u16 ga_rand(u16 lo, u16 hi)   // xorshift 16 bits, bornes incluses
{
    ga_seed ^= ga_seed << 7;
    ga_seed ^= ga_seed >> 9;
    ga_seed ^= ga_seed << 8;
    return lo + ga_seed % (u16)(hi - lo + 1);
}

static u16 ga_dec(u16 t, u16 dt)
{
    return t > dt ? t - dt : 0;
}

// Écrit l'image f des yeux (eyes TRUE) ou de la bouche. FALSE si VRAM pleine.
static bool ga_write(bool eyes, u8 f)
{
    const GueuleDef *g = &gueule_defs[ga.id];
    u16 rx = eyes ? g->ex : g->mx, ry = eyes ? g->ey : g->my;
    u16 rw = eyes ? g->ew : g->mw, rh = eyes ? g->eh : g->mh;
    const Image *img;
    u16 t, sx, sy;

    if (rw == 0)
        return TRUE;
    if (f == F_REST) {   // retour à l'expression : sous-rectangle du patch (ou du portrait)
        if (ga.expr == EXPR_NEUTRE) {
            img = g->base;
            sx = rx;
            sy = ry;
        } else {
            img = g->expr[ga.expr - 1];
            sx = rx - g->px;
            sy = ry - g->py;
        }
        t = load_tileset(gueule_key(ga.id, ga.expr), img->tileset);
    } else {
        img = g->anim[ga.expr];
        if (ga.anim_tile == 0)   // chargée à la première image : une seule planche par gueule
            ga.anim_tile = load_tileset(gueule_anim_key(ga.id, ga.expr), img->tileset);
        t = ga.anim_tile;
        sx = (f == F_TWO) ? rw : 0;
        sy = eyes ? 0 : g->eh;
    }
    if (t == 0)
        return FALSE;
    VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(ga.pal, t), ga.x + rx, ga.y + ry, sx, sy, rw, rh, CPU);
    return TRUE;
}

// Remet les yeux et la bouche au repos (gueule toujours affichée).
static void ga_rest(void)
{
    if (!ga.on)
        return;
    if (ga.eye_shown != F_REST)
        ga_write(TRUE, F_REST);
    if (ga.mouth_shown != F_REST)
        ga_write(FALSE, F_REST);
    ga.eye_shown = ga.mouth_shown = F_REST;
}

static void ga_start(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    if (ga.on && (ga.x != x || ga.y != y))
        ga_rest();   // l'ancienne gueule reste affichée, immobile
    ga.on = TRUE;
    ga.id = (u8)id;
    ga.expr = (u8)e;
    ga.x = (u8)x;
    ga.y = (u8)y;
    ga.pal = (u8)pal;
    ga.anim_tile = 0;
    ga.eye_state = EYE_OPEN;
    ga.eye_shown = ga.mouth_shown = ga.mouth_want = F_REST;
    ga.double_blink = FALSE;
    ga.blink_t = ga_rand(MS(1200), MS(3500));   // premier clignement assez tôt
    ga.look_t = ga_rand(MS(5000), MS(12000));
    ga.talk_t = 0;
    ga.sung = FALSE;
    ga.mouth_t = 0;
    ga.sung = FALSE;
}

static void ga_off(void)
{
    ga.on = FALSE;
    ga.sung = FALSE;
    voice_say_stop();
    ga.talk_t = 0;
}

void ui_gueule_anim_stop(void)
{
    ga.sung = FALSE;
    voice_say_stop();
    ga_rest();
    ga.on = FALSE;
}

static bool overlaps(u16 x, u16 y, u16 w, u16 h, u16 rx, u16 ry, u16 rw, u16 rh)
{
    return rw && x < rx + rw && rx < x + w && y < ry + rh && ry < y + h;
}

void ui_gueule_anim_cover(u16 x, u16 y, u16 w, u16 h)
{
    const GueuleDef *g;

    if (!ga.on)
        return;
    g = &gueule_defs[ga.id];
    if (overlaps(x, y, w, h, ga.x + g->ex, ga.y + g->ey, g->ew, g->eh) ||
        overlaps(x, y, w, h, ga.x + g->mx, ga.y + g->my, g->mw, g->mh))
    {
        ga.on = FALSE;   // visage recouvert : ne plus y écrire
        ga.sung = FALSE;
        ga.talk_t = 0;
        voice_say_stop();   // et se taire
    }
}

void ui_gueule_say(u16 ms)
{
    ga.sung = FALSE;
    if (ga.on && voice_say(ga.id)) {
        ga.sung = TRUE;
        ga.talk_t = 0;
        return;
    }
    ui_gueule_talk(ms);   // voix coupée : la bouche remue sans son, comme en V3
}

void ui_gueule_talk(u16 ms)
{
    ga.sung = FALSE;
    ga.talk_t = (u32)ms * 3 / 10;
    ga.mouth_t = 0;
}

u16 ui_gueule_talk_ms(const char *text)
{
    u32 n = 0;

    while (text && *text) {
        if ((u8)*text < 0x80 || (u8)*text >= 0xC0)   // un caractère UTF-8 = un octet de tête
            n++;
        text++;
    }
    n *= 45;
    return (u16)(n < 600 ? 600 : (n > 4000 ? 4000 : n));
}

static void ga_eyes(u16 dt, u8 flags)
{
    ga.blink_t = ga_dec(ga.blink_t, dt);
    if (ga.eye_state != EYE_LOOK)
        ga.look_t = ga_dec(ga.look_t, dt);
    switch (ga.eye_state) {
    case EYE_OPEN:
        if (ga.blink_t == 0) {
            ga.eye_state = EYE_CLOSED;
            ga.eye_t = ga_rand(MS(100), MS(130));
            ga.double_blink = ga_rand(0, 5) == 0;   // 1 fois sur 6
        } else if (ga.look_t == 0 && (flags & GA_LOOK)) {
            ga.eye_state = EYE_LOOK;
            ga.eye_t = ga_rand(MS(800), MS(1500));
        }
        break;
    case EYE_CLOSED:
        ga.eye_t = ga_dec(ga.eye_t, dt);
        if (ga.eye_t == 0) {
            if (ga.double_blink) {
                ga.double_blink = FALSE;
                ga.eye_state = EYE_GAP;
                ga.eye_t = ga_rand(MS(110), MS(170));
            } else {
                ga.eye_state = EYE_OPEN;
                ga.blink_t = ga_rand(MS(2500), MS(6000));
            }
        }
        break;
    case EYE_GAP:
        ga.eye_t = ga_dec(ga.eye_t, dt);
        if (ga.eye_t == 0) {
            ga.eye_state = EYE_CLOSED;
            ga.eye_t = ga_rand(MS(90), MS(120));
        }
        break;
    default:   // EYE_LOOK : le regard revient, parfois avec un clignement
        ga.eye_t = ga_dec(ga.eye_t, dt);
        if (ga.eye_t == 0) {
            ga.eye_state = EYE_OPEN;
            ga.look_t = ga_rand(MS(8000), MS(15000));
            if (ga.blink_t < MS(400) || ga_rand(0, 2) == 0)
                ga.blink_t = ga_rand(MS(60), MS(250));
        }
        break;
    }
}

static void ga_mouth(u16 dt, u8 flags)
{
    if (ga.sung) {   // charabia : la bouche suit le son, puis repos (seuls les yeux bougent)
        if (!(flags & GA_TALK) || !voice_talking()) {
            ga.sung = FALSE;
            ga.mouth_want = F_REST;
        } else {
            ga.mouth_want = voice_mouth();
        }
        return;
    }
    if (ga.talk_t == 0 || !(flags & GA_TALK)) {
        ga.talk_t = 0;
        ga.mouth_want = F_REST;
        return;
    }
    ga.talk_t = ga.talk_t > dt ? ga.talk_t - dt : 0;
    if (ga.talk_t == 0 || (input_pressed() & (BUTTON_A | BUTTON_B | BUTTON_C | BUTTON_START))) {
        ga.talk_t = 0;   // arrêt net à la fin de la réplique ou à l'appui sur un bouton
        ga.mouth_want = F_REST;
        return;
    }
    ga.mouth_t = ga_dec(ga.mouth_t, dt);
    if (ga.mouth_t == 0) {   // image suivante, jamais deux fois la même
        u16 r = ga_rand(0, 9);
        if (ga.mouth_want == F_REST)
            ga.mouth_want = r < 6 ? F_ONE : F_TWO;
        else if (ga.mouth_want == F_ONE)
            ga.mouth_want = r < 5 ? F_TWO : F_REST;
        else
            ga.mouth_want = r < 6 ? F_ONE : F_REST;
        ga.mouth_t = ga_rand(MS(90), MS(160));
    }
}

void ui_gueule_anim_update(void)
{
    u16 dt = input_frame_ticks();
    u8 flags, eye_want;

    if (!ga.on)
        return;
    if (!ga_enabled || !ui_anim_allowed() || ui_msgbox_is_open()) {
        ga_rest();   // fondu ou boîte de message : visage au repos, horloges arrêtées
        return;
    }
    flags = gueule_defs[ga.id].aflags[ga.expr];
    ga_eyes(dt, flags);
    ga_mouth(dt, flags);
    eye_want = F_REST;
    if (ga.eye_state == EYE_CLOSED && (flags & GA_BLINK))
        eye_want = F_ONE;
    else if (ga.eye_state == EYE_LOOK && (flags & GA_LOOK))
        eye_want = F_TWO;
    // Une seule écriture de rectangle par image : les yeux d'abord, la bouche ensuite.
    if (eye_want != ga.eye_shown) {
        if (!ga_write(TRUE, eye_want)) {
            ga.on = FALSE;
            return;
        }
        ga.eye_shown = eye_want;
    } else if (ga.mouth_want != ga.mouth_shown) {
        if (!ga_write(FALSE, ga.mouth_want)) {
            ga.on = FALSE;
            return;
        }
        ga.mouth_shown = ga.mouth_want;
    }
}

bool ui_gueule_expr(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    u8 old;

    if (id >= GUEULE_COUNT || e >= EXPR_COUNT)
        return FALSE;
    pal = gueule_pal(pal);
    if (!expr_draw(id, e, x, y, pal))
        return FALSE;
    if (!(ga.on && ga.id == id && ga.x == x && ga.y == y))
        return TRUE;
    // Le patch vient de recouvrir yeux et bouche : ils sont au repos.
    old = ga.expr;
    ga.expr = (u8)e;
    ga.pal = (u8)pal;
    ga.eye_shown = ga.mouth_shown = F_REST;
    if (old != e) {
        ga.anim_tile = 0;
        // Changement d'expression masqué par un clignement de 80 ms, posé tout de suite.
        ga.eye_state = EYE_CLOSED;
        ga.eye_t = MS(80);
        ga.double_blink = FALSE;
        if ((gueule_defs[id].aflags[e] & GA_BLINK) && ga_enabled && ui_anim_allowed() && ga_write(TRUE, F_ONE))
            ga.eye_shown = F_ONE;
    }
    return TRUE;
}

bool ui_gueule_draw(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    const Image *img;
    u16 t;

    if (id >= GUEULE_COUNT || e >= EXPR_COUNT)
        return FALSE;
    pal = gueule_pal(pal);
    img = gueule_defs[id].base;
    t = load_tileset(gueule_key(id, EXPR_NEUTRE), img->tileset);
    if (t == 0)
        return FALSE;
    if (ga.on && ga.x == x && ga.y == y)
        ga.on = FALSE;   // même emplacement : l'ancienne gueule est recouverte
    use_palette(pal, img->palette->data);
    VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(pal, t), x, y, 0, 0, GUEULE_W, GUEULE_H, CPU);
    if (e != EXPR_NEUTRE && !expr_draw(id, e, x, y, pal))
        return FALSE;
    ga_start(id, e, x, y, pal);
    return TRUE;
}

void ui_gueule_erase(u16 x, u16 y)
{
    ui_gueule_anim_cover(x, y, GUEULE_W, GUEULE_H);
    VDP_clearTileMapRect(BG_B, x, y, GUEULE_W, GUEULE_H);
}

bool ui_portrait(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    ui_medallion(x, y, UI_PORTRAIT_W, UI_PORTRAIT_H);
    return ui_gueule_draw(id, e, x, y, pal);
}
