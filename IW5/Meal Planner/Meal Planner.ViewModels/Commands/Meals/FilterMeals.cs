using System;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.ViewModels;

namespace Meal_Planner.ViewModels.Commands.Meals
{
	public class FilterMealsCommand : CommandBase<MealsViewModel>
	{
		public FilterMealsCommand(MealsViewModel viewModel)
			: base(viewModel)
		{
		}

		public override void Execute(object parameter)
		{
			ViewModel.FilterMeals();
		}
	}
}
