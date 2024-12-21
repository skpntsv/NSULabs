using Hackathon.Core.Api.Records;
using Hackathon.Core.Model.DataBase;

namespace Hackathon.Core.Model.Mappers;

public static class TeamMapper
{
    public static TeamDB ToTeamDB(this Team team, int experimentId, int hackathonCount)
    {
        ArgumentNullException.ThrowIfNull(team);

        return new TeamDB
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            TeamLead = team.TeamLead.ToEmployeeDB(experimentId, hackathonCount),
            Junior = team.Junior.ToEmployeeDB(experimentId, hackathonCount),
        };
    }

    public static Team ToTeam(this TeamDB teamDb)
    {
        ArgumentNullException.ThrowIfNull(teamDb);

        return new Team(
            teamDb.TeamLead.ToEmployee(),
            teamDb.Junior.ToEmployee()
        );
    }
}
