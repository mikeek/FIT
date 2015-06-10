using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Meal_Planner.Model;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.ViewModels;

namespace Meal_Planner.ViewModels.Commands.FoodStore
{
    public class AddNewIngredientCommand : CommandBase<FoodStoreViewModel>
    {
        public AddNewIngredientCommand(FoodStoreViewModel viewModel) : base(viewModel)
        {
        }

        public override void Execute(object parameter)
        {
            this.ViewModel.NewItem = new Ingredient(IngredientUnit.Weight);
        }
    }
}
