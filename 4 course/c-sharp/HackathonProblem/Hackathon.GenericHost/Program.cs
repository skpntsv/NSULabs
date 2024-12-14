using Hackathon.Core.Models;
using Hackathon.Core.Strategy;
using Hackathon.Core.Strategy.Api;
using Hackathon.Core.Strategy.Implementation;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace Hackathon.GenericHost;

class Program
{
    static async Task Main(string[] args)
    {
        var host = Host.CreateDefaultBuilder(args)
            .ConfigureServices((hostContext, services) =>
            {
                var configuration = hostContext.Configuration;
                const string teamLeadsCsvPath = "../Resources/TeamLeads20.csv";
                const string juniorsCsvPath = "../../Resources/Juniors20.csv";

                services.AddHostedService<HackathonWorker>();
                services.AddSingleton<Hackathon.Core.Models.Hackathon>(_ =>
                    new Core.Models.Hackathon(teamLeadsCsvPath, juniorsCsvPath));
                services.AddSingleton<ITeamBuildingStrategy, RandomTeamBuildingStrategy>();
                services.AddScoped<HrManager>();
                services.AddTransient<HrDirector>();
            })
            .Build();

        _ = host.RunAsync();
        await host.StopAsync();
    }
}