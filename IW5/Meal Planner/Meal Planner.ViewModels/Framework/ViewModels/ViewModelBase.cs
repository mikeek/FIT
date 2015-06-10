using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Meal_Planner.Model.Annotations;
using Meal_Planner.Model.Base.Interface;
using Meal_Planner.Services;
using Meal_Planner.Services.Services.Repository;

namespace Meal_Planner.ViewModels.Framework.ViewModels
{
	public abstract class ViewModelBase<T> : INotifyPropertyChanged, IDisposable
		where T : class, IModel
	{
		private bool disposed;

		private string name;

		public string Name
		{
			get
			{
				return name;
			}
			set
			{
				if (value == name)
				{
					return;
				}
				name = value;
				OnPropertyChanged();
			}
		}

		public Repository<MealPlannerDbContext, T> Service { get; protected set; }

		public event PropertyChangedEventHandler PropertyChanged;

		~ViewModelBase()
		{
			Dispose(false);
		}

		public virtual void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		public abstract void LoadData();

		public virtual void SaveData()
		{
			Service.Save();
		}

		[NotifyPropertyChangedInvocator]
		protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
		{
			var handler = PropertyChanged;
			if (handler != null)
			{
				handler(this, new PropertyChangedEventArgs(propertyName));
			}
		}

		private void Dispose(bool disposing)
		{
			if (!disposed)
			{
				if (disposing)
				{
					Service.Dispose();
				}
				disposed = true;
			}
		}

		public virtual void OnProgramShutdownStarted(object sender, EventArgs e)
		{
			Service.Save();
			Dispose();
		}
	}
}