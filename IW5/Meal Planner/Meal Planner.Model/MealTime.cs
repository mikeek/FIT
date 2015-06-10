using Meal_Planner.Model.Base.Implementation;
using System;

namespace Meal_Planner.Model
{
    public class MealTime: BaseModel, IComparable<MealTime>
    {
        // Vlastnost uchovávající konkrétní datum.
        private DateTime _day;
        public DateTime Day
        {
            get { return _day.Date; }
            set { _day = new DateTime(value.Year, value.Month, value.Day); }
        }

        // Čas na jídlo v rámci dne.
        public enum MealDayPart
        {
            Breakfast,
            Lunch,
            Dinner
        }
        public MealDayPart DayPart { get; set; }

        // Konstruktor třídy.
	    public MealTime() : base()
	    {
	    }

	    public MealTime(DateTime day, MealDayPart dayPart) : base()
        {
            Day = day;
            DayPart = dayPart;
        }

        // Výpis do hodnoty řetězce.
        public override string ToString()
        {
            return String.Format("{0} - {1}", this.Day.ToString(), this.DayPart);
        }

        // Metoda pro sortování.
        public int CompareTo(MealTime other)
        {
            int i = this.Day.CompareTo(other.Day);
            if (i == 0)
                return this.DayPart.CompareTo(other.DayPart);
            else
                return i;
        }
    }
}
