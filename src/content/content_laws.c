// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "content/content_laws.h"

const char *const law_name[LOI_COUNT] = {
    "Hausse du SMIC", // LOI_SMIC
    "Baisse de l'IS", // LOI_IS
    "Rénovation thermique", // LOI_RENOV
    "Recrutement de policiers", // LOI_POLICE
    "Crédit d'impôt recherche", // LOI_CIR
};

const char *const law_desc[LOI_COUNT] = {
    "Revalorise le SMIC de 5 %. Salaires\nsuivent, les marges patronales\nprotestent. L'UPE parle de « drame ».", // LOI_SMIC
    "Réduit l'impôt sur les sociétés pour\n« libérer l'investissement ». Les\nrecettes de l'État s'évaporent.", // LOI_IS
    "Finance l'isolation de millions de\nlogements. Les passoires thermiques\nchauffent moins, les chantiers plus.", // LOI_RENOV
    "Recrute 10 000 policiers sur le\nquinquennat. Plus de bleu dans les\nrues, plus de lignes dans le budget.", // LOI_POLICE
    "Renforce le crédit d'impôt recherche.\nLes labos respirent, les cabinets de\nconseil en optimisation aussi.", // LOI_CIR
};

const char *const law_journal_ok[LOI_COUNT] = {
    "Le SMIC grimpe, les caisses sonnent", // LOI_SMIC
    "IS en baisse, on trinque en haut", // LOI_IS
    "Plan rénovation : les murs s'isolent", // LOI_RENOV
    "10 000 policiers dans les rues", // LOI_POLICE
    "Recherche : crédit pour les labos", // LOI_CIR
};

const char *const law_journal_ko[LOI_COUNT] = {
    "SMIC rejeté, les prix eux montent", // LOI_SMIC
    "IS : texte enterré, l'UPE fulmine", // LOI_IS
    "Rénovation : les passoires restent", // LOI_RENOV
    "Police : recrutement bloqué", // LOI_POLICE
    "CIR rejeté, labos sur la paillasse", // LOI_CIR
};
