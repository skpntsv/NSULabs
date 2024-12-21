namespace EmployeeService.Models;

public class Options
{
    public required string EmployeeType { get; set; }
    public required int EmployeeId { get; set; }
    public required string TeamLeadsFile { get; set; }
    public required string JuniorsFile { get; set; }
}