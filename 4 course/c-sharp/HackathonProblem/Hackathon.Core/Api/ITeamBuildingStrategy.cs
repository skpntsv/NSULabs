using Hackathon.Core.Api.Records;

namespace Hackathon.Core.Api;

public interface ITeamBuildingStrategy
{
    List<Team> BuildTeams(List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists);
}