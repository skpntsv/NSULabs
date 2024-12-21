namespace Hackathon.Core.Models;

public record Team(Employee TeamLead, Employee Junior) 
{
    public override string ToString()
    {
        return $"Team({TeamLead} : {Junior})";
    }
}