using Meal_Planner.Model.Base.Interface;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.Commands.Collection
{
	public class SaveChangesCommand<T> : CommandBase<ViewModelCollection<T>>
		where T : class, IModel, new()
	{
		public SaveChangesCommand(ViewModelCollection<T> viewModel) 
			: base(viewModel)
		{
		}

		public override void Execute(object parameter)
		{
			ViewModel.SaveData();
		}
	}
}
