using Hackathon.Core.Models;

namespace Hackathon.Core.Strategy 
{
    public class MegaTeamBuildingStrategy : ITeamBuildingStrategy
    {
        public IEnumerable<Team> BuildTeams(
            IEnumerable<Employee> teamLeads,
            IEnumerable<Employee> juniors,
            IEnumerable<Wishlist> teamLeadsWishlists,
            IEnumerable<Wishlist> juniorsWishlists
        ) {
            var teamLeadsList = teamLeads.ToList();
            var juniorsList = juniors.ToList();

            int n = teamLeadsList.Count;
            int m = juniorsList.Count;

            if (n != m) {
                throw new Exception("Количество Тимлидов и Джунов должно совпадать!");
            }

            var teamLeadIdToIndex = teamLeadsList.Select((e, idx) => new { e.Id, idx })
                                                .ToDictionary(x => x.Id, x => x.idx);
            var juniorIdToIndex = juniorsList.Select((e, idx) => new { e.Id, idx })
                                                .ToDictionary(x => x.Id, x => x.idx);

            var teamLeadWishlists = teamLeadsWishlists.ToDictionary(w => w.EmployeeId, w => w.DesiredEmployees);
            var juniorWishlists = juniorsWishlists.ToDictionary(w => w.EmployeeId, w => w.DesiredEmployees);

            int[,] costMatrix = new int[n, m];

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    var teamLead = teamLeadsList[i];
                    var junior = juniorsList[j];

                    var teamLeadWishlist = teamLeadWishlists.ContainsKey(teamLead.Id) ? teamLeadWishlists[teamLead.Id] : [];
                    var juniorWishlist = juniorWishlists.ContainsKey(junior.Id) ? juniorWishlists[junior.Id] : [];

                    int teamLeadPreference = GetPreferenceScore(teamLeadWishlist, junior.Id);
                    int juniorPreference = GetPreferenceScore(juniorWishlist, teamLead.Id);

                    int totalPreference = teamLeadPreference + juniorPreference;

                    costMatrix[i, j] = -totalPreference;
                }
            }

            var assignments = HungarianAlgorithm.HungarianAlgorithm.FindAssignments(costMatrix);

            var teams = new List<Team>();
            for (int i = 0; i < assignments.Length; i++)
            {
                int juniorIndex = assignments[i];

                if (i < n && juniorIndex < m)
                {
                    var teamLead = teamLeadsList[i];
                    var junior = juniorsList[juniorIndex];
                    teams.Add(new Team(teamLead, junior));
                }
            }

            return teams;
        }

        private int GetPreferenceScore(int[] wishlist, int employeeId)
        {
            int index = Array.IndexOf(wishlist, employeeId);
            return index >= 0 ? wishlist.Length - index : 0;
        }
    }
}