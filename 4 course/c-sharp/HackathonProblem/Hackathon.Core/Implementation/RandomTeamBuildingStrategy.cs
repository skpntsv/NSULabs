using Hackathon.Core.Api;
using Hackathon.Core.Api.Records;

namespace Hackathon.Core.Implementation;

public class RandomTeamBuildingStrategy : ITeamBuildingStrategy
{
    public List<Team> BuildTeams(
        List<Wishlist> teamLeadsWishlists, 
        List<Wishlist> juniorsWishlists
    ) {
        List<Team> teams = [];
        var random = new Random();
        var leaderList = teamLeadsWishlists.Select(wishList => wishList.Employee).ToList();
        var juniorList = juniorsWishlists.Select(wishList => wishList.Employee).ToList();

        var shuffledJuniors = juniorList.OrderBy(i => random.Next()).ToList();
        var shuffledTeamLeads = leaderList.OrderBy(i => random.Next()).ToList();

        teams.AddRange(shuffledTeamLeads.Select((t, i) => new Team(t, shuffledJuniors[i])));
        return teams;
    }
}