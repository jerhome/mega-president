// Mega Président — hémicycle dynamique (voir ui_hemi.h).
#include "ui/ui_internal.h"
#include "ui/ui_hemi.h"
#include "ui/ui_art.h"
#include "ui/input.h"
#include "ui/ui_hemi_data.h"

#define HEMI_PAL   PAL3
#define STEP_TICKS 75      // 250 ms (1/300 s) entre deux groupes qui s'allument
#define BLINK_TICKS 120    // demi-période du clignotement du groupe en évidence

// Couleurs (niveaux VDP 0-7) : par groupe, reflet et fond du siège, allumé puis éteint.
#define RGB(r, g, b) ((u16)(((r) << 1) | ((g) << 5) | ((b) << 9)))
static const u16 lit[UI_HEMI_GROUPS][2] = {
    { RGB(7, 3, 3), RGB(6, 1, 1) },   // G1 : rouge
    { RGB(7, 5, 6), RGB(7, 3, 5) },   // G2 : rose
    { RGB(7, 7, 4), RGB(7, 5, 1) },   // G3 : jaune orangé
    { RGB(3, 5, 7), RGB(1, 3, 7) },   // G4 : bleu
    { RGB(5, 3, 7), RGB(3, 1, 5) },   // G5 : violet
};
static const u16 off[UI_HEMI_GROUPS][2] = {   // « contre » : teintes assourdies
    { RGB(4, 2, 2), RGB(3, 1, 1) },
    { RGB(4, 3, 3), RGB(3, 2, 3) },
    { RGB(4, 4, 2), RGB(3, 3, 1) },
    { RGB(2, 3, 4), RGB(1, 2, 4) },
    { RGB(3, 2, 4), RGB(2, 1, 3) },
};

static u16 pal[16];
static u8 group_on[UI_HEMI_GROUPS];   // état affiché
static u8 group_target[UI_HEMI_GROUPS];
static u8 anim_next;                   // prochain groupe à allumer (UI_HEMI_GROUPS : fini)
static u16 anim_ticks;
static u8 focus = 0xFF;
static u16 focus_ticks;
static bool shown;

static void set_group(u8 g, bool on)
{
    group_on[g] = on;
    pal[2 + 2 * g] = on ? lit[g][0] : off[g][0];
    pal[3 + 2 * g] = on ? lit[g][1] : off[g][1];
    ui_pal_set(HEMI_PAL * 16 + 2 + 2 * g, &pal[2 + 2 * g], 2);
}

// Génération des tuiles : préparée dans ui_hemi_draw (table rang -> groupe), puis
// étalée sur plusieurs images par ui_hemi_update (CHUNK tuiles par image, envoyées
// par DMA_QUEUE depuis un tampon statique).
#define CHUNK 24
#define TBL_MAX 577
static u8 tbl[TBL_MAX];        // rang de siège -> couleur du fond (reflet = fond - 1)
static u32 chunk[CHUNK * 8];
static UiHemiChamber b_ch;
static bool b_map;                       // carte de tuiles encore à poser
static u16 b_x, b_y, b_base, b_i, b_n;   // b_i >= b_n : génération terminée

static void build_chunk(void)
{
    const u16 *first = b_ch == UI_HEMI_SENATE ? hemi_senat_first : hemi_an_first;
    const u32 *pix = b_ch == UI_HEMI_SENATE ? hemi_senat_pix : hemi_an_pix;
    const u16 *dfirst = b_ch == UI_HEMI_SENATE ? hemi_senat_dfirst : hemi_an_dfirst;
    const u8 *deco = b_ch == UI_HEMI_SENATE ? hemi_senat_deco : hemi_an_deco;
    u16 i0 = b_i, n = b_n - b_i, k, j;

    if (n > CHUNK)
        n = CHUNK;
    memsetU32(chunk, 0, n * 8);
    for (k = 0; k < n; k++) {
        u16 i = i0 + k;
        u32 *rows = chunk + k * 8;
        for (j = dfirst[i]; j < dfirst[i + 1]; j++) {
            u8 p = deco[2 * j], c = deco[2 * j + 1];
            rows[p >> 3] |= (u32)c << ((7 - (p & 7)) * 4);
        }
        for (j = first[i]; j < first[i + 1]; j++) {
            u32 v = pix[j];
            u8 c = (u8)(tbl[(u16)(v >> 7)] - ((v >> 6) & 1));
            rows[(v >> 3) & 7] |= (u32)c << ((7 - (v & 7)) * 4);
        }
    }
    VDP_loadTileData(chunk, b_base + i0, n, DMA_QUEUE);
    b_i = i0 + n;
}

// Carte de tuiles posée une fois toutes les tuiles envoyées (image suivante, après le
// DMA du dernier lot) : aucune tuile non prête n'est visible.
static void place_map(void)
{
    const u16 *txy = b_ch == UI_HEMI_SENATE ? hemi_senat_tile_xy : hemi_an_tile_xy;
    u16 i;

    for (i = 0; i < b_n; i++)
        VDP_setTileMapXY(BG_A, UI_ATTR(HEMI_PAL, b_base + i), b_x + (txy[i] & 31), b_y + (txy[i] >> 5));
    b_map = FALSE;
}

void ui_hemi_draw(u16 x, u16 y, UiHemiChamber ch, const u16 *seats)
{
    u16 ntiles = ch == UI_HEMI_SENATE ? HEMI_SENAT_TILES : HEMI_AN_TILES;
    u16 total = ch == UI_HEMI_SENATE ? HEMI_SENAT_SEATS : HEMI_AN_SEATS;
    u16 acc = 0, start = 0, base, i;

    b_n = b_i = 0;
    b_map = FALSE;
    base = ui_art_reserve(0x100 + ch, ntiles);
    if (base == 0)
        return;
    // Table rang -> groupe : un seul parcours cumulatif (plus de recherche par pixel).
    for (i = 0; i < UI_HEMI_GROUPS; i++) {
        u8 c = (u8)(3 + 2 * i);
        acc += seats[i];
        if (acc > total || i == UI_HEMI_GROUPS - 1)
            acc = total;
        for (; start < acc; start++)
            tbl[start] = c;
    }
    b_ch = ch;
    b_x = x;
    b_y = y;
    b_base = base;
    b_n = ntiles;
    b_map = TRUE;

    pal[0] = 0;
    pal[1] = 0;
    for (i = 0; i < UI_HEMI_GROUPS; i++) {
        pal[2 + 2 * i] = lit[i][0];
        pal[3 + 2 * i] = lit[i][1];
        group_on[i] = group_target[i] = 1;
    }
    pal[12] = RGB(1, 1, 3);   // sol
    pal[13] = RGB(4, 3, 1);   // tribune
    pal[14] = RGB(6, 5, 3);   // tribune, reflet
    pal[15] = RGB(4, 5, 6);   // arc extérieur
    PAL_setColors(HEMI_PAL * 16, pal, 16, DMA);
    ui_art_claim_palette(HEMI_PAL);
    anim_next = UI_HEMI_GROUPS;
    focus = 0xFF;
    shown = TRUE;
}

void ui_hemi_vote(const u8 *pour, bool animate)
{
    u8 g;

    for (g = 0; g < UI_HEMI_GROUPS; g++) {
        group_target[g] = pour[g] ? 1 : 0;
        set_group(g, animate ? FALSE : group_target[g]);
    }
    anim_next = animate ? 0 : UI_HEMI_GROUPS;
    anim_ticks = 0;
}

void ui_hemi_focus(u8 group)
{
    if (focus < UI_HEMI_GROUPS)
        set_group(focus, group_on[focus]);
    focus = group;
    focus_ticks = 0;
}

bool ui_hemi_update(void)
{
    if (b_i < b_n)
        build_chunk();
    else if (b_map)
        place_map();
    if (!shown || PAL_isDoingFade() || b_i < b_n || b_map)
        return anim_next >= UI_HEMI_GROUPS && b_i >= b_n && !b_map;
    if (anim_next < UI_HEMI_GROUPS) {
        anim_ticks += input_frame_ticks();
        if (anim_ticks >= STEP_TICKS) {
            anim_ticks -= STEP_TICKS;
            set_group(anim_next, group_target[anim_next]);
            anim_next++;
        }
    }
    if (focus < UI_HEMI_GROUPS && anim_next >= UI_HEMI_GROUPS) {
        u16 old = focus_ticks;
        focus_ticks += input_frame_ticks();
        if (focus_ticks >= 2 * BLINK_TICKS)
            focus_ticks -= 2 * BLINK_TICKS;
        if ((old < BLINK_TICKS) != (focus_ticks < BLINK_TICKS)) {
            // demi-période « éteinte » : reflet blanc sur le fond du groupe
            if (focus_ticks < BLINK_TICKS) {
                set_group(focus, group_on[focus]);
            } else {
                static u16 c = RGB(7, 7, 7);
                ui_pal_set(HEMI_PAL * 16 + 2 + 2 * focus, &c, 1);
            }
        }
    }
    return anim_next >= UI_HEMI_GROUPS;
}

void ui_hemi_majority(u16 x, u16 y, u16 w, u16 votes, u16 total, u16 majority)
{
    u16 mx;

    if (w < 2 || total == 0)
        return;
    ui_clear_rect(x, y, w, 1);
    mx = (u16)(((u32)majority * (w * 8) / total) / 8);
    if (mx >= w)
        mx = w - 1;
    ui_icon_draw(x + mx, y, UI_ICON_ARROW_DOWN);
    ui_gauge_draw(x, y + 1, w, (s16)votes, (s16)total,
                  votes >= majority ? UI_GAUGE_GOLD : UI_GAUGE_NEGATIVE);
}
