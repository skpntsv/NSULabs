namespace Shared.Model.DataBase;

using System.Collections.Generic;

public class WishlistDB
{
    public int? Id { get; set; } = null;
    public int ExperimentId { get; set; }
    public int HackathonCount {get; set;}
    public EmployeeDB Employee { get; set; } 
    public IEnumerable<EmployeeDB> PreferredEmployees { get; set; }
}