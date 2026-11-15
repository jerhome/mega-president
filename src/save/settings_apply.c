// Mega Président — application des réglages (voir settings_apply.h).
#include <genesis.h>
#include "save/settings_apply.h"
#include "audio/music.h"
#include "audio/sfx.h"
#include "audio/voice.h"
#include "ui/ui.h"
#include "ui/ui_art.h"

void settings_apply(SettingId id)
{
    u8 v = settings_get(id);

    switch (id) {
    case SET_MUSIC:  music_set_volume(v); break;
    case SET_SFX:    sfx_set_mode(v); break;
    case SET_VOICE:  voice_set_enabled(v != 0); break;
    case SET_FACES:  ui_gueule_anim_enable(v != 0); break;
    case SET_FADES:  ui_set_fade_speed(v); break;
    case SET_COLORS: ui_set_colorblind(v != 0); break;
    default: break;
    }
}

void settings_apply_all(void)
{
    u8 i;
    for (i = 0; i < SET_COUNT; i++)
        settings_apply((SettingId)i);
}
