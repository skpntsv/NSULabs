using Hackathon.Contracts;
using Hackathon.HR;

namespace HackathonTest
{
    public class HrManagerTests
    {
        [Fact]
        public void GenerateTeamsNumberOfTeams()
        {
            var hrManager = new HrManager();
            var teamLeads = new List<Employee>
            {
                new(1, "TeamLead 1"),
                new(2, "TeamLead 2")
            };
            var juniors = new List<Employee>
            {
                new(3, "Junior 1"),
                new(4, "Junior 2")
            };
            var hackathon = new Hackathon(teamLeads, juniors);

            var strategy = new RandomTeamBuildingStrategy();

            var teams = hrManager.GenerateTeams(strategy, hackathon);

            Assert.Equal(2, teams.Count);
        }

        [Fact]
        public void GenerateTeamsReturnsExpectedDistribution()
        {
            var hrManager = new HrManager();
            var teamLeads = new List<Employee>
            {
                new(1, "TeamLead 1"),
                new(2, "TeamLead 2")
            };
            var juniors = new List<Employee>
            {
                new(3, "Junior 1"),
                new(4, "Junior 2")
            };
            var hackathon = new Hackathon(teamLeads, juniors);

            var teamLeadWishlists = new List<Wishlist>
            {
                new(1, [3, 4]),
                new(2, [4, 3])
            };
            var juniorWishlists = new List<Wishlist>
            {
                new(3, [1, 2]),
                new(4, [2, 1])
            };

            foreach (var wishlist in teamLeadWishlists)
            {
                hrManager.ReceiveWishlist(wishlist, "TeamLead");
            }
            foreach (var wishlist in juniorWishlists)
            {
                hrManager.ReceiveWishlist(wishlist, "Junior");
            }

            var strategy = new MegaTeamBuildingStrategy();
            var teams = hrManager.GenerateTeams(strategy, hackathon);

            Assert.Equal(2, teams.Count);
            Assert.Contains(teams, t => t.TeamLead.Id == 1 && t.Junior.Id == 3);
            Assert.Contains(teams, t => t.TeamLead.Id == 2 && t.Junior.Id == 4);
        }

        [Fact]
        public void GenerateTeamsStrategyCalledOnce()
        {
            var hrManager = new HrManager();
            var teamLeads = new List<Employee>();
            var juniors = new List<Employee>();
            var hackathon = new Hackathon(teamLeads, juniors);

            var mockStrategy = new Mock<ITeamBuildingStrategy>();
            mockStrategy.Setup(s => s.BuildTeams(
                It.IsAny<List<Employee>>(),
                It.IsAny<List<Employee>>(),
                It.IsAny<List<Wishlist>>(),
                It.IsAny<List<Wishlist>>()))
                .Returns(new List<Team>());

            hrManager.GenerateTeams(mockStrategy.Object, hackathon);

            mockStrategy.Verify(s => s.BuildTeams(
                It.IsAny<List<Employee>>(),
                It.IsAny<List<Employee>>(),
                It.IsAny<List<Wishlist>>(),
                It.IsAny<List<Wishlist>>()), Times.Once);
        }
    }
}
