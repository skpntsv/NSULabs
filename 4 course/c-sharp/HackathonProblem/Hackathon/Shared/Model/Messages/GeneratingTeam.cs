using Shared.Model.Records;

namespace Shared.Model.Messages;

public record GeneratingTeam 
{
    public int ExperimentId {get; set;}
    public int HackathonCount {get; set;}
    public required IEnumerable<Team> Teams { get; set; }
        
}