using Hackathon.DataBase.Context;
using Hackathon.DataBase.Strategy;
using Microsoft.EntityFrameworkCore;

namespace Hackathon.DataBase.Models;

public class HrManager
{
    private readonly HackathonContext _context;

    private readonly List<int> juniorWishlistIds = new List<int>();
    private readonly List<int> teamLeadWishlistIds = new List<int>();

    public HrManager(HackathonContext context)
    {
        _context = context;
    }

    public void ReceiveWishlist(int wishlistId, string type)
    {
        if (type.Equals("Junior"))
        {
            juniorWishlistIds.Add(wishlistId);
        }
        else
        {
            teamLeadWishlistIds.Add(wishlistId);
        }
    }

    public List<Team> GenerateTeams(ITeamBuildingStrategy teamBuildingStrategy, int hackathonId)
    {
        var hackathon = _context.Hackathons
                            .Include(h => h.Teams)
                            .FirstOrDefault(h => h.HackathonId == hackathonId)
                        ?? throw new Exception("Hackathon not found");

        var teamLeads = _context.Employees.Where(e => hackathon.TeamLeadIds.Contains(e.Id)).ToList();
        var juniors = _context.Employees.Where(e => hackathon.JuniorIds.Contains(e.Id)).ToList();

        var teamLeadWishlists = _context.Wishlists.Where(w => teamLeadWishlistIds.Contains(w.WishlistId)).ToList();
        var juniorWishlists = _context.Wishlists.Where(w => juniorWishlistIds.Contains(w.WishlistId)).ToList();

        var teams = teamBuildingStrategy.BuildTeams(
            teamLeads,
            juniors,
            teamLeadWishlists,
            juniorWishlists
        ).ToList();

        return teams;
    }

    public void SendTeamsToHrDirector(List<int> teamIds, HrDirector hrDirector)
    {
        hrDirector.ReceiveTeamsAndWishlists(teamIds, teamLeadWishlistIds.Concat(juniorWishlistIds).ToList());
    }

    public void ClearAllWishlists()
    {
        juniorWishlistIds.Clear();
        teamLeadWishlistIds.Clear();
    }
}