using Hackathon.Core.Api;
using Hackathon.Core.Api.Records;

namespace Hackathon.Core.Implementation;

public class SmartBuildingStrategy : ITeamBuildingStrategy
{
    public List<Team> BuildTeams(List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists)
    {
        var leaderList = teamLeadsWishlists.Select(wishList => wishList.Employee).ToList();
        var juniorList = juniorsWishlists.Select(wishList => wishList.Employee).ToList();

        var leaderPreferences = teamLeadsWishlists
            .ToDictionary(wishlist => wishlist.Employee.Id, wishlist => wishlist.PreferredEmployees.Select(e => e.Id).ToArray());
        var juniorPreferences = juniorsWishlists
            .ToDictionary(wishlist => wishlist.Employee.Id, wishlist => wishlist.PreferredEmployees.Select(e => e.Id).ToArray());

        var compatibilityMatrix = new int[leaderList.Count, juniorList.Count];

        for (var leaderIndex = 0; leaderIndex < leaderList.Count; ++leaderIndex)
        {
            for (var juniorIndex = 0; juniorIndex < juniorList.Count; ++juniorIndex)
            {
                var leaderId = leaderList[leaderIndex].Id;
                var juniorId = juniorList[juniorIndex].Id;

                var leaderPreference = leaderPreferences.GetValueOrDefault(leaderId, []);
                var juniorPreference = juniorPreferences.GetValueOrDefault(juniorId, []);

                compatibilityMatrix[leaderIndex, juniorIndex] =
                    -(CalculatePreferenceScore(leaderPreference, juniorId) +
                      CalculatePreferenceScore(juniorPreference, leaderId));
            }
        }

        var assignments = HungarianAlgorithm.HungarianAlgorithm.FindAssignments(compatibilityMatrix);

        return CreateTeams(leaderList, juniorList, assignments);
    }

    private static int CalculatePreferenceScore(int[] preferences, int employeeId)
    {
        var index = Array.IndexOf(preferences, employeeId);
        return index < 0 ? 0 : preferences.Length - index;
    }

    private static List<Team> CreateTeams(List<Employee> leaders, List<Employee> juniors, int[] assignments)
    {
        var teams = new List<Team>();

        for (var leaderIndex = 0; leaderIndex < assignments.Length; ++leaderIndex)
        {
            var juniorIndex = assignments[leaderIndex];
            if (juniorIndex >= 0 && juniorIndex < juniors.Count)
            {
                teams.Add(new Team(leaders[leaderIndex], juniors[juniorIndex]));
            }
        }

        return teams;
    }
}