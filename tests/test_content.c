// Vérifications simples des tables de texte générées (src/content/).
#include <string.h>
#include "test.h"
#include "content/content_events.h"
#include "content/content_laws.h"
#include "content/content_world.h"
#if defined(__has_include)
#if __has_include("content/content_texts.h")
#include "content/content_texts.h"
#define HAVE_TEXTS 1
#endif
#endif

// Colonnes d'une ligne UTF-8 (jusqu'à '\n' ou fin) ; *next = début de la suivante ou NULL.
static int cols(const char *s, const char **next)
{
    int n = 0;
    for (; *s && *s != '\n'; s++)
        if (((unsigned char)*s & 0xC0) != 0x80)
            n++;
    *next = (*s == '\n') ? s + 1 : NULL;
    return n;
}

// Texte valide : non NULL, non vide, au plus max_lines lignes de max_cols colonnes.
static int text_ok(const char *t, int max_lines, int max_cols)
{
    int lines = 0, c;
    const char *next;
    if (!t || !*t)
        return 0;
    do {
        c = cols(t, &next);
        if (c == 0 || c > max_cols)
            return 0;
        lines++;
        t = next;
    } while (t);
    return lines <= max_lines;
}

void test_content(void)
{
    int i, j;
    printf("  content : tables générées\n");
    for (i = 0; i < EVT_COUNT; i++) {
        CHECK(text_ok(evt_title[i], 1, 30));
        CHECK(text_ok(evt_text[i], 6, 38));
        for (j = 0; j < EVENT_MAX_CHOICES; j++) {
            if (!evt_choice[i][j])
                continue;
            CHECK(text_ok(evt_choice[i][j], 1, 34));
            CHECK(text_ok(evt_journal[i][j], 2, 38));
        }
    }
    for (i = 0; i < LOI_COUNT; i++) {
        CHECK(text_ok(law_name[i], 1, 24));
        CHECK(text_ok(law_desc[i], 3, 38));
        CHECK(text_ok(law_journal_ok[i], 2, 38));
        CHECK(text_ok(law_journal_ko[i], 2, 38));
    }
    for (i = 0; i < GRP_COUNT; i++)
        CHECK(grp_name[i] && *grp_name[i]);
#ifdef HAVE_TEXTS
    for (i = 0; i < TXT_COUNT; i++)
        CHECK(text_ok(txt[i], 6, 38));   // TXT_COHAB_AIDE : 6 lignes
    CHECK(strlen(txt[TXT_PM_TECHNOCRATE_NOM]) > 0);
    CHECK(text_ok(txt[TXT_BUD_RELANCE], 1, 20));
    CHECK(text_ok(txt[TXT_BUD_RELANCE_DESC], 2, 38));
    CHECK(text_ok(txt[TXT_THEME_ECOLOGIE], 1, 24));
#endif
}
