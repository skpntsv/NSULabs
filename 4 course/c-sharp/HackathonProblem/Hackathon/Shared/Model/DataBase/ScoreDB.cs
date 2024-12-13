namespace Shared.Model.DataBase;

public class ScoreDB
{
    public int Id { get; set; }
    public int ExperimentId { get; set; }
    public int HackathonCount { get; set; }
    public double Score { get; set; }
}