using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using Meal_Planner.Model.Base.Implementation;
using System.ComponentModel;

namespace Meal_Planner.Model
{
    public enum RecipeType
    {
        [Description("Breakfast")]
        Breakfast,
        [Description("Main Course")]
        MainCourse,
        [Description("Soup")]
        Soup,
        [Description("Desert and Cake")]
        DessertAndCake,
        [Description("Salat")]
        Salat,
        [Description("Side Dish")]
        SideDish,
        [Description("Drink")]
        Drink,
        [Description("Other")]
        Other
    }

    public class Recipe : BaseModel
    {
        public virtual ObservableCollection<IngredientAmount> Ingredients { get; private set; }
        public string PreparationSteps { get; set; }

        /* Filterable items */
        public RecipeType Type { get; set; }

        private int _minutes;
        public int Minutes { 
            get { return _minutes; }
            set
            {
                if (value < 0)
                {
                    throw new ArgumentOutOfRangeException();
                }
                _minutes = value;
            }
        }

        private byte? _rating;
        public byte? Rating
        {
            get { return _rating; }
            set
            {
                if (value != null && (value < 1 || value > 5))
                {
                    throw new ArgumentOutOfRangeException();
                }
                _rating = value;
            }
        }

        public Recipe()
			: base()
        {
            Ingredients = new ObservableCollection<IngredientAmount>();
        }

        public void AddIngredient(Ingredient ingredient, decimal amount)
        {
            IngredientAmount ingr = Ingredients.FirstOrDefault(i => i.Ingredient.Id == ingredient.Id);
            if (ingr == null)
            {

                ingr = new IngredientAmount(ingredient, amount);
                Ingredients.Add(ingr);
            }
            else
            {
                throw new Exception("Item is already present in the list of ingredients.");
            }
        }

        public void RemoveIngredient(Ingredient ingredient)
        {
            IngredientAmount ingr = Ingredients.FirstOrDefault(i => i.Ingredient.Id == ingredient.Id);
            if (ingr == null)
            {
                throw new Exception("Item is not present");
            }

            Ingredients.Remove(ingr);
        }
    }
}
