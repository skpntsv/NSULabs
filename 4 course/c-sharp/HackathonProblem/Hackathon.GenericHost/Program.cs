using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Hackathon.Core;

namespace Hackathon.GenericHost;

class Program
{
    static async Task Main(string[] args)
    {
        var host = Host.CreateDefaultBuilder(args)
            .ConfigureServices((hostContext, services) =>
            {
                var configuration = hostContext.Configuration;
                string teamLeadsCsvPath = configuration["TeamLeadsCsvPath"] ??
                                          throw new FileLoadException("TeamLeadsCsvPath not found");
                string juniorsCsvPath = configuration["JuniorsCsvPath"] ??
                                        throw new FileLoadException("JuniorsCsvPath not found");

                services.AddHostedService<HackathonWorker>();
                services.AddSingleton<Hackathon.Core.Models.Hackathon>(_ =>
                    new Hackathon(teamLeadsCsvPath, juniorsCsvPath));
                services.AddSingleton<ITeamBuildingStrategy, RandomTeamBuildingStrategy>();
                services.AddScoped<HrManager>();
                services.AddTransient<HrDirector>();
            })
            .Build();

        _ = host.RunAsync();
        await host.StopAsync();
    }
}