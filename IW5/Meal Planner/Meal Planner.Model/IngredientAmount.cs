using Meal_Planner.Model.Base.Implementation;

namespace Meal_Planner.Model
{
    public class IngredientAmount : BaseModel
    {
        public Ingredient Ingredient { get; set; }
        
        private decimal _amount;
        public decimal Amount
        {
            get { return _amount; }
            set {
                _amount = value;
                OnPropertyChanged();
            }
        }

	    public IngredientAmount()
		    : base()
	    {
	    }

        public IngredientAmount(Ingredient ingredient, decimal amount = 0) 
			: base()
        {
            Ingredient = ingredient;
            Amount = amount;
        }

    }
}
