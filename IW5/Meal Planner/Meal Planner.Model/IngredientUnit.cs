using System.ComponentModel;

namespace Meal_Planner.Model
{
    public enum IngredientUnit
    {
        [Description("g")]Weight,
        [Description("ml")]Volume,
        [Description("pc")]Pieces,
    }
}
