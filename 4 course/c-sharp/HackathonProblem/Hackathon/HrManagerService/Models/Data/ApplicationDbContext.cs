using Hackathon.Core.Model.DataBase;
using Microsoft.EntityFrameworkCore;

namespace HrManagerService.Models.Data;

public class ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) : DbContext(options)
{
    public DbSet<EmployeeDB> Employees { get; set; }
    public DbSet<WishlistDB> Wishlists { get; set; }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<EmployeeDB>().ToTable("Employees");

        modelBuilder.Entity<WishlistDB>().ToTable("Wishlists")
            .HasMany(w => w.PreferredEmployees)
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