using System;
using Meal_Planner.Model.Base.Interface;

namespace Meal_Planner.Model.Base.Implementation
{
	public abstract class BaseModel : Model, INameModel
	{
		private string name;

		public virtual string Name 
		{
			get
			{
				return name;
			}
			set
			{
				if (name != value)
				{
					name = value;
					OnPropertyChanged();
				}
			}
		}
		
		protected BaseModel()
		{
		}

		protected BaseModel(Guid id)
			: base(id)
		{
		}
	}
}
