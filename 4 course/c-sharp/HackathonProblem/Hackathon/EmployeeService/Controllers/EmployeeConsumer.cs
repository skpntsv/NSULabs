using EmployeeService.Models;
using MassTransit;
using Microsoft.Extensions.Options;
using Shared.Model.Messages;
using Shared.Model.Records;
using Options = EmployeeService.Models.Options;


namespace EmployeeService.Controllers;

public class EmployeeConsumer : IConsumer<StartingHackathon>
{
    private readonly Employee _thisEmployee;
    private readonly IEnumerable<Employee> _coworkers;

    private readonly ILogger<EmployeeConsumer> _logger;
    private readonly IEnumerable<Employee> _juniors;
    private readonly IEnumerable<Employee> _teamLeads;

    public EmployeeConsumer(ILogger<EmployeeConsumer> logger, EmployeesReader reader, IOptions<Options> options)
    {
        _logger = logger;
        if (!Enum.TryParse<EmployeeTypes>(options.Value.EmployeeType, true, out var employeeType))
        {
            throw new InvalidOperationException("Invalid Employee type " + options.Value.EmployeeType);
        }

        _teamLeads = reader.ReadTeamLeads(options.Value.TeamLeadsFile);
        _juniors = reader.ReadJuniors(options.Value.JuniorsFile);

        var thisEmployeeId = options.Value.EmployeeId;

        var employeesWithTheSameType = employeeType.Equals(EmployeeTypes.TeamLead) ? _teamLeads : _juniors;

        _thisEmployee = employeesWithTheSameType.FirstOrDefault(x => x.Id == thisEmployeeId) ??
                        throw new InvalidOperationException("Uncorrected EmployeeId");
        _coworkers = employeeType.Equals(EmployeeTypes.TeamLead) ? _juniors : _teamLeads;
    }

    public async Task Consume(ConsumeContext<StartingHackathon> context)
    {
        _logger.LogInformation(
            $"{_thisEmployee.Type.ToString()} {_thisEmployee.Name} (Id = {_thisEmployee.Id}) received StartingHackathon message: ExperimentId = {context.Message.ExperimentId}, HackathonCout = {context.Message.HackathonCount}");

        await context.Publish<GeneratingWishlist>(new
        {
            context.Message.ExperimentId,
            context.Message.HackathonCount,
            Wishlist = _thisEmployee.GenerateWishList(_coworkers),
        });
    }
}