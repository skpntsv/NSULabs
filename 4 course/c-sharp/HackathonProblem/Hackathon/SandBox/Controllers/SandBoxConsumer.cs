using MassTransit;
using Microsoft.EntityFrameworkCore;
using SandBox.Models.Data;
using Shared.Model.DataBase;
using Shared.Model.Mappers;
using Shared.Model.Messages;

namespace SandBox.Controllers;

public class SandBoxConsumer(ILogger<SandBoxConsumer> logger, ApplicationDbContext dbContext)
    : IConsumer<GeneratingWishlist>, IConsumer<GeneratingTeam>, IConsumer<CalculatingScore>
{
    private const int RequiredNumber = 5;
    private const int HackathonRepeats = 1;

    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        logger.LogInformation("SandBox received GeneratingWishlist");
        await SaveNewWishlist(dbContext, context.Message);
    }

    public async Task Consume(ConsumeContext<GeneratingTeam> context)
    {
        logger.LogInformation("SandBox received GeneratingTeam");

        await SaveNewTeam(dbContext, context.Message);
    }

    public async Task Consume(ConsumeContext<CalculatingScore> context)
    {
        logger.LogInformation("SandBox received CalculatingScore");
        await SaveNewScore(dbContext, context.Message);
    }

    private async Task SaveNewScore(ApplicationDbContext dbContext, CalculatingScore message)
    {
        var scoreDb = new ScoreDB
        {
            ExperimentId = message.ExperimentId,
            HackathonCount = message.HackathonCount,
            Score = message.Score,
        };
        await dbContext.Scores.AddRangeAsync(scoreDb);
        await dbContext.SaveChangesAsync();


        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;

        var totalScores = await dbContext.Scores
            .CountAsync(s => s.ExperimentId == experimentId);

        logger.LogInformation(
            $"SandBox experimentId = {message.ExperimentId} | hackathonCount = {hackathonCount} | totalScores = {totalScores} ");
        if (totalScores == HackathonRepeats)
        {
            await UpdateExperimentStatus(dbContext, experimentId, ExperimentStatus.Completed);
        }
        else
        {
            await UpdateExperimentStatus(dbContext, experimentId, ExperimentStatus.InProgress);
        }
    }

    private async Task UpdateExperimentStatus(ApplicationDbContext dbContext, int experimentId,
        ExperimentStatus experimentStatus)
    {
        var experiment = await dbContext.Experiments
            .FirstOrDefaultAsync(e => e.Id == experimentId);

        if (experiment != null)
        {
            experiment.Status = experimentStatus;
            await dbContext.SaveChangesAsync();
        }
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