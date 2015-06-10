using System;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Globalization;
using System.Linq;
using System.Windows.Input;
using Meal_Planner.Model;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Commands.WeekPlan;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
	public class WeekPlanViewModel : ViewModelCollection<Meal>
	{
		private const int DaysInWeek = 7;

		private DateTime _currentMonday;
		private DateTime _nextMonday;

		public ICommand NextWeek { get; private set; }
		public ICommand PreviousWeek { get; private set; }

		private int _weekOffset = 0;
		public int WeekOffset
		{
			get
			{
				return _weekOffset;
			}
			set
			{
				if (_weekOffset != value)
				{
					_weekOffset = value;
					LoadWeekBoundaries();
					LoadMeals();
					OnPropertyChanged();
					OnPropertyChanged("WeekNumber");
				}
			}
		}

		public int WeekNumber
		{
			get
			{
				return CultureInfo.CurrentCulture.Calendar.GetWeekOfYear(_currentMonday, CalendarWeekRule.FirstDay, DayOfWeek.Monday);
			}
		}

		public ObservableCollection<ObservableCollection<ObservableCollection<Meal>>> WeekMeals { get; private set; }

		public WeekPlanViewModel(MealService mealService)
		{
			Service = mealService;
			InitData();
			InitCommands();
		}

		private void InitData()
		{
			WeekMeals = new ObservableCollection<ObservableCollection<ObservableCollection<Meal>>>();

			for (int i = 0; i < DaysInWeek; ++i)
			{
				WeekMeals.Add(new ObservableCollection<ObservableCollection<Meal>>());

				for (MealTime.MealDayPart dayPart = MealTime.MealDayPart.Breakfast;
					dayPart <= MealTime.MealDayPart.Dinner;
					dayPart++)
				{
					WeekMeals[i].Add(new ObservableCollection<Meal>());
				}
			}

			Items.CollectionChanged += HandleItemsChange;
		}

		public override void LoadData()
		{
			/* Service is loaded in MainViewModel - no need for base.LoadData() */
			LoadWeekBoundaries();
			LoadMeals();
		}

		public void InitCommands()
		{
			NextWeek = new NextWeekCommand(this);
			PreviousWeek = new PreviousWeekCommand(this);
		}

		private void LoadWeekBoundaries()
		{
			_currentMonday = GetMondayOfSelectedWeek();
			_nextMonday = _currentMonday.AddDays(DaysInWeek);
		}

		private void LoadMeals()
		{
			int i = 0;
			for (DateTime date = _currentMonday; date < _nextMonday; date = date.AddDays(1), ++i)
			{
				int j = 0;
				for (MealTime.MealDayPart dayPart = MealTime.MealDayPart.Breakfast;
					dayPart <= MealTime.MealDayPart.Dinner;
					dayPart++, ++j)
				{
					WeekMeals[i][j] = new ObservableCollection<Meal>(Items.Where(item => item.MealTime != null && item.MealTime.Day.DayOfYear == date.DayOfYear && item.MealTime.DayPart == dayPart));
				}
			}
		}

		private DateTime GetMondayOfSelectedWeek()
		{
			DateTime today = DateTime.Now;
			int dayOffset = DayOfWeek.Monday - today.DayOfWeek;

			if (dayOffset > 0)
			{
				dayOffset -= DaysInWeek;
			}

			DateTime monday = today.AddDays(dayOffset + _weekOffset * DaysInWeek);

			return monday;
		}

		private void HandleItemsChange(object sender, NotifyCollectionChangedEventArgs args)
		{
			LoadMeals();
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
