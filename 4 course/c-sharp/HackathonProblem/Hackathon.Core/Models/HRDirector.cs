namespace Hackathon.Core.Models
{
    public class HrDirector
    {
        private List<Team> Teams = [];
        private List<Wishlist> Wishlists = [];
        public void ReceiveTeamsAndWishlists(List<Team> teams, IEnumerable<Wishlist> wishlists) 
        {
            Teams = teams;
            Wishlists = (List<Wishlist>)wishlists;

            // Console.WriteLine($"[LOG] Received by HrDirector team {teams.Count}, wishlist {wishlists.Count}");
        }

        public double CalculateTeamsScore() 
        {
            double totalSatisfaction = 0;
            int participantCount = Wishlists.Count;

            foreach (var team in Teams)
            {
                int teamLeadSatisfaction = CalculateSatisfaction(team.TeamLead, team.Junior);
                int juniorSatisfaction = CalculateSatisfaction(team.Junior, team.TeamLead);

                totalSatisfaction += 1.0 / teamLeadSatisfaction + 1.0 / juniorSatisfaction;
                // Console.WriteLine($"[LOG] teamLeadSatisfaction: {teamLeadSatisfaction}");
                // Console.WriteLine($"[LOG] juniorSatisfaction: {juniorSatisfaction}");
                // Console.WriteLine($"[LOG] totalSatisfaction: {totalSatisfaction}");
            }

            return participantCount / totalSatisfaction;
        }

        private int CalculateSatisfaction(Employee current, Employee partner)
        {
            var wishlist = Wishlists.FirstOrDefault(it => it.EmployeeId == current.Id);
            // Console.WriteLine($"[LOG] wishlist: {wishlist}");
            if (wishlist == null) {
                return 0;
            }

            int index = wishlist.DesiredEmployees.ToList().FindIndex(it => it == partner.Id);
            // Console.WriteLine($"[LOG] index: {20 - index}");
            return index >= 0 ? Teams.Count - index : 0;
        }
    }
}
