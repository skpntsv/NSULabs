using Hackathon.Core;
using Microsoft.Extensions.Hosting;

namespace Hackathon.Workers;

class SandBoxWorker(IHostApplicationLifetime host, SandBox sandBox) : BackgroundService
{
    protected override Task ExecuteAsync(CancellationToken cancellationToken)
    {
        sandBox.Start();
        host.StopApplication();
        return Task.CompletedTask;
    }
}