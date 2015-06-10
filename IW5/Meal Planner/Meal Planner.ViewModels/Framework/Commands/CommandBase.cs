using System;
using System.Windows.Input;

namespace Meal_Planner.ViewModels.Framework.Commands
{
	public abstract class CommandBase<TViewModel> : ICommand
	{
		protected TViewModel ViewModel { get; set; }

		public event EventHandler CanExecuteChanged;

		protected CommandBase(TViewModel viewModel)
		{
			ViewModel = viewModel;
		}

		public virtual bool CanExecute(object parameter)
		{
			return true;
		}

		public abstract void Execute(object parameter);
	}
}
