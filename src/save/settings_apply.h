// Mega Président — application des réglages de l'écran Options aux modules
// (console seulement). Seuls appelants de music_set_volume, sfx_set_mode,
// voice_set_enabled, ui_set_fade_speed, ui_set_colorblind et ui_gueule_anim_enable.
#ifndef SAVE_SETTINGS_APPLY_H
#define SAVE_SETTINGS_APPLY_H

#include "save/settings.h"

void settings_apply(SettingId id);
void settings_apply_all(void);

#endif
