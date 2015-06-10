using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using Meal_Planner.Model;

namespace Meal_Planner.Converters
{
	public class IngredientAmountToStringConverter : IValueConverter
	{
		private string FormatWeight(decimal amount)
		{
			if (amount < 500.0M)
			{
				return String.Format("{0} {1}", amount, "g");
			}
			else
			{
				return String.Format("{0} {1}", amount / 1000.0M, "kg");
			} //TODO: dalsi moznosti
		}

		private string FormatVolume(decimal amount)
		{
			if (amount < 500.0M)
			{
				return String.Format("{0} {1}", amount, "ml");
			}
			else
			{
				return String.Format("{0} {1}", amount / 1000.0M, "l");
			} //TODO: dalsi moznosti
		}

		private string FormatPieces(decimal amount)
		{
			return String.Format("{0:N1}", amount);
		}

		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null)
			{
				return null;
			}

			IngredientAmount amount = (IngredientAmount) value;

            if (amount.Ingredient == null)
                return FormatPieces(0);

			switch (amount.Ingredient.Unit)
			{
				case IngredientUnit.Pieces:
					return FormatPieces(amount.Amount);
				case IngredientUnit.Volume:
					return FormatVolume(amount.Amount);
				case IngredientUnit.Weight:
					return FormatWeight(amount.Amount);
				default:
					return amount.Amount.ToString(CultureInfo.InvariantCulture);
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
