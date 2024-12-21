using Hackathon.Contracts;

namespace Hackathon.Strategies;

public class BuildingStrategy : ITeamBuildingStrategy
{
    public List<Team> BuildTeams(List<Employee> teamLeads,
        List<Employee> juniors,
        List<Wishlist> teamLeadsWishlists,
        List<Wishlist> juniorsWishlists)
    {
        var teams = new List<Team>();
        var assignedJuniors = new HashSet<int>();
        var assignedTeamLeads = new HashSet<int>();

        foreach (var lead in teamLeads)
        {
            var leadWishlist = teamLeadsWishlists.FirstOrDefault(w => w.EmployeeId == lead.Id);
            if (leadWishlist == null) continue;

            foreach (var desiredId in leadWishlist.DesiredEmployees)
            {
                if (assignedJuniors.Contains(desiredId)) continue;

                var junior = juniors.FirstOrDefault(j => j.Id == desiredId);
                if (junior != null && !assignedTeamLeads.Contains(lead.Id))
                {
                    teams.Add(new Team(lead, junior));
                    assignedTeamLeads.Add(lead.Id);
                    assignedJuniors.Add(junior.Id);
                    break;
                }
            }
        }

        return teams;
    }
}