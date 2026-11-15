// Mega Président — graphismes de l'interface V3 (noms stables, voir src/ui/README.md
// et docs/CHARTE_V3.md). Images générées par tools/ui/make_ui.py.

// Palettes : l'interface n'utilise que PAL0 (pal_ui_text) et PAL1 (pal_ui_highlight).
// pal_ui_dim et pal_ui_alert sont des copies neutres de PAL0 : ui_art_close() les
// recharge dans PAL2/PAL3, lignes réservées aux illustrations en V3.
PALETTE pal_ui_text      "gfx/pal_ui_text.png"
PALETTE pal_ui_highlight "gfx/pal_ui_highlight.png"
PALETTE pal_ui_dim       "gfx/pal_ui_dim.png"
PALETTE pal_ui_alert     "gfx/pal_ui_alert.png"

// Jeux de tuiles : sans compression ni optimisation (l'ordre des tuiles compte).
TILESET ts_ui_font   "gfx/font.png"      NONE NONE
TILESET ts_ui_frame  "gfx/ui_frame.png"  NONE NONE
TILESET ts_ui_gauge  "gfx/ui_gauge.png"  NONE NONE
TILESET ts_ui_icons  "gfx/ui_icons.png"  NONE NONE
// Images du curseur animé : jamais chargées en bloc, copiées une à une dans la
// tuile UI_ICON_CURSOR (ui_frame_end).
TILESET ts_ui_cursor "gfx/ui_cursor.png" NONE NONE
