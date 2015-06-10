using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Meal_Planner.Model;
using Meal_Planner.Services;
using Meal_Planner.Services.Services;
using Meal_Planner.Services.Services.Repository;
using Meal_Planner.ViewModels.Commands.ShoppingList;
using Meal_Planner.ViewModels.Framework.ViewModels;

namespace Meal_Planner.ViewModels.ViewModels
{
    public class ShoppingViewModel : ViewModelCollection<Meal>
    {
        private const int _defaultShoppingDays = 7;
        private ObservableCollection<IngredientAmount> _totalRequiredAmounts;

        public ICommand AddIngredientAmount { get; private set; }

        // Required ingredients without enought "InStore" amount -> for binding
        public ObservableCollection<IngredientAmount> RequiredAmounts { get; private set; }

        private int _showDays;
        public int ShowDays
        {
            get { return _showDays; }
            set
            {
                if (_showDays != value)
                {
                    if (value < 1)
                    {
                        throw new ArgumentOutOfRangeException();
                    }

                    _showDays = value;
                    GetIngredientsRequests();
                    OnPropertyChanged();
                }
            }
        }

        private DateTime _startDate;
        public DateTime StartDate
        {
            get { return _startDate; }
            set
            {
                if (_startDate.Date != value.Date)
                {
                    _startDate = value;
                    GetIngredientsRequests();
                    OnPropertyChanged();
                }
            }
        }

        public ShoppingViewModel(MealService mealService)
        {
	        Service = mealService;
            InitData();
            InitCommands();
        }

        private void InitData()
        {
            Name = "Shopping list";

			_totalRequiredAmounts = new ObservableCollection<IngredientAmount>();
            RequiredAmounts = new ObservableCollection<IngredientAmount>();
            
            // Default values
            _startDate = DateTime.Now;
            _showDays = _defaultShoppingDays;
        }

        public override void LoadData()
	    {
			/* Service is loaded in MainViewModel - no need for base.LoadData() */
			GetIngredientsRequests();
            Items.CollectionChanged += HandleItemsChange;
	    }

	    private void InitCommands()
        {
            this.AddIngredientAmount = new AddIngredientAmount(this);
        }

        private void HandleItemsChange(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            GetIngredientsRequests();
        }

        public void GetIngredientsRequests()
        {
            // Remove current list of required ingredients
            _totalRequiredAmounts.Clear();
            RequiredAmounts.Clear();

            // Get all meals in selected time range
            DateTime endDate = _startDate.AddDays(ShowDays);
            var plannedMeals = new ObservableCollection<Meal>(
                Items.Where(it => it.MealTime.Day.Date >= _startDate.Date && it.MealTime.Day.Date <= endDate.Date));

            // For each meal update list of required ingredients
            foreach (var meal in plannedMeals)
            {
                foreach (var mealIngredient in meal.Recipe.Ingredients)
                {
                    var requiredIngr = GetAmount(_totalRequiredAmounts, mealIngredient.Ingredient);
                    requiredIngr.Amount += meal.Count*mealIngredient.Amount;
                }
            }
            
            UpdateRequiredIngredients();
        }

        public void UpdateRequiredIngredients()
        {
            RequiredAmounts.Clear();
            foreach (IngredientAmount amount in _totalRequiredAmounts)
            {
                if (amount.Amount > amount.Ingredient.InStoreAmount)
                {
                    IngredientAmount updatedAmount = GetAmount(RequiredAmounts, amount.Ingredient);
                    updatedAmount.Amount = amount.Amount - amount.Ingredient.InStoreAmount;
                }
                else
                {
                    IngredientAmount deleteAmount = RequiredAmounts.FirstOrDefault(
                        i => i.Ingredient.Id == amount.Ingredient.Id);
                    if (deleteAmount != null)
                    {
                        RequiredAmounts.Remove(deleteAmount);
                    }
                }
            }
        }

        private static IngredientAmount GetAmount(ObservableCollection<IngredientAmount> collection, Ingredient ingredient)
        {
            // Find ingredient
            IngredientAmount ingr = collection.FirstOrDefault(i => i.Ingredient.Id == ingredient.Id);
            if (ingr == null)
            {
                // Add ingredient
                ingr = new IngredientAmount(ingredient);
                collection.Add(ingr);
            }

            return ingr;
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
