using Shared.Model.Records;

public class SimpleStrategy : ITeamBuildingStrategy
{
    public IEnumerable<Team> BuildTeams(IEnumerable<Wishlist> teamLeadsWishlists,
        IEnumerable<Wishlist> juniorsWishlists)
    {
        var teams = new List<Team>();

        var teamLeads = teamLeadsWishlists.Select(wishList => wishList.Employee);
        var juniors = juniorsWishlists.Select(wishList => wishList.Employee);

        var availableTeamLeads = teamLeads.ToDictionary(t => t.Id, t => t); // Свободные тимлиды по ID

        var availableJuniors = juniors.ToDictionary(j => j.Id, j => j); // Свободные джуны по ID

        var teamLeadPreferences = teamLeadsWishlists
            .ToDictionary(wishList => wishList.Employee.Id, wishList => wishList.PreferredEmployees);
        var juniorPreferences = juniorsWishlists
            .ToDictionary(wishList => wishList.Employee.Id, wishList => wishList.PreferredEmployees);

        foreach (var teamLead in teamLeads)
        {
            if (!availableTeamLeads.ContainsKey(teamLead.Id)) continue; // тимлид уже присоединён к команде

            foreach (var preferredJunior in teamLeadPreferences[teamLead.Id])
            {
                if (availableJuniors.TryGetValue(preferredJunior.Id, out Employee? value))
                {
                    // Если джун доступен, создаём команду
                    var junior = value;

                    // Находим или создаём команду для тимлида
                    teams.Add(new Team(teamLead, junior));

                    // Удаляем джуна из свободных
                    availableJuniors.Remove(preferredJunior.Id);

                    // Удаляем тимлида из свободных
                    availableTeamLeads.Remove(teamLead.Id);

                    break; // Переходим к следующему тимлиду
                }
            }
        }

        return teams;
    }
}