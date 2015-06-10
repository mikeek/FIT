using System;
using System.Collections.ObjectModel;
using System.Data.Entity;
using System.Linq;
using System.Linq.Expressions;

using Meal_Planner.Model.Base.Interface;

namespace Meal_Planner.Services.Services.Repository
{
	public abstract class Repository<TContext, T> : IDisposable
		where T : class, IModel
		where TContext : DbContext, new()
	{
		private TContext context = new TContext();

		private bool disposed;

		public TContext Context
		{
			get
			{
				return this.context;
			}
			set
			{
				this.context = value;
			}
		}

		public virtual void Add(T entity)
		{
			this.context.Set<T>().Add(entity);
		}

		public virtual void Delete(T entity)
		{
			this.context.Set<T>().Remove(entity);
		}

		public void Dispose()
		{
			this.Dispose(true);
			GC.SuppressFinalize(this);
		}

		public virtual void Edit(T entity)
		{
			this.context.Entry(entity).State = EntityState.Modified;
		}

		public virtual IQueryable<T> FindBy(Expression<Func<T, bool>> predicate)
		{
			IQueryable<T> query = this.context.Set<T>().Where(predicate);
			return query;
		}

		public virtual IQueryable<T> GetAll()
		{
			return this.context.Set<T>();
		}

		public virtual T GetByID(Guid id)
		{
			return this.context.Set<T>().Single(i => i.Id == id);
		}

		public ObservableCollection<T> GetObservableCollection()
		{
			return this.context.Set<T>().Local;
		}

		public virtual void LoadAll()
		{
			this.context.Set<T>().Load();
		}

		public virtual void Save()
		{
			this.context.SaveChanges();
		}

		private void Dispose(bool disposing)
		{
			if (!this.disposed)
			{
				if (disposing)
				{
					this.context.Dispose();
				}
				this.disposed = true;
			}
		}
	}
}
