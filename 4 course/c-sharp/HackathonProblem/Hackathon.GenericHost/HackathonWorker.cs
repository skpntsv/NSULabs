using Hackathon.Core.Models;
using Microsoft.Extensions.Hosting;

namespace Hackathon.GenericHost;

public class HackathonWorker : IHostedService
{
    private readonly Core.Models.Hackathon hackathon;
    private readonly HrManager hrManager;
    private readonly HrDirector hrDirector;

    public HackathonWorker(Core.Models.Hackathon hackathon, HrManager hrManager, HrDirector hrDirector)
    {
        this.hackathon = hackathon;
        this.hrManager = hrManager;
        this.hrDirector = hrDirector;
    }

    public Task StartAsync(CancellationToken cancellationToken)
    {
        double harmonicGlobalMean = 0;
        int countIteration = 1000;
        for (int i = 0; i < countIteration; i++)
        {
            foreach (Employee teamLead in hackathon.TeamLeads)
            {
                Wishlist wishlist = teamLead.GenerateWishlist(hackathon.Juniors);
                teamLead.SendWishlistToHrManager(wishlist, "TeamLead", hrManager);
            }

            foreach (Employee junior in hackathon.Juniors)
            {
                Wishlist wishlist = junior.GenerateWishlist(hackathon.TeamLeads);
                junior.SendWishlistToHrManager(wishlist, "Junior", hrManager);
            }

            List<Team> teams = hrManager.GenerateTeams(hackathon);
            hrManager.SendTeamsToHrDirector(teams, hrDirector);
            double harmonicMean = hrDirector.CalculateTeamsScore();

            Console.WriteLine($"Harmonic Mean of Satisfaction: {harmonicMean}");

            harmonicGlobalMean += harmonicMean;
            hrManager.ClearAllWishlists();
        }
        harmonicGlobalMean /= countIteration;
        Console.WriteLine($"Harmonic Global Mean of Satisfaction: {harmonicGlobalMean}");

        return Task.CompletedTask;
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        return Task.CompletedTask;
    }
}