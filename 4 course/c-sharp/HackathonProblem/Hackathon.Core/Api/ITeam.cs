namespace Hackathon.Core.Api;

public interface ITeam
{
    public IEmployee TeamLead { get; }
    public IEmployee Junior { get; }
}