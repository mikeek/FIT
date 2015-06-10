using System;
using Meal_Planner.Model.Base.Interface;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
	public class MainViewModel : ViewModelBase<IModel>
	{
		public MenuViewModel MenuViewModel { get; private set; }
        public ShoppingViewModel ShoppingViewModel { get; private set; }
		public RecipiesViewModel RecipiesViewModel { get; private set; }
        public FoodStoreViewModel FoodStoreViewModel { get; private set; }

		private MealService mealService;

		private bool _isMenuViewModelSelected = true;
		public bool IsMenuViewModelSelected
		{
			get { return _isMenuViewModelSelected; }
			set 
			{ 
				if (value != _isMenuViewModelSelected)
				{
					_isMenuViewModelSelected = value;
					if (_isMenuViewModelSelected)
					{
						mealService.LoadReferences();
					}
				}
			} 
		}

	    private bool _isShoppingListSelected = false;
	    public bool IsShoppingListSelected
	    {
            get { return _isShoppingListSelected;}
	        set
	        {
	            if (value != _isShoppingListSelected)
	            {
	                _isShoppingListSelected = value;
	                if (_isShoppingListSelected)
	                {
	                    ShoppingViewModel.GetIngredientsRequests();
	                }
	            }
	        }
	    }

		public MainViewModel()
		{
			mealService = new MealService();

			MenuViewModel = new MenuViewModel(mealService);
            ShoppingViewModel = new ShoppingViewModel(mealService);
			RecipiesViewModel = new RecipiesViewModel(mealService);
            FoodStoreViewModel = new FoodStoreViewModel(mealService.Context);

			LoadData();
		}

		public override void LoadData()
		{
			mealService.LoadAll();
			MenuViewModel.LoadData();
            ShoppingViewModel.LoadData();
			RecipiesViewModel.LoadData();
            FoodStoreViewModel.LoadData();
		}

		public override void OnProgramShutdownStarted(object sender, EventArgs e)
		{
			MenuViewModel.SaveData();
            ShoppingViewModel.SaveData();
			RecipiesViewModel.SaveData();
            FoodStoreViewModel.SaveData();
			mealService.Save();

			MenuViewModel.Dispose();
            ShoppingViewModel.Dispose();
			RecipiesViewModel.Dispose();
            FoodStoreViewModel.Dispose();
			mealService.Dispose();
		}
	}
}
