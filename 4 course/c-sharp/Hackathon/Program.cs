using Hackathon.Contracts;
using Hackathon.Core;
using Hackathon.HR;
using Hackathon.Strategies;
using Hackathon.Workers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices((_, services) =>
    {
        services.AddHostedService<SandBoxWorker>();

        services.AddTransient<SandBox>();
        services.AddTransient<Hackathon.Core.Hackathon>();
        services.AddTransient<ITeamBuildingStrategy, HungarianStrategy>();
        services.AddTransient<HrManager>();
        services.AddTransient<HrDirector>();
    }).Build();

host.Run();