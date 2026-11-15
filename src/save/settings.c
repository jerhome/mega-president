// Mega Président — réglages de l'écran Options (voir settings.h).
#include "save/settings.h"

static const u8 s_count[SET_COUNT]   = { 5, 3, 2, 2, 3, 2 };
static const u8 s_default[SET_COUNT] = { 4, 0, 1, 1, 0, 0 };

static u8 s_val[SET_COUNT] = { 4, 0, 1, 1, 0, 0 };
static u8 s_dirty;

u8 settings_get(SettingId id)
{
    return id < SET_COUNT ? s_val[id] : 0;
}

u8 settings_count(SettingId id)
{
    return id < SET_COUNT ? s_count[id] : 1;
}

u8 settings_default(SettingId id)
{
    return id < SET_COUNT ? s_default[id] : 0;
}

void settings_set(SettingId id, u8 v)
{
    if (id >= SET_COUNT)
        return;
    if (v >= s_count[id])
        v = (u8)(s_count[id] - 1);
    if (v != s_val[id]) {
        s_val[id] = v;
        s_dirty = 1;
    }
}

void settings_reset(void)
{
    u8 i;
    for (i = 0; i < SET_COUNT; i++)
        settings_set((SettingId)i, s_default[i]);
}

u8 settings_all_default(void)
{
    u8 i;
    for (i = 0; i < SET_COUNT; i++)
        if (s_val[i] != s_default[i])
            return 0;
    return 1;
}

u8 settings_dirty(void)
{
    return s_dirty;
}

void settings_clear_dirty(void)
{
    s_dirty = 0;
}

static u16 checksum(const u8 *buf)
{
    u16 sum = 0, i;
    for (i = 0; i < SETTINGS_BLOB_SIZE - 2; i++)
        sum = (u16)(sum + buf[i]);
    return (u16)(sum ^ SETTINGS_CHECK_XOR);
}

u16 settings_serialize(u8 *buf)
{
    u16 i, c;

    for (i = 0; i < SETTINGS_BLOB_SIZE; i++)
        buf[i] = 0;
    buf[0] = 'O';
    buf[1] = 'P';
    buf[2] = SETTINGS_VERSION;
    buf[3] = SET_COUNT;
    for (i = 0; i < SET_COUNT; i++)
        buf[4 + i] = s_val[i];
    c = checksum(buf);
    buf[14] = (u8)(c >> 8);
    buf[15] = (u8)c;
    return SETTINGS_BLOB_SIZE;
}

static void load_defaults(void)
{
    u8 i;
    for (i = 0; i < SET_COUNT; i++)
        s_val[i] = s_default[i];
}

u8 settings_deserialize(const u8 *buf, u16 len)
{
    u8 i, n, r = 0;

    if (!buf || len < SETTINGS_BLOB_SIZE || buf[0] != 'O' || buf[1] != 'P'
        || buf[2] == 0 || checksum(buf) != (u16)((buf[14] << 8) | buf[15])) {
        load_defaults();
        return 2;
    }
    // Version plus récente : on lit les valeurs connues, bornées.
    n = buf[3];
    if (n > SETTINGS_MAX_VALUES)
        n = SETTINGS_MAX_VALUES;
    for (i = 0; i < SET_COUNT; i++) {
        if (i >= n) {
            s_val[i] = s_default[i];     // réglage ajouté depuis : défaut
        } else if (buf[4 + i] >= s_count[i]) {
            s_val[i] = s_default[i];     // valeur folle : elle seule reprend son défaut
            r = 1;
        } else {
            s_val[i] = buf[4 + i];
        }
    }
    return r;
}
