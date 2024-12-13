using Shared.Model.Records;

public interface ITeamBuildingStrategy
{
    /// <summary>
    /// Распределяет тиилидов и джунов по командам
    /// </summary>
    /// <param name="teamLeads">Тимлиды</param>
    /// <param name="juniors">Джуны</param>
    /// <returns>Список команд</returns>
    IEnumerable<Team> BuildTeams(IEnumerable<Wishlist> teamLeadsWishlists, IEnumerable<Wishlist> juniorsWishlists);
}