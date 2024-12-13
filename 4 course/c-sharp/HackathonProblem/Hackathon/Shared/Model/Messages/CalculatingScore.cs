namespace Shared.Model.Messages;

public record CalculatingScore
{
    public int ExperimentId { get; set; }
    public int HackathonCount { get; set; }
    public double Score { get; set; }
}