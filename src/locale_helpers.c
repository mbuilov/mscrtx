/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* locale_helpers.c */

/* convert: Language tag -> lang id -> Language/County for setlocale() */

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for winnls.h GetLocaleInfoA */

#define LOCALE_RPL_IMPL
#include "mscrtx/localerpl.h" /* for localerpl_is_utf8 */
#include "mscrtx/locale_helpers.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

/* maximum locale name length */
#define LOCALE_BUF_SIZE 128

/* not defined for _WIN32_WINNT < _WIN32_WINNT_VISTA */
#ifndef LOCALE_CUSTOM_UNSPECIFIED
#define LOCALE_CUSTOM_UNSPECIFIED 0x1000
#endif

struct lang_tag {
	const char *tag;
	const char *lang_country;
	unsigned id;
};

/* https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/a9eac961-e77d-41a6-90a5-ce1a8b0cdb9c */
static const struct lang_tag lang_tag_table[] = {
{"aa",             "Afar",                                                         0x1000},
{"aa-DJ",          "Afar_Djibouti",                                                0x1000},
{"aa-ER",          "Afar_Eritrea",                                                 0x1000},
{"aa-ET",          "Afar_Ethiopia",                                                0x1000},
{"af",             "Afrikaans",                                                    0x0036},
{"af-NA",          "Afrikaans_Namibia",                                            0x1000},
{"af-ZA",          "Afrikaans_South Africa",                                       0x0436},
{"agq",            "Aghem",                                                        0x1000},
{"agq-CM",         "Aghem_Cameroon",                                               0x1000},
{"ak",             "Akan",                                                         0x1000},
{"ak-GH",          "Akan_Ghana",                                                   0x1000},
{"sq",             "Albanian",                                                     0x001C},
{"sq-AL",          "Albanian_Albania",                                             0x041C},
{"sq-MK",          "Albanian_North Macedonia",                                     0x1000},
{"gsw",            "Alsatian",                                                     0x0084},
{"gsw-FR",         "Alsatian_France",                                              0x0484},
{"gsw-LI",         "Alsatian_Liechtenstein",                                       0x1000},
{"gsw-CH",         "Alsatian_Switzerland",                                         0x1000},
{"am",             "Amharic",                                                      0x005E},
{"am-ET",          "Amharic_Ethiopia",                                             0x045E},
{"ar",             "Arabic",                                                       0x0001},
{"ar-DZ",          "Arabic_Algeria",                                               0x1401},
{"ar-BH",          "Arabic_Bahrain",                                               0x3C01},
{"ar-TD",          "Arabic_Chad",                                                  0x1000},
{"ar-KM",          "Arabic_Comoros",                                               0x1000},
{"ar-DJ",          "Arabic_Djibouti",                                              0x1000},
{"ar-EG",          "Arabic_Egypt",                                                 0x0c01},
{"ar-ER",          "Arabic_Eritrea",                                               0x1000},
{"ar-IQ",          "Arabic_Iraq",                                                  0x0801},
{"ar-IL",          "Arabic_Israel",                                                0x1000},
{"ar-JO",          "Arabic_Jordan",                                                0x2C01},
{"ar-KW",          "Arabic_Kuwait",                                                0x3401},
{"ar-LB",          "Arabic_Lebanon",                                               0x3001},
{"ar-LY",          "Arabic_Libya",                                                 0x1001},
{"ar-MR",          "Arabic_Mauritania",                                            0x1000},
{"ar-MA",          "Arabic_Morocco",                                               0x1801},
{"ar-OM",          "Arabic_Oman",                                                  0x2001},
{"ar-PS",          "Arabic_Palestinian Authority",                                 0x1000},
{"ar-QA",          "Arabic_Qatar",                                                 0x4001},
{"ar-SA",          "Arabic_Saudi Arabia",                                          0x0401},
{"ar-SO",          "Arabic_Somalia",                                               0x1000},
{"ar-SS",          "Arabic_South Sudan",                                           0x1000},
{"ar-SD",          "Arabic_Sudan",                                                 0x1000},
{"ar-SY",          "Arabic_Syria",                                                 0x2801},
{"ar-TN",          "Arabic_Tunisia",                                               0x1C01},
{"ar-AE",          "Arabic_U.A.E.",                                                0x3801},
{"ar-001",         "Arabic_World",                                                 0x1000},
{"ar-YE",          "Arabic_Yemen",                                                 0x2401},
{"hy",             "Armenian",                                                     0x002B},
{"hy-AM",          "Armenian_Armenia",                                             0x042B},
{"as",             "Assamese",                                                     0x004D},
{"as-IN",          "Assamese_India",                                               0x044D},
{"ast",            "Asturian",                                                     0x1000},
{"ast-ES",         "Asturian_Spain",                                               0x1000},
{"asa",            "Asu",                                                          0x1000},
{"asa-TZ",         "Asu_Tanzania",                                                 0x1000},
{"az-Cyrl",        "Azerbaijani (Cyrillic)",                                       0x742C},
{"az-Cyrl-AZ",     "Azerbaijani (Cyrillic)_Azerbaijan",                            0x082C},
{"az",             "Azerbaijani (Latin)",                                          0x002C},
{"az-Latn",        "Azerbaijani (Latin)",                                          0x782C},
{"az-Latn-AZ",     "Azerbaijani (Latin)_Azerbaijan",                               0x042C},
{"ksf",            "Bafia",                                                        0x1000},
{"ksf-CM",         "Bafia_Cameroon",                                               0x1000},
{"bm",             "Bamanankan",                                                   0x1000},
{"bm-Latn-ML",     "Bamanankan (Latin)_Mali",                                      0x1000},
{"bn",             "Bangla",                                                       0x0045},
{"bn-BD",          "Bangla_Bangladesh",                                            0x0845},
{"bn-IN",          "Bangla_India",                                                 0x0445},
{"bas",            "Basaa",                                                        0x1000},
{"bas-CM",         "Basaa_Cameroon",                                               0x1000},
{"ba",             "Bashkir",                                                      0x006D},
{"ba-RU",          "Bashkir_Russia",                                               0x046D},
{"eu",             "Basque",                                                       0x002D},
{"eu-ES",          "Basque_Spain",                                                 0x042D},
{"be",             "Belarusian",                                                   0x0023},
{"be-BY",          "Belarusian_Belarus",                                           0x0423},
{"bem",            "Bemba",                                                        0x1000},
{"bem-ZM",         "Bemba_Zambia",                                                 0x1000},
{"bez",            "Bena",                                                         0x1000},
{"bez-TZ",         "Bena_Tanzania",                                                0x1000},
{"byn",            "Blin",                                                         0x1000},
{"byn-ER",         "Blin_Eritrea",                                                 0x1000},
{"brx",            "Bodo",                                                         0x1000},
{"brx-IN",         "Bodo_India",                                                   0x1000},
{"bs-Cyrl",        "Bosnian (Cyrillic)",                                           0x641A},
{"bs-Cyrl-BA",     "Bosnian (Cyrillic)_Bosnia and Herzegovina",                    0x201A},
{"bs-Latn",        "Bosnian (Latin)",                                              0x681A},
{"bs",             "Bosnian (Latin)",                                              0x781A},
{"bs-Latn-BA",     "Bosnian (Latin)_Bosnia and Herzegovina",                       0x141A},
{"br",             "Breton",                                                       0x007E},
{"br-FR",          "Breton_France",                                                0x047E},
{"bg",             "Bulgarian",                                                    0x0002},
{"bg-BG",          "Bulgarian_Bulgaria",                                           0x0402},
{"my",             "Burmese",                                                      0x0055},
{"my-MM",          "Burmese_Myanmar",                                              0x0455},
{"ca",             "Catalan",                                                      0x0003},
{"ca-AD",          "Catalan_Andorra",                                              0x1000},
{"ca-FR",          "Catalan_France",                                               0x1000},
{"ca-IT",          "Catalan_Italy",                                                0x1000},
{"ca-ES",          "Catalan_Spain",                                                0x0403},
{"ceb",            "Cebuano",                                                      0x1000},
{"ceb-Latn",       "Cebuan (Latin)",                                               0x1000},
{"ceb-Latn-PH",    "Cebuan (Latin)_Philippines",                                   0x1000},
{"tzm-Latn-MA",    "Central Atlas Tamazight (Latin)_Morocco",                      0x1000},
{"ku",             "Central Kurdish",                                              0x0092},
{"ku-Arab",        "Central Kurdish",                                              0x7c92},
{"ku-Arab-IQ",     "Central Kurdish_Iraq",                                         0x0492},
{"ccp",            "Chakma",                                                       0x1000},
{"ccp-Cakm",       "Chakma_Chakma",                                                0x1000},
{"ccp-Cakm-BD",    "Chakma_Bangladesh",                                            0x1000},
{"ccp-Cakm-IN",    "Chakma_India",                                                 0x1000},
{"cd-RU",          "Chechen_Russia",                                               0x1000},
{"chr",            "Cherokee",                                                     0x005C},
{"chr-Cher",       "Cherokee",                                                     0x7c5C},
{"chr-Cher-US",    "Cherokee_United States",                                       0x045C},
{"cgg",            "Chiga",                                                        0x1000},
{"cgg-UG",         "Chiga_Uganda",                                                 0x1000},
{"zh-Hans",        "Chinese (Simplified)",                                         0x0004},
{"zh",             "Chinese (Simplified)",                                         0x7804},
{"zh-CN",          "Chinese (Simplified)_People's Republic of China",              0x0804},
{"zh-SG",          "Chinese (Simplified)_Singapore",                               0x1004},
{"zh-Hant",        "Chinese (Traditional)",                                        0x7C04},
{"zh-HK",          "Chinese (Traditional)_Hong Kong S.A.R.",                       0x0C04},
{"zh-MO",          "Chinese (Traditional)_Macao S.A.R.",                           0x1404},
{"zh-TW",          "Chinese (Traditional)_Taiwan",                                 0x0404},
{"cu-RU",          "Church Slavic_Russia",                                         0x1000},
{"swc",            "Congo Swahili",                                                0x1000},
{"swc-CD",         "Congo Swahili_Congo DRC",                                      0x1000},
{"kw",             "Cornish",                                                      0x1000},
{"kw-GB",          "Cornish_United Kingdom",                                       0x1000},
{"co",             "Corsican",                                                     0x0083},
{"co-FR",          "Corsican_France",                                              0x0483},
{"hr",             "Croatian",                                                     0x001A},
{"hr-HR",          "Croatian_Croatia",                                             0x041A},
{"hr-BA",          "Croatian (Latin)_Bosnia and Herzegovina",                      0x101A},
{"cs",             "Czech",                                                        0x0005},
{"cs-CZ",          "Czech_Czech Republic",                                         0x0405},
{"da",             "Danish",                                                       0x0006},
{"da-DK",          "Danish_Denmark",                                               0x0406},
{"da-GL",          "Danish_Greenland",                                             0x1000},
{"prs",            "Dari",                                                         0x008C},
{"prs-AF",         "Dari_Afghanistan",                                             0x048C},
{"dv",             "Divehi",                                                       0x0065},
{"dv-MV",          "Divehi_Maldives",                                              0x0465},
{"dua",            "Duala",                                                        0x1000},
{"dua-CM",         "Duala_Cameroon",                                               0x1000},
{"nl",             "Dutch",                                                        0x0013},
{"nl-AW",          "Dutch_Aruba",                                                  0x1000},
{"nl-BE",          "Dutch_Belgium",                                                0x0813},
{"nl-BQ",          "Dutch_Bonaire, Sint Eustatius and Saba",                       0x1000},
{"nl-CW",          "Dutch_Cura?ao",                                                0x1000},
{"nl-NL",          "Dutch_Netherlands",                                            0x0413},
{"nl-SX",          "Dutch_Sint Maarten",                                           0x1000},
{"nl-SR",          "Dutch_Suriname",                                               0x1000},
{"dz",             "Dzongkha",                                                     0x1000},
{"dz-BT",          "Dzongkha_Bhutan",                                              0x0C51},
{"ebu",            "Embu",                                                         0x1000},
{"ebu-KE",         "Embu_Kenya",                                                   0x1000},
{"en",             "English",                                                      0x0009},
{"en-AS",          "English_American Samoa",                                       0x1000},
{"en-AI",          "English_Anguilla",                                             0x1000},
{"en-AG",          "English_Antigua and Barbuda",                                  0x1000},
{"en-AU",          "English_Australia",                                            0x0C09},
{"en-AT",          "English_Austria",                                              0x1000},
{"en-BS",          "English_Bahamas",                                              0x1000},
{"en-BB",          "English_Barbados",                                             0x1000},
{"en-BE",          "English_Belgium",                                              0x1000},
{"en-BZ",          "English_Belize",                                               0x2809},
{"en-BM",          "English_Bermuda",                                              0x1000},
{"en-BW",          "English_Botswana",                                             0x1000},
{"en-IO",          "English_British Indian Ocean Territory",                       0x1000},
{"en-VG",          "English_British Virgin Islands",                               0x1000},
{"en-BI",          "English_Burundi",                                              0x1000},
{"en-CM",          "English_Cameroon",                                             0x1000},
{"en-CA",          "English_Canada",                                               0x1009},
{"en-029",         "English_Caribbean",                                            0x2409},
{"en-KY",          "English_Cayman Islands",                                       0x1000},
{"en-CX",          "English_Christmas Island",                                     0x1000},
{"en-CC",          "English_Cocos [Keeling] Islands",                              0x1000},
{"en-CK",          "English_Cook Islands",                                         0x1000},
{"en-CY",          "English_Cyprus",                                               0x1000},
{"en-DK",          "English_Denmark",                                              0x1000},
{"en-DM",          "English_Dominica",                                             0x1000},
{"en-ER",          "English_Eritrea",                                              0x1000},
{"en-150",         "English_Europe",                                               0x1000},
{"en-FK",          "English_Falkland Islands",                                     0x1000},
{"en-FI",          "English_Finland",                                              0x1000},
{"en-FJ",          "English_Fiji",                                                 0x1000},
{"en-GM",          "English_Gambia",                                               0x1000},
{"en-DE",          "English_Germany",                                              0x1000},
{"en-GH",          "English_Ghana",                                                0x1000},
{"en-GI",          "English_Gibraltar",                                            0x1000},
{"en-GD",          "English_Grenada",                                              0x1000},
{"en-GU",          "English_Guam",                                                 0x1000},
{"en-GG",          "English_Guernsey",                                             0x1000},
{"en-GY",          "English_Guyana",                                               0x1000},
{"en-HK",          "English_Hong Kong",                                            0x3C09},
{"en-IN",          "English_India",                                                0x4009},
{"en-IE",          "English_Ireland",                                              0x1809},
{"en-IM",          "English_Isle of Man",                                          0x1000},
{"en-IL",          "English_Israel",                                               0x1000},
{"en-JM",          "English_Jamaica",                                              0x2009},
{"en-JE",          "English_Jersey",                                               0x1000},
{"en-KE",          "English_Kenya",                                                0x1000},
{"en-KI",          "English_Kiribati",                                             0x1000},
{"en-LS",          "English_Lesotho",                                              0x1000},
{"en-LR",          "English_Liberia",                                              0x1000},
{"en-MO",          "English_Macao SAR",                                            0x1000},
{"en-MG",          "English_Madagascar",                                           0x1000},
{"en-MW",          "English_Malawi",                                               0x1000},
{"en-MY",          "English_Malaysia",                                             0x4409},
{"en-MT",          "English_Malta",                                                0x1000},
{"en-MH",          "English_Marshall Islands",                                     0x1000},
{"en-MU",          "English_Mauritius",                                            0x1000},
{"en-FM",          "English_Micronesia",                                           0x1000},
{"en-MS",          "English_Montserrat",                                           0x1000},
{"en-NA",          "English_Namibia",                                              0x1000},
{"en-NR",          "English_Nauru",                                                0x1000},
{"en-NL",          "English_Netherlands",                                          0x1000},
{"en-NZ",          "English_New Zealand",                                          0x1409},
{"en-NG",          "English_Nigeria",                                              0x1000},
{"en-NU",          "English_Niue",                                                 0x1000},
{"en-NF",          "English_Norfolk Island",                                       0x1000},
{"en-MP",          "English_Northern Mariana Islands",                             0x1000},
{"en-PK",          "English_Pakistan",                                             0x1000},
{"en-PW",          "English_Palau",                                                0x1000},
{"en-PG",          "English_Papua New Guinea",                                     0x1000},
{"en-PN",          "English_Pitcairn Islands",                                     0x1000},
{"en-PR",          "English_Puerto Rico",                                          0x1000},
{"en-PH",          "English_Republic of the Philippines",                          0x3409},
{"en-RW",          "English_Rwanda",                                               0x1000},
{"en-KN",          "English_Saint Kitts and Nevis",                                0x1000},
{"en-LC",          "English_Saint Lucia",                                          0x1000},
{"en-VC",          "English_Saint Vincent and the Grenadines",                     0x1000},
{"en-WS",          "English_Samoa",                                                0x1000},
{"en-SC",          "English_Seychelles",                                           0x1000},
{"en-SL",          "English_Sierra Leone",                                         0x1000},
{"en-SG",          "English_Singapore",                                            0x4809},
{"en-SX",          "English_Sint Maarten",                                         0x1000},
{"en-SI",          "English_Slovenia",                                             0x1000},
{"en-SB",          "English_Solomon Islands",                                      0x1000},
{"en-ZA",          "English_South Africa",                                         0x1C09},
{"en-SS",          "English_South Sudan",                                          0x1000},
{"en-SH",          "English_St Helena, Ascension, Tristan da Cunha",               0x1000},
{"en-SD",          "English_Sudan",                                                0x1000},
{"en-SZ",          "English_Swaziland",                                            0x1000},
{"en-SE",          "English_Sweden",                                               0x1000},
{"en-CH",          "English_Switzerland",                                          0x1000},
{"en-TZ",          "English_Tanzania",                                             0x1000},
{"en-TK",          "English_Tokelau",                                              0x1000},
{"en-TO",          "English_Tonga",                                                0x1000},
{"en-TT",          "English_Trinidad and Tobago",                                  0x2c09},
{"en-TC",          "English_Turks and Caicos Islands",                             0x1000},
{"en-TV",          "English_Tuvalu",                                               0x1000},
{"en-UG",          "English_Uganda",                                               0x1000},
{"en-AE",          "English_United Arab Emirates",                                 0x4C09},
{"en-GB",          "English_United Kingdom",                                       0x0809},
{"en-US",          "English_United States",                                        0x0409},
{"en-UM",          "English_US Minor Outlying Islands",                            0x1000},
{"en-VI",          "English_US Virgin Islands",                                    0x1000},
{"en-VU",          "English_Vanuatu",                                              0x1000},
{"en-001",         "English_World",                                                0x1000},
{"en-ZM",          "English_Zambia",                                               0x1000},
{"en-ZW",          "English_Zimbabwe",                                             0x3009},
{"eo",             "Esperanto",                                                    0x1000},
{"eo-001",         "Esperanto_World",                                              0x1000},
{"et",             "Estonian",                                                     0x0025},
{"et-EE",          "Estonian_Estonia",                                             0x0425},
{"ee",             "Ewe",                                                          0x1000},
{"ee-GH",          "Ewe_Ghana",                                                    0x1000},
{"ee-TG",          "Ewe_Togo",                                                     0x1000},
{"ewo",            "Ewondo",                                                       0x1000},
{"ewo-CM",         "Ewondo_Cameroon",                                              0x1000},
{"fo",             "Faroese",                                                      0x0038},
{"fo-DK",          "Faroese_Denmark",                                              0x1000},
{"fo-FO",          "Faroese_Faroe Islands",                                        0x0438},
{"fil",            "Filipino",                                                     0x0064},
{"fil-PH",         "Filipino_Philippines",                                         0x0464},
{"fi",             "Finnish",                                                      0x000B},
{"fi-FI",          "Finnish_Finland",                                              0x040B},
{"fr",             "French",                                                       0x000C},
{"fr-DZ",          "French_Algeria",                                               0x1000},
{"fr-BE",          "French_Belgium",                                               0x080C},
{"fr-BJ",          "French_Benin",                                                 0x1000},
{"fr-BF",          "French_Burkina Faso",                                          0x1000},
{"fr-BI",          "French_Burundi",                                               0x1000},
{"fr-CM",          "French_Cameroon",                                              0x2c0C},
{"fr-CA",          "French_Canada",                                                0x0c0C},
{"fr-CF",          "French_Central African Republic",                              0x1000},
{"fr-TD",          "French_Chad",                                                  0x1000},
{"fr-KM",          "French_Comoros",                                               0x1000},
{"fr-CG",          "French_Congo",                                                 0x1000},
{"fr-CD",          "French_Congo, DRC",                                            0x240C},
{"fr-CI",          "French_C?te d'Ivoire",                                         0x300C},
{"fr-DJ",          "French_Djibouti",                                              0x1000},
{"fr-GQ",          "French_Equatorial Guinea",                                     0x1000},
{"fr-FR",          "French_France",                                                0x040C},
{"fr-GF",          "French_French Guiana",                                         0x1000},
{"fr-PF",          "French_French Polynesia",                                      0x1000},
{"fr-GA",          "French_Gabon",                                                 0x1000},
{"fr-GP",          "French_Guadeloupe",                                            0x1000},
{"fr-GN",          "French_Guinea",                                                0x1000},
{"fr-HT",          "French_Haiti",                                                 0x3c0C},
{"fr-LU",          "French_Luxembourg",                                            0x140C},
{"fr-MG",          "French_Madagascar",                                            0x1000},
{"fr-ML",          "French_Mali",                                                  0x340C},
{"fr-MQ",          "French_Martinique",                                            0x1000},
{"fr-MR",          "French_Mauritania",                                            0x1000},
{"fr-MU",          "French_Mauritius",                                             0x1000},
{"fr-YT",          "French_Mayotte",                                               0x1000},
{"fr-MA",          "French_Morocco",                                               0x380C},
{"fr-NC",          "French_New Caledonia",                                         0x1000},
{"fr-NE",          "French_Niger",                                                 0x1000},
{"fr-MC",          "French_Principality of Monaco",                                0x180C},
{"fr-RE",          "French_Reunion",                                               0x200C},
{"fr-RW",          "French_Rwanda",                                                0x1000},
{"fr-BL",          "French_Saint Barth?lemy",                                      0x1000},
{"fr-MF",          "French_Saint Martin",                                          0x1000},
{"fr-PM",          "French_Saint Pierre and Miquelon",                             0x1000},
{"fr-SN",          "French_Senegal",                                               0x280C},
{"fr-SC",          "French_Seychelles",                                            0x1000},
{"fr-CH",          "French_Switzerland",                                           0x100C},
{"fr-SY",          "French_Syria",                                                 0x1000},
{"fr-TG",          "French_Togo",                                                  0x1000},
{"fr-TN",          "French_Tunisia",                                               0x1000},
{"fr-VU",          "French_Vanuatu",                                               0x1000},
{"fr-WF",          "French_Wallis and Futuna",                                     0x1000},
{"fy",             "Frisian",                                                      0x0062},
{"fy-NL",          "Frisian_Netherlands",                                          0x0462},
{"fur",            "Friulian",                                                     0x1000},
{"fur-IT",         "Friulian_Italy",                                               0x1000},
{"ff",             "Fulah",                                                        0x0067},
{"ff-Latn",        "Fulah (Latin)",                                                0x7C67},
{"ff-Latn-BF",     "Fulah (Latin)_Burkina Faso",                                   0x1000},
{"ff-CM",          "Fulah_Cameroon",                                               0x1000},
{"ff-Latn-CM",     "Fulah (Latin)_Cameroon",                                       0x1000},
{"ff-Latn-GM",     "Fulah (Latin)_Gambia",                                         0x1000},
{"ff-Latn-GH",     "Fulah (Latin)_Ghana",                                          0x1000},
{"ff-GN",          "Fulah_Guinea",                                                 0x1000},
{"ff-Latn-GN",     "Fulah (Latin)_Guinea",                                         0x1000},
{"ff-Latn-GW",     "Fulah (Latin)_Guinea-Bissau",                                  0x1000},
{"ff-Latn-LR",     "Fulah (Latin)_Liberia",                                        0x1000},
{"ff-MR",          "Fulah_Mauritania",                                             0x1000},
{"ff-Latn-MR",     "Fulah (Latin)_Mauritania",                                     0x1000},
{"ff-Latn-NE",     "Fulah (Latin)_Niger",                                          0x1000},
{"ff-NG",          "Fulah_Nigeria",                                                0x1000},
{"ff-Latn-NG",     "Fulah (Latin)_Nigeria",                                        0x1000},
{"ff-Latn-SN",     "Fulah_Senegal",                                                0x0867},
{"ff-Latn-SL",     "Fulah (Latin)_Sierra Leone",                                   0x1000},
{"gl",             "Galician",                                                     0x0056},
{"gl-ES",          "Galician_Spain",                                               0x0456},
{"lg",             "Ganda",                                                        0x1000},
{"lg-UG",          "Ganda_Uganda",                                                 0x1000},
{"ka",             "Georgian",                                                     0x0037},
{"ka-GE",          "Georgian_Georgia",                                             0x0437},
{"de",             "German",                                                       0x0007},
{"de-AT",          "German_Austria",                                               0x0C07},
{"de-BE",          "German_Belgium",                                               0x1000},
{"de-DE",          "German_Germany",                                               0x0407},
{"de-IT",          "German_Italy",                                                 0x1000},
{"de-LI",          "German_Liechtenstein",                                         0x1407},
{"de-LU",          "German_Luxembourg",                                            0x1007},
{"de-CH",          "German_Switzerland",                                           0x0807},
{"el",             "Greek",                                                        0x0008},
{"el-CY",          "Greek_Cyprus",                                                 0x1000},
{"el-GR",          "Greek_Greece",                                                 0x0408},
{"kl",             "Greenlandic",                                                  0x006F},
{"kl-GL",          "Greenlandic_Greenland",                                        0x046F},
{"gn",             "Guarani",                                                      0x0074},
{"gn-PY",          "Guarani_Paraguay",                                             0x0474},
{"gu",             "Gujarati",                                                     0x0047},
{"gu-IN",          "Gujarati_India",                                               0x0447},
{"guz",            "Gusii",                                                        0x1000},
{"guz-KE",         "Gusii_Kenya",                                                  0x1000},
{"ha",             "Hausa (Latin)",                                                0x0068},
{"ha-Latn",        "Hausa (Latin)",                                                0x7C68},
{"ha-Latn-GH",     "Hausa (Latin)_Ghana",                                          0x1000},
{"ha-Latn-NE",     "Hausa (Latin)_Niger",                                          0x1000},
{"ha-Latn-NG",     "Hausa (Latin)_Nigeria",                                        0x0468},
{"haw",            "Hawaiian",                                                     0x0075},
{"haw-US",         "Hawaiian_United States",                                       0x0475},
{"he",             "Hebrew",                                                       0x000D},
{"he-IL",          "Hebrew_Israel",                                                0x040D},
{"hi",             "Hindi",                                                        0x0039},
{"hi-IN",          "Hindi_India",                                                  0x0439},
{"hu",             "Hungarian",                                                    0x000E},
{"hu-HU",          "Hungarian_Hungary",                                            0x040E},
{"is",             "Icelandic",                                                    0x000F},
{"is-IS",          "Icelandic_Iceland",                                            0x040F},
{"ig",             "Igbo",                                                         0x0070},
{"ig-NG",          "Igbo_Nigeria",                                                 0x0470},
{"id",             "Indonesian",                                                   0x0021},
{"id-ID",          "Indonesian_Indonesia",                                         0x0421},
{"ia",             "Interlingua",                                                  0x1000},
{"ia-FR",          "Interlingua_France",                                           0x1000},
{"ia-001",         "Interlingua_World",                                            0x1000},
{"iu",             "Inuktitut (Latin)",                                            0x005D},
{"iu-Latn",        "Inuktitut (Latin)",                                            0x7C5D},
{"iu-Latn-CA",     "Inuktitut (Latin)_Canada",                                     0x085D},
{"iu-Cans",        "Inuktitut (Syllabics)",                                        0x785D},
{"iu-Cans-CA",     "Inuktitut (Syllabics)_Canada",                                 0x045d},
{"ga",             "Irish",                                                        0x003C},
{"ga-IE",          "Irish_Ireland",                                                0x083C},
{"it",             "Italian",                                                      0x0010},
{"it-IT",          "Italian_Italy",                                                0x0410},
{"it-SM",          "Italian_San Marino",                                           0x1000},
{"it-CH",          "Italian_Switzerland",                                          0x0810},
{"it-VA",          "Italian_Vatican City",                                         0x1000},
{"ja",             "Japanese",                                                     0x0011},
{"ja-JP",          "Japanese_Japan",                                               0x0411},
{"jv",             "Javanese",                                                     0x1000},
{"jv-Latn",        "Javanese_Latin",                                               0x1000},
{"jv-Latn-ID",     "Javanese_Latin, Indonesia",                                    0x1000},
{"dyo",            "Jola-Fonyi",                                                   0x1000},
{"dyo-SN",         "Jola-Fonyi_Senegal",                                           0x1000},
{"kea",            "Kabuverdianu",                                                 0x1000},
{"kea-CV",         "Kabuverdianu_Cabo Verde",                                      0x1000},
{"kab",            "Kabyle",                                                       0x1000},
{"kab-DZ",         "Kabyle_Algeria",                                               0x1000},
{"kkj",            "Kako",                                                         0x1000},
{"kkj-CM",         "Kako_Cameroon",                                                0x1000},
{"kln",            "Kalenjin",                                                     0x1000},
{"kln-KE",         "Kalenjin_Kenya",                                               0x1000},
{"kam",            "Kamba",                                                        0x1000},
{"kam-KE",         "Kamba_Kenya",                                                  0x1000},
{"kn",             "Kannada",                                                      0x004B},
{"kn-IN",          "Kannada_India",                                                0x044B},
{"ks",             "Kashmiri",                                                     0x0060},
{"ks-Arab",        "Kashmiri_Perso-Arabic",                                        0x0460},
{"ks-Arab-IN",     "Kashmiri_Perso-Arabic",                                        0x1000},
{"kk",             "Kazakh",                                                       0x003F},
{"kk-KZ",          "Kazakh_Kazakhstan",                                            0x043F},
{"km",             "Khmer",                                                        0x0053},
{"km-KH",          "Khmer_Cambodia",                                               0x0453},
{"quc",            "K'iche",                                                       0x0086},
{"quc-Latn-GT",    "K'iche_Guatemala",                                             0x0486},
{"ki",             "Kikuyu",                                                       0x1000},
{"ki-KE",          "Kikuyu_Kenya",                                                 0x1000},
{"rw",             "Kinyarwanda",                                                  0x0087},
{"rw-RW",          "Kinyarwanda_Rwanda",                                           0x0487},
{"sw",             "Kiswahili",                                                    0x0041},
{"sw-KE",          "Kiswahili_Kenya",                                              0x0441},
{"sw-TZ",          "Kiswahili_Tanzania",                                           0x1000},
{"sw-UG",          "Kiswahili_Uganda",                                             0x1000},
{"kok",            "Konkani",                                                      0x0057},
{"kok-IN",         "Konkani_India",                                                0x0457},
{"ko",             "Korean",                                                       0x0012},
{"ko-KR",          "Korean_Korea",                                                 0x0412},
{"ko-KP",          "Korean_North Korea",                                           0x1000},
{"khq",            "Koyra Chiini",                                                 0x1000},
{"khq-ML",         "Koyra Chiini_Mali",                                            0x1000},
{"ses",            "Koyraboro Senni",                                              0x1000},
{"ses-ML",         "Koyraboro Senni_Mali",                                         0x1000},
{"nmg",            "Kwasio",                                                       0x1000},
{"nmg-CM",         "Kwasio_Cameroon",                                              0x1000},
{"ky",             "Kyrgyz",                                                       0x0040},
{"ky-KG",          "Kyrgyz_Kyrgyzstan",                                            0x0440},
{"ku-Arab-IR",     "Kurdish_Perso-Arabic, Iran",                                   0x1000},
{"lkt",            "Lakota",                                                       0x1000},
{"lkt-US",         "Lakota_United States",                                         0x1000},
{"lag",            "Langi",                                                        0x1000},
{"lag-TZ",         "Langi_Tanzania",                                               0x1000},
{"lo",             "Lao",                                                          0x0054},
{"lo-LA",          "Lao_Lao P.D.R.",                                               0x0454},
{"lv",             "Latvian",                                                      0x0026},
{"lv-LV",          "Latvian_Latvia",                                               0x0426},
{"ln",             "Lingala",                                                      0x1000},
{"ln-AO",          "Lingala_Angola",                                               0x1000},
{"ln-CF",          "Lingala_Central African Republic",                             0x1000},
{"ln-CG",          "Lingala_Congo",                                                0x1000},
{"ln-CD",          "Lingala_Congo DRC",                                            0x1000},
{"lt",             "Lithuanian",                                                   0x0027},
{"lt-LT",          "Lithuanian_Lithuania",                                         0x0427},
{"nds",            "Low German",                                                   0x1000},
{"nds-DE",         "Low German_Germany",                                           0x1000},
{"nds-NL",         "Low German_Netherlands",                                       0x1000},
{"dsb",            "Lower Sorbian",                                                0x7C2E},
{"dsb-DE",         "Lower Sorbian_Germany",                                        0x082E},
{"lu",             "Luba-Katanga",                                                 0x1000},
{"lu-CD",          "Luba-Katanga_Congo DRC",                                       0x1000},
{"luo",            "Luo",                                                          0x1000},
{"luo-KE",         "Luo_Kenya",                                                    0x1000},
{"lb",             "Luxembourgish",                                                0x006E},
{"lb-LU",          "Luxembourgish_Luxembourg",                                     0x046E},
{"luy",            "Luyia",                                                        0x1000},
{"luy-KE",         "Luyia_Kenya",                                                  0x1000},
{"mk",             "Macedonian",                                                   0x002F},
{"mk-MK",          "Macedonian_North Macedonia",                                   0x042F},
{"jmc",            "Machame",                                                      0x1000},
{"jmc-TZ",         "Machame_Tanzania",                                             0x1000},
{"mgh",            "Makhuwa-Meetto",                                               0x1000},
{"mgh-MZ",         "Makhuwa-Meetto_Mozambique",                                    0x1000},
{"kde",            "Makonde",                                                      0x1000},
{"kde-TZ",         "Makonde_Tanzania",                                             0x1000},
{"mg",             "Malagasy",                                                     0x1000},
{"mg-MG",          "Malagasy_Madagascar",                                          0x1000},
{"ms",             "Malay",                                                        0x003E},
{"ms-BN",          "Malay_Brunei Darussalam",                                      0x083E},
{"ms-MY",          "Malay_Malaysia",                                               0x043E},
{"ml",             "Malayalam",                                                    0x004C},
{"ml-IN",          "Malayalam_India",                                              0x044C},
{"mt",             "Maltese",                                                      0x003A},
{"mt-MT",          "Maltese_Malta",                                                0x043A},
{"gv",             "Manx",                                                         0x1000},
{"gv-IM",          "Manx_Isle of Man",                                             0x1000},
{"mi",             "Maori",                                                        0x0081},
{"mi-NZ",          "Maori_New Zealand",                                            0x0481},
{"arn",            "Mapudungun",                                                   0x007A},
{"arn-CL",         "Mapudungun_Chile",                                             0x047A},
{"mr",             "Marathi",                                                      0x004E},
{"mr-IN",          "Marathi_India",                                                0x044E},
{"mas",            "Masai",                                                        0x1000},
{"mas-KE",         "Masai_Kenya",                                                  0x1000},
{"mas-TZ",         "Masai_Tanzania",                                               0x1000},
{"mzn-IR",         "Mazanderani_Iran",                                             0x1000},
{"mer",            "Meru",                                                         0x1000},
{"mer-KE",         "Meru_Kenya",                                                   0x1000},
{"mgo",            "Meta'",                                                        0x1000},
{"mgo-CM",         "Meta'_Cameroon",                                               0x1000},
{"moh",            "Mohawk",                                                       0x007C},
{"moh-CA",         "Mohawk_Canada",                                                0x047C},
{"mn",             "Mongolian (Cyrillic)",                                         0x0050},
{"mn-Cyrl",        "Mongolian (Cyrillic)",                                         0x7850},
{"mn-MN",          "Mongolian (Cyrillic)_Mongolia",                                0x0450},
{"mn-Mong",        "Mongolian (Traditional Mongolian)",                            0x7C50},
{"mn-Mong-CN",     "Mongolian (Traditional Mongolian)_People's Republic of China", 0x0850},
{"mn-Mong-MN",     "Mongolian (Traditional Mongolian)_Mongolia",                   0x0C50},
{"mfe",            "Morisyen",                                                     0x1000},
{"mfe-MU",         "Morisyen_Mauritius",                                           0x1000},
{"mua",            "Mundang",                                                      0x1000},
{"mua-CM",         "Mundang_Cameroon",                                             0x1000},
{"nqo",            "N'ko",                                                         0x1000},
{"nqo-GN",         "N'ko_Guinea",                                                  0x1000},
{"naq",            "Nama",                                                         0x1000},
{"naq-NA",         "Nama_Namibia",                                                 0x1000},
{"ne",             "Nepali",                                                       0x0061},
{"ne-IN",          "Nepali_India",                                                 0x0861},
{"ne-NP",          "Nepali_Nepal",                                                 0x0461},
{"nnh",            "Ngiemboon",                                                    0x1000},
{"nnh-CM",         "Ngiemboon_Cameroon",                                           0x1000},
{"jgo",            "Ngomba",                                                       0x1000},
{"jgo-CM",         "Ngomba_Cameroon",                                              0x1000},
{"lrc-IQ",         "Northern Luri_Iraq",                                           0x1000},
{"lrc-IR",         "Northern Luri_Iran",                                           0x1000},
{"nd",             "North Ndebele",                                                0x1000},
{"nd-ZW",          "North Ndebele_Zimbabwe",                                       0x1000},
{"no",             "Norwegian (Bokmal)",                                           0x0014},
{"nb",             "Norwegian (Bokmal)",                                           0x7C14},
{"nb-NO",          "Norwegian (Bokmal)_Norway",                                    0x0414},
{"nn",             "Norwegian (Nynorsk)",                                          0x7814},
{"nn-NO",          "Norwegian (Nynorsk)_Norway",                                   0x0814},
{"nb-SJ",          "Norwegian Bokm?l_Svalbard and Jan Mayen",                      0x1000},
{"nus",            "Nuer",                                                         0x1000},
{"nus-SD",         "Nuer_Sudan",                                                   0x1000},
{"nus-SS",         "Nuer_South Sudan",                                             0x1000},
{"nyn",            "Nyankole",                                                     0x1000},
{"nyn-UG",         "Nyankole_Uganda",                                              0x1000},
{"oc",             "Occitan",                                                      0x0082},
{"oc-FR",          "Occitan_France",                                               0x0482},
{"or",             "Odia",                                                         0x0048},
{"or-IN",          "Odia_India",                                                   0x0448},
{"om",             "Oromo",                                                        0x0072},
{"om-ET",          "Oromo_Ethiopia",                                               0x0472},
{"om-KE",          "Oromo_Kenya",                                                  0x1000},
{"os",             "Ossetian",                                                     0x1000},
{"os-GE",          "Ossetian_Cyrillic, Georgia",                                   0x1000},
{"os-RU",          "Ossetian_Cyrillic, Russia",                                    0x1000},
{"ps",             "Pashto",                                                       0x0063},
{"ps-AF",          "Pashto_Afghanistan",                                           0x0463},
{"ps-PK",          "Pashto_Pakistan",                                              0x1000},
{"fa",             "Persian",                                                      0x0029},
{"fa-AF",          "Persian_Afghanistan",                                          0x1000},
{"fa-IR",          "Persian_Iran",                                                 0x0429},
{"pl",             "Polish",                                                       0x0015},
{"pl-PL",          "Polish_Poland",                                                0x0415},
{"pt",             "Portuguese",                                                   0x0016},
{"pt-AO",          "Portuguese_Angola",                                            0x1000},
{"pt-BR",          "Portuguese_Brazil",                                            0x0416},
{"pt-CV",          "Portuguese_Cabo Verde",                                        0x1000},
{"pt-GQ",          "Portuguese_Equatorial Guinea",                                 0x1000},
{"pt-GW",          "Portuguese_Guinea-Bissau",                                     0x1000},
{"pt-LU",          "Portuguese_Luxembourg",                                        0x1000},
{"pt-MO",          "Portuguese_Macao SAR",                                         0x1000},
{"pt-MZ",          "Portuguese_Mozambique",                                        0x1000},
{"pt-PT",          "Portuguese_Portugal",                                          0x0816},
{"pt-ST",          "Portuguese_S?o Tom? and Pr?ncipe",                             0x1000},
{"pt-CH",          "Portuguese_Switzerland",                                       0x1000},
{"pt-TL",          "Portuguese_Timor-Leste",                                       0x1000},
{"prg-001",        "Prussian",                                                     0x1000},
{"qps-ploca",      "Pseudo Language_Pseudo locale",                                0x05FE},
{"qps-ploc",       "Pseudo Language_Pseudo locale",                                0x0501},
{"qps-plocm",      "Pseudo Language_Pseudo locale",                                0x09FF},
{"pa",             "Punjabi",                                                      0x0046},
{"pa-Arab",        "Punjabi",                                                      0x7C46},
{"pa-IN",          "Punjabi_India",                                                0x0446},
{"pa-Arab-PK",     "Punjabi_Islamic Republic of Pakistan",                         0x0846},
{"quz",            "Quechua",                                                      0x006B},
{"quz-BO",         "Quechua_Bolivia",                                              0x046B},
{"quz-EC",         "Quechua_Ecuador",                                              0x086B},
{"quz-PE",         "Quechua_Peru",                                                 0x0C6B},
{"ksh",            "Ripuarian",                                                    0x1000},
{"ksh-DE",         "Ripuarian_Germany",                                            0x1000},
{"ro",             "Romanian",                                                     0x0018},
{"ro-MD",          "Romanian_Moldova",                                             0x0818},
{"ro-RO",          "Romanian_Romania",                                             0x0418},
{"rm",             "Romansh",                                                      0x0017},
{"rm-CH",          "Romansh_Switzerland",                                          0x0417},
{"rof",            "Rombo",                                                        0x1000},
{"rof-TZ",         "Rombo_Tanzania",                                               0x1000},
{"rn",             "Rundi",                                                        0x1000},
{"rn-BI",          "Rundi_Burundi",                                                0x1000},
{"ru",             "Russian",                                                      0x0019},
{"ru-BY",          "Russian_Belarus",                                              0x1000},
{"ru-KZ",          "Russian_Kazakhstan",                                           0x1000},
{"ru-KG",          "Russian_Kyrgyzstan",                                           0x1000},
{"ru-MD",          "Russian_Moldova",                                              0x0819},
{"ru-RU",          "Russian_Russia",                                               0x0419},
{"ru-UA",          "Russian_Ukraine",                                              0x1000},
{"rwk",            "Rwa",                                                          0x1000},
{"rwk-TZ",         "Rwa_Tanzania",                                                 0x1000},
{"ssy",            "Saho",                                                         0x1000},
{"ssy-ER",         "Saho_Eritrea",                                                 0x1000},
{"sah",            "Sakha",                                                        0x0085},
{"sah-RU",         "Sakha_Russia",                                                 0x0485},
{"saq",            "Samburu",                                                      0x1000},
{"saq-KE",         "Samburu_Kenya",                                                0x1000},
{"smn",            "Sami (Inari)",                                                 0x703B},
{"smn-FI",         "Sami (Inari)_Finland",                                         0x243B},
{"smj",            "Sami (Lule)",                                                  0x7C3B},
{"smj-NO",         "Sami (Lule)_Norway",                                           0x103B},
{"smj-SE",         "Sami (Lule)_Sweden",                                           0x143B},
{"se",             "Sami (Northern)",                                              0x003B},
{"se-FI",          "Sami (Northern)_Finland",                                      0x0C3B},
{"se-NO",          "Sami (Northern)_Norway",                                       0x043B},
{"se-SE",          "Sami (Northern)_Sweden",                                       0x083B},
{"sms",            "Sami (Skolt)",                                                 0x743B},
{"sms-FI",         "Sami (Skolt)_Finland",                                         0x203B},
{"sma",            "Sami (Southern)",                                              0x783B},
{"sma-NO",         "Sami (Southern)_Norway",                                       0x183B},
{"sma-SE",         "Sami (Southern)_Sweden",                                       0x1C3B},
{"sg",             "Sango",                                                        0x1000},
{"sg-CF",          "Sango_Central African Republic",                               0x1000},
{"sbp",            "Sangu",                                                        0x1000},
{"sbp-TZ",         "Sangu_Tanzania",                                               0x1000},
{"sa",             "Sanskrit",                                                     0x004F},
{"sa-IN",          "Sanskrit_India",                                               0x044F},
{"gd",             "Scottish Gaelic",                                              0x0091},
{"gd-GB",          "Scottish Gaelic_United Kingdom",                               0x0491},
{"seh",            "Sena",                                                         0x1000},
{"seh-MZ",         "Sena_Mozambique",                                              0x1000},
{"sr-Cyrl",        "Serbian (Cyrillic)",                                           0x6C1A},
{"sr-Cyrl-BA",     "Serbian (Cyrillic)_Bosnia and Herzegovina",                    0x1C1A},
{"sr-Cyrl-ME",     "Serbian (Cyrillic)_Montenegro",                                0x301A},
{"sr-Cyrl-RS",     "Serbian (Cyrillic)_Serbia",                                    0x281A},
{"sr-Cyrl-CS",     "Serbian (Cyrillic)_Serbia and Montenegro (Former)",            0x0C1A},
{"sr-Latn",        "Serbian (Latin)",                                              0x701A},
{"sr",             "Serbian (Latin)",                                              0x7C1A},
{"sr-Latn-BA",     "Serbian (Latin)_Bosnia and Herzegovina",                       0x181A},
{"sr-Latn-ME",     "Serbian (Latin)_Montenegro",                                   0x2c1A},
{"sr-Latn-RS",     "Serbian (Latin)_Serbia",                                       0x241A},
{"sr-Latn-CS",     "Serbian (Latin)_Serbia and Montenegro (Former)",               0x081A},
{"nso",            "Sesotho sa Leboa",                                             0x006C},
{"nso-ZA",         "Sesotho sa Leboa_South Africa",                                0x046C},
{"tn",             "Setswana",                                                     0x0032},
{"tn-BW",          "Setswana_Botswana",                                            0x0832},
{"tn-ZA",          "Setswana_South Africa",                                        0x0432},
{"ksb",            "Shambala",                                                     0x1000},
{"ksb-TZ",         "Shambala_Tanzania",                                            0x1000},
{"sn",             "Shona",                                                        0x1000},
{"sn-Latn",        "Shona_Latin",                                                  0x1000},
{"sn-Latn-ZW",     "Shona_Zimbabwe",                                               0x1000},
{"sd",             "Sindhi",                                                       0x0059},
{"sd-Arab",        "Sindhi",                                                       0x7C59},
{"sd-Arab-PK",     "Sindhi_Islamic Republic of Pakistan",                          0x0859},
{"si",             "Sinhala",                                                      0x005B},
{"si-LK",          "Sinhala_Sri Lanka",                                            0x045B},
{"sk",             "Slovak",                                                       0x001B},
{"sk-SK",          "Slovak_Slovakia",                                              0x041B},
{"sl",             "Slovenian",                                                    0x0024},
{"sl-SI",          "Slovenian_Slovenia",                                           0x0424},
{"xog",            "Soga",                                                         0x1000},
{"xog-UG",         "Soga_Uganda",                                                  0x1000},
{"so",             "Somali",                                                       0x0077},
{"so-DJ",          "Somali_Djibouti",                                              0x1000},
{"so-ET",          "Somali_Ethiopia",                                              0x1000},
{"so-KE",          "Somali_Kenya",                                                 0x1000},
{"so-SO",          "Somali_Somalia",                                               0x0477},
{"st",             "Sotho",                                                        0x0030},
{"st-ZA",          "Sotho_South Africa",                                           0x0430},
{"nr",             "South Ndebele",                                                0x1000},
{"nr-ZA",          "South Ndebele_South Africa",                                   0x1000},
{"st-LS",          "Southern Sotho_Lesotho",                                       0x1000},
{"es",             "Spanish",                                                      0x000A},
{"es-AR",          "Spanish_Argentina",                                            0x2C0A},
{"es-BZ",          "Spanish_Belize",                                               0x1000},
{"es-VE",          "Spanish_Bolivarian Republic of Venezuela",                     0x200A},
{"es-BO",          "Spanish_Bolivia",                                              0x400A},
{"es-BR",          "Spanish_Brazil",                                               0x1000},
{"es-CL",          "Spanish_Chile",                                                0x340A},
{"es-CO",          "Spanish_Colombia",                                             0x240A},
{"es-CR",          "Spanish_Costa Rica",                                           0x140A},
{"es-CU",          "Spanish_Cuba",                                                 0x5c0A},
{"es-DO",          "Spanish_Dominican Republic",                                   0x1c0A},
{"es-EC",          "Spanish_Ecuador",                                              0x300A},
{"es-SV",          "Spanish_El Salvador",                                          0x440A},
{"es-GQ",          "Spanish_Equatorial Guinea",                                    0x1000},
{"es-GT",          "Spanish_Guatemala",                                            0x100A},
{"es-HN",          "Spanish_Honduras",                                             0x480A},
{"es-419",         "Spanish_Latin America",                                        0x580A},
{"es-MX",          "Spanish_Mexico",                                               0x080A},
{"es-NI",          "Spanish_Nicaragua",                                            0x4C0A},
{"es-PA",          "Spanish_Panama",                                               0x180A},
{"es-PY",          "Spanish_Paraguay",                                             0x3C0A},
{"es-PE",          "Spanish_Peru",                                                 0x280A},
{"es-PH",          "Spanish_Philippines",                                          0x1000},
{"es-PR",          "Spanish_Puerto Rico",                                          0x500A},
{"es-ES_tradnl",   "Spanish_Spain",                                                0x040A},
{"es-ES",          "Spanish_Spain",                                                0x0c0A},
{"es-US",          "Spanish_United States",                                        0x540A},
{"es-UY",          "Spanish_Uruguay",                                              0x380A},
{"zgh",            "Standard Moroccan Tamazight",                                  0x1000},
{"zgh-Tfng-MA",    "Standard Moroccan Tamazight_Morocco",                          0x1000},
{"zgh-Tfng",       "Standard Moroccan Tamazight_Tifinagh",                         0x1000},
{"ss",             "Swati",                                                        0x1000},
{"ss-ZA",          "Swati_South Africa",                                           0x1000},
{"ss-SZ",          "Swati_Swaziland",                                              0x1000},
{"sv",             "Swedish",                                                      0x001D},
{"sv-AX",          "Swedish_?land Islands",                                        0x1000},
{"sv-FI",          "Swedish_Finland",                                              0x081D},
{"sv-SE",          "Swedish_Sweden",                                               0x041D},
{"syr",            "Syriac",                                                       0x005A},
{"syr-SY",         "Syriac_Syria",                                                 0x045A},
{"shi",            "Tachelhit",                                                    0x1000},
{"shi-Tfng",       "Tachelhit_Tifinagh",                                           0x1000},
{"shi-Tfng-MA",    "Tachelhit_Tifinagh, Morocco",                                  0x1000},
{"shi-Latn",       "Tachelhit (Latin)",                                            0x1000},
{"shi-Latn-MA",    "Tachelhit (Latin)_Morocco",                                    0x1000},
{"dav",            "Taita",                                                        0x1000},
{"dav-KE",         "Taita_Kenya",                                                  0x1000},
{"tg",             "Tajik (Cyrillic)",                                             0x0028},
{"tg-Cyrl",        "Tajik (Cyrillic)",                                             0x7C28},
{"tg-Cyrl-TJ",     "Tajik (Cyrillic)_Tajikistan",                                  0x0428},
{"tzm",            "Tamazight (Latin)",                                            0x005F},
{"tzm-Latn",       "Tamazight (Latin)",                                            0x7C5F},
{"tzm-Latn-DZ",    "Tamazight (Latin)_Algeria",                                    0x085F},
{"ta",             "Tamil",                                                        0x0049},
{"ta-IN",          "Tamil_India",                                                  0x0449},
{"ta-MY",          "Tamil_Malaysia",                                               0x1000},
{"ta-SG",          "Tamil_Singapore",                                              0x1000},
{"ta-LK",          "Tamil_Sri Lanka",                                              0x0849},
{"twq",            "Tasawaq",                                                      0x1000},
{"twq-NE",         "Tasawaq_Niger",                                                0x1000},
{"tt",             "Tatar",                                                        0x0044},
{"tt-RU",          "Tatar_Russia",                                                 0x0444},
{"te",             "Telugu",                                                       0x004A},
{"te-IN",          "Telugu_India",                                                 0x044A},
{"teo",            "Teso",                                                         0x1000},
{"teo-KE",         "Teso_Kenya",                                                   0x1000},
{"teo-UG",         "Teso_Uganda",                                                  0x1000},
{"th",             "Thai",                                                         0x001E},
{"th-TH",          "Thai_Thailand",                                                0x041E},
{"bo",             "Tibetan",                                                      0x0051},
{"bo-IN",          "Tibetan_India",                                                0x1000},
{"bo-CN",          "Tibetan_People's Republic of China",                           0x0451},
{"tig",            "Tigre",                                                        0x1000},
{"tig-ER",         "Tigre_Eritrea",                                                0x1000},
{"ti",             "Tigrinya",                                                     0x0073},
{"ti-ER",          "Tigrinya_Eritrea",                                             0x0873},
{"ti-ET",          "Tigrinya_Ethiopia",                                            0x0473},
{"to",             "Tongan",                                                       0x1000},
{"to-TO",          "Tongan_Tonga",                                                 0x1000},
{"ts",             "Tsonga",                                                       0x0031},
{"ts-ZA",          "Tsonga_South Africa",                                          0x0431},
{"tr",             "Turkish",                                                      0x001F},
{"tr-CY",          "Turkish_Cyprus",                                               0x1000},
{"tr-TR",          "Turkish_Turkey",                                               0x041F},
{"tk",             "Turkmen",                                                      0x0042},
{"tk-TM",          "Turkmen_Turkmenistan",                                         0x0442},
{"uk",             "Ukrainian",                                                    0x0022},
{"uk-UA",          "Ukrainian_Ukraine",                                            0x0422},
{"hsb",            "Upper Sorbian",                                                0x002E},
{"hsb-DE",         "Upper Sorbian_Germany",                                        0x042E},
{"ur",             "Urdu",                                                         0x0020},
{"ur-IN",          "Urdu_India",                                                   0x0820},
{"ur-PK",          "Urdu_Islamic Republic of Pakistan",                            0x0420},
{"ug",             "Uyghur",                                                       0x0080},
{"ug-CN",          "Uyghur_People's Republic of China",                            0x0480},
{"uz-Arab",        "Uzbek_Perso-Arabic",                                           0x1000},
{"uz-Arab-AF",     "Uzbek_Perso-Arabic, Afghanistan",                              0x1000},
{"uz-Cyrl",        "Uzbek (Cyrillic)",                                             0x7843},
{"uz-Cyrl-UZ",     "Uzbek (Cyrillic)_Uzbekistan",                                  0x0843},
{"uz",             "Uzbek (Latin)",                                                0x0043},
{"uz-Latn",        "Uzbek (Latin)",                                                0x7C43},
{"uz-Latn-UZ",     "Uzbek (Latin)_Uzbekistan",                                     0x0443},
{"vai",            "Vai",                                                          0x1000},
{"vai-Vaii",       "Vai",                                                          0x1000},
{"vai-Vaii-LR",    "Vai_Liberia",                                                  0x1000},
{"vai-Latn-LR",    "Vai (Latin)_Liberia",                                          0x1000},
{"vai-Latn",       "Vai (Latin)",                                                  0x1000},
{"ca-ES-valencia", "Valencian_Spain",                                              0x0803},
{"ve",             "Venda",                                                        0x0033},
{"ve-ZA",          "Venda_South Africa",                                           0x0433},
{"vi",             "Vietnamese",                                                   0x002A},
{"vi-VN",          "Vietnamese_Vietnam",                                           0x042A},
{"vo",             "Volap?k",                                                      0x1000},
{"vo-001",         "Volap?k_World",                                                0x1000},
{"vun",            "Vunjo",                                                        0x1000},
{"vun-TZ",         "Vunjo_Tanzania",                                               0x1000},
{"wae",            "Walser",                                                       0x1000},
{"wae-CH",         "Walser_Switzerland",                                           0x1000},
{"cy",             "Welsh",                                                        0x0052},
{"cy-GB",          "Welsh_United Kingdom",                                         0x0452},
{"wal",            "Wolaytta",                                                     0x1000},
{"wal-ET",         "Wolaytta_Ethiopia",                                            0x1000},
{"wo",             "Wolof",                                                        0x0088},
{"wo-SN",          "Wolof_Senegal",                                                0x0488},
{"xh",             "Xhosa",                                                        0x0034},
{"xh-ZA",          "Xhosa_South Africa",                                           0x0434},
{"yav",            "Yangben",                                                      0x1000},
{"yav-CM",         "Yangben_Cameroon",                                             0x1000},
{"ii",             "Yi",                                                           0x0078},
{"ii-CN",          "Yi_People's Republic of China",                                0x0478},
{"yo",             "Yoruba",                                                       0x006A},
{"yo-BJ",          "Yoruba_Benin",                                                 0x1000},
{"yo-NG",          "Yoruba_Nigeria",                                               0x046A},
{"dje",            "Zarma",                                                        0x1000},
{"dje-NE",         "Zarma_Niger",                                                  0x1000},
{"zu",             "Zulu",                                                         0x0035},
{"zu-ZA",          "Zulu_South Africa",                                            0x0435},
};

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(info_tag, A_In_z)
A_At(tag, A_In_z)
A_Success(return)
#endif
static unsigned cmp_tags(const char info_tag[], const char tag[])
{
	unsigned j = 1;
	for (;;) {
		j++;
		if (info_tag[j] != tag[j]) {
			/* Ignore code page after a dot.  */
			if (!info_tag[j])
				return tag[j] == '.' ? j : 0;
			/* '-' and '_' are equivalent.  */
			if (info_tag[j] != '-' || tag[j] != '_')
				return 0;
		}
		else if (!tag[j])
			return j;
	}
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(tag, A_In_z)
A_Success(return)
#endif
static const struct lang_tag *find_tag(const char tag[])
{
	unsigned i = 0, k = 0;
	if (!tag[0] || !tag[1])
		return NULL;
	memcpy(&k, tag, 2);
	for (; i < sizeof(lang_tag_table)/sizeof(lang_tag_table[0]); i++) {
		const struct lang_tag *const info = &lang_tag_table[i];
		unsigned ik = 0;
		memcpy(&ik, info->tag, 2);
		if (k == ik) {
			const unsigned len = cmp_tags(info->tag, tag);
			if (len)
				return info;
		}
	}
	return NULL;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_Pre_writable_size(sz) A_Post_z A_Post_readable_size(return))
A_At(sz, A_In_range(>,0))
A_Post_satisfies(return <= sz)
A_Success(return)
#endif
static unsigned get_locale_info(const unsigned id, const LCTYPE cat, char buf[], const unsigned sz)
{
	const int ret = GetLocaleInfoA(MAKELCID(id, SORT_DEFAULT), cat, buf, (int)sz);
	return (ret <= 0 || (unsigned)ret > sz) ? 0 : (unsigned)ret;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_Pre_writable_size(sz) A_Post_z A_Post_readable_size(return + 1))
A_At(sz, A_In_range(>,0))
A_Post_satisfies(return < sz)
A_Success(return)
#endif
static unsigned lang_tag_to_locale_name(const struct lang_tag *const info,
	char buf[], const unsigned sz)
{
	const unsigned ret1 = get_locale_info(info->id, LOCALE_SENGLANGUAGE, buf, sz);
	if (ret1 && ret1 < sz) {
		const unsigned ret2 = get_locale_info(info->id, LOCALE_SENGCOUNTRY, buf + ret1, sz - ret1);
		if (ret2) {
			buf[ret1 - 1] = '_';
			return ret1 + ret2 - 1;
		}
	}
	return 0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(cp, A_In_z)
#endif
static int is_utf8_code_page(const char cp[])
{
	return
		(cp[0] == '6' &&
		 cp[1] == '5' &&
		 cp[2] == '0' &&
		 cp[3] == '0' &&
		 cp[4] == '1' &&
		 cp[5] == '\0') ||
		((cp[0] == 'c' || cp[0] == 'C') &&
		 (cp[1] == 'p' || cp[1] == 'P') &&
		  cp[2] == '6' &&
		  cp[3] == '5' &&
		  cp[4] == '0' &&
		  cp[5] == '0' &&
		  cp[6] == '1' &&
		  cp[7] == '\0') ||
		((cp[0] == 'u' || cp[0] == 'U') &&
		 (cp[1] == 't' || cp[1] == 'T') &&
		 (cp[2] == 'f' || cp[2] == 'F') &&
		 ((cp[3] == '8' && cp[4] == '\0') ||
		  (cp[3] == '-' && cp[4] == '8' && cp[5] == '\0')));
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Success(return)
A_At(return, A_Writable_elements(LOCALE_BUF_SIZE))
#endif
static char *get_locale_buf(const int cat)
{
	static char locale_bufs[6][LOCALE_BUF_SIZE];
	switch (cat) {
		case LC_ALL:      return locale_bufs[0];
		case LC_COLLATE:  return locale_bufs[1];
		case LC_CTYPE:    return locale_bufs[2];
		case LC_MONETARY: return locale_bufs[3];
		case LC_NUMERIC:  return locale_bufs[4];
		case LC_TIME:     return locale_bufs[5];
		default:          return NULL;
	}
}

A_Use_decl_annotations
char *locale_helper_add_utf8_cp(const int cat, const char locale[])
{
	char *const buf = get_locale_buf(cat);
	if (buf) {
		const char *name_end = strchr(locale, '.');
		if (!name_end)
			name_end = strchr(locale, '\0');
		/* Replace resulting code page.  */
		{
			const size_t name_len = (size_t)(name_end - locale);
			if (name_len > LOCALE_BUF_SIZE - sizeof(".UTF-8"))
				return NULL; /* should not happen */
			memcpy(buf, locale, name_len);
			memcpy(&buf[name_len], ".UTF-8", sizeof(".UTF-8"));
		}
	}
	return buf;
}

#define MAP_CHAR(c, def) \
	switch (c) { \
		C(' ');C('!');C('"');C('#');C('$');C('%');C('&');C('\''); \
		C('(');C(')');C('*');C('+');C(',');C('-');C('.');C('/'); \
		C('0');C('1');C('2');C('3');C('4');C('5');C('6');C('7');C('8');C('9'); \
		C(':');C(';');C('<');C('=');C('>');C('?');C('@'); \
		C('A');C('B');C('C');C('D');C('E');C('F');C('G');C('H');C('I');C('J');C('K');C('L');C('M'); \
		C('N');C('O');C('P');C('Q');C('R');C('S');C('T');C('U');C('V');C('W');C('X');C('Y');C('Z'); \
		C('[');C('\\');C(']');C('^');C('_');C('`'); \
		C('a');C('b');C('c');C('d');C('e');C('f');C('g');C('h');C('i');C('j');C('k');C('l');C('m'); \
		C('n');C('o');C('p');C('q');C('r');C('s');C('t');C('u');C('v');C('w');C('x');C('y');C('z'); \
		C('{');C('|');C('}');C('~'); \
		default: return def; \
	}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
static char is_wascii_char(const wchar_t c)
{
#define C(x) case L ## x: return x
	MAP_CHAR(c, L'\0')
#undef C
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(w, A_In_z)
A_At(b, A_Pre_writable_size(mx) A_Post_z)
A_Success(!return)
#endif
static int name_to_narrow(const wchar_t w[], char b[], const unsigned mx)
{
	unsigned i = 0;
	for (;; i++) {
		if (i == mx)
			return -2; /* Too small buffer.  */
		b[i] = is_wascii_char(w[i]);
		if (b[i] == '\0') {
			if (w[i] == L'\0')
				return 0;
			return -1; /* Non-ASCII character.  */
		}
	}
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(locale, A_In_z)
A_At(buf, A_Pre_writable_size(buf_sz))
A_Ret_z
A_Success(return)
#endif
static char *set_locale_helper_(const int cat, const char locale[],
	char buf[], const unsigned buf_sz)
{
	const char *loc = locale;
	unsigned len = 0;
	size_t cp_len = 0;
	const struct lang_tag *info;
	char *ret;

	const char *const cp = strchr(locale, '.');
	const int is_utf8 = cp && is_utf8_code_page(cp + 1);

	if (is_utf8) {
		/* Trim utf8 code page.  */
		const size_t loc_len = (size_t)(cp - locale);
		if (locale != buf) {
			if (loc_len >= buf_sz)
				return NULL;
			memcpy(buf, locale, loc_len*sizeof(*locale));
		}
		buf[loc_len] = '\0';
		loc = buf;
	}

	if ((ret = setlocale(cat, loc)) != NULL)
		goto ok;

	info = find_tag(loc);
	if (!info)
		return NULL;

	loc = info->lang_country;

	if (!is_utf8 && cp) {
		len = (unsigned)strlen(loc);
		if (len >= buf_sz)
			return NULL;

		/* Append non-utf8 code page.
		   Trim "cp" prefix of "cp1234".  */
		{
			const char *b = cp;
			if ((b[1] == 'c' || b[1] == 'C') &&
				(b[2] == 'p' || b[2] == 'P') &&
				('0' <= b[3] && b[3] <= '9'))
			{
				for (b += 4;; b++) {
					if (*b < '0' || '9' < *b)
						break;
				}
			}

			cp_len = (size_t)(b - cp);
			if (!*b) {
				cp_len -= 2; /* "cp" */
				b = cp + 2; /* "cp" */
			}
			else {
				cp_len += strlen(b);
				b = cp;
			}

			if (cp_len >= buf_sz - len)
				return NULL;

			memmove(buf + len + 1/*'.'*/, b + 1/*'.'*/, cp_len/*with '\0'*/);
			memcpy(buf, loc, len);
			buf[len] = '.';
		}

		loc = buf;
	}

	if ((ret = setlocale(cat, loc)) != NULL)
		goto ok;

	if (LOCALE_CUSTOM_UNSPECIFIED == info->id)
		return NULL;

	/* Get space in buf - move code page to the end of buf.  */
	if (cp_len)
		memmove(buf + buf_sz - cp_len, buf + len, cp_len);

	len = lang_tag_to_locale_name(info, buf, buf_sz - (unsigned)cp_len);
	if (!len)
		return NULL;

	if (cp_len) {
		/* Append non-utf8 code page.  */
		memmove(buf + len, buf + buf_sz - cp_len, cp_len);
		buf[len + cp_len] = '\0';
	}

	if ((ret = setlocale(cat, buf)) == NULL)
		return NULL;

ok:
	/* Replace standard locale codepage-dependent CRT Api with utf8-wrappers:
	   - if any LC_... category specifies a code set or
	   - locale category is LC_CTYPE (no code set - set system default) or
	   - locale category is LC_ALL - it affects LC_CTYPE.  */
	if (cp || LC_CTYPE == cat || LC_ALL == cat)
		localerpl_change(is_utf8);

	return localerpl_is_utf8() ? locale_helper_add_utf8_cp(cat, ret) : ret;
}

A_Use_decl_annotations
char *set_locale_helper(const int cat, const char locale[])
{
	char buf[LOCALE_BUF_SIZE];
	char *curr = set_locale_helper_(cat, locale, buf, sizeof(buf)/sizeof(buf[0]));
	return curr;
}

A_Use_decl_annotations
int wset_locale_helper(const int cat, const wchar_t locale[])
{
	char *curr;
	char buf[LOCALE_BUF_SIZE];
	if (name_to_narrow(locale, buf, sizeof(buf)/sizeof(buf[0])))
		return -1;
	curr = set_locale_helper_(cat, buf, buf, sizeof(buf)/sizeof(buf[0]));
	return curr ? 0 : -1;
}

static int set_lcat_from_env(int cat, const wchar_t name[], const char def[],
	struct set_locale_err *err/*NULL?,out*/)
{
	/* Note: use _wgetenv() because multibyte-character _environ array
	   if not (automatically) initialized yet.  Try to not initialize
	   it at all in case of UTF-8 encoding - we will use locale-wrapped
	   setlocale() */
	const wchar_t *const lc = _wgetenv(name);
	if (lc) {
		if (!wset_locale_helper(cat, lc))
			return 0;
		goto err;
	}
	if (def) {
		if (set_locale_helper(cat, def))
			return 0;
		goto err;
	}
	return 1;
err:
	if (err) {
		err->cat = name;
		err->lc = lc;
	}
	return -1;
}

A_Use_decl_annotations
int set_locale_from_env(const char def[]/*NULL?*/, struct set_locale_err *err/*NULL?,out*/)
{
	int ret;

	if (1 != (ret = set_lcat_from_env(LC_ALL, L"LC_ALL", NULL, err)))
		return ret;

	/* LANG is the default for all LC_...
	   If LANG is not defined - set the system default.
	   (If def is "", locale may be be set to e.g. Russian_Russia.1251).  */
	if ((ret = set_lcat_from_env(LC_ALL, L"LANG", def, err)) < 0)
		return ret;

	/* Set all locale categories from the environment:
	   specific LC_... take precedence over LANG.  */
	if ((ret = set_lcat_from_env(LC_CTYPE,    L"LC_CTYPE",    NULL, err)) < 0)
		return ret;
	if ((ret = set_lcat_from_env(LC_COLLATE,  L"LC_COLLATE",  NULL, err)) < 0)
		return ret;
	if ((ret = set_lcat_from_env(LC_MONETARY, L"LC_MONETARY", NULL, err)) < 0)
		return ret;
	if ((ret = set_lcat_from_env(LC_NUMERIC,  L"LC_NUMERIC",  NULL, err)) < 0)
		return ret;
	if ((ret = set_lcat_from_env(LC_TIME,     L"LC_TIME",     NULL, err)) < 0)
		return ret;

	return 0;
}
