// Mega Président — écran d'aide en pages (tâche 1.17).

#include <genesis.h>
#include "screens.h"
#include "scr_help.h"
#include "ui/ui.h"
#include "ui/input.h"

typedef struct {
    const char *title;
    const char *text;
} HelpPage;

static const HelpPage pages[] = {
    { "LE BUT DU JEU",
      "Vous voilà élu président de la République. Vous gouvernez cinq ans "
      "(20 trimestres), vous faites avancer le pays, vous restez populaire, "
      "puis vous tentez la réélection.\n\n"
      "Pas de troisième mandat : la Constitution y veille, et les "
      "conseillers aussi.\n\n"
      "Défaite : la destitution par le Parlement. Une partie qui s'achève "
      "ainsi n'entre pas dans les high scores. On ne s'en vante pas." },

    { "LE TOUR DE JEU",
      "Un tour = un trimestre.\n\n"
      "1. Le journal du matin résume ce qui a bougé.\n"
      "2. Un événement à trancher, avec deux ou trois choix. Aucun n'est "
      "gratuit.\n"
      "3. Vos actions, payées en capital politique.\n"
      "4. Les votes de l'Assemblée.\n"
      "5. Fin du tour : les effets des lois s'appliquent, l'opinion et "
      "le Parlement réagissent.\n\n"
      "Une fois par an, vous arbitrez le budget." },

    { "LES INDICATEURS",
      "Popularité : moyenne de trois électorats (populaires, moyennes, "
      "aisées). Une décision ne plaît presque jamais aux trois.\n"
      "Économie : croissance, chômage, dette.\n"
      "Cinq piliers de 0 à 100 : social, économie, technologie, écologie, "
      "sécurité.\n\n"
      "Trois jauges de danger :\n"
      "Tension : trop haute, la rue s'invite à l'Élysée.\n"
      "Scandale : la presse a de la mémoire.\n"
      "Défiance : l'humeur du Parlement.\n"
      "Scandale et défiance au plus haut ouvrent la voie à la "
      "destitution." },

    { "CAPITAL POLITIQUE ET ACTIONS",
      "Le capital politique (CP) est votre monnaie : 3 par trimestre, plus "
      "des bonus de popularité et de majorité, plafonné à 15.\n\n"
      "Loi : 2 à 6 CP. Elle met 1 à 4 trimestres à produire ses effets.\n\n"
      "49.3 : 4 CP. Adoption sans vote, mais la défiance explose.\n\n"
      "Intervention télévisée : 2 CP, pour regagner un peu de "
      "popularité." },

    { "L'ASSEMBLÉE",
      "577 députés, cinq groupes fictifs. Une loi passe avec 289 voix. "
      "Chaque groupe a une relation avec vous, qui évolue selon vos choix.\n\n"
      "Censure : si la défiance est haute et que l'opposition réunit 289 "
      "sièges, le gouvernement tombe. Vous restez.\n\n"
      "Dissolution : au plus une fois par an. Les sièges sont redistribués "
      "selon votre popularité. Pari risqué." },

    { "LE BUDGET ANNUEL",
      "Chaque année, un choix simple pour la loi de finances :\n\n"
      "Rigueur : la dette recule, les piliers et la popularité "
      "trinquent.\n\n"
      "Équilibre : ni l'un ni l'autre, ni triomphe ni drame.\n\n"
      "Relance : les piliers et l'emploi respirent, la dette s'alourdit. "
      "Les marchés prennent des notes." },

    { "ÉLECTION ET SCORE",
      "Au dernier tour du mandat, choisissez un thème de campagne. Le "
      "premier tour vous oppose à quatre candidats fictifs, puis les deux "
      "premiers vont au second tour.\n\n"
      "Score : popularité, progrès des piliers, emploi, dette, plus 500 "
      "points si réélu.\n\n"
      "Deux tableaux de 10 entrées : le 1er mandat, et le cumul sur dix "
      "ans. Trois initiales, pour la postérité." },

    { "COHABITATION, SÉNAT, RÉFÉRENDUM",
      "Cohabitation : législatives perdues, "
      "un PM d'opposition gouverne. Vous "
      "ne proposez que les lois de "
      "souveraineté (*) ; il fixe le "
      "budget.\n\n"
      "Sénat : 348 sièges, renouvelé par "
      "moitié tous les 3 ans. Destitution : "
      "deux tiers de l'Assemblée ET du "
      "Sénat.\n\n"
      "Référendum : 6 CP, une fois par "
      "mandat, sur une loi non adoptée.\n\n"
      "Remaniement : 3 CP, une fois par an, "
      "hors cohabitation." },

    { "LES COMMANDES",
      "Croix : naviguer, changer de choix.\n"
      "A : valider.\n"
      "B : retour, annuler.\n"
      "C : valider, ou aide.\n"
      "START : pause, ou retour.\n\n"
      "Dans l'aide : gauche et droite changent de page, B ou START "
      "ferment.\n\n"
      "Bon mandat, Madame, Monsieur le Président." }
};

#define N_PAGES ((u8)(sizeof(pages) / sizeof(pages[0])))

static ScreenId return_screen = SCREEN_TITLE;
static u8 page;
static bool dirty;

void help_open(ScreenId return_to)
{
    return_screen = return_to;
    screen_change(SCREEN_HELP);
}

static void help_init(void)
{
    page = 0;
    dirty = TRUE;
}

static void help_update(void)
{
    u16 p = input_repeat();
    u16 pr = input_pressed();

    if (pr & (INPUT_B | INPUT_START)) {
        screen_change(return_screen);
        return;
    }
    if ((p & INPUT_RIGHT) && page + 1 < N_PAGES) { page++; dirty = TRUE; }
    else if ((p & INPUT_LEFT) && page > 0) { page--; dirty = TRUE; }
}

static void help_draw(void)
{
    char buf[8];
    u16 i, x;

    if (!dirty) return;
    dirty = FALSE;

    ui_clear();
    ui_window_title(0, 0, 40, 28, pages[page].title, TRUE);
    buf[0] = '0' + page + 1;
    buf[1] = '/';
    buf[2] = '0' + N_PAGES;
    buf[3] = 0;
    ui_text_right(39, 1, buf, UI_STYLE_DIM);
    ui_text_wrap(2, 4, 36, 19, pages[page].text, UI_STYLE_TEXT);

    // Pastilles de pagination : la page courante en or.
    x = 20 - N_PAGES;
    for (i = 0; i < N_PAGES; i++)
        if (i == page) ui_icon_draw(x + i * 2, 24, UI_ICON_BULLET);
        else ui_text_draw(x + i * 2, 24, "·", UI_STYLE_DIM);
    if (page > 0)
        ui_text_draw(1, 26, "← préc.", UI_STYLE_DIM);
    if (page + 1 < N_PAGES)
        ui_text_right(39, 26, "suiv. →", UI_STYLE_DIM);
    ui_footer(9, 26, 22, "B : retour");
}

const Screen scr_help = { help_init, help_update, help_draw, NULL };
