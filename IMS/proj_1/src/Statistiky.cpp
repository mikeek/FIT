/* 
 * @file Statistiky.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida statistiky, projekt do predmetu IMS 2014
 */

#include "Statistiky.h"

#define HLINE "-----------------------------------------------\n"
#define PLINE "+++++++++++++++++++++++++++++++++++++++++++++++\n"

/**
 * \brief Podil tydne/vikendu
 */
double Statistiky::TyzdenVsVikend(int celkom, int tyzden)
{
	return tyzden * 100 / celkom;
}

/**
 * \brief Pridanie statistiky o zasielke 
 */
void Statistiky::PridajStatistiku(Zasilka *z)
{
	statistikaZasielok.push_back(Time - z->Vstup());
}

/**
 * \brief Začátek nového pracovního dne
 */
void Statistiky::NovyPracovniDen()
{
	/* vytviření statistik / day, čítač dnů, posun ve vektoru dnů... */
}

/**
 * \brief Naformátování času (v minutách)
 */
std::string Statistiky::Cas(double c)
{
	std::stringstream ss;
	if (c < 60.0) {
		ss << c << " minut";
	} else if (c < (24.0 * 60.0)) {
		ss << (c / 60.0) << " hodin";
	} else {
		ss << (c / (24.0 * 60.0)) << " dnu";
	}

	return ss.str();
}

/**
 * \brief Vypis statistik o elementoch 
 */
void Statistiky::ZobrazStatistiky()
{
	STATS(0) << HLINE;
	STATS(0) << "Pocet uplynulych dnu: " << (PDOBA->AktualniDen() + 1) << "\n"; // pdoba cisluje od 0, proto +1
	STATS(0) << "Pocet odeslanych zasilek: " << odeslanych << "\n";
	STATS(0) << "Pocet neuspesnych pokusu o vyzvednuti zasilky: " << neuspechu << " / " << (neuspechu + uspechu) << "\n";
	STATS(0) << "Pouzita metoda pro nakladani zasilek: " << JMENA_METOD[METODA_NAKL] << "\n";
	STATS(0) << "Pouzita metoda pro planovani trasy: " << JMENA_METOD[METODA_PREVOZ] << "\n";
	STATS(0) << HLINE;

	double priemerNaZasielky{0.0};
	double pocet = static_cast<double> (statistikaZasielok.size());

	/* Statistiky o celkovom case zasielky v systeme */
	for (auto s : statistikaZasielok) {
		priemerNaZasielky += s;
	}

	/* Vypočítej průměrný čas v systému */
	double casVSys = (pocet > 0.0) ? (priemerNaZasielky / pocet) : 0.0;

	STATS(1) << HLINE;
	STATS(1) << "Priemerny cas zasielky v systeme: " << Cas(casVSys) << " (" << casVSys << " minut)\n";
	STATS(1) << HLINE;

	int priemerNaAuta{0}, priemerNaAutaVikend{0}, vAutech{0};
	int priemerBalikov{0}, priemerBalikovVikend{0};
	double priemerCasCesta{0.0};
	double priemerCasNRampa{0.0};
	double priemerCasVRampa{0.0};
	double priemerCasCestaVikend{0.0};
	double priemerCasNRampaVikend{0.0};
	double priemerCasVRampaVikend{0.0};

	/* Statistiky o aute */
	//	for (int i = 0; i < AUT; i++) {
	for (auto A : AUTA) {
		STATS(2) << "++++++++++++++++++++++++++\n";
		STATS(2) << "Statistiky o aute cislo: " << A->Id() << "\n";
		STATS(2) << "----------SUHRN-----------\n";
		STATS(2) << "Prepravilo: " << A->Prepravene(VSE) << " zasielok\n";
		STATS(2) << "Najazdilo: " << A->Vzdialenost(VSE) << " km\n";
		STATS(2) << "Stravilo na ceste: " << A->NaCeste(VSE) << " minut\n";
		STATS(2) << "Stravilo na nakladacej rampe: " << A->NaNRampe(VSE) << " minut\n";
		STATS(2) << "Stravilo na vykladacej rampe: " << A->NaVRampe(VSE) << " minut\n";
		STATS(2) << "Stravilo na celkovo na rampach: " << A->NaNRampe(VSE) + A->NaVRampe(VSE) << " minut\n";
		STATS(2) << "Aktualne je v nem " << A->Zasilek() << " zasilek\n";
		STATS(2) << "-----------TYZDEN---------\n";
		STATS(2) << "Prepravilo: " << A->Prepravene(TYDEN) << " zasielok\n";
		STATS(2) << "Najazdilo: " << A->Vzdialenost(TYDEN) << " km\n";
		STATS(2) << "Stravilo na ceste: " << A->NaCeste(TYDEN) << " minut\n";
		STATS(2) << "Stravilo na nakladacej rampe: " << A->NaNRampe(TYDEN) << " minut\n";
		STATS(2) << "Stravilo na vykladacej rampe: " << A->NaVRampe(TYDEN) << " minut\n";
		STATS(2) << "Stravilo na celkovo na rampach: " << A->NaNRampe(TYDEN) + A->NaVRampe(TYDEN) << " minut\n";
		STATS(2) << "-----------VIKEND---------\n";
		STATS(2) << "Prepravilo: " << A->Prepravene(VIKEND) << " zasielok\n";
		STATS(2) << "Najazdilo: " << A->Vzdialenost(VIKEND) << " km\n";
		STATS(2) << "Stravilo na ceste: " << A->NaCeste(VIKEND) << " minut\n";
		STATS(2) << "Stravilo na nakladacej rampe: " << A->NaNRampe(VIKEND) << " minut\n";
		STATS(2) << "Stravilo na vykladacej rampe: " << A->NaVRampe(VIKEND) << " minut\n";
		STATS(2) << "Stravilo na celkovo na rampach: " << A->NaNRampe(VIKEND) + A->NaVRampe(VIKEND) << " minut\n";
		STATS(2) << "\n";

		priemerNaAuta += A->Vzdialenost(VSE);
		priemerCasCesta += A->NaCeste(VSE);
		priemerCasNRampa += A->NaNRampe(VSE);
		priemerCasVRampa += A->NaVRampe(VSE);
		priemerBalikov += A->Prepravene(VSE);

		priemerNaAutaVikend += A->Vzdialenost(VIKEND);
		priemerCasCestaVikend += A->NaCeste(VIKEND);
		priemerCasNRampaVikend += A->NaNRampe(VIKEND);
		priemerCasVRampaVikend += A->NaVRampe(VIKEND);
		priemerBalikovVikend += A->Prepravene(VIKEND);

		vAutech += A->Zasilek();
	}

	// přetypování na double kvůli správnému dělení
	double dAUT = (double) AUT;
	double tyden = TyzdenVsVikend(priemerNaAuta / dAUT, (priemerNaAuta - priemerNaAutaVikend) / dAUT);

	STATS(3) << PLINE;
	STATS(3) << "----------SUHRN----------\n";
	STATS(3) << "Auta priemerne prepravili " << priemerBalikov / dAUT << " balikov\n";
	STATS(3) << "Auta priemerne najazdili " << priemerNaAuta / dAUT << " km\n";
	STATS(3) << "Auta priemerne stravili na ceste " << priemerCasCesta / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na nakladacej rampe " << priemerCasNRampa / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na vykladacej rampe " << priemerCasVRampa / dAUT << " minut\n";
	STATS(3) << "V autech je jeste dohromady " << vAutech << " zasilek\n";
	STATS(3) << "-----------TYZDEN----[" << tyden << " %]\n";
	STATS(3) << "Auta priemerne prepravili " << (priemerBalikov - priemerBalikovVikend) / dAUT << " balikov\n";
	STATS(3) << "Auta priemerne najazdili " << (priemerNaAuta - priemerNaAutaVikend) / dAUT << " km\n";
	STATS(3) << "Auta priemerne stravili na ceste " << (priemerCasCesta - priemerCasCestaVikend) / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na nakladacej rampe " << (priemerCasNRampa - priemerCasNRampaVikend) / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na vykladacej rampe " << (priemerCasVRampa - priemerCasVRampaVikend) / dAUT << " minut\n";
	STATS(3) << "-----------VIKEND----[" << 100 - tyden << " %]\n";
	STATS(3) << "Auta priemerne prepravili " << priemerBalikovVikend / dAUT << " balikov\n";
	STATS(3) << "Auta priemerne najazdili " << priemerNaAutaVikend / dAUT << " km\n";
	STATS(3) << "Auta priemerne stravili na ceste " << priemerCasCestaVikend / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na nakladacej rampe " << priemerCasNRampaVikend / dAUT << " minut\n";
	STATS(3) << "Auta priemerne stravili na vykladacej rampe " << priemerCasVRampaVikend / dAUT << " minut\n";
	STATS(3) << HLINE;
}

/**
 * \brief Vypis statistik z kniznice SIMLIB
 */
void Statistiky::SimlibStatistky()
{

	/* Zobrazenie statistik o jednotlivych pobockach */
	for (auto p : POBOCKY) {
		p->Prepazky().Output();
		p->KNalozeni().Output();
		p->VRampa().Output();
		p->NRampa().Output();
	}
}
