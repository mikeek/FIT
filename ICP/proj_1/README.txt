Projektu do předmětu ICP (bludiště)
Autoři: Lukáš Huták (xhutak01), Michal Kozubík (xkozub03)

Zadání
-------

Cílem projektu bylo vytvořit jednoduchou 2D hru (bludiště) pro více hráčů v 
programovacím jazyce C++. 

Výsledkem jsou 3 spustitelné soubory:
    bludiste2014:        Klient s grafickým uživatelským rozhraním 
                         implementovaným pomocí toolkitu Qt
    
    bludiste2014-cli:    Klient s textovým rozhraním implemetovaným pomocí 
                         knihovny ncurses
    
    bludiste2014-server: Server obsahující herní logiku

    
Komunikace
-----------

Komunikace probíhá přes síťové rozhraní (pomocí knihovny boost::asio).
Pro tento účel jsme vymysleli komunikační protokol sdílený mezi několika týmy 
(Marek Hurta+Zuzana Lietavcová, Vladimír Čillo+Vítězslav Kříž, Lukáš Horký).
Jedná se o jednoduchý protokol založený na číselných kódech, který má v sobě
i prostor pro implementaci různých rozšíření, aniž by narušil chod programů,
které by jej nepodporovaly.

Sdílení protokolu samozřejmě předcházel dotaz na fóru, jestli je to povoleno.


Klientská aplikace
-------------------

Komunikace se serverem je od výstupu (textového či grafického) oddělena v 
samostatné třídě.

Pro textový výstup jsme zvolili knihovnu ncurses, jelikož umožňuje snadné
zobrazování barev a překreslování herního okna. Samotné ovládání hry je možné 
buďto zadáváním textových příkazů nebo kurzorovými šipkami.

Grafické uživatelské rozhraní je implementováno pomocí toolkitu Qt 5.2.1.
Pohyb v menu je intuitivní a pro ovládání hry je na výběr mezi ručním zadáváním
textových příkazů a klikáním myší na příslušná ovládací tlačítka.


Serverová aplikace
-------------------

Server obsahuje logiku celé hry (klient je pouze zobrazovač). Jako parametr
při spuštění je možné zadat port, na kterém má naslouchat. Ve výchozím stavu
používá port 4568.
Jednotlivým  klientům při vstupu do hry (nové nebo rozehrané) odešle podobu 
mapy a rozmístění jednotlivých hráčů a hlídačů. Dále už zasílá pouze změny, 
které se na mapě odehrály a informaci o (ne)úspěchu jejich žádostí.

Nabízené mapy jsou uložené ve složce maps. V ní je stejnojmenný soubor
obsahující názvy všech dostupných map. Každá z nich je reprezentována 
samostatným souborem v následujícím formátu:

R C
XXXXXX

kde R je počet řádků, C je počet sloupců a zbytek souboru vyplňuje samotná
podoba mapy (řádky mohou, ale nemusí být odděleny znakem nového řádku).
Zde je výčet jednotlivých prvků:

'#' == zeď
' ' == prázdné políčko
'+' == klíč
'G' == brána (na začátku vždy zavřena)
'C' == cíl
'S' == místo, na kterém se může objevit hráč při připojení nebo úmrtí
'H' == startovací políčko pro hlídače
