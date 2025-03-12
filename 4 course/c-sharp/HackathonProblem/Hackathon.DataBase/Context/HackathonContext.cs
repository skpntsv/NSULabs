using Hackathon.DataBase.Models;
using Microsoft.EntityFrameworkCore;

namespace Hackathon.DataBase.Context;

public class HackathonContext : DbContext
{
    public HackathonContext(DbContextOptions<HackathonContext> options) : base(options) { }

    public DbSet<Employee> Employees { get; set; } = null!;
    public DbSet<Models.Hackathon> Hackathons { get; set; } = null!;
    public DbSet<Team> Teams { get; set; } = null!;
    public DbSet<Wishlist> Wishlists { get; set; } = null!;
}