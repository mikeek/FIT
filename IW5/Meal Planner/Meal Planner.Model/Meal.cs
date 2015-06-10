using System;
using Meal_Planner.Model.Base.Implementation;

namespace Meal_Planner.Model
{
    public class Meal : BaseModel
    {
        public Recipe Recipe { get; set; }
        public MealTime MealTime { get; set; }

        private int _count;
        public int Count
        {
            get { return _count; }
            set
            {
                if (value < 1)
                {
                    throw new ArgumentOutOfRangeException();
                }
                _count = value;
            }
        }

        public Meal(Recipe recipe, MealTime time, int count = 1)
			: base()
        {
            Recipe = recipe;
            MealTime = time;
            Count = count;
        }

	    public Meal()
		    : base()
	    {
		    Count = 1;
	    }

	    private string _name;
	    public override string Name
	    {
		    get
		    {
			    _name = String.Format("{0} ({1})", (Recipe == null) ? "Some meal" : Recipe.Name, Count);
			    return _name;
		    }
		    set { _name = value; }
	    }
    }
}
