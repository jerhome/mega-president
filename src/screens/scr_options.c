// Mega Président — écran Options (tâche 3.19, docs/SPEC_OPTIONS.md).
//
// Six réglages à effet immédiat (son, affichage) et trois actions sur les données,
// présentés par Firmin Tisserand, huissier de l'Élysée. Les réglages sont écrits en
// SRAM en sortie d'écran s'ils ont changé (jamais à chaque appui).
#include <genesis.h>
#include "screens.h"
#include "screens/scr_options.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "ui/ui_art.h"
#include "audio/music.h"
#include "audio/sfx.h"
#include "audio/voice.h"
#include "save/settings.h"
#include "save/settings_apply.h"
#include "save/sram.h"
#include "save/sram_game.h"
#include "save/scores.h"
#include "game/game.h"

// --- Lignes -----------------------------------------------------------------------

enum {
    R_MUSIC = 0, R_SFX, R_VOICE,              // = SET_MUSIC .. SET_VOICE
    R_FACES, R_FADES, R_COLORS,               // = SET_FACES .. SET_COLORS
    R_ERASE_SCORES, R_ERASE_GAME, R_DEFAULTS,
    R_COUNT
};
#define R_IS_SETTING(r) ((r) < R_ERASE_SCORES)

static const u8 row_y[R_COUNT] = { 5, 6, 7, 10, 11, 12, 15, 16, 17 };
static const char *const row_label[R_COUNT] = {
    "Musique", "Bruitages", "Voix", "Visages", "Transitions", "Couleurs",
    "Effacer les records", "Effacer la partie", "Réglages d'origine"
};

// Valeurs affichées (SET_MUSIC est dessiné à part : jauge et pourcentage).
static const char *const val_sfx[3]    = { "Tous", "Essentiels", "Aucun" };
static const char *const val_voice[2]  = { "Non", "Oui" };
static const char *const val_faces[2]  = { "Figés", "Animés" };
static const char *const val_fades[3]  = { "Douces", "Rapides", "Aucune" };
static const char *const val_colors[2] = { "Standard", "Daltonisme" };
static const char *const *const val_text[SET_COUNT] = {
    NULL, val_sfx, val_voice, val_faces, val_fades, val_colors
};

// Aides (ivoire) et répliques de l'huissier (gris bleuté), 38 colonnes au plus.
static const char *const row_help[R_COUNT] = {
    "Volume de la musique, effet immédiat.",
    "Essentiels : sans les clics de menu, avec les sons qui informent.",
    "« For sure ! » quand la console s'allume.",
    "Clignements et regards des portraits.",
    "Durée des fondus entre les écrans.",
    "Carte et jauges lisibles par tous.",
    "Vide les deux classements (1er mandat et 10 ans).",
    "Supprime le mandat sauvegardé.",
    "Remet le son et l'affichage comme au premier jour."
};
static const char *const row_quip[R_COUNT] = {
    "« La Garde républicaine vous salue. »",
    "« Le parquet craque. C'est d'époque. »",
    "« On ne coupe pas la parole. Si ? »",
    "« Ici, les portraits vous épient. »",
    "« Tout est dans la mise en scène. »",
    "« Chacun sa couleur. Même le centre. »",
    "« L'Histoire retiendra. Ou pas. »",
    "« Comme une promesse électorale. »",
    "« On revient à la Constitution. »"
};
// Lignes de données inactives : raison (ivoire) et réplique (gris bleuté).
static const char *const why_text[4] = {
    "Aucun record pour l'instant.",
    "Impossible pendant une partie.",
    "Aucune partie sauvegardée.",
    "Les réglages sont déjà ceux d'origine."
};
static const char *const why_quip[4] = {
    "« Page blanche. L'Histoire attend. »",
    "« Pas pendant le mandat, voyons. »",
    "« Rien à effacer. Tout est en ordre. »",
    "« Tout est d'origine. Même moi. »"
};
static const char quip_mute[] = "« Le silence. Comme après un 49.3. »";

// --- Mise en page (lot B : médaillon de l'huissier à droite) ------------------------

#define LIST_W    27   // barre de sélection x1-27
#define VAL_X     16   // zone de valeur x16-25
#define VAL_W     10
#define ARROW_L   15
#define ARROW_R   26
#define MED_X     29
#define MED_Y     4
#define HELP_Y    20
#define CONTENT_MS 1500   // huissier content après un changement de valeur

// --- Confirmations ------------------------------------------------------------------

typedef enum { CF_NONE, CF_SCORES1, CF_SCORES2, CF_GAME1, CF_GAME2, CF_DEFAULTS, CF_DONE } Confirm;

static const char *const ch_erase[2]  = { "Effacer", "Annuler" };
static const char *const ch_erase2[2] = { "Oui, effacer", "Annuler" };
static const char *const ch_reset[2]  = { "Rétablir", "Annuler" };

// --- État ---------------------------------------------------------------------------

static ScreenId return_screen = SCREEN_TITLE;
static bool returning;      // options_take_return() : l'appelant revient de l'écran Options
static u8 cur;              // ligne sélectionnée
static u8 confirm;          // Confirm
static bool dirty_all, dirty_rows, dirty_help;
static u8 expr;             // expression de l'huissier
static u32 expr_until;      // fin de l'expression « content » (screen_time_ms)
static bool has_scores, has_game;

void options_open(ScreenId return_to)
{
    return_screen = return_to;
    screen_change(SCREEN_OPTIONS);
}

bool options_take_return(void)
{
    bool r = returning;
    returning = FALSE;
    return r;
}

static bool scores_exist(void)
{
    ScoreBoard b;   // ≈ 200 octets de pile, le temps de la lecture
    u8 t, i;

    sram_scores_load(&b);
    for (t = 0; t < SCORES_TABLES; t++)
        for (i = 0; i < SCORES_COUNT; i++)
            if (b.t[t].e[i].score)
                return TRUE;
    return FALSE;
}

static bool row_enabled(u8 r)
{
    switch (r) {
    case R_ERASE_SCORES: return has_scores;
    case R_ERASE_GAME:   return has_game;
    case R_DEFAULTS:     return !settings_all_default();
    default:             return TRUE;
    }
}

// Index de la raison d'inactivité d'une ligne de données (la cause « depuis le jeu » prime).
static u8 why_index(u8 r)
{
    if (r == R_ERASE_SCORES) return 0;
    if (r == R_ERASE_GAME)   return return_screen != SCREEN_TITLE ? 1 : 2;
    return 3;
}

// Valeur suivante / précédente d'un réglage (le cran 25 % peut être sauté).
static s16 step_value(u8 id, u8 v, s16 dir)
{
    s16 n = (s16)v + dir;
    if (id == SET_MUSIC && n == 1 && !MUSIC_HAS_25)
        n += dir;
    if (n < 0 || n >= settings_count((SettingId)id))
        return -1;
    return n;
}

static void set_expr(u8 e)
{
    if (e == expr)
        return;
    expr = e;
    ui_gueule_expr(GUEULE_HUISSIER, (Expr)e, MED_X, MED_Y, PAL2);
}

static bool greet;   // le huissier lance un mot à l'entrée de l'écran (charabia 4.18)

static void options_init(void)
{
    greet = TRUE;
    cur = R_MUSIC;
    confirm = CF_NONE;
    expr = EXPR_NEUTRE;
    expr_until = 0;
    has_scores = scores_exist();
    has_game = return_screen == SCREEN_TITLE && sram_game_exists();
    dirty_all = TRUE;
}

// --- Dessin -------------------------------------------------------------------------

static void draw_value(u8 r, u16 y)
{
    u8 v = settings_get((SettingId)r);

    if (r == R_MUSIC) {
        if (v == 0) {
            ui_text_center(VAL_X, y, VAL_W, "Coupée", UI_STYLE_TEXT);
        } else {
            static const char *const pct[5] = { "", "25 %", "50 %", "75 %", "100 %" };
            ui_gauge_draw(VAL_X, y, 4, (s16)(v * 25), 100, UI_GAUGE_GOLD);
            ui_text_draw(VAL_X + 5, y, pct[v], UI_STYLE_TEXT);
        }
    } else {
        ui_text_center(VAL_X, y, VAL_W, val_text[r][v], UI_STYLE_TEXT);
    }
    if (r == cur) {
        if (step_value(r, v, -1) >= 0)
            ui_icon_draw(ARROW_L, y, UI_ICON_ARROW_LEFT);
        if (step_value(r, v, 1) >= 0)
            ui_icon_draw(ARROW_R, y, UI_ICON_ARROW_RIGHT);
    }
}

static void draw_row(u8 r)
{
    u16 y = row_y[r];
    bool sel = (r == cur);
    UiStyle st;

    if (sel)
        ui_select_bar(1, y, LIST_W);
    else
        ui_select_bar_clear(1, y, LIST_W);
    ui_clear_rect(1, y, LIST_W, 1);
    if (sel)
        ui_icon_draw(1, y, UI_ICON_CURSOR);
    if (R_IS_SETTING(r)) {
        st = sel ? UI_STYLE_HIGHLIGHT : UI_STYLE_DIM;
        ui_text_draw_n(3, y, row_label[r], 11, 0, st);
        draw_value(r, y);
    } else {
        st = !row_enabled(r) ? UI_STYLE_DIM : (sel ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
        ui_text_draw_n(3, y, row_label[r], 24, 0, st);
    }
}

static void draw_group(u16 y, UiIcon icon, const char *title)
{
    u16 x;
    ui_icon_draw(1, y, icon);
    x = 3 + ui_text_draw(3, y, title, UI_STYLE_HIGHLIGHT) + 1;
    if (x < LIST_W)
        ui_separator(x, y, LIST_W + 1 - x);
}

static void draw_help(void)
{
    const char *quip;

    ui_clear_rect(1, HELP_Y + 1, UI_TEXT_COLS, 5);
    if (!row_enabled(cur)) {
        u8 w = why_index(cur);
        ui_text_draw(1, HELP_Y + 1, why_text[w], UI_STYLE_TEXT);
        ui_text_center(1, HELP_Y + 3, UI_TEXT_COLS, why_quip[w], UI_STYLE_DIM);
        ui_footer(1, HELP_Y + 5, UI_TEXT_COLS, "B : retour");
        return;
    }
    ui_text_wrap(1, HELP_Y + 1, UI_TEXT_COLS, 2, row_help[cur], UI_STYLE_TEXT);
    if (cur == R_COLORS) {
        // Les deux flèches d'exemple changent de couleur avec le réglage.
        ui_icon_draw(9, HELP_Y + 2, UI_ICON_UP_GOOD);
        ui_text_draw(11, HELP_Y + 2, "favorable", UI_STYLE_DIM);
        ui_icon_draw(22, HELP_Y + 2, UI_ICON_DOWN_BAD);
        ui_text_draw(24, HELP_Y + 2, "défavorable", UI_STYLE_DIM);
    }
    quip = (cur == R_MUSIC && settings_get(SET_MUSIC) == 0) ? quip_mute : row_quip[cur];
    ui_text_center(1, HELP_Y + 3, UI_TEXT_COLS, quip, UI_STYLE_DIM);
    if (R_IS_SETTING(cur))
        ui_footer(1, HELP_Y + 5, UI_TEXT_COLS, "Gauche/Droite : régler   B : retour");
    else if (cur == R_DEFAULTS)
        ui_footer(1, HELP_Y + 5, UI_TEXT_COLS, "A : rétablir   B : retour");
    else
        ui_footer(1, HELP_Y + 5, UI_TEXT_COLS, "A : effacer   B : retour");
}

static void draw_all(void)
{
    u8 r;

    ui_banner(0, 0, UI_COLS, "OPTIONS", FALSE);
    ui_window(0, 3, UI_COLS, 16);
    draw_group(4, UI_ICON_NOTE, "SON");
    draw_group(9, UI_ICON_EYE, "AFFICHAGE");
    draw_group(14, UI_ICON_CART, "DONNÉES");
    for (r = 0; r < R_COUNT; r++)
        draw_row(r);
    ui_medallion(MED_X, MED_Y, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(GUEULE_HUISSIER, (Expr)expr, MED_X, MED_Y, PAL2);
    ui_text_center(MED_X, 16, UI_PORTRAIT_W, "Tisserand", UI_STYLE_TEXT);
    ui_text_center(MED_X, 17, UI_PORTRAIT_W, "huissier", UI_STYLE_DIM);
    ui_window(0, HELP_Y, UI_COLS, 7);
    draw_help();
}

static void options_draw(void)
{
    if (dirty_all) {
        draw_all();
        if (greet) {
            greet = FALSE;
            ui_gueule_say(900);
        }
        dirty_all = dirty_rows = dirty_help = FALSE;
        return;
    }
    if (dirty_rows) {
        u8 r;
        for (r = 0; r < R_COUNT; r++)
            draw_row(r);
        dirty_rows = FALSE;
    }
    if (dirty_help) {
        draw_help();
        dirty_help = FALSE;
    }
}

// --- Logique ------------------------------------------------------------------------

// Ligne suivante dans la direction dir (pas de bouclage) ; from si on est au bout.
static u8 next_row(u8 from, s16 dir)
{
    s16 r = (s16)from + dir;
    return (r >= 0 && r < R_COUNT) ? (u8)r : from;
}

static void open_confirm(u8 kind)
{
    confirm = kind;
    switch (kind) {
    case CF_SCORES1:
        ui_msgbox_open("Effacer les meilleurs scores ?", "Les deux classements seront vidés.", ch_erase, 2);
        break;
    case CF_SCORES2:
        ui_msgbox_open("Vraiment ?", "C'est définitif. Même le Conseil constitutionnel n'y pourra rien.",
                       ch_erase2, 2);
        break;
    case CF_GAME1:
        ui_msgbox_open("Effacer la partie sauvegardée ?",
                       "Le mandat en cours sera perdu. Les meilleurs scores ne changent pas.", ch_erase, 2);
        break;
    case CF_GAME2:
        ui_msgbox_open("Vraiment ?", "Aucun recours possible. Pas même un référendum.", ch_erase2, 2);
        break;
    case CF_DEFAULTS:
        ui_msgbox_open("Rétablir les réglages d'origine ?",
                       "Musique, bruitages, voix et affichage reprennent leurs valeurs par défaut.",
                       ch_reset, 2);
        break;
    default:   // CF_DONE
        ui_msgbox_open(NULL, "C'est fait. Je n'ai rien vu, rien entendu.", NULL, 0);
        return;
    }
    ui_msgbox_set_sel(1);   // sélection initiale sur « Annuler »
    set_expr(kind == CF_DEFAULTS ? EXPR_NEUTRE : EXPR_GENE);
}

static void close_confirm(void)
{
    ui_msgbox_close();
    confirm = CF_NONE;
    set_expr(EXPR_NEUTRE);
    dirty_all = TRUE;   // la boîte a recouvert l'aide (et parfois le bas de la liste)
}

static void confirm_update(void)
{
    s16 c;

    if (input_pressed() & INPUT_B) {   // B annule à chaque étape
        sfx_play(SFX_CANCEL);
        close_confirm();
        return;
    }
    c = ui_msgbox_update();
    if (c < 0)
        return;
    switch (confirm) {
    case CF_SCORES1:
        if (c == 0) open_confirm(CF_SCORES2); else close_confirm();
        break;
    case CF_GAME1:
        if (c == 0) open_confirm(CF_GAME2); else close_confirm();
        break;
    case CF_SCORES2:
        if (c == 0) {
            ScoreBoard b;
            scores_reset(&b);
            sram_scores_save(&b);
            has_scores = FALSE;
            open_confirm(CF_DONE);
        } else {
            close_confirm();
        }
        break;
    case CF_GAME2:
        if (c == 0) {
            sram_game_erase();
            has_game = FALSE;
            open_confirm(CF_DONE);
        } else {
            close_confirm();
        }
        break;
    case CF_DEFAULTS:
        if (c == 0) {
            settings_reset();
            settings_apply_all();
            sram_settings_save();
        }
        close_confirm();
        break;
    default:
        close_confirm();
        break;
    }
}

// Applique la nouvelle valeur v du réglage de la ligne cur, avec son retour sonore.
static void change_value(u8 v)
{
    u8 id = cur;

    settings_set((SettingId)id, v);
    settings_apply((SettingId)id);
    switch (id) {
    case SET_MUSIC:  break;                          // la musique est l'exemple
    case SET_SFX:    sfx_play(SFX_OK); break;        // après l'application : rien en « Aucun »
    case SET_VOICE:  if (v) ui_gueule_say(900); else sfx_play(SFX_MOVE); break;   // l'huissier répond
    case SET_FADES:  sfx_play(SFX_MOVE); ui_soft_replay(); break;
    default:         sfx_play(SFX_MOVE); break;
    }
    set_expr(EXPR_CONTENT);
    expr_until = screen_time_ms() + CONTENT_MS;
    dirty_rows = TRUE;     // valeur, flèches, et « Réglages d'origine » grisée ou non
    dirty_help = TRUE;     // réplique de la musique coupée
}

static void options_update(void)
{
    u16 rep = input_repeat();
    u16 prs = input_pressed();

    if (expr == EXPR_CONTENT && confirm == CF_NONE && screen_time_ms() >= expr_until)
        set_expr(EXPR_NEUTRE);

    if (confirm != CF_NONE) {
        confirm_update();
        return;
    }
    if (prs & (INPUT_B | INPUT_START)) {
        if (settings_dirty())
            sram_settings_save();
        sfx_play(SFX_CANCEL);
        returning = TRUE;
        screen_change(return_screen);
        return;
    }
    if (rep & (INPUT_UP | INPUT_DOWN)) {
        u8 n = next_row(cur, (rep & INPUT_UP) ? -1 : 1);
        if (n != cur) {
            cur = n;
            sfx_play(SFX_MOVE);
            dirty_rows = dirty_help = TRUE;
        }
        return;
    }
    if (R_IS_SETTING(cur)) {
        // Répétition de la manette sur la ligne Musique seulement (on balaie les crans).
        u16 lr = (cur == R_MUSIC) ? rep : prs;
        u8 v = settings_get((SettingId)cur);
        s16 n = -1;
        if (lr & INPUT_LEFT)
            n = step_value(cur, v, -1);
        else if (lr & INPUT_RIGHT)
            n = step_value(cur, v, 1);
        else if (prs & INPUT_OK) {   // A seul : valeur suivante, en boucle
            n = step_value(cur, v, 1);
            if (n < 0)
                n = 0;
        }
        if (n >= 0 && n != v)
            change_value((u8)n);
        return;
    }
    if (prs & INPUT_OK) {
        if (!row_enabled(cur)) {
            sfx_play(SFX_ERROR);
            return;
        }
        sfx_play(SFX_OK);
        open_confirm(cur == R_ERASE_SCORES ? CF_SCORES1 : (cur == R_ERASE_GAME ? CF_GAME1 : CF_DEFAULTS));
    }
}

static void options_exit(void)
{
    ui_art_close();
}

const Screen scr_options = { options_init, options_update, options_draw, options_exit };
