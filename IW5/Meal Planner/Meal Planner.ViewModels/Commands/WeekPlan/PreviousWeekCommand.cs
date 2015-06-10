using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.ViewModels;

namespace Meal_Planner.ViewModels.Commands.WeekPlan
{
	public class PreviousWeekCommand : CommandBase<WeekPlanViewModel>
	{
		public PreviousWeekCommand(WeekPlanViewModel viewModel)
			: base(viewModel)
		{
		}

		public override void Execute(object parameter)
		{
			ViewModel.WeekOffset--;
		}
	}
}
