using System.Diagnostics;
using Hackathon.Core.Api.Records;

namespace HrDirectorService.Models.Logic;

public class HrDirectorLogic
{
    public double CalculateHarmonicMeanByTeams(IEnumerable<Team> teams, IEnumerable<Wishlist> teamLeadsWishlists,
        IEnumerable<Wishlist> juniorsWishlists)
    {
        var satisfactions = new List<int>();

        var teamLeadPreferences = teamLeadsWishlists
            .ToDictionary(w => w.Employee.Id, w => w.PreferredEmployees);
        var juniorPreferences = juniorsWishlists
            .ToDictionary(w => w.Employee.Id, w => w.PreferredEmployees);

        Debug.Assert(teamLeadPreferences.Count == juniorPreferences.Count);

        foreach (var team in teams)
        {
            var teamLead = team.TeamLead;
            var junior = team.Junior;

            // Считаем очки предпочтений
            var teamLeadSatisfactionScore = CalculateSatisfactionScore(teamLeadPreferences[teamLead.Id], junior);
            satisfactions.Add(teamLeadSatisfactionScore);

            var juniorSatisfactionScore = CalculateSatisfactionScore(juniorPreferences[junior.Id], teamLead);
            satisfactions.Add(juniorSatisfactionScore);
        }

        return Calculator.CalculateHarmonicMean(satisfactions);
    }

    private static int CalculateSatisfactionScore(IEnumerable<Employee> preferences, Employee coworker)
    {
        var maxScore = preferences.Count();
        var rank = Array.IndexOf(preferences.Select(e => e.Id).ToArray(), coworker.Id);
        Debug.Assert(rank >= 0);

        var satisfactionScore = maxScore - rank;
        return satisfactionScore;
    }
}