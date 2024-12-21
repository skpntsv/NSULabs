using System.ComponentModel.DataAnnotations;

namespace Hackathon.DataBase.Models;

public class Hackathon
{
    [Key]
    public int HackathonId { get; set; }

    public List<int> TeamLeadIds { get; set; } = [];
    public List<int> JuniorIds { get; set; } = [];

    public virtual ICollection<Team> Teams { get; set; } = [];

    public double HarmonyScore { get; set; }

    public Hackathon()
    {
    }

    public Hackathon(List<int> teamLeadIds, List<int> juniorIds)
    {
        TeamLeadIds = teamLeadIds;
        JuniorIds = juniorIds;
    }
}