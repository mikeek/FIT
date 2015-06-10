using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Meal_Planner.Model;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.ViewModels;

namespace Meal_Planner.ViewModels.Commands.ShoppingList
{
    public class AddIngredientAmount : CommandBase<ShoppingViewModel>
    {
        private ShoppingViewModel _viewModel;

        public AddIngredientAmount(ShoppingViewModel viewModel)
            : base(viewModel)
        {
            _viewModel = viewModel;
        }


        public override void Execute(object parameter)
        {
            var values = (object[]) parameter;
            IngredientAmount amount = (IngredientAmount) values[0];
            string addAmountStr = Convert.ToString(values[1]);
            decimal addAmount = decimal.Parse(addAmountStr, CultureInfo.InvariantCulture);

            amount.Ingredient.InStoreAmount += addAmount;
            _viewModel.UpdateRequiredIngredients();
        }
    }
}
