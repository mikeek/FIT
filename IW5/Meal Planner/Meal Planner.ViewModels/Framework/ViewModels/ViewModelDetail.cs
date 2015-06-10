using Meal_Planner.Model.Base.Interface;

namespace Meal_Planner.ViewModels.Framework.ViewModels
{
	public abstract class ViewModelDetail<T> : ViewModelBase<T>
		where T : class, IModel
	{
		protected ViewModelDetail(T item)
		{
			Item = item;
		}

		public T Item { get; private set; }

		public override void LoadData()
		{
			Item = Service.GetByID(Item.Id);
		}
	}
}
