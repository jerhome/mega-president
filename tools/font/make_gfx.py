#!/usr/bin/env python3
"""Ancien point d'entrée (make gfx) : les graphismes d'interface V3 sont produits
par tools/ui/make_ui.py (police : tools/font/glyphs.txt). Voir docs/CHARTE_V3.md."""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.argv[0] = os.path.join(HERE, "..", "ui", "make_ui.py")
runpy.run_path(sys.argv[0], run_name="__main__")
