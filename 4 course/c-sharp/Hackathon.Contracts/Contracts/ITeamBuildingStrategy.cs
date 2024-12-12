namespace Hackathon.Contracts
{
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
        List<Team> BuildTeams(List<Employee> teamLeads, List<Employee> juniors,
            List<Wishlist> teamLeadsWishlists, List<Wishlist> juniorsWishlists);
    }
}
