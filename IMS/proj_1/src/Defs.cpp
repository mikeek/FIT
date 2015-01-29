/* 
 * @file Defs.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Soubor s definicemi konstant pro projekt do predmetu IMS 2014
 */

#include "Defs.h"

/* Povolení debug/stats výpisů */
bool DEBUG_EN{false};
bool STATS_EN{false};
std::ostringstream nullstream;

/**< Pracovni doba, čítač dnů atp. */	
const double T_VYRIZENI{3.0};		/**< Doba zpracování požadavku zákazníka (minuty) */
const double T_VYLOZENI{0.25};		/**< Doba vyložení zásilky (minuty) */
const double T_NALOZENI{0.25};		/**< Doba naložení zásilky (minuty) */
const double TIMEOUT_NALOZENI{3};	/**< Doba potřebná pro naložení 1 balíku do auta */
const double T_POBOCKA_ZPRAC{2};	/**< Zpracovani baliku pobočkou */
const double T_CENTRALA_ZPRAC{1};	/**< Zpracování balíku centrálou */
const int POBOCEK{10};				/**< Počet poboček */
const int AUT{10};					/**< Počet aut */
const int C_POBOCKY{100000};			/**< Kapacita pobočky pro odesílané balíky */
const int C_AUTA{400};				/**< Kapacita auta */
const int P_OD{6};					/**< Začátek pracovní doby (hodiny) */
const int P_DO{20};					/**< Konec pracovní doby   (hodiny) */
const int SIM_START{P_OD*60};		/**< Začátek simulace */
const int SIM_END{44640};			/**< Konec simulace (mesic) */
const int METODA_NAKL{POPORADE};	/**< Metoda nakládání zásilek na auto */
//const int METODA_NAKL{CETNOST};		/**< Metoda nakládání zásilek na auto */
//const int METODA_NAKL{NEJBLIZSI};	/**< Metoda nakládání zásilek na auto */
//const int METODA_NAKL{NEJSTARSI};	/**< Metoda nakládání zásilek na auto */
const int METODA_PREVOZ{POPORADE};	/**< Metoda výběru dalšího cíle při transportu */
const int NRAMP{1};					/**< Počet nakládacích ramp na každé pobočce */
const int VRAMP{1};					/**< Počet vykládacích ramp na každé pobočce */

Statistiky *hlavneStat;				/**< Statistiky simulace */
PDoba *PDOBA;						/**< Hlídač pracovní doby */

std::vector<Pobocka *> POBOCKY{};	/**< Seznam všech poboček */
std::vector<Auto *> AUTA{};			/**< Seznam všech aut */

const std::vector<int> CENTRALY{1};	/**< ID Centralnich pobocek */
const std::vector<int> ZAMESTNANCU{ /**< Počet zaměstnanců na každé pobočce */
	/*	PRA	BRN	OVA	HKR	SLU	LIB	OLO	ZLN	PLZ	PCE */
		5,	5,	5,	5,	5,	5,	5,	5,	5,	5
};

const std::vector<double> T_ZAKAZNIK{ /**< střední hodnota mezi příchody zákazníků na pobočky */
	/*	PRA	BRN	OVA	HKR	SLU	LIB	OLO	ZLN	PLZ	PCE */
//		0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5
//	    1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0
};

const std::vector<int> TRIDICU{	/**< Počet třídících strojů na jednotlivých pobočkách */
	/*	PRA	BRN	OVA	HKR	SLU	LIB	OLO	ZLN	PLZ	PCE */
		5,	5,	5,	5,	5,	5,	5,	5,	5,	5
};

const std::vector<std::string> JMENA_METOD{ /**< Jmena metod pro nakládání */
	"POPORADE", "CETNOST", "NEJBLIZSI", "NEJSTARSI"
};

const std::vector<std::string> JMENA_POBOCEK{  /**< Názvy poboček */
	"Praha",	"Brno",    "Ostrava", "Hradec Kr.", "Slusovice",
	"Liberec",	"Olomouc", "Zlin",    "Plzen",      "Pardubice"
};

const std::vector<std::vector<int>> VZDALENOST{ /**< Doba přejezdu mezi pobočkami (minuty) */
	/*  Z | do ->	PRA		BRN		OVA		HKR		SLU		LIB		OLO		ZLN		PLZ		PCE */
	/* Praha	*/ {0,		118,	211,	77,		187,	71,		159,	171,	64,		81	},
	/* Brno		*/ {118,	0,		106,	134,	82,		176,	54,		66,		171,	123	},
	/* Ostrava	*/ {211,	106,	0,		171,	100,	253,	68,		101,	264,	171	},
	/* Hradec K.*/ {77,		134,	171,	0,		179,	89,		115,	165,	130,	25	},
	/* Slušovice*/ {187,	82,		100,	179,	0,		242,	73,		17,		238,	176	},
	/* Liberec	*/ {71,		176,	253,	89,		242,	0,		197,	229,	128,	106	},
	/* Olomouc	*/ {159,	54,		68,		115,	73,		197,	0,		60,		211,	112	},
	/* Zlin		*/ {171,	66,		101,	165,	17,		229,	60,		0,		224,	163	},
	/* Plzen	*/ {64,		171,	264,	130,	238,	128,	211,	224,	0,		135	},
	/* Pardub.	*/ {81,		123,	171,	25,		176,	106,	112,	163,	135,	0	}
};

const std::vector<std::vector<int>> VZDALENOST_KM{ /**< Vzdálenosti mezi pobočkami (km) */
	/*  Z | do ->	PRA		BRN		OVA		HKR		SLU		LIB		OLO		ZLN		PLZ		PCE */
	/* Praha	*/ {0,		206,	375,	119,	313,	111,	281,	300,	92,		125	},
	/* Brno		*/ {206,	0,		174,	144,	115,	309,	80,		99,		296,	140	},
	/* Ostrava	*/ {375,	174,	0,		241,	111,	341,	98,		130,	465,	239	},
	/* Hradec K.*/ {119,	144,	241,	0,		220,	102,	141,	208,	210,	26	},
	/* Slušovice*/ {313,	115,	111,	220,	0,		416,	76,		14,		402,	217	},
	/* Liberec	*/ {111,	309,	341,	102,	416,	0,		240,	403,	206,	125	},
	/* Olomouc	*/ {281,	80,		98,		141,	76,		240,	0,		65,		371,	137	},
	/* Zlin		*/ {300,	99,		130,	208,	14,		403,	65,		0,		390,	204	},
	/* Plzen	*/ {92,		296,	465,	210,	402,	206,	371,	390,	0,		217	},
	/* Pardub.	*/ {125,	140,	239,	26,		217,	125,	137,	204,	217,	0	}
};
