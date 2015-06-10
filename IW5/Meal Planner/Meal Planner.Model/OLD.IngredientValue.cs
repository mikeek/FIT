/*
 * TODO:
 * Tento soubor možná bude možné v budoucnu smazat.
 * Obsahuje třídu pro práci s jednotkou ingredience v textové formě.
 * */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Meal_Planner.Model
{
    public class IngredientValue
    {
        // Výčet metrických typů
        public enum MetricType
        {
            Weight, // Hmotnost
            Volume, // Objem
            Pieces, // Kusy
            None    // Nenastaveno
        }

        // Třída určená pro uchování záznamů jednotek.
        private class UnitRecord
        {
            public string Unit { get; set; }
            public double Conversion { get; set; }
            public MetricType Type { get; set; }
            public bool DefaultPrint { get; set; }

            public UnitRecord(string unit, double conversion, MetricType type, bool defaultPrint)
            {
                this.Unit = unit;
                this.Conversion = conversion;
                this.Type = type;
                this.DefaultPrint = defaultPrint;
            }
        }

        // Záznamy všech jednotek.
        // POZNÁMKA: Nejmenší jednotka by vždy měla být printable, jinak bude
        // readable výsledek zaokrouhlený. Což až tak ničemu nevadí, ale není to
        // pak přesné. Vnitřní reprezentace dat s tím ovšem nesouvisí, ta bude vždy
        // perfektně přesná. Jedná se jen o to, co bude vypsáno a co ne.
        private static List<UnitRecord> UnitRecords = new List<UnitRecord>()
        {
            // Hmotnost:
            new UnitRecord("t", 1000, MetricType.Weight, false),
            new UnitRecord("kg", 1, MetricType.Weight, true),
            new UnitRecord("dkg", 0.01, MetricType.Weight, false),
            new UnitRecord("g", 0.001, MetricType.Weight, true),
            new UnitRecord("mg", 0.000001, MetricType.Weight, true),
            // Objem:
            new UnitRecord("hl", 100, MetricType.Volume, false),
            new UnitRecord("l", 1, MetricType.Volume, true),
            new UnitRecord("dl", 0.1, MetricType.Volume, false),
            new UnitRecord("cl", 0.01, MetricType.Volume, false),
            new UnitRecord("ml", 0.001, MetricType.Volume, true),
            // Kusy:
            new UnitRecord("ks", 1, MetricType.Pieces, true),
        };

        // Konstruktor bez parametru.
        public IngredientValue()
        {
            this.Clear();
        }

        // Konstruktor s parametrem ve formě řetězce.
        public IngredientValue(string valueString)
        {
            this.Set(valueString);
        }

        // Konstruktor s parametrem ve hodnoty a základní jednotky.
        public IngredientValue(decimal value, MetricType basicUnit)
        {
            this.Set(value, basicUnit);
        }

        // Vymazání hodnoty. Nastavení na nulu.
        public void Clear()
        {
            this.pType = MetricType.None;
            this.pValue = 0;
        }


        // Chráněné proměnné. Potřebujeme být schopni měnit je z metody ParseValue,
        // která vrací nový objekt s těmito nastavenými proměnnými, ale zvenku nikdo
        // nesmí být schopen na ně vidět. K tomu slouží read-only vlasntnosti.
        protected MetricType pType = MetricType.None;
        protected decimal pValue = 0;

        // Read-only vlastnosti:
        public MetricType Type { get { return pType; } }
        public decimal Value { get { return pValue; } }

        //Nastavení hodnoty pomocí řetězce.
        public void Set(string valueString)
        {
            IngredientValue i = ParseValue(valueString);
            this.pType = i.pType;
            this.pValue = i.pValue;
        }

        // Nastavení hodnoty pomocí čísla a základní jednotky.
        public void Set(decimal value, MetricType basicUnit)
        {
            this.pValue = value;
            this.pType = basicUnit;
        }

        // Přičtení hodnoty pomocí řetězce.
        public void Add(string valueString)
        {
            IngredientValue i = ParseValue(valueString);
            if (i.Type != this.Type)
                throw new Exception(String.Format("Can not add unit of type {0} to unit of type {1}!", i.Type, this.Type));
            else
                this.pValue += i.Value;
        }

        // Přičtení hodnoty pomocí čísla v základní jednotce.
        public void Add(decimal value)
        {
            this.pValue += value;
        }

        // Odečtení hodnoty pomocí řetězce.
        public void Sub(string valueString)
        {
            IngredientValue i = ParseValue(valueString);
            if (i.Type != this.Type)
                throw new Exception(String.Format("Can not subtract unit of type {0} from unit of type {1}!", i.Type, this.Type));
            else
                this.pValue -= i.Value;
        }

        // Odečtení hodnoty pomocí čísla v základní jednotce.
        public void Sub(decimal value)
        {
            this.pValue -= value;
        }

        // Styly, jak je možno vypsat hodnotu
        public enum OutputStyle
        {
            Pretty,   // Např.: 1205 kg, 800 g
            Compact,  // Např.: 1205.8 kg
            Expanded, // Např.: 1 t, 205 kg, 80 dg
            Full      // Např.: 1 t, 205 kg, 80 dg, 0 g, 0 mg
        }

        // Vrácení hodnoty v čitelné podobě prostřednictvím řetězce.
        // Nezadaný argument znamená, že nebudou použity jednotky,
        // které nejsou nastaveny jako DefaultPrint.
        public string Get()
        {
            return Get(OutputStyle.Pretty);
        }

        // Vrácení hodnoty v čitelné podobě prostřednictvím řetězce.
        public string Get(OutputStyle style)
        {
            // Pokud je vyžadován kompaktní výstup, nebo jsme na nule,
            // je to jednoduché:
            if (style == OutputStyle.Compact || this.Value == 0)
            {
                var obj = UnitRecords.FindAll(w => w.Type == this.Type && w.Conversion == 1);
                if (obj.Count != 1 && style == OutputStyle.Compact)
                    throw new Exception(String.Format("Can not print in compact mode, no suffitien unit found for type {0}!", this.Type.ToString()));
                if (style == OutputStyle.Compact)
                    return this.Value.ToString() + "\u00A0" + obj[0].Unit;
                else
                {
                    if (obj.Count == 1)
                        return "0\u00A0" + obj[0].Unit;
                    else
                        return "0";
                }
            }

            // Najdeme si všechny jednotky, které můžeme použít,
            // a seřadíme je od největší po nejmenší.
            var records = UnitRecords.FindAll(w => w.Type == this.Type)
                .OrderByDescending(x => x.Conversion).ToList();

            // Pokud nemáme nastavenou jednotku, vytiskneme prostě jen číslo:
            if (this.Type == MetricType.None)
                return this.Value.ToString();

            // Teď projdeme všechny tyto jednotky a pokusíme se sestavit
            // čitelný řetězec s použitím těchto jednotek.
            decimal valueLeft = this.Value;
            // Pokud jsme v záporu, invertujeme a nastavíme příznak:
            bool negative = this.Value < 0;
            if (negative) valueLeft *= -1;
            string output = "";
            foreach (UnitRecord record in records)
            {
                if (((decimal)record.Conversion <= valueLeft || style == OutputStyle.Full) &&
                    (record.DefaultPrint || style == OutputStyle.Expanded || style == OutputStyle.Full))
                {
                    int num = (int)(valueLeft / (decimal)record.Conversion);
                    if (output != "")
                        output += ", ";
                    int outNum = num;
                    if (negative)
                        outNum *= -1;
                    output += outNum.ToString() + "\u00A0" + record.Unit;
                    decimal minus = (decimal)((decimal)(num) * (decimal)(record.Conversion));
                    valueLeft -= minus;
                    if (valueLeft == 0 && style != OutputStyle.Full)
                        break;
                }
            }
            return output;
        }

        // Parsovací funkce, které se předhodí nějaký text jako "10,2 kg 800g 1.25 t"
        // a ona vrátí objekt této samotné třdy s nastavenou hodnotou a metrickým typem.
        public static IngredientValue ParseValue(string valueString)
        {
            // Odstranění mezer, tedy např. z "10 kg 80 g"
            // se stane "10kg80g".
            valueString = new string(valueString.Where(c => !Char.IsWhiteSpace(c)).ToArray());

            // Pokud jsme dostali prázdný řetězec, vracíme prázdnou jednotku:
            if (valueString == "")
                return new IngredientValue();

            // Nahrazení tečky za čárky u čísel kvůli správné
            // konverzi. V ČR je to jinak než v jiných zemích.
            string separator = System.Globalization.CultureInfo.CurrentCulture.NumberFormat.NumberDecimalSeparator;
            valueString = valueString.Replace(".", separator);
            valueString = valueString.Replace(",", separator);

            // Rozparsování dle metrických celků, např.
            // z "10kg80g" se stane "10kg" a "80g".
            // Výsledky jsou v tokensMetric a tokensValue
            List<string> tokensMetric = new List<string>();
            List<string> tokensValue = new List<string>();
            string number = "";
            string unit = "";
            bool ending = false;
            foreach (char c in valueString)
            {
                if (Char.IsNumber(c) || c.ToString() == separator)
                {
                    if (ending)
                    {
                        tokensMetric.Add(unit);
                        tokensValue.Add(number);
                        number = "";
                        unit = "";
                        ending = false;
                    }
                    number += c;
                }
                else
                {
                    ending = true;
                    unit += c;
                }
            }
            tokensMetric.Add(unit);
            tokensValue.Add(number);

            // Nyní máme text rozparsovaný na jednotlivé hodnoty a jednotky.
            // Převedeme tyto tokeny do jedné společné jednotky.
            decimal trueValue = 0;
            bool foundType = false;
            MetricType type = MetricType.None;
            for (int i = 0; i < tokensValue.Count; i++)
            {
                number = tokensValue[i];
                unit = tokensMetric[i];

                var units = UnitRecords.FindAll(w => w.Unit == unit);
                if (units.Count == 0)
                    throw new Exception(String.Format("Unknown unit: [{0}]", unit));
                if (units.Count > 1)
                    throw new Exception(String.Format("More than one unit: [{0}]; Not sure which one to use.", unit));
                var unitFound = units[0];

                if (!foundType)
                {
                    foundType = true;
                    type = unitFound.Type;
                }
                else
                {
                    if (type != unitFound.Type)
                        throw new Exception(String.Format("Combining multiple metric units! [{0}] is not a {1} type.",
                            unit, type));
                }
                trueValue += ((decimal)unitFound.Conversion * Convert.ToDecimal(number));
            }

            // Návrat hodnoty jakožto instance nového objektu:
            return new IngredientValue(trueValue, type);
        }


        // Přetížení operátoru == pro porovnávání množství surovin:
        public static bool operator ==(IngredientValue a, IngredientValue b)
        {
            // Pokud jsou oba objekty null, jsou si rovny.
            if (System.Object.ReferenceEquals(a, b))
            {
                return true;
            }

            // Pokud jeden je null a druhý ne, nejsou si rovny.
            if (((object)a == null) || ((object)b == null))
            {
                return false;
            }

            // Bude se porovnávat
            if (a.Type != b.Type)
                throw new Exception(String.Format("Can not compare {0} and {1}!", a.Type, b.Type));
            return a.Value == b.Value;
        }

        // Přetížení operátoru != pro porovnávání množství surovin:
        public static bool operator !=(IngredientValue a, IngredientValue b)
        {
            return !(a == b);
        }

        // Přetížení operátoru > pro porovnávání množství surovin:
        public static bool operator >(IngredientValue a, IngredientValue b)
        {
            // Null není větší než nic.
            if ((object)a == null)
                return false;
            // Vše je větší než null
            if ((object)b == null)
                return true;

            if (a.Type != b.Type)
                throw new Exception(String.Format("Can not compare {0} and {1}!", a.Type, b.Type));

            return a.Value > b.Value;
        }

        // Přetížení operátoru < pro porovnávání množství surovin:
        public static bool operator <(IngredientValue a, IngredientValue b)
        {
            // Null není větší než nic.
            if ((object)a == null)
                return true;
            // Vše je větší než null
            if ((object)b == null)
                return false;

            if (a.Type != b.Type)
                throw new Exception(String.Format("Can not compare {0} and {1}!", a.Type, b.Type));

            return a.Value < b.Value;
        }

        // Přetížení operátoru < pro porovnávání množství surovin:
        public static bool operator >=(IngredientValue a, IngredientValue b)
        {
            return !(a.Value < b.Value);
        }

        // Přetížení operátoru < pro porovnávání množství surovin:
        public static bool operator <=(IngredientValue a, IngredientValue b)
        {
            return !(a.Value > b.Value);
        }

        // Přetížení operátoru na odečítání - verze s objektem:
        public static IngredientValue operator -(IngredientValue a, IngredientValue b)
        {
            if ((object)a == null || (object)b == null)
            {
                if ((object)a == null)
                {
                    IngredientValue i = new IngredientValue(b.Value * -1, b.Type);
                    return b;
                }
                return a;
            }

            IngredientValue j = new IngredientValue(a.Get());
            j.Sub(b.Get());
            return j;
        }

        // Přetížení operátoru na odečítání - verze s textem:
        public static IngredientValue operator -(IngredientValue a, string b)
        {
            return (a - new IngredientValue(b));
        }

        // Přetížení operátoru na přičítání - verze s objektem:
        public static IngredientValue operator +(IngredientValue a, IngredientValue b)
        {
            if ((object)a == null || (object)b == null)
            {
                if ((object)a == null)
                    return b;
                return a;
            }

            IngredientValue i = new IngredientValue(a.Get());
            i.Add(b.Get());
            return i;
        }

        // Přetížení operátoru na přičítání - verze s textem:
        public static IngredientValue operator +(IngredientValue a, string b)
        {
            return (a + new IngredientValue(b));
        }
    }
}