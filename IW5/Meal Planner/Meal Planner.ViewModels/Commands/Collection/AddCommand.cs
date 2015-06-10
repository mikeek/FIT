using Meal_Planner.Model.Base.Interface;
using Meal_Planner.ViewModels.Framework.Commands;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.Commands.Collection
{
    public class AddCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : class, IModel, new()
    {
        public AddCommand(ViewModelCollection<T> viewModelCollection)
            : base(viewModelCollection)
        {
        }

        public override void Execute(object item)
        {
            var typeItem = item as T;
            if (typeItem != null)
            {
                ViewModel.Items.Add(typeItem);
            }
        }
    }
}