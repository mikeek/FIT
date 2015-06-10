using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Meal_Planner.Model.Base.Implementation;

namespace Meal_Planner.Model
{
    public class Ingredient : BaseModel
    {
        private IngredientUnit _unit;
        public IngredientUnit Unit
        {
            get { return _unit; }
            set {
                _unit = value;
                OnPropertyChanged(); 
            }
        }

        private decimal _inStoreAmount;
        public decimal InStoreAmount {
            get { return _inStoreAmount; }
            set
            {
                _inStoreAmount = value;
                OnPropertyChanged();
            }
        }

	    public Ingredient()
		    : base()
	    {
	    }

	    public Ingredient(IngredientUnit unit)
			: base()
	    {
		    Unit = unit;
	        InStoreAmount = 0;
	    }

        public IEnumerable<IngredientUnit> UnitValues
        {
            get { return Enum.GetValues(typeof (IngredientUnit)).Cast<IngredientUnit>(); }
        }
    }
}
