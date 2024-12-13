using Shared.Model.Records;

namespace Shared.Model.DataBase;

public class EmployeeDB
{
    public int? Id { get; set; } = null;
    public int ExperimentId { get; set; }
    public int HackathonCount { get; set; }
    public Employee Employee { get; set; }
}