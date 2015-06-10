using System;
using System.Globalization;
using System.Windows.Data;

namespace Meal_Planner.Converters
{
	public class DateToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null)
			{
				return null;
			}

			DateTime date = (DateTime) value;

			return String.Format("{0}. {1}. {2}, {3}", date.Day, date.Month, date.Year,
				Enum.GetName(typeof (DayOfWeek), date.DayOfWeek));
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
