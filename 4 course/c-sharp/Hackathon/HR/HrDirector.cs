using Hackathon.Contracts;

namespace Hackathon.HR
{
    public class HrDirector
    {
        private enum EmployeeLevel
        {
            TeamLead,
            Junior
        }
        public double CountScore(List<Employee> teamLeads, List<Employee> juniors, List<Team> teams,
            List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists)
        {
            var sum = 0.0;
            var indexes = CountSatisfactionIndexes(EmployeeLevel.Junior, juniors, teams, juniorsWishlists);
            sum += indexes.Where(index => index != 0).Sum(index => 1.0 / index);

            indexes = CountSatisfactionIndexes(EmployeeLevel.TeamLead, teamLeads, teams, teamLeadsWishlists);
            sum += indexes.Where(index => index != 0).Sum(index => 1.0 / index);

            return sum == 0 ? 0 : (teamLeads.Count() + juniors.Count()) / sum;
        }

        private int[] CountSatisfactionIndexes(EmployeeLevel employeeLevel, List<Employee> employees, List<Team> teams, List<Wishlist> wishlists)
        {
            var indexes = new int[employees.Count + 1];
            foreach (var employee in employees)
            {
                indexes[employee.Id] = 0;
                foreach (var team in teams)
                {
                    if ((employeeLevel == EmployeeLevel.TeamLead && team.TeamLead.Id == employee.Id)
                         || (employeeLevel == EmployeeLevel.Junior && team.Junior.Id == employee.Id))
                    {
                        var wishlist = wishlists.FirstOrDefault(w => w.EmployeeId == employee.Id);
                        if (wishlist == null) continue;
                        var rating = wishlist.DesiredEmployees;
                        var teammateId = (employeeLevel == EmployeeLevel.TeamLead ? team.Junior.Id : team.TeamLead.Id);
                        var teammateIndex = FindTeammateIndex(rating, teammateId);
                        if (teammateIndex == -1) continue;
                        indexes[employee.Id] = employees.Count() - teammateIndex;
                    }
                }
            }
            return indexes;
        }
        private int FindTeammateIndex(int[] rating, int id)
        {
            for (var i = 0; i < rating.Length; i++)
            {
                if (rating[i] == id) return i;
            }
            return -1;
        }
    }
}