#include "test.h"
#include <string.h>
#include "util/strbuf.h"

static void num(char *out, s32 v, u16 d)
{
    StrBuf sb;
    sb_init(&sb, out, 32);
    sb_num(&sb, v, d);
}

void test_strbuf(void)
{
    char b[32];
    StrBuf sb;

    printf("StrBuf\n");

    // Nombres : zéro, négatifs, décimales
    num(b, 0, 0);          CHECK(strcmp(b, "0") == 0);
    num(b, 0, 2);          CHECK(strcmp(b, "0,00") == 0);
    num(b, 5, 2);          CHECK(strcmp(b, "0,05") == 0);
    num(b, 1234, 2);       CHECK(strcmp(b, "12,34") == 0);
    num(b, 1234, 0);       CHECK(strcmp(b, "1234") == 0);
    num(b, -7, 0);         CHECK(strcmp(b, "-7") == 0);
    num(b, -7, 1);         CHECK(strcmp(b, "-0,7") == 0);
    num(b, -1234, 3);      CHECK(strcmp(b, "-1,234") == 0);
    num(b, 12, 12);        CHECK(strcmp(b, "0,000000012") == 0);  // plafonné à 9

    // Valeurs limites s32
    num(b, 2147483647, 0);  CHECK(strcmp(b, "2147483647") == 0);
    num(b, -2147483647 - 1, 0); CHECK(strcmp(b, "-2147483648") == 0);
    num(b, 2147483647, 9);  CHECK(strcmp(b, "2,147483647") == 0);
    num(b, -2147483647 - 1, 9); CHECK(strcmp(b, "-2,147483648") == 0);
    num(b, -2147483647 - 1, 1); CHECK(strcmp(b, "-214748364,8") == 0);

    // Signé
    sb_init(&sb, b, sizeof b); sb_signed(&sb, 5, 0);   CHECK(strcmp(b, "+5") == 0);
    sb_init(&sb, b, sizeof b); sb_signed(&sb, -5, 0);  CHECK(strcmp(b, "-5") == 0);
    sb_init(&sb, b, sizeof b); sb_signed(&sb, 0, 0);   CHECK(strcmp(b, "0") == 0);
    sb_init(&sb, b, sizeof b); sb_signed(&sb, 25, 1);  CHECK(strcmp(b, "+2,5") == 0);

    // Flèche
    sb_init(&sb, b, sizeof b); sb_arrow(&sb, 12, -3, 0);
    CHECK(strcmp(b, "12 \xe2\x86\x92 -3") == 0);
    CHECK_EQ(sb.len, strlen(b));
    sb_init(&sb, b, sizeof b); sb_arrow(&sb, 105, 98, 1);
    CHECK(strcmp(b, "10,5 \xe2\x86\x92 9,8") == 0);

    // Concaténation, NULL, len
    sb_init(&sb, b, sizeof b);
    sb_str(&sb, "ab"); sb_str(&sb, NULL); sb_str(&sb, "cd");
    CHECK(strcmp(b, "abcd") == 0);
    CHECK_EQ(sb.len, 4);
    CHECK(!sb.truncated);

    // Troncature ASCII : cap 5 -> 4 octets
    {
        char s[5];
        sb_init(&sb, s, sizeof s);
        sb_str(&sb, "abcdefgh");
        CHECK(strcmp(s, "abcd") == 0);
        CHECK(sb.truncated);
        sb_str(&sb, "x");
        CHECK(strcmp(s, "abcd") == 0);
        CHECK_EQ(sb.len, 4);
    }
    // Remplissage exact : pas de troncature
    {
        char s[5];
        sb_init(&sb, s, sizeof s);
        sb_str(&sb, "abcd");
        CHECK(!sb.truncated);
        CHECK(strcmp(s, "abcd") == 0);
    }
    // Troncature UTF-8 : « é » = 2 octets, « → » = 3 octets
    {
        char s[5];
        sb_init(&sb, s, sizeof s);   // 4 octets utiles
        sb_str(&sb, "abc\xc3\xa9\xc3\xa9");  // abc + é + é
        CHECK(strcmp(s, "abc") == 0);
        CHECK(sb.truncated);
        CHECK_EQ(sb.len, 3);
    }
    {
        char s[6];
        sb_init(&sb, s, sizeof s);   // 5 octets utiles
        sb_str(&sb, "a\xc3\xa9\xc3\xa9\xc3\xa9");  // a é é é : coupe après 2 é
        CHECK(strcmp(s, "a\xc3\xa9\xc3\xa9") == 0);
        CHECK(sb.truncated);
    }
    {
        char s[5];
        sb_init(&sb, s, sizeof s);
        sb_str(&sb, "ab\xe2\x86\x92");  // ab + → (3) : 5 octets, rien ne tient
        CHECK(strcmp(s, "ab") == 0);
        CHECK(sb.truncated);
    }
    {
        char s[2];
        sb_init(&sb, s, sizeof s);
        sb_str(&sb, "\xc3\xa9");
        CHECK(strcmp(s, "") == 0);
        CHECK(sb.truncated);
    }
    // Capacité 0 : ne plante pas
    {
        StrBuf z;
        sb_init(&z, b, 0);
        sb_str(&z, "abc");
        sb_num(&z, 42, 0);
        CHECK_EQ(z.len, 0);
    }
    // Un nombre qui ne tient pas est coupé proprement
    {
        char s[4];
        sb_init(&sb, s, sizeof s);
        sb_num(&sb, 123456, 0);
        CHECK(strcmp(s, "123") == 0);
        CHECK(sb.truncated);
    }
    // SB_LINE : tampon de ligne, jamais de dépassement
    {
        SB_LINE(l);
        int i;
        for (i = 0; i < 40; i++)
            sb_str(&l, "\xc3\xa9\xc3\xa9");   // 160 octets demandés
        CHECK(l.truncated);
        CHECK(l.len <= SB_LINE_SIZE - 1);
        CHECK_EQ(l.len % 2, 0);                // jamais au milieu d'un é
        CHECK_EQ(strlen(l.buf), l.len);
    }
}
