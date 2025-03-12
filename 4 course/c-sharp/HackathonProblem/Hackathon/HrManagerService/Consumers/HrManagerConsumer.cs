using Hackathon.Core.Api.Messages;
using Hackathon.Core.Api.Records;
using Hackathon.Core.Implementation;
using Hackathon.Core.Model.Mappers;
using MassTransit;
using HrManager = HrManagerService.Models.HrManager;

namespace HrManagerService.Consumers;

public class HrManagerConsumer(
    ILogger<HrManagerConsumer> logger,
    Services.HrManagerService hrManagerService,
    HrManager hrManager) : IConsumer<GeneratingWishlist>
{
    private const int RequiredNumber = 5;
    private static readonly SemaphoreSlim _lock = new SemaphoreSlim(1, 1);

    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        logger.LogInformation("HrManager received GeneratingWishlist message");

        var message = context.Message;
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        logger.LogInformation(
            $"ExperimentId = {experimentId} | HackathonCount = {hackathonCount} | Wishlist = {message.Wishlist}");
        await hrManagerService.SaveNewWishlist(message);

        var teamLeadsCount = await hrManagerService.GetEmployeeNumber(message, EmployeeTypes.TeamLead);
        var juniorsCount = await hrManagerService.GetEmployeeNumber(message, EmployeeTypes.Junior);

        var isWishlistsEnough = (teamLeadsCount == RequiredNumber && juniorsCount == RequiredNumber);
        logger.LogInformation(
            $"ExperimentId = {experimentId} | HackathonCount = {hackathonCount} | teamLeadCount = {teamLeadsCount} | JuniorCount = {juniorsCount}");
        if (isWishlistsEnough)
        {
            await _lock.WaitAsync();
            try
            {
                teamLeadsCount = await hrManagerService.GetEmployeeNumber(message, EmployeeTypes.TeamLead);
                juniorsCount = await hrManagerService.GetEmployeeNumber(message, EmployeeTypes.Junior);
                isWishlistsEnough = (teamLeadsCount == RequiredNumber && juniorsCount == RequiredNumber);
                if (isWishlistsEnough)
                {
                    await PublishGeneratingTeam(context, message);
                    await hrManagerService.DeleteWishListsAndEmployees(message);
                }
            }
            finally
            {
                _lock.Release();
            }
        }
    }

    private async Task PublishGeneratingTeam(IPublishEndpoint publisher, GeneratingWishlist message)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;

        var employeesWishlists = await hrManagerService.GetWishlistsWithEmployees(message);

        var teamLeadsWishlists = employeesWishlists
            .Where(w => w.Employee.Employee.Type == EmployeeTypes.TeamLead)
            .Select(w => w.ToWishlist())
            .ToList();

        var juniorsWishLists = employeesWishlists
            .Where(w => w.Employee.Employee.Type == EmployeeTypes.Junior)
            .Select(w => w.ToWishlist())
            .ToList();

        await publisher.Publish<GeneratingTeam>(new
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            Teams = hrManager.BuildTeams(teamLeadsWishlists, juniorsWishLists),
        });
    }
}