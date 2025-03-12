using Hackathon.Contracts;

namespace Hackathon.HR;

public class HrManager(ITeamBuildingStrategy strategy)
{
    public List<Team> BuildTeams(List<Employee> teamLeads, List<Employee> juniors,
        List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists)
    {
        return strategy.BuildTeams(teamLeads, juniors, teamLeadsWishlists, juniorsWishlists);
    }
}