using Hackathon.Core.Api;
using Hackathon.Core.Api.Records;

namespace HrManagerService.Models;

public class HrManager(ITeamBuildingStrategy teamBuildingStrategy)
{
    public IEnumerable<Team> BuildTeams(List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists)
    {
        return teamBuildingStrategy.BuildTeams(teamLeadsWishlists, juniorsWishlists);
    }
}