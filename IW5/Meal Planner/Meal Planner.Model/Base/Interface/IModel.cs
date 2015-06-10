using System;
using System.ComponentModel;

namespace Meal_Planner.Model.Base.Interface
{
	public interface IModel : INotifyPropertyChanged
	{
		Guid Id { get; }
	}
}
