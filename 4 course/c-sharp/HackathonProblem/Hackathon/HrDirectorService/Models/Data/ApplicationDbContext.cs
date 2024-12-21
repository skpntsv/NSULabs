using Hackathon.Core.Model.DataBase;
using Microsoft.EntityFrameworkCore;

namespace HrDirectorService.Models.Data;

public class ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) : DbContext(options)
{
    public DbSet<EmployeeDB> Employees { get; set; }
    public DbSet<WishlistDB> Wishlists { get; set; }
    public DbSet<TeamDB> Teams { get; set; }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<EmployeeDB>().ToTable("Employees").HasKey(e => e.Id);
        modelBuilder.Entity<TeamDB>().ToTable("Teams").HasKey(t => t.Id);
        modelBuilder.Entity<WishlistDB>().ToTable("Wishlists").HasKey(w => w.Id);
        
        modelBuilder.Entity<WishlistDB>().HasMany(w => w.PreferredEmployees)
            .WithMany()
            .UsingEntity<Dictionary<string, object>>(
                "EmployeesInWishLists", // Название таблицы
                right => right
                    .HasOne<EmployeeDB>()
                    .WithMany()
                    .HasForeignKey("EmployeeId")
                    .OnDelete(DeleteBehavior.Cascade),
                left => left
                    .HasOne<WishlistDB>()
                    .WithMany()
                    .HasForeignKey("WishlistId")
                    .OnDelete(DeleteBehavior.Cascade)
            );
        modelBuilder.Entity<EmployeeDB>().OwnsOne(e => e.Employee);
    }
}