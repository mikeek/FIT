using System;
using Meal_Planner.Model.Base.Interface;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
	public class MenuViewModel : ViewModelBase<IModel>
	{
		public MealsViewModel MealsViewModel { get; private set; }
		public WeekPlanViewModel WeekPlanViewModel { get; private set; }

		public MenuViewModel(MealService mealService)
		{
			MealsViewModel = new MealsViewModel(mealService);
			WeekPlanViewModel = new WeekPlanViewModel(mealService);
		}

		public override void LoadData()
		{
			MealsViewModel.LoadData();
			WeekPlanViewModel.LoadData();
		}

		public override void SaveData()
		{
			MealsViewModel.SaveData();
			WeekPlanViewModel.SaveData();
		}

		public override void Dispose()
		{
			MealsViewModel.Dispose();
			WeekPlanViewModel.Dispose();
		}
	}
}
