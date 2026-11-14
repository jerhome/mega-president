// Mega Président — illustrations : seul fichier à toucher VDP/PAL pour les illustrations.
#include "ui/ui_internal.h"
#include "ui/ui_art.h"
#include "gfx_ui.h"
#include "gfx_art.h"

#define ART_SLOTS 16  // illustrations différentes affichées en même temps, au plus

// Même ordre que l'enum ArtId (et que res/gfx_art.res).
static const Image *const art_images[ART_COUNT] = {
    &art_title,
    &art_pres_gauche, &art_pres_centre, &art_pres_droite,
    &art_pm_technocrate, &art_pm_rassembleur, &art_pm_reformateur,
    &art_cohab_gauche, &art_cohab_centre, &art_cohab_droite,
    &art_cand_roussel, &art_cand_vasseur, &art_cand_marchetti, &art_cand_valmont, &art_cand_lenoir,
    &art_evt_greve, &art_evt_canicule, &art_evt_inondation, &art_evt_scandale, &art_evt_sommet,
    &art_evt_bourse, &art_evt_sante, &art_evt_pollution, &art_evt_securite, &art_evt_numerique,
    &art_evt_education, &art_evt_energie, &art_evt_agriculture, &art_evt_industrie, &art_evt_fete,
    &art_decor_hemicycle, &art_decor_urne, &art_decor_journal,
};

// Tuiles déjà chargées (réutilisées si la même illustration est redessinée).
static u8 slot_id[ART_SLOTS];
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

// Renvoie la première tuile en VRAM de l'illustration (chargée au besoin), ou 0.
static u16 load_tiles(ArtId id)
{
    const TileSet *ts = art_images[id]->tileset;
    u16 i, t;

    for (i = 0; i < slot_count; i++)
        if (slot_id[i] == id)
            return slot_tile[i];
    if (next_tile == 0)
        next_tile = art_base();
    if (slot_count >= ART_SLOTS || (u32)next_tile + ts->numTile > (u32)TILE_USER_MAX_INDEX + 1)
        return 0;
    t = next_tile;
    VDP_loadTileSet(ts, t, DMA);
    next_tile += ts->numTile;
    slot_id[slot_count] = id;
    slot_tile[slot_count] = t;
    slot_count++;
    return t;
}

bool ui_art_draw_pal(ArtId id, u16 x, u16 y, u16 pal)
{
    const Image *img;
    u16 t;

    if (id >= ART_COUNT)
        return FALSE;
    if (pal == PAL0 || pal > PAL3)
        pal = PAL2;
    img = art_images[id];
    t = load_tiles(id);
    if (t == 0)
        return FALSE;
    if (!same_palette(pal_loaded[pal], img->palette->data)) {
        PAL_setPalette(pal, img->palette->data, DMA);
        pal_loaded[pal] = img->palette->data;
    }
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

    for (i = PAL1; i <= PAL3; i++) {
        if (pal_loaded[i] != NULL) {
            PAL_setPalette(i, normal[i]->data, DMA);
            pal_loaded[i] = NULL;
        }
    }
    slot_count = 0;
    next_tile = 0;
}
