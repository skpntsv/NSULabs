using HrDirectorService.Models.Data;
using HrDirectorService.Models.Logic;
using MassTransit;
using Microsoft.EntityFrameworkCore;
using Shared.Model.Mappers;
using Shared.Model.Messages;
using Shared.Model.Records;

namespace HrDirectorService.Controllers;

public class HrDirectorConsumer(
    ILogger<HrDirectorConsumer> _logger,
    ApplicationDbContext _dbContext,
    HrDirectorLogic _hrDirector)
    : IConsumer<GeneratingWishlist>, IConsumer<GeneratingTeam>
{
    private static readonly SemaphoreSlim _lock = new SemaphoreSlim(1, 1);
    private const int RequiredNumber = 5;

    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        _logger.LogInformation("HrDirector received GeneratingWishlist");
        await SaveNewWishlist(_dbContext, context.Message);
        await ProcessingIfSufficientData(_dbContext, context.Message.ExperimentId, context.Message.HackathonCount,
            context);
    }

    public async Task Consume(ConsumeContext<GeneratingTeam> context)
    {
        _logger.LogInformation($"HrDirector received GeneratingTeam");

        await SaveNewTeam(_dbContext, context.Message);
        await ProcessingIfSufficientData(_dbContext, context.Message.ExperimentId, context.Message.HackathonCount,
            context);
    }


    private async Task ProcessingIfSufficientData(ApplicationDbContext dbContext, int experimentId, int hackathonCount,
        IPublishEndpoint publisher)
    {
        bool isSufficientData = await CheckingForSufficientData(dbContext, experimentId, hackathonCount);
        if (!isSufficientData) return;

        var teams = await dbContext.Teams
            .Include(t => t.TeamLead)
            .Include(t => t.Junior)
            .Where(t => t.ExperimentId == experimentId && t.HackathonCount == hackathonCount)
            .Select(t => t.ToTeam())
            .ToListAsync();

        var employeesWishlists = await dbContext.Wishlists
            .Include(w => w.Employee)
            .Include(w => w.PreferredEmployees)
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .Select(w => w.ToWishlist())
            .ToListAsync();

        var teamLeadsWishlists = employeesWishlists
            .Where(w => w.Employee.Type == EmployeeTypes.TeamLead)
            .ToList();

        var juniorsWishLists = employeesWishlists
            .Where(w => w.Employee.Type == EmployeeTypes.Junior)
            .ToList();

        var score = _hrDirector.CalculateHarmonicMeanByTeams(teams.ToList(),
            teamLeadsWishlists,
            juniorsWishLists);


        //Если сначала пришли teams, потом одновременно пришли wishlists
        await _lock.WaitAsync();
        try
        {
            isSufficientData = await CheckingForSufficientData(dbContext, experimentId, hackathonCount);
            if (isSufficientData)
            {
                await PublishCalculatedScore(score, experimentId, hackathonCount, publisher);
                await DeleteUnusefulData(dbContext, experimentId, hackathonCount);
            }
        }
        finally
        {
            _lock.Release();
        }
    }

    private async Task PublishCalculatedScore(double score, int experimentId, int hackathonCount,
        IPublishEndpoint publisher)
    {
        _logger.LogInformation("HrDirector published CalculatedScore");
        await publisher.Publish<CalculatingScore>(new
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            Score = score,
        });
    }

    private async Task DeleteUnusefulData(ApplicationDbContext dbContext, int experimentId, int hackathonCount)
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

    private async Task<bool> CheckingForSufficientData(ApplicationDbContext dbContext, int experimentId,
        int hackathonCount)
    {
        var teamsNumber = await dbContext.Teams
            .CountAsync(t => t.ExperimentId == experimentId &&
                             t.HackathonCount == hackathonCount);

        var teamLeadsWishListsNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId &&
                             w.HackathonCount == hackathonCount &&
                             w.Employee.Employee.Type == EmployeeTypes.TeamLead);

        var juniorsWishlistsNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId &&
                             w.HackathonCount == hackathonCount &&
                             w.Employee.Employee.Type == EmployeeTypes.Junior);

        return teamLeadsWishListsNumber == RequiredNumber && juniorsWishlistsNumber == RequiredNumber &&
               teamsNumber == RequiredNumber;
    }

    private async Task SaveNewWishlist(ApplicationDbContext dbContext, GeneratingWishlist message)
    {
        var wishlistDb = message.Wishlist.ToWishlistDB(message.ExperimentId, message.HackathonCount);
        await dbContext.Wishlists.AddRangeAsync(wishlistDb);
        await dbContext.SaveChangesAsync();
    }

    private async Task SaveNewTeam(ApplicationDbContext dbContext, GeneratingTeam message)
    {
        var teamsDb = message.Teams.Select(t => t.ToTeamDB(message.ExperimentId, message.HackathonCount));
        await dbContext.Teams.AddRangeAsync(teamsDb);
        await dbContext.SaveChangesAsync();
    }
}