using System;
using System.Globalization;
using System.Windows.Data;
using Meal_Planner.Model;

namespace Meal_Planner.Converters
{
	public class RecipeTypeToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null || value.ToString() == "")
			{
				return "Unknown";
			}

            RecipeType type = (RecipeType)value;

            switch (type)
            {
                case RecipeType.DessertAndCake:
                    return "Dessert and Cake";
                case RecipeType.MainCourse:
                    return "Main Course";
                case RecipeType.SideDish:
                    return "Side Dish";
                default:
                    return Enum.GetName(typeof(RecipeType), type);
            }
            

		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null)
            {
                return "Unknown";
            }

            string val = (string) value;

            switch (val)
            {
                case "Dessert and Cake":
                    return RecipeType.DessertAndCake;
                case "Main Course":
                    return RecipeType.MainCourse;
                case "Side Dish":
                    return RecipeType.SideDish;
                default:
                    return (RecipeType)System.Enum.Parse(typeof(RecipeType), val);
            }
		}
	}
}
