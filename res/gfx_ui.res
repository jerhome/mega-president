// Mega Président — graphismes de l'interface (noms stables, voir src/ui/README.md).
// Les images sont générées par tools/font/make_gfx.py ; en V2, un pixel artiste
// les remplace en gardant noms, dimensions et convention d'indices de couleur.

// Palettes nommées : une par ligne de palette du VDP (16 couleurs).
PALETTE pal_ui_text      "gfx/pal_ui_text.png"
PALETTE pal_ui_highlight "gfx/pal_ui_highlight.png"
PALETTE pal_ui_dim       "gfx/pal_ui_dim.png"
PALETTE pal_ui_alert     "gfx/pal_ui_alert.png"

// Jeux de tuiles : sans compression ni optimisation (l'ordre des tuiles compte).
TILESET ts_ui_font  "gfx/font.png"     NONE NONE
TILESET ts_ui_frame "gfx/ui_frame.png" NONE NONE
TILESET ts_ui_gauge "gfx/ui_gauge.png" NONE NONE
TILESET ts_ui_icons "gfx/ui_icons.png" NONE NONE
