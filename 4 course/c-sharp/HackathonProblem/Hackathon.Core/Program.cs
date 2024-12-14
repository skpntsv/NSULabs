using Hackathon.Core.Models;
using Hackathon.Core.Strategy;
using Hackathon.Core.Strategy.Api;
using Hackathon.Core.Strategy.Implementation;

namespace Hackathon.Core;

class Program
{
    static void Main(string[] args)
    {
        string teamLeadsCsvPath = "../csvFiles/Resources/TeamLeads20.csv";
        string juniorsCsvPath = "../csvFiles/Resources/Juniors20.csv";

        Models.Hackathon hackathon = new(teamLeadsCsvPath, juniorsCsvPath);

        HrManager hrManager = new();
        HrDirector hrDirector = new();

        var harmonicGlobalMean = 0.0;
        var countIteration = 1000;
        for (var i = 0; i < countIteration; i++) {
            foreach (Employee teamLead in hackathon.TeamLeads) {
                Wishlist wishlist = teamLead.GenerateWishlist(hackathon.Juniors.ToList());
                teamLead.SendWishlistToHrManager(wishlist, "TeamLead", hrManager);
            }
            foreach (Employee junior in hackathon.Juniors) {
                Wishlist wishlist = junior.GenerateWishlist(hackathon.TeamLeads.ToList());
                junior.SendWishlistToHrManager(wishlist, "Junior", hrManager);
            }
                
            ITeamBuildingStrategy strategy = new SmartBuildingStrategy();
            List<Team> teams = hrManager.GenerateTeams(strategy, hackathon);
            hrManager.SendTeamsToHrDirector(teams, hrDirector);
            var harmonicMean = hrDirector.CalculateTeamsScore();

            Console.WriteLine($"Harmonic Mean of Satisfaction: {harmonicMean}");

            harmonicGlobalMean += harmonicMean;
            hrManager.ClearAllWishlists();
        }
        harmonicGlobalMean /= countIteration;
        Console.WriteLine($"Harmonic Global Mean of Satisfaction: {harmonicGlobalMean}");
    }
}