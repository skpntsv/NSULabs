namespace Hackathon.Core.Api;

public interface ITeamBuildingStrategy
{
    /// <summary>
    /// Распределяет тиилидов и джунов по командам
    /// </summary>
    /// <param name="teamLeads">Тимлиды</param>
    /// <param name="juniors">Джуны</param>
    /// <param name="teamLeadsWishlists"></param>
    /// <param name="juniorsWishlists"></param>
    /// <returns>Список команд</returns>
    List<ITeam> BuildTeams(List<IEmployee> teamLeads, List<IEmployee> juniors,
        List<IWishlist> teamLeadsWishlists, List<IWishlist> juniorsWishlists);
}