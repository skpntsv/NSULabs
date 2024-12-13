using Shared.Model.Records;

namespace HrManager.Models.Logic;

public class HrManagerLogic(ITeamBuildingStrategy teamBuildingStrategy)
{
    
    public IEnumerable<Team> BuildTeams(IEnumerable<Wishlist> teamLeadsWishlists, IEnumerable<Wishlist> juniorsWishlists)
    {
        return teamBuildingStrategy.BuildTeams(teamLeadsWishlists, juniorsWishlists);
    }
}