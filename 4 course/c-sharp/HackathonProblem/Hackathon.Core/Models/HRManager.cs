using Hackathon.Core.Strategy;

namespace Hackathon.Core.Models
{
    public class HrManager
    {
        private readonly List<Wishlist> juniorWishlists = [];
        private readonly List<Wishlist> teamLeadWishlists = [];
        private readonly ITeamBuildingStrategy? strategyDI;

        public HrManager(ITeamBuildingStrategy strategy) {
            strategyDI = strategy;
        }

        public HrManager() {
        }


        public void ReceiveWishlist(Wishlist wishlist, string type)
        {
            if (type.Equals("Junior")) {
                juniorWishlists.Add(wishlist);
            } else {
                teamLeadWishlists.Add(wishlist);
            }
        }

        public List<Team> GenerateTeams(ITeamBuildingStrategy teamBuildingStrategy, Hackathon hackaton)
        {
            return (List<Team>)teamBuildingStrategy.BuildTeams(
                hackaton.TeamLeads,
                hackaton.Juniors,
                teamLeadWishlists,
                juniorWishlists
            );
        }

        public List<Team> GenerateTeams(Hackathon hackaton)
        {
            return (List<Team>)strategyDI!.BuildTeams(
                hackaton.TeamLeads,
                hackaton.Juniors,
                teamLeadWishlists,
                juniorWishlists
            );
        }

        public void SendTeamsToHrDirector(List<Team> teams, HrDirector hrDirector)
        {
            var allWishlists = teamLeadWishlists.Concat(juniorWishlists).ToList();
            hrDirector.ReceiveTeamsAndWishlists(teams, allWishlists);
        }

        public void ClearAllWishlists() {
            juniorWishlists.Clear();
            teamLeadWishlists.Clear();
        }
    }
}
