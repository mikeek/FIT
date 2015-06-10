using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Meal_Planner.Model
{
	// TODO: Michal Kozubík: dodělat, až bude implementována IngredientAmount
    public class IngredientStore
    {
	    private ObservableCollection<IngredientAmount> storeIngredients;
	    public virtual ReadOnlyObservableCollection<IngredientAmount> Ingredients { get; private set; }

	    public IngredientStore()
	    {
			/* Create store and it's read-only public version */
		    storeIngredients = new ObservableCollection<IngredientAmount>();
			Ingredients = new ReadOnlyObservableCollection<IngredientAmount>(storeIngredients);
	    }

	    public void StoreIngredient(IngredientAmount ingredient)
	    {
			/* Find ingredient in store */
		    IngredientAmount storeIngredient = FindOrInsertIngredient(ingredient.Ingredient);
			
			//storeIngredient.Value += value;
	    }
		
	    public void TakeIngredient(IngredientAmount ingredient)
	    {
		    IngredientAmount storeIngredient = FindOrInsertIngredient(ingredient.Ingredient);

			//storeIngredient.Value -= value;
	    }

		private IngredientAmount FindOrInsertIngredient(Ingredient ingredient)
		{
			/* Find ingredient in store */
			IngredientAmount storeIngredient = storeIngredients.FirstOrDefault(
                i => i.Ingredient.Id == ingredient.Id);

			if (storeIngredient == null)
			{
				/* Add new ingredient into the store */
                
				storeIngredient = new IngredientAmount(ingredient);
				storeIngredients.Add(storeIngredient);
			}

			return storeIngredient;
		}
    }
}
