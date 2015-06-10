using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Windows.Input;
using Meal_Planner.Model;
using Meal_Planner.Services.Services;
using Meal_Planner.ViewModels.Commands.Collection;
using Meal_Planner.ViewModels.Commands.Recipe;
using Meal_Planner.ViewModels.Framework.ViewModels;
using System.ComponentModel;
using System;

namespace Meal_Planner.ViewModels.ViewModels
{
    public class RecipiesViewModel : ViewModelCollection<Recipe>, INotifyPropertyChanged
    {
        public ICommand AddRecipe { get; private set; }
        public ICommand RemoveRecipe { get; private set; }
        public ICommand AddIngredience { get; private set; }
        public ICommand RemoveIngredience { get; private set; }
        public ICommand PlanRecipe { get; private set; }
        public ICommand StornoPlanning { get; private set; }
        public ICommand SavePlanning { get; private set; }

        public ObservableCollection<Recipe> AllRecipies { get; private set; }
        public ObservableCollection<Ingredient> AllIngredients { get; private set; }

        private IngredientService ingredientService;
	    private MealService mealService;

        private bool _isPlanning = false;
        public bool IsPlanning
        {
            get
            {
                return _isPlanning;
            }
            set
            {
                if (value == _isPlanning)
                {
                    return;
                }
                _isPlanning = value;
                OnPropertyChanged();
            }
        }

        private Recipe _lastPlannedRecipe = null;
        public Recipe LastPlannedRecipe
        {
            get
            {
                return _lastPlannedRecipe;
            }
            set
            {
                if (value == _lastPlannedRecipe)
                {
                    return;
                }
                _lastPlannedRecipe = value;
                OnPropertyChanged();
            }
        }

        public RecipiesViewModel(MealService globalMealService)
        {
	        mealService = globalMealService;
            InitData();
            InitCommands();
        }

        private void InitData()
        {
            Name = "Recipies";

			/* Context injection - recipe change will be visible in Menu View without app restart */
	        Service = new RecipeService
	        {
		        Context = mealService.Context
	        };
            ingredientService = new IngredientService
            {
                Context = mealService.Context
            };
            ingredientService.LoadAll();

            AllRecipies = new ObservableCollection<Recipe>(Items);
			Items.CollectionChanged += HandleItemsChange;
            AllIngredients = ingredientService.GetObservableCollection();
        }

	    public override void LoadData()
	    {
		    base.LoadData();
			ReloadRecipies();
	    }

	    private void InitCommands()
        {
            AddRecipe = new AddRecipe(this);
            RemoveRecipe = new RemoveCommand<Recipe>(this);
            AddIngredience = new AddIngredience(this);
            RemoveIngredience = new RemoveIngredience(this);
            PlanRecipe = new PlanRecipe(this);
            StornoPlanning = new StornoPlanning(this);
            SavePlanning = new SavePlanning(this);
        }

        private void ReloadRecipies()
        {
            AllRecipies.Clear();
            foreach (var rec in Items)
            {
                AllRecipies.Add(rec);
            }
        }

        private void HandleItemsChange(object sender, NotifyCollectionChangedEventArgs args)
        {
            ReloadRecipies();
        }

        public void PlanNewMeal(object parameter)
        {
            object[] pars = parameter as object[];
            Recipe r = (Recipe)pars[0];
            DateTime dt = (DateTime)pars[1];
            MealTime.MealDayPart dp = (MealTime.MealDayPart)pars[2];
            double c = (double)pars[3];

            MealTime mt = new MealTime(dt, dp);
            Meal m = new Meal(r, mt, Convert.ToInt16(c));
            mealService.GetObservableCollection().Add(m);
        }

		/* Context will be saved and disposed by MainViewModel */
		public override void Dispose()
		{
		}

		public override void SaveData()
		{
		}
    }
}
