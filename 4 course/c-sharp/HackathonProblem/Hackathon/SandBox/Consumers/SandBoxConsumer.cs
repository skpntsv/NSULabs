using MassTransit;
using SandBox.Services;
using Shared.Model.Messages;
using Shared.Model.DataBase;

namespace SandBox.Consumers;

public class SandBoxConsumer(
    ILogger<SandBoxConsumer> logger,
    ExperimentSaveService saveService,
    ExperimentFetchService fetchService,
    IPublishEndpoint publishEndpoint
) : IConsumer<GeneratingWishlist>, IConsumer<GeneratingTeam>, IConsumer<CalculatingScore>
{
    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        logger.LogInformation("Получено сообщение GeneratingWishlist");
        await saveService.SaveNewWishlistAsync(context.Message);
    }

    public async Task Consume(ConsumeContext<GeneratingTeam> context)
    {
        logger.LogInformation("Получено сообщение GeneratingTeam");
        await saveService.SaveNewTeamAsync(context.Message);
    }

    public async Task Consume(ConsumeContext<CalculatingScore> context)
    {
        logger.LogInformation("Получено сообщение CalculatingScore");
        await saveService.SaveNewScoreAsync(context.Message);

        // Проверка количества завершённых итераций
        var experimentId = context.Message.ExperimentId;
        var totalScores = await fetchService.GetTotalScoresAsync(experimentId);
        var hackathonCount = await fetchService.GetTotalCountOfHackathonsAsync(experimentId);
        
        if (totalScores == hackathonCount)
        {
            await saveService.UpdateExperimentStatusAsync(experimentId, ExperimentStatus.Completed);
            logger.LogInformation($"Эксперимент ID {experimentId} завершён.");
        }
        else if (totalScores > hackathonCount)
        {
            logger.LogError($"Эксперимент ID {experimentId} завершён с ошибкой.");
            await saveService.UpdateExperimentStatusAsync(experimentId, ExperimentStatus.Failed);
        }
        else
        {
            await saveService.UpdateExperimentStatusAsync(experimentId, ExperimentStatus.InProgress);

            // Запуск нового хакатона
            logger.LogInformation(
                $"Запуск нового хакатона для эксперимента ID {experimentId}, итерация {totalScores}"
            );

            await publishEndpoint.Publish(new StartingHackathon
            {
                ExperimentId = experimentId,
                HackathonNumber = totalScores
            });
        }
    }
}