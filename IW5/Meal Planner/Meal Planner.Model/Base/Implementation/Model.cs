using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Meal_Planner.Model.Annotations;

namespace Meal_Planner.Model.Base.Implementation
{
	public abstract class Model : INotifyPropertyChanged
	{
		protected Model()
		{
			Id = Guid.NewGuid();
		}

		protected Model(Guid id)
		{
			Id = id;
		}

		public Guid Id { get; private set; }

		public event PropertyChangedEventHandler PropertyChanged;

		[NotifyPropertyChangedInvocator]
		protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
		{
			var handler = PropertyChanged;
			if (handler != null)
			{
				handler(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
}
