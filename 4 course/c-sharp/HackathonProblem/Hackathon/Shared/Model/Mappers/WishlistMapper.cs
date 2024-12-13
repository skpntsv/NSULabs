using Shared.Model.DataBase;
using Shared.Model.Records;

namespace Shared.Model.Mappers;

public static class WishlistMapper
{
    public static WishlistDB ToWishlistDB(this Wishlist wishlist, int experimentId, int hackathonCount)
    {
        ArgumentNullException.ThrowIfNull(wishlist);

        return new WishlistDB
        {
            ExperimentId = experimentId,
            HackathonCount = hackathonCount,
            Employee = wishlist.Employee.ToEmployeeDB(experimentId, hackathonCount),
            PreferredEmployees = wishlist.PreferredEmployees
                .Select(e => e.ToEmployeeDB(experimentId, hackathonCount))
                .ToList()
        };
    }

    public static Wishlist ToWishlist(this WishlistDB wishlistDb)
    {
        ArgumentNullException.ThrowIfNull(wishlistDb);

        return new Wishlist(
            wishlistDb.Employee.ToEmployee(),
            wishlistDb.PreferredEmployees.Select(e => e.ToEmployee()).ToList()
        );
    }
}
