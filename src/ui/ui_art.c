// Mega Président — illustrations : seul fichier à toucher VDP/PAL pour les illustrations.
#include "ui/ui_internal.h"
#include "ui/ui_art.h"
#include "gfx_ui.h"
#include "gfx_art.h"

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
}

// --- Gueules V3 ------------------------------------------------------------------

typedef struct {
    const Image *base;                 // portrait neutre (GUEULE_W x GUEULE_H tuiles)
    const Image *expr[EXPR_COUNT - 1]; // patchs content, furieux, gêné
    u8 px, py, pw, ph;                 // rectangle des patchs dans le portrait (tuiles)
} GueuleDef;

#define UI_ART_GUEULES_TABLE
#include "ui/ui_art_gueules.h"

static u16 gueule_key(GueuleId id, Expr e)
{
    return ART_COUNT + (u16)id * EXPR_COUNT + e;
}

static u16 gueule_pal(u16 pal)
{
    return (pal == PAL3) ? PAL3 : PAL2;   // contrat V3 : PAL0/PAL1 réservées à l'interface
}

bool ui_gueule_expr(GueuleId id, Expr e, u16 x, u16 y, u16 pal)
{
    const GueuleDef *g;
    const Image *img;
    u16 t;

    if (id >= GUEULE_COUNT || e >= EXPR_COUNT)
        return FALSE;
    g = &gueule_defs[id];
    pal = gueule_pal(pal);
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
    use_palette(pal, img->palette->data);
    VDP_setTileMapEx(BG_B, img->tilemap, UI_ATTR(pal, t), x, y, 0, 0, GUEULE_W, GUEULE_H, CPU);
    if (e != EXPR_NEUTRE)
        return ui_gueule_expr(id, e, x, y, pal);
    return TRUE;
}

void ui_gueule_erase(u16 x, u16 y)
{
    VDP_clearTileMapRect(BG_B, x, y, GUEULE_W, GUEULE_H);
}
