
using Meal_Planner.Model.Base.Interface;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.Commands.Collection
{
	public class SaveItemCommand<T> : CommandBase<ViewModelCollection<T>>
		where T : class, IModel, new()
	{
		public SaveItemCommand(ViewModelCollection<T> viewModelCollection)
			: base(viewModelCollection)
		{
		}

		public override void Execute(object item)
		{
			ViewModel.Service.Add(ViewModel.NewItem);
			ViewModel.Items.Add(ViewModel.NewItem);
			ViewModel.NewItem = null;
		}
	}
}