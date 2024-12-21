using Hackathon.Contracts;

namespace Hackathon.Utils;

public static class WishListGenerator
{
    public static List<Wishlist> GenerateWishLists(
        List<Employee> employees,
        List<Employee> others,
        Random rand
    )
    {
        return employees.Select(e =>
                new Wishlist(e.Id, others
                    .Select(o => o.Id)
                    .OrderBy(x => rand.Next())
                    .Take(employees.Count)
                    .ToArray()))
            .ToList();
    }
}