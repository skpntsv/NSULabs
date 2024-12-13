using HrDirectorService.Models.Data;
using Microsoft.EntityFrameworkCore;
using Shared.Model.Mappers;
using Shared.Model.Messages;
using Shared.Model.Records;

namespace HrDirectorService.Service;

public class HrDirectorService(ApplicationDbContext dbContext)
{
    private const int RequiredNumber = 5;

    public async Task SaveNewWishlist(GeneratingWishlist message)
    {
        var wishlistDb = message.Wishlist.ToWishlistDB(message.ExperimentId, message.HackathonCount);
        await dbContext.Wishlists.AddRangeAsync(wishlistDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task SaveNewTeam(GeneratingTeam message)
    {
        var teamsDb = message.Teams.Select(t => t.ToTeamDB(message.ExperimentId, message.HackathonCount));
        await dbContext.Teams.AddRangeAsync(teamsDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task<bool> CheckingForSufficientData(int experimentId, int hackathonCount)
    {
        var teamsNumber = await dbContext.Teams
            .CountAsync(t => t.ExperimentId == experimentId && t.HackathonCount == hackathonCount);

        var teamLeadsWishListsNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount &&
                             w.Employee.Employee.Type == EmployeeTypes.TeamLead);

        var juniorsWishlistsNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount &&
                             w.Employee.Employee.Type == EmployeeTypes.Junior);

        return teamLeadsWishListsNumber == RequiredNumber && juniorsWishlistsNumber == RequiredNumber &&
               teamsNumber == RequiredNumber;
    }

    public async Task<List<Team>> GetTeams(int experimentId, int hackathonCount)
    {
        return await dbContext.Teams
            .Include(t => t.TeamLead)
            .Include(t => t.Junior)
            .Where(t => t.ExperimentId == experimentId && t.HackathonCount == hackathonCount)
            .Select(t => t.ToTeam())
            .ToListAsync();
    }

    public async Task<List<Wishlist>> GetWishlists(int experimentId, int hackathonCount)
    {
        return await dbContext.Wishlists
            .Include(w => w.Employee)
            .Include(w => w.PreferredEmployees)
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .Select(w => w.ToWishlist())
            .ToListAsync();
    }

    public async Task DeleteUnusefulData(int experimentId, int hackathonCount)
    {
        var teamsToDelete = await dbContext.Teams
            .Where(t => t.ExperimentId == experimentId && t.HackathonCount == hackathonCount)
            .ToListAsync();

        var wishlistsToDelete = await dbContext.Wishlists
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .ToListAsync();

        var employeesToDelete = await dbContext.Employees
            .Where(e => e.ExperimentId == experimentId && e.HackathonCount == hackathonCount)
            .ToListAsync();

        dbContext.RemoveRange(teamsToDelete);
        dbContext.RemoveRange(wishlistsToDelete);
        dbContext.RemoveRange(employeesToDelete);
        await dbContext.SaveChangesAsync();
    }
}
