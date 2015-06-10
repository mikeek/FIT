using System.Data.Entity;
using System.Linq;
using Meal_Planner.Model;
using Meal_Planner.Services.Services.Repository;

namespace Meal_Planner.Services.Services
{
	public class MealService : Repository<MealPlannerDbContext, Meal>
	{
		public override void LoadAll()
		{
			base.LoadAll();

			LoadReferences();
		}

		public void LoadReferences()
		{
			Context.Meals.Include(item => item.Recipe).Load();
			Context.Meals.Include(item => item.MealTime).Load();
			Context.Recipes.Include(item => item.Ingredients).Load();
			Context.IngredientAmounts.Include(item => item.Ingredient).Load();
		}
	}
}
