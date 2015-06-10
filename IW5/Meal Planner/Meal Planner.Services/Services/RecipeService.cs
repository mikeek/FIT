using System.Data.Entity;
using Meal_Planner.Model;
using Meal_Planner.Services.Services.Repository;

namespace Meal_Planner.Services.Services
{
	public class RecipeService : Repository<MealPlannerDbContext, Recipe>
	{
		public override void LoadAll()
		{
			base.LoadAll();

			Context.Recipes.Include("Ingredients").Load();
            Context.IngredientAmounts.Include(item => item.Ingredient).Load();
		}
	}
}
