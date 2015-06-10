using System;
using System.Data.Entity;
using System.Data.Entity.Migrations;
using Meal_Planner.Model;

namespace Meal_Planner.Services
{
    public class MealPlannerDbContext : DbContext
    {
	    public DbSet<Recipe> Recipes { get; set; }
		public DbSet<Meal> Meals { get; set; }
		public DbSet<Ingredient> Ingredients { get; set; }
		public DbSet<IngredientAmount> IngredientAmounts { get; set; }

	    public MealPlannerDbContext()
            : base(@"Data Source=(LocalDB)\v11.0;AttachDbFileName=C:\Users\Lukáš\Source\Workspaces\Meal Planner\Meal Planner\Meal Planner.Services\LocalDb.mdf;Persist Security Info=True;MultipleActiveResultSets=True;Integrated Security=SSPI")
	    {
   
	    }

	    protected override void OnModelCreating(DbModelBuilder modelBuilder)
	    {
		    Database.SetInitializer(new MigrateDatabaseToLatestVersion<MealPlannerDbContext, MealPlannerDbMigrationsConfiguration<MealPlannerDbContext>>());
			Database.SetInitializer(new MealPlannerInitializer());
			base.OnModelCreating(modelBuilder);
	    }
    }

	public class MealPlannerInitializer : DropCreateDatabaseAlways<MealPlannerDbContext>
	{
		protected override void Seed(MealPlannerDbContext context)
		{
			Ingredient spagety	= new Ingredient(IngredientUnit.Weight) { Name = "špagety"};
			Ingredient olej		= new Ingredient(IngredientUnit.Volume) { Name = "oliv. olej"};
			Ingredient cili		= new Ingredient(IngredientUnit.Pieces) { Name = "chilli pap."};
			Ingredient cesnek	= new Ingredient(IngredientUnit.Pieces) { Name = "česnek" };
			Ingredient rukola	= new Ingredient(IngredientUnit.Weight) { Name = "rukola" };
			Ingredient sunka	= new Ingredient(IngredientUnit.Weight) { Name = "parmská šunka" };
			Ingredient rajce	= new Ingredient(IngredientUnit.Pieces) { Name = "cherry rajče" };
			Ingredient vejce	= new Ingredient(IngredientUnit.Pieces) { Name = "vejce" };
			Ingredient sul		= new Ingredient(IngredientUnit.Weight) { Name = "sůl" };
			Ingredient pepr		= new Ingredient(IngredientUnit.Weight) { Name = "pepř" };
			Ingredient krupice	= new Ingredient(IngredientUnit.Weight) { Name = "krupice" };
			Ingredient cukr		= new Ingredient(IngredientUnit.Weight) { Name = "cukr"};
			Ingredient mleko	= new Ingredient(IngredientUnit.Volume) { Name = "mléko" };
			Ingredient kakao	= new Ingredient(IngredientUnit.Weight) { Name = "kakao" };
			Ingredient ananas	= new Ingredient(IngredientUnit.Pieces) { Name = "ananas" };
			Ingredient mrkev	= new Ingredient(IngredientUnit.Pieces) { Name = "mrkev"};
			Ingredient citron	= new Ingredient(IngredientUnit.Pieces) { Name = "citron"};

			Recipe italskeSpagety = new Recipe()
			{
				Name = "Italské špagety",
				Minutes = 15,
				Type = RecipeType.MainCourse,
				PreparationSteps = 
@"1) Dáme si vařit vodu. Až začne vařit, vložíme těstoviny a vodu osolíme.
2) Na olivovém oleji si orestujeme půl chilli papričky. Asi po minutě přidáme utřený česnek.
3) K česneku a chilli přidáme nakrájená rajčata a nakrájenou parmskou šunku. Necháme chvíli podusit.
4) Uvařené těstoviny scedíme a přidíme ke zbytku.
5) Podáváme s natrhanou rukolou."
			};

			italskeSpagety.AddIngredient(sul, 10);
			italskeSpagety.AddIngredient(spagety, 100);
			italskeSpagety.AddIngredient(olej, 30);
			italskeSpagety.AddIngredient(cili, 0.5M);
			italskeSpagety.AddIngredient(cesnek, 1);
			italskeSpagety.AddIngredient(rukola, 100);
			italskeSpagety.AddIngredient(sunka, 80);
			italskeSpagety.AddIngredient(rajce, 10);

			Recipe michanaVejce = new Recipe()
			{
				Name = "Míchaná vejce",
				Minutes = 5,
				Type = RecipeType.Breakfast,
				PreparationSteps =
@"1) Na rozehřátou pánev si rozklepneme vejce
2) Osolíme, opepříme a mícháme, dokud nejsou vejce hotová"
			};

			michanaVejce.AddIngredient(vejce, 2);
			michanaVejce.AddIngredient(sul, 5);
			michanaVejce.AddIngredient(pepr, 5);

			Recipe krupicovaKase = new Recipe()
			{
				Name = "Krupicová kaše",
				Minutes = 15,
				Type = RecipeType.DessertAndCake,
				PreparationSteps = 
@"1) Hrnek mléka vlejeme do hrnce a přidáme krupici, sůl a cukr.
2) Za stálého míchání přivedeme k varu.
3) Po zhoustnutí nalijeme do misky a posypeme kakaem"
			};

			krupicovaKase.AddIngredient(krupice, 50);
			krupicovaKase.AddIngredient(mleko, 250);
			krupicovaKase.AddIngredient(sul, 5);
			krupicovaKase.AddIngredient(cukr, 7);
			krupicovaKase.AddIngredient(kakao, 25);

			Recipe mrkvovySalat = new Recipe()
			{
				Name = "Mrkvový salát",
				Minutes = 5,
				Type = RecipeType.DessertAndCake,
				PreparationSteps = 
@"1) Nastrouhanou mrkev smícháme s kostkami ananasu.
2) Zakápneme citronovou šťávou",
			};

			mrkvovySalat.AddIngredient(mrkev, 2);
			mrkvovySalat.AddIngredient(ananas, 0.5M);
			mrkvovySalat.AddIngredient(citron, 0.5M);

			Meal pondelniSnidane = new Meal(michanaVejce, new MealTime(new DateTime(2015, 5, 4), MealTime.MealDayPart.Breakfast));
			Meal pondelniObed	= new Meal(italskeSpagety, new MealTime(new DateTime(2015, 5, 4), MealTime.MealDayPart.Lunch), 2);
			Meal pondelniDezert = new Meal(mrkvovySalat, new MealTime(new DateTime(2015, 5, 4), MealTime.MealDayPart.Lunch));
			Meal uterniVecere	= new Meal(krupicovaKase, new MealTime(new DateTime(2015, 5, 5), MealTime.MealDayPart.Dinner), 2);

			context.Ingredients.Add(spagety);
			context.Ingredients.Add(olej);
			context.Ingredients.Add(cili);
			context.Ingredients.Add(cesnek);
			context.Ingredients.Add(rukola);
			context.Ingredients.Add(sunka);
			context.Ingredients.Add(rajce);
			context.Ingredients.Add(vejce);
			context.Ingredients.Add(sul);
			context.Ingredients.Add(pepr);
			context.Ingredients.Add(krupice);
			context.Ingredients.Add(cukr);
			context.Ingredients.Add(mleko);
			context.Ingredients.Add(kakao);
			context.Ingredients.Add(mrkev);
			context.Ingredients.Add(ananas);
			context.Ingredients.Add(citron);

			context.Recipes.Add(michanaVejce);
			context.Recipes.Add(italskeSpagety);
			context.Recipes.Add(krupicovaKase);
			context.Recipes.Add(mrkvovySalat);

			context.Meals.Add(pondelniSnidane);
			context.Meals.Add(pondelniObed);
			context.Meals.Add(pondelniDezert);
			context.Meals.Add(uterniVecere);

			base.Seed(context);
		}
	}

	public class MealPlannerDbMigrationsConfiguration<T> : DbMigrationsConfiguration<T>
		where T : DbContext
	{
		public MealPlannerDbMigrationsConfiguration()
		{
			AutomaticMigrationsEnabled = true;
		}
	}
}
