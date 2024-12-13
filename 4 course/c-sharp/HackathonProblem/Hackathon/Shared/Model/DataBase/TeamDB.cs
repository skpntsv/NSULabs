namespace Shared.Model.DataBase;

public class TeamDB
{
    public int? Id { get; set; } = null;
    public int ExperimentId { get; set; } 
    public int HackathonCount {get; set;}
    public EmployeeDB TeamLead { get; set; }
    public EmployeeDB Junior { get; set; }
}