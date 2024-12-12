namespace Hackathon.DataBase.Strategy;

public class RandomTeamBuildingStrategy : ITeamBuildingStrategy
{
    public IEnumerable<Team> BuildTeams(
        IEnumerable<Employee> teamLeads,
        IEnumerable<Employee> juniors,
        IEnumerable<Wishlist> teamLeadsWishlists,
        IEnumerable<Wishlist> juniorsWishlists)
    {
        var random = new Random();
        var shuffledTeamLeads = teamLeads.OrderBy(_ => random.Next()).ToList();
        var shuffledJuniors = juniors.OrderBy(_ => random.Next()).ToList();

        var teams = new List<Team>();
        int teamCount = Math.Min(shuffledTeamLeads.Count, shuffledJuniors.Count);

        for (int i = 0; i < teamCount; i++)
        {
            teams.Add(new Team(shuffledTeamLeads[i].Id, shuffledJuniors[i].Id));
        }
        return teams;
    }
}