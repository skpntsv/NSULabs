using Hackathon.Contracts;
using Hackathon.HR;
using Hackathon.Utils;

namespace Hackathon.Core;

public class Hackathon(HrDirector hrDirector, HrManager hrManager)
{
    public double RunHackathon(List<Employee> teamLeads, List<Employee> juniors)
    {
        var rand = new Random();
        var juniorsWishlists = WishListGenerator.GenerateWishLists(juniors, teamLeads, rand);
        var teamLeadsWishlists = WishListGenerator.GenerateWishLists(teamLeads, juniors, rand);
        // foreach (var jw in juniorsWishlists)
        // {
        //     Console.WriteLine($"junior_id: {jw.EmployeeId}, wishlist: {jw.DesiredEmployees[0]}, {jw.DesiredEmployees[1]}, {jw.DesiredEmployees[2]}, {jw.DesiredEmployees[3]}, {jw.DesiredEmployees[4]}");
        // }
        // foreach (var tw in teamLeadsWishlists)
        // {
        //     Console.WriteLine($"teamLead_id: {tw.EmployeeId}, wishlist: {tw.DesiredEmployees[0]}, {tw.DesiredEmployees[1]}, {tw.DesiredEmployees[2]}, {tw.DesiredEmployees[3]}, {tw.DesiredEmployees[4]}");
        // }
        var teams = hrManager.BuildTeams(teamLeads, juniors, teamLeadsWishlists, juniorsWishlists);
        // foreach (var team in teams)
        // {
        //     Console.WriteLine($"Team Lead: {team.TeamLead.Name}, Junior: {team.Junior.Name}");
        // }
        
        var score = hrDirector.CountScore(teamLeads, juniors, teams, juniorsWishlists, teamLeadsWishlists);
        return score;
    }
}