using System;
using System.Globalization;
using System.Windows.Data;
using Meal_Planner.Model;
using System.Windows.Media.Imaging;
using System.Reflection;

namespace Meal_Planner.Converters
{
    public class RecipeTypeToImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            RecipeType type = (RecipeType)value;

            switch (type)
            {
                case RecipeType.Breakfast:
                    return findSource("ico_breakfast.png");
                case RecipeType.DessertAndCake:
                    return findSource("ico_cake.png");
                case RecipeType.Drink:
                    return findSource("ico_drink.png");
                case RecipeType.MainCourse:
                    return findSource("ico_mainCourse.png");
                case RecipeType.Salat:
                    return findSource("ico_salat.png");
                case RecipeType.SideDish:
                    return findSource("ico_sideDish.png");
                case RecipeType.Soup:
                    return findSource("ico_soup.png");
                default:
                    return findSource("ico_other.png");
            }


        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        private BitmapImage findSource(string file)
        {
            return new BitmapImage(new Uri(@"pack://application:,,,/"
             + Assembly.GetExecutingAssembly().GetName().Name
             + ";component/"
             + "Resources/" + file, UriKind.Absolute));
        }
    }
}
