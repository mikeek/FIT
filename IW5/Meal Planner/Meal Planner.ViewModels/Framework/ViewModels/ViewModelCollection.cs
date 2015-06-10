using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Meal_Planner.Model.Base.Interface;
using Meal_Planner.ViewModels.Commands.Collection;

namespace Meal_Planner.ViewModels.Framework.ViewModels
{
	public abstract class ViewModelCollection<T> : ViewModelBase<T>
		where T : class, IModel, new()
	{
		private string status;

		private T newItem;

		public ICommand SaveNewItem { get; set; }
		public ICommand DiscardNewItem { get; set; }
		public ICommand RemoveItem { get; set; }
		public ICommand SaveChanges { get; set; }

		public ObservableCollection<T> Items
		{
			get
			{
				return Service.GetObservableCollection();
			}
		}

		public T NewItem
		{
			get
			{
				return newItem;
			}
			set
			{
				if (Equals(value, newItem))
				{
					return;
				}
				newItem = value;

				OnPropertyChanged();
			}
		}

		public string Status
		{
			get
			{
				return status;
			}
			set
			{
				if (status != value)
				{
					status = value;
					OnPropertyChanged();
				}
			}
		}

		protected ViewModelCollection()
		{
			Status = "Data nebyla načtena";
			RemoveItem = new RemoveCommand<T>(this);
			SaveNewItem = new SaveItemCommand<T>(this);
			DiscardNewItem = new DiscardItemCommand<T>(this);
			SaveChanges = new SaveChangesCommand<T>(this);
			NewItem = new T();
		}

		public override void LoadData()
		{
			Status = "Načítám";
			Service.LoadAll();
			Status = "Data načtena";

			OnPropertyChanged("Items");
		}
	}
}