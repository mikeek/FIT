namespace Meal_Planner.Services.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class initial : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.IngredientAmounts",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Amount = c.Decimal(nullable: false, precision: 18, scale: 2),
                        Name = c.String(),
                        Ingredient_Id = c.Guid(),
                        Recipe_Id = c.Guid(),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Ingredients", t => t.Ingredient_Id)
                .ForeignKey("dbo.Recipes", t => t.Recipe_Id)
                .Index(t => t.Ingredient_Id)
                .Index(t => t.Recipe_Id);
            
            CreateTable(
                "dbo.Ingredients",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Unit = c.Int(nullable: false),
                        InStoreAmount = c.Decimal(nullable: false, precision: 18, scale: 2),
                        Name = c.String(),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.Meals",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Count = c.Int(nullable: false),
                        Name = c.String(),
                        MealTime_Id = c.Guid(),
                        Recipe_Id = c.Guid(),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.MealTimes", t => t.MealTime_Id)
                .ForeignKey("dbo.Recipes", t => t.Recipe_Id)
                .Index(t => t.MealTime_Id)
                .Index(t => t.Recipe_Id);
            
            CreateTable(
                "dbo.MealTimes",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        Day = c.DateTime(nullable: false),
                        DayPart = c.Int(nullable: false),
                        Name = c.String(),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.Recipes",
                c => new
                    {
                        Id = c.Guid(nullable: false),
                        PreparationSteps = c.String(),
                        Type = c.Int(nullable: false),
                        Minutes = c.Int(nullable: false),
                        Rating = c.Byte(),
                        Name = c.String(),
                    })
                .PrimaryKey(t => t.Id);
            
        }
        
        public override void Down()
        {
            DropForeignKey("dbo.Meals", "Recipe_Id", "dbo.Recipes");
            DropForeignKey("dbo.IngredientAmounts", "Recipe_Id", "dbo.Recipes");
            DropForeignKey("dbo.Meals", "MealTime_Id", "dbo.MealTimes");
            DropForeignKey("dbo.IngredientAmounts", "Ingredient_Id", "dbo.Ingredients");
            DropIndex("dbo.Meals", new[] { "Recipe_Id" });
            DropIndex("dbo.Meals", new[] { "MealTime_Id" });
            DropIndex("dbo.IngredientAmounts", new[] { "Recipe_Id" });
            DropIndex("dbo.IngredientAmounts", new[] { "Ingredient_Id" });
            DropTable("dbo.Recipes");
            DropTable("dbo.MealTimes");
            DropTable("dbo.Meals");
            DropTable("dbo.Ingredients");
            DropTable("dbo.IngredientAmounts");
        }
    }
}
