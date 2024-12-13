using Microsoft.EntityFrameworkCore;
using SandBox.Models.Data;
using Shared.Model.DataBase;
using Shared.Model.Mappers;
using Shared.Model.Messages;

namespace SandBox.Services;

public class ExperimentSaveService(ApplicationDbContext dbContext, ILogger<ExperimentSaveService> logger)
{
    public async Task<ExperimentDB> CreateExperimentAsync(int hackathonRepeats)
    {
        logger.LogInformation("Создание нового эксперимента...");
        var experiment = new ExperimentDB
        {
            HackathonCount = hackathonRepeats,
            Status = ExperimentStatus.Opened
        };

        dbContext.Add(experiment);
        await dbContext.SaveChangesAsync();

        logger.LogInformation($"Эксперимент с ID {experiment.Id} создан.");
        return experiment;
    }
    
    public async Task SaveNewScoreAsync(CalculatingScore message)
    {
        var scoreDb = new ScoreDB
        {
            ExperimentId = message.ExperimentId,
            HackathonCount = message.HackathonCount,
            Score = message.Score,
        };
        await dbContext.Scores.AddAsync(scoreDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task SaveNewWishlistAsync(GeneratingWishlist message)
    {
        var wishlistDb = message.Wishlist.ToWishlistDB(message.ExperimentId, message.HackathonCount);
        await dbContext.Wishlists.AddRangeAsync(wishlistDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task SaveNewTeamAsync(GeneratingTeam message)
    {
        var teamsDb = message.Teams.Select(t => t.ToTeamDB(message.ExperimentId, message.HackathonCount));
        await dbContext.Teams.AddRangeAsync(teamsDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task UpdateExperimentStatusAsync(int experimentId, ExperimentStatus status)
    {
        var experiment = await dbContext.Experiments.FirstOrDefaultAsync(e => e.Id == experimentId);
        if (experiment != null)
        {
            experiment.Status = status;
            await dbContext.SaveChangesAsync();
        }
    }
}