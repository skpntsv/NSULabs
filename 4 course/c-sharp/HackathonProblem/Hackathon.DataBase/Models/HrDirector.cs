using Hackathon.DataBase.Context;

namespace Hackathon.DataBase.Models;

public class HrDirector
{
    private readonly HackathonContext _context;
    private List<Team> Teams = new List<Team>();
    private List<Wishlist> Wishlists = new List<Wishlist>();

    public HrDirector(HackathonContext context)
    {
        _context = context;
    }

    public void ReceiveTeamsAndWishlists(List<int> teamIds, List<int> wishlistIds)
    {
        Teams = _context.Teams.Where(t => teamIds.Contains(t.TeamId)).ToList();
        Wishlists = _context.Wishlists.Where(w => wishlistIds.Contains(w.WishlistId)).ToList();
    }

    public double CalculateTeamsScore()
    {
        double totalSatisfaction = 0;
        int participantCount = 0;

        foreach (var team in Teams)
        {
            var teamLead = _context.Employees.Find(team.TeamLeadId);
            var junior = _context.Employees.Find(team.JuniorId);

            if (teamLead == null || junior == null) continue;

            int teamLeadSatisfaction = CalculateSatisfaction(teamLead.Id, junior.Id);
            int juniorSatisfaction = CalculateSatisfaction(junior.Id, teamLead.Id);

            if (teamLeadSatisfaction > 0)
            {
                totalSatisfaction += teamLeadSatisfaction;
                participantCount++;
            }

            if (juniorSatisfaction > 0)
            {
                totalSatisfaction += juniorSatisfaction;
                participantCount++;
            }
        }

        return participantCount != 0 ? totalSatisfaction / participantCount : 0;
    }

    private int CalculateSatisfaction(int currentEmployeeId, int partnerId)
    {
        var wishlist = Wishlists.FirstOrDefault(w => w.EmployeeId == currentEmployeeId);
        if (wishlist == null) return 0;

        int index = Array.IndexOf(wishlist.DesiredEmployeeIds, partnerId);
        return index >= 0 ? wishlist.DesiredEmployeeIds.Length - index : 0;
    }
}