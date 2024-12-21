using Hackathon.DataBase.Models;

namespace Hackathon.DataBase.Strategy;

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
    IEnumerable<Team> BuildTeams(IEnumerable<Employee> teamLeads, IEnumerable<Employee> juniors,
        IEnumerable<Wishlist> teamLeadsWishlists, IEnumerable<Wishlist> juniorsWishlists);
}