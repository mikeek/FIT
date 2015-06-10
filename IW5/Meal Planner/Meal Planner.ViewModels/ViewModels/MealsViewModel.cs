using System;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Windows.Input;
using Meal_Planner.Model;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Commands.Collection;
using Meal_Planner.ViewModels.Commands.Meals;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
	/* Wrap bool value so it can be two-way binded */
	public class BooleanHelper
	{
		public bool Value { get; set; }
		public string Name { get; set; }
	}

	public class MealsViewModel : ViewModelCollection<Meal>
	{
		public ICommand RemoveMeal { get; private set; }
		public ICommand FilterMealList { get; private set; }

		public ObservableCollection<Meal> FilteredMeals { get; private set; }

		public ObservableCollection<BooleanHelper> DayOfWeekFilter { get; private set; }
		public ObservableCollection<BooleanHelper> DayPartFilter { get; private set; }

		public MealsViewModel(MealService mealService)
		{
			Service = mealService;
			InitData();
			InitCommands();
		}

		private void InitData()
		{
			FilteredMeals = new ObservableCollection<Meal>(Items);
			DayOfWeekFilter = new ObservableCollection<BooleanHelper>();
			DayPartFilter = new ObservableCollection<BooleanHelper>();

			for (DayOfWeek day = DayOfWeek.Sunday; day <= DayOfWeek.Saturday; ++day )
			{
				DayOfWeekFilter.Add(new BooleanHelper { Value = true, Name = Enum.GetName(typeof (DayOfWeek), day) });
			}

			for (MealTime.MealDayPart part = MealTime.MealDayPart.Breakfast; part <= MealTime.MealDayPart.Dinner; part++)
			{
				DayPartFilter.Add(new BooleanHelper { Value = true, Name = Enum.GetName(typeof(MealTime.MealDayPart), part)});
			}

			Items.CollectionChanged += HandleItemsChange;
		}

		public override void LoadData()
		{
			/* Service is loaded in MainViewModel - no need for base.LoadData() */
			FilterMeals();
		}

		private void InitCommands()
		{
			RemoveMeal = new RemoveCommand<Meal>(this);
			FilterMealList = new FilterMealsCommand(this);
		}

		public void FilterMeals()
		{
			FilteredMeals.Clear();
			foreach (var meal in Items.Where(item => 
				item.MealTime != null && DayOfWeekFilter[(int) item.MealTime.Day.DayOfWeek].Value && DayPartFilter[(int)item.MealTime.DayPart].Value))
			{
				FilteredMeals.Add(meal);
			}
		}

		private void HandleItemsChange(object sender, NotifyCollectionChangedEventArgs args)
		{
			FilterMeals();
		}

        public void PlanNewMeal(object parameter)
        {

        }

		/* Service will be saved and disposed by MainViewModel */
		public override void Dispose()
		{
		}

		public override void SaveData()
		{
		}
	}
}
