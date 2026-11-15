#include <string.h>
#include "test.h"
#include "save/settings.h"

static void all_defaults(void)
{
    u8 i;
    for (i = 0; i < SET_COUNT; i++)
        CHECK_EQ(settings_get((SettingId)i), settings_default((SettingId)i));
}

static void fix_sum(u8 *buf)
{
    u16 s = 0, i;
    for (i = 0; i < 14; i++)
        s = (u16)(s + buf[i]);
    s ^= SETTINGS_CHECK_XOR;
    buf[14] = (u8)(s >> 8);
    buf[15] = (u8)s;
}

void test_settings(void)
{
    u8 buf[SETTINGS_BLOB_SIZE], b2[SETTINGS_BLOB_SIZE];

    printf("  réglages (écran Options)\n");
    // Défauts de la spec
    settings_reset();
    CHECK_EQ(settings_get(SET_MUSIC), 4);
    CHECK_EQ(settings_get(SET_SFX), 0);
    CHECK_EQ(settings_get(SET_VOICE), 1);
    CHECK_EQ(settings_get(SET_FACES), 1);
    CHECK_EQ(settings_get(SET_FADES), 0);
    CHECK_EQ(settings_get(SET_COLORS), 0);
    CHECK(settings_all_default());
    CHECK_EQ(settings_count(SET_MUSIC), 5);
    CHECK_EQ(settings_count(SET_SFX), 3);

    // Bornes et drapeau « modifié »
    settings_clear_dirty();
    settings_set(SET_MUSIC, 4);
    CHECK(!settings_dirty());
    settings_set(SET_MUSIC, 9);
    CHECK_EQ(settings_get(SET_MUSIC), 4);
    settings_set(SET_SFX, 2);
    CHECK(settings_dirty());
    CHECK(!settings_all_default());

    // SRAM vierge (0xFF), à zéro, NULL : tous les défauts
    memset(buf, 0xFF, sizeof(buf));
    CHECK_EQ(settings_deserialize(buf, sizeof(buf)), 2);
    all_defaults();
    settings_set(SET_COLORS, 1);
    memset(buf, 0x00, sizeof(buf));
    CHECK_EQ(settings_deserialize(buf, sizeof(buf)), 2);
    all_defaults();
    settings_set(SET_COLORS, 1);
    CHECK_EQ(settings_deserialize(NULL, 0), 2);
    all_defaults();

    // Aller-retour
    settings_set(SET_MUSIC, 1);
    settings_set(SET_SFX, 1);
    settings_set(SET_VOICE, 0);
    settings_set(SET_FACES, 0);
    settings_set(SET_FADES, 2);
    settings_set(SET_COLORS, 1);
    CHECK_EQ(settings_serialize(buf), SETTINGS_BLOB_SIZE);
    CHECK_EQ(buf[0], 'O');
    CHECK_EQ(buf[1], 'P');
    CHECK_EQ(buf[2], SETTINGS_VERSION);
    CHECK_EQ(buf[3], SET_COUNT);
    CHECK_EQ(buf[12], 0);
    settings_reset();
    CHECK_EQ(settings_deserialize(buf, sizeof(buf)), 0);
    CHECK_EQ(settings_get(SET_MUSIC), 1);
    CHECK_EQ(settings_get(SET_SFX), 1);
    CHECK_EQ(settings_get(SET_VOICE), 0);
    CHECK_EQ(settings_get(SET_FACES), 0);
    CHECK_EQ(settings_get(SET_FADES), 2);
    CHECK_EQ(settings_get(SET_COLORS), 1);
    settings_serialize(b2);
    CHECK(memcmp(buf, b2, sizeof(buf)) == 0);

    // Somme fausse : défauts
    memcpy(b2, buf, sizeof(buf));
    b2[15] ^= 1;
    CHECK_EQ(settings_deserialize(b2, sizeof(b2)), 2);
    all_defaults();

    // Valeur hors bornes : elle seule reprend son défaut
    memcpy(b2, buf, sizeof(buf));
    b2[4 + SET_FADES] = 7;
    fix_sum(b2);
    CHECK_EQ(settings_deserialize(b2, sizeof(b2)), 1);
    CHECK_EQ(settings_get(SET_FADES), 0);
    CHECK_EQ(settings_get(SET_MUSIC), 1);
    CHECK_EQ(settings_get(SET_COLORS), 1);

    // Moins de valeurs stockées que SET_COUNT : réglages manquants par défaut
    memcpy(b2, buf, sizeof(buf));
    b2[3] = 3;
    fix_sum(b2);
    CHECK_EQ(settings_deserialize(b2, sizeof(b2)), 0);
    CHECK_EQ(settings_get(SET_MUSIC), 1);
    CHECK_EQ(settings_get(SET_VOICE), 0);
    CHECK_EQ(settings_get(SET_FACES), 1);
    CHECK_EQ(settings_get(SET_FADES), 0);
    CHECK_EQ(settings_get(SET_COLORS), 0);

    // Version plus récente : valeurs connues lues
    memcpy(b2, buf, sizeof(buf));
    b2[2] = 9;
    b2[3] = 8;
    fix_sum(b2);
    CHECK_EQ(settings_deserialize(b2, sizeof(b2)), 0);
    CHECK_EQ(settings_get(SET_COLORS), 1);

    settings_reset();
    settings_clear_dirty();
}
