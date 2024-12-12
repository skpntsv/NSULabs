using Hackathon.Core.Models;
using Hackathon.Core.Strategy;

namespace Hackathon.Tests;

public class HackathonTests
{
    [Fact]
    public void HackathonTest()
    {
        var teamLeads = new List<Employee>
        {
            new(1, "TeamLead 1")
        };
        var juniors = new List<Employee>
        {
            new(2, "Junior 1")
        };

        var hackathon = new Core.Models.Hackathon(teamLeads, juniors);
        var hrManager = new HrManager();
        var hrDirector = new HrDirector();

        var teamLeadWishlist = new Wishlist(1, [2]);
        var juniorWishlist = new Wishlist(2, [1]);

        hrManager.ReceiveWishlist(teamLeadWishlist, "TeamLead");
        hrManager.ReceiveWishlist(juniorWishlist, "Junior");

        var strategy = new RandomTeamBuildingStrategy();

        var teams = hrManager.GenerateTeams(strategy, hackathon);
        hrManager.SendTeamsToHrDirector(teams, hrDirector);
        var harmonicMean = hrDirector.CalculateTeamsScore();

        Assert.Equal(1, harmonicMean);
    }
}