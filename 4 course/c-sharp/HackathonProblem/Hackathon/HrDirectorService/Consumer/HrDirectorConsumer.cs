using HrDirectorService.Models.Data;
using HrDirectorService.Models.Logic;
using MassTransit;
using Microsoft.EntityFrameworkCore;
using Shared.Model.Mappers;
using Shared.Model.Messages;
using Shared.Model.Records;

namespace HrDirectorService.Consumer;

public class HrDirectorConsumer(
    ILogger<HrDirectorConsumer> logger,
    HrDirectorLogic hrDirector,
    Service.HrDirectorService hrDirectorService
) : IConsumer<GeneratingWishlist>, IConsumer<GeneratingTeam>
{
    private static readonly SemaphoreSlim _lock = new SemaphoreSlim(1, 1);

    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        logger.LogInformation("HrDirector received GeneratingWishlist");
        await hrDirectorService.SaveNewWishlist(context.Message);
        await ProcessingIfSufficientData(context.Message.ExperimentId, context.Message.HackathonCount, context);
    }

    public async Task Consume(ConsumeContext<GeneratingTeam> context)
    {
        logger.LogInformation($"HrDirector received GeneratingTeam");
        await hrDirectorService.SaveNewTeam(context.Message);
        await ProcessingIfSufficientData(context.Message.ExperimentId, context.Message.HackathonCount, context);
    }

    private async Task ProcessingIfSufficientData(int experimentId, int hackathonCount, IPublishEndpoint publisher)
    {
        var isSufficientData = await hrDirectorService.CheckingForSufficientData(experimentId, hackathonCount);
        if (!isSufficientData) return;

        var teams = await hrDirectorService.GetTeams(experimentId, hackathonCount);
        var employeesWishlists = await hrDirectorService.GetWishlists(experimentId, hackathonCount);

        var teamLeadsWishlists = employeesWishlists
            .Where(w => w.Employee.Type == EmployeeTypes.TeamLead)
            .ToList();

        var juniorsWishLists = employeesWishlists
            .Where(w => w.Employee.Type == EmployeeTypes.Junior)
            .ToList();

        var score = hrDirector.CalculateHarmonicMeanByTeams(
            teams.ToList(),
            teamLeadsWishlists,
            juniorsWishLists);

        await _lock.WaitAsync();
        try
        {
            isSufficientData = await hrDirectorService.CheckingForSufficientData(experimentId, hackathonCount);
            if (isSufficientData)
            {
                await PublishCalculatedScore(score, experimentId, hackathonCount, publisher);
                await hrDirectorService.DeleteUnusefulData(experimentId, hackathonCount);
            }
        }
        finally
        {
            _lock.Release();
        }
    }

    private async Task PublishCalculatedScore(double score, int experimentId, int hackathonCount, IPublishEndpoint publisher)
    {
        logger.LogInformation("HrDirector published CalculatedScore");
        await publisher.Publish<CalculatingScore>(new
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            Score = score,
        });
    }
}