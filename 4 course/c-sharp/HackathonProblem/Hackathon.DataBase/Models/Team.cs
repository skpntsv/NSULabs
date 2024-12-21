using System.ComponentModel.DataAnnotations;

namespace Hackathon.DataBase.Models;

public class Team
{
    [Key]
    public int TeamId { get; set; }

    public int TeamLeadId { get; set; }
    public int JuniorId { get; set; }

    public int HackathonId { get; set; }
        
    public virtual global::Hackathon.DataBase.Models.Hackathon? Hackathon { get; set; }

    public Team(int teamLeadId, int juniorId)
    {
        TeamLeadId = teamLeadId;
        JuniorId = juniorId;
    }

    public override string ToString()
    {
        return $"Team(TeamLeadId: {TeamLeadId}, JuniorId: {JuniorId})";
    }
}