using System.Windows.Input;
using Meal_Planner.Model;
using Meal_Planner.Services;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Commands.FoodStore;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
    public class FoodStoreViewModel: ViewModelCollection<Ingredient>
    {
        public ICommand AddNewIngredient { get; private set; }

        public FoodStoreViewModel(MealPlannerDbContext dbContext)
        {
            InitData(dbContext);
            InitCommands();
        }

        private void InitData(MealPlannerDbContext dbContext)
        {
            Name = "Food store";
	        Service = new IngredientService
	        {
		        Context = dbContext
	        };
        }

	    public override void LoadData()
	    {
		    base.LoadData();
		    NewItem = null;
	    }

	    private void InitCommands()
        {
            AddNewIngredient = new AddNewIngredientCommand(this);
        }

		/* Context will be saved and disposed by MainViewModel */
		public override void Dispose()
		{
		}

		public override void SaveData()
		{
		}
    }
}
