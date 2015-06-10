using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace Meal_Planner.Converters
{
	public class ListboxItemIndexToVisibilityConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null)
			{
				return null;
			}

			return (int) value == -1 ? Visibility.Hidden : Visibility.Visible;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
