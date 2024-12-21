using Hackathon.DataBase.Context;
using Hackathon.DataBase.Models;
using Hackathon.DataBase.Strategy;
using Microsoft.EntityFrameworkCore;

namespace Hackathon.DataBase;

class Program
{
    static void Main(string[] args)
    {
        var serviceProvider = new ServiceCollection()
            .AddDbContext<HackathonContext>(options =>
                options.UseSqlite("Data Source=hackathons.db"))
            .AddSingleton<HrManager>()
            .AddSingleton<HrDirector>()
            .BuildServiceProvider();

        using var context = serviceProvider.GetRequiredService<HackathonContext>();
        // context.Database.EnsureDeleted();
        context.Database.EnsureCreated();

        var teamLeadNames = Core.Models.CsvLoader.LoadEmployeeNamesFromCsv("../csvFiles/Teamleads20.csv");
        var teamLeads = new List<Employee>();
        foreach (var name in teamLeadNames)
        {
            var employee = new Employee { Name = name };
            context.Employees.Add(employee);
            teamLeads.Add(employee);
        }

        var juniorNames = Core.Models.CsvLoader.LoadEmployeeNamesFromCsv("../csvFiles/Juniors20.csv");
        var juniors = new List<Employee>();
        foreach (var name in juniorNames)
        {
            var employee = new Employee { Name = name };
            context.Employees.Add(employee);
            juniors.Add(employee);
        }

        context.SaveChanges();

        var hackathon = new Models.Hackathon
        {
            TeamLeadIds = teamLeads.Select(tl => tl.Id).ToList(),
            JuniorIds = juniors.Select(jr => jr.Id).ToList()
        };
        context.Hackathons.Add(hackathon);
        context.SaveChanges();

        var hrManager = serviceProvider.GetRequiredService<HrManager>();

        foreach (var teamLead in teamLeads)
        {
            var wishlist = teamLead.GenerateWishlist(juniors);
            context.Wishlists.Add(wishlist);
        }

        foreach (var junior in juniors)
        {
            var wishlist = junior.GenerateWishlist(teamLeads);
            context.Wishlists.Add(wishlist);
        }

        context.SaveChanges();

        var teamLeadWishlistIds = context.Wishlists
            .Where(w => teamLeads.Select(tl => tl.Id).Contains(w.EmployeeId))
            .Select(w => w.WishlistId).ToList();

        var juniorWishlistIds = context.Wishlists
            .Where(w => juniors.Select(jr => jr.Id).Contains(w.EmployeeId))
            .Select(w => w.WishlistId).ToList();

        foreach (var wishlistId in teamLeadWishlistIds)
        {
            hrManager.ReceiveWishlist(wishlistId, "TeamLead");
        }

        foreach (var wishlistId in juniorWishlistIds)
        {
            hrManager.ReceiveWishlist(wishlistId, "Junior");
        }

        ITeamBuildingStrategy strategy = new RandomTeamBuildingStrategy();
        var teams = hrManager.GenerateTeams(strategy, hackathon.HackathonId);

        foreach (var team in teams)
        {
            team.HackathonId = hackathon.HackathonId;
            context.Teams.Add(team);
        }

        context.SaveChanges();

        var hrDirector = serviceProvider.GetRequiredService<HrDirector>();
        hrManager.SendTeamsToHrDirector(teams.Select(t => t.TeamId).ToList(), hrDirector);

        hackathon.HarmonyScore = hrDirector.CalculateTeamsScore();
        context.SaveChanges();

        Console.WriteLine($"Хакатон ID: {hackathon.HackathonId}");
        Console.WriteLine($"Среднее гармоническое: {hackathon.HarmonyScore}");

        var averageHarmony = context.Hackathons.Average(h => h.HarmonyScore);
        Console.WriteLine($"Общее среднее гармоническое: {averageHarmony}");
    }
}