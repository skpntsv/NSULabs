using EmployeeService.Models;
using MassTransit;
using Microsoft.Extensions.Options;
using Shared.Model.Messages;
using Shared.Model.Records;
using Options = EmployeeService.Models.Options;

namespace EmployeeService.Consumers;

public class EmployeeConsumer : IConsumer<StartingHackathon>
{
    private readonly Employee _thisEmployee;
    private readonly List<Employee> _coworkers;

    private readonly ILogger<EmployeeConsumer> _logger;

    public EmployeeConsumer(ILogger<EmployeeConsumer> logger, EmployeesReader reader, IOptions<Options> options)
    {
        _logger = logger;
        if (!Enum.TryParse<EmployeeTypes>(options.Value.EmployeeType, true, out var employeeType))
        {
            throw new InvalidOperationException("Invalid Employee type " + options.Value.EmployeeType);
        }

        var teamLeads = reader.ReadTeamLeads(options.Value.TeamLeadsFile);
        var juniors = reader.ReadJuniors(options.Value.JuniorsFile);

        var thisEmployeeId = options.Value.EmployeeId;

        var employeesWithTheSameType = employeeType.Equals(EmployeeTypes.TeamLead) ? teamLeads : juniors;

        _thisEmployee = employeesWithTheSameType.FirstOrDefault(x => x.Id == thisEmployeeId) ??
                        throw new InvalidOperationException("Uncorrected EmployeeId");
        _coworkers = employeeType.Equals(EmployeeTypes.TeamLead) ? juniors : teamLeads;
    }

    public async Task Consume(ConsumeContext<StartingHackathon> context)
    {
        _logger.LogInformation(
            $"{_thisEmployee.Type.ToString()} {_thisEmployee.Name} (Id = {_thisEmployee.Id}) received StartingHackathon message: ExperimentId = {context.Message.ExperimentId}, HackathonCout = {context.Message.HackathonNumber}");

        await context.Publish<GeneratingWishlist>(new
        {
            context.Message.ExperimentId,
            HackathonCount = context.Message.HackathonNumber,
            Wishlist = _thisEmployee.GenerateWishList(_coworkers),
        });
    }
}