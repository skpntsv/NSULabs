using Hackathon.Core.Model.DataBase;
using Microsoft.EntityFrameworkCore;

namespace SandBox.Models.Data;

public class ApplicationDbContext : DbContext
{
    public DbSet<ExperimentDB> Experiments { get; set; }
    public DbSet<ScoreDB> Scores { get; set; }
    public DbSet<EmployeeDB> Employees { get; set; }
    public DbSet<WishlistDB> Wishlists { get; set; }
    public DbSet<TeamDB> Teams { get; set; }

    public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) : base(options)
    {
        Database.EnsureCreated();
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<ExperimentDB>().ToTable("Experiments");
        modelBuilder.Entity<ScoreDB>().ToTable("Scores");
        modelBuilder.Entity<EmployeeDB>().ToTable("Employees");
        modelBuilder.Entity<TeamDB>().ToTable("Teams");
        modelBuilder.Entity<WishlistDB>().ToTable("Wishlists")
            .HasMany(w => w.PreferredEmployees)
            .WithMany()
            .UsingEntity<Dictionary<string, object>>(
                "EmployeesInWishLists", // Название таблицы
                right => right.HasOne<EmployeeDB>().WithMany().HasForeignKey("EmployeeId"),
                left => left.HasOne<WishlistDB>().WithMany().HasForeignKey("WishlistId")
            );
        modelBuilder.Entity<EmployeeDB>()
            .OwnsOne(e => e.Employee);
    }
}