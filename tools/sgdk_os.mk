# Surcharge de makefile.gen de SGDK : compile src/screens/*.c et src/game/game.c en -Os
# (le reste reste en -O3). Les options par fichier viennent après FLAGS, donc gagnent.
include $(SGDK_PATH)/makefile.gen
$(OUT)/src/screens/%.o: CFLAGS += -Os
$(OUT)/src/game/game.o: CFLAGS += -Os
