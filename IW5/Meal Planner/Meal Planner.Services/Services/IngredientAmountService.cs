using System.Data.Entity;
using Meal_Planner.Model;
using Meal_Planner.Services.Services.Repository;

namespace Meal_Planner.Services.Services
{
	public class IngredientAmountService : Repository<MealPlannerDbContext, IngredientAmount>
	{
		public override void LoadAll()
		{
			base.LoadAll();

			Context.IngredientAmounts.Include("Ingredient").Load();
		}
	}
}
