using Meal_Planner.Model;
using Meal_Planner.Services.Services.Repository;

namespace Meal_Planner.Services.Services
{
	public class IngredientService : Repository<MealPlannerDbContext, Ingredient>
	{
	}
}
