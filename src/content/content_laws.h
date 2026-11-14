#ifndef CONTENT_LAWS_H
#define CONTENT_LAWS_H

// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "sim/laws.h"

// Textes UTF-8 indexés par LOI_*.
extern const char *const law_name[LOI_COUNT];        // 24 car. max
extern const char *const law_desc[LOI_COUNT];        // 3 lignes max ('\n')
extern const char *const law_journal_ok[LOI_COUNT];  // si adoptée
extern const char *const law_journal_ko[LOI_COUNT];  // si rejetée

#endif
