using Meal_Planner.Model;
using Meal_Planner.ViewModels.Commands.Collection;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.ViewModels;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Meal_Planner.ViewModels.Commands.Recipe
{
    public class AddRecipe : CommandBase<RecipiesViewModel>
    {
        private RecipiesViewModel _viewModel;

        public AddRecipe(RecipiesViewModel viewModel)
            : base(viewModel)
        {
            _viewModel = viewModel;
        }


        public override void Execute(object parameter)
        {
            // Because of namespace we have to use "Model" prefix.
            Model.Recipe newRecipe = new Model.Recipe();
            newRecipe.Name = "New Recipe";
            newRecipe.PreparationSteps = "Write the preparation steps here.";
            _viewModel.Items.Add(newRecipe);
        }
    }
}
