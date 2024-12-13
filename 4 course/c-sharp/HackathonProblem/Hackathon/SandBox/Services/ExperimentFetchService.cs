using Microsoft.EntityFrameworkCore;
using SandBox.Models.Data;
using Shared.Model.DataBase;

namespace SandBox.Services;

public class ExperimentFetchService(ApplicationDbContext dbContext, ILogger<ExperimentFetchService> logger)
{
    public async Task<ExperimentDB?> GetExperimentByIdAsync(int experimentId)
    {
        logger.LogInformation($"Поиск эксперимента с ID {experimentId}...");
        var experiment = await dbContext.Experiments.FirstOrDefaultAsync(e => e.Id == experimentId);

        if (experiment == null)
        {
            logger.LogWarning($"Эксперимент с ID {experimentId} не найден.");
        }

        return experiment;
    }

    public async Task<List<ScoreDB>> GetScoresAsync(int experimentId)
    {
        logger.LogInformation($"Получение scores для эксперимента ID {experimentId}...");

        var scoresQuery = dbContext.Scores.Where(s => s.ExperimentId == experimentId);

        var scores = await scoresQuery.ToListAsync();
        logger.LogInformation($"Найдено {scores.Count} scores для эксперимента ID {experimentId}.");
        return scores;
    }

    public async Task<List<TeamDB>> GetTeamsAsync(int experimentId)
    {
        logger.LogInformation($"Получение команд для эксперимента ID {experimentId}...");

        var teamsQuery = dbContext.Teams.Where(t => t.ExperimentId == experimentId);

        var teams = await teamsQuery
            .Include(t => t.TeamLead)
            .Include(t => t.Junior)
            .ToListAsync();

        logger.LogInformation($"Найдено {teams.Count} команд для эксперимента ID {experimentId}.");
        return teams;
    }

    public async Task<double?> CalculateAverageScoreAsync(int experimentId)
    {
        logger.LogInformation($"Расчёт среднего score для эксперимента ID {experimentId}...");

        var scores = await dbContext.Scores
            .Where(s => s.ExperimentId == experimentId)
            .ToListAsync();

        if (!scores.Any())
        {
            logger.LogWarning($"Для эксперимента ID {experimentId} отсутствуют данные о score.");
            return null;
        }

        var averageScore = scores.Average(s => s.Score);
        logger.LogInformation($"Средний score для эксперимента ID {experimentId}: {averageScore}");
        return averageScore;
    }
    
    public async Task<int> GetTotalScoresAsync(int experimentId)
    {
        return await dbContext.Scores.CountAsync(s => s.ExperimentId == experimentId);
    }
    
    public async Task<int> GetTotalCountOfHackathonsAsync(int experimentId)
    {
        return await dbContext.Experiments
            .Where(e => e.Id == experimentId)
            .Select(e => e.HackathonCount)
            .FirstOrDefaultAsync();
    }
}